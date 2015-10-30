/*
 * Copyright 2015 
 * Hélène Perrier <helene.perrier@liris.cnrs.fr>
 * Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Jacques-Olivier Lachaud <jacques-olivier.lachaud@univ-savoie.fr>
 * Jean-Philippe Farrugia <jean-philippe.farrugia@liris.cnrs.fr>
 * Jean-Claude Iehl <jean-claude.iehl@liris.cnrs.fr>
 * 
 * This file is part of ICTV.
 * 
 * ICTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ICTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ICTV.  If not, see <http://www.gnu.org/licenses/>
 */

#version 430

uniform usampler1D code_class_tr;
uniform usampler2D code_vertices_tr;
uniform usampler2D class_triangles_tr;

// ---------------------------------------------------------
// Vertex shader

struct tgl_vertices
{
	vec4 vertices_list[12];
};

struct indirect_draw
{
	uint vertex_count;
	uint instance_count;
	uint first_index;
	uint base_vertex;
	uint base_instance;
};

#ifdef VERTEX_SHADER
layout (location = 0) in uvec2 i_key; // translation + scale
layout (location = 1) in vec4 neighbours;

layout (binding = 2, std430) buffer vertices
{
	tgl_vertices verts[];
};
layout (binding = 3, std430) buffer indirect_draws
{
	indirect_draw cmd[];
};

vec3 getVerticesPosition_transitionCell(vec3 corners[13], uint edge)
{
    uint corner_ids = edge & 0xFFu;
    uint corner_id_2 = corner_ids & 0x0Fu;
    uint corner_id_1 = corner_ids >> 4u;
    
    if (corner_id_1 == corner_id_2 && corner_id_2 == 0)
        return vec3(-1, 0, 0);

    vec3 pos_orig = corners[corner_id_1];
    vec3 pos_end = corners[corner_id_2];
    
    float d0 = getVolume(corners[corner_id_1]);
    float d1 = getVolume(corners[corner_id_2]);

    float t = 0.5;//d1 / (d1 - d0);
    vec3 pos_displ = interpolate(t)*pos_end + (1.0-interpolate(t))*pos_orig;

    return pos_displ;
}

bool isHalfSide(uint edge)
{
	uint corner_ids = edge & 0xFFu;
	uint corner_id_2 = corner_ids & 0x0Fu;
	uint corner_id_1 = corner_ids >> 4u;
	
	return (corner_id_1 >= 9 && corner_id_2 >= 9);
}

void main (void) 
{
	// extract octree position and scale
	vec3 c; //00 corner
	float s;
	lt_cell_3_20 (i_key.xy, c, s);
	
	int max_instance_id = u_tessellation*u_tessellation;
	int buffer_id = gl_VertexID * max_instance_id + gl_InstanceID;
	int max_vertices = 12;
	int max_indices = 36; //12 tgl max * 3

	cell voxel;
	voxel.block_info.xyz = c;
	voxel.block_info.w = s;
	
	voxel.corner = c;
	voxel.scales = vec3(s) / u_tessellation;
	
	vec3 side_neighbour = vec3(0);
	int id = int(neighbours[3]);
	side_neighbour[id] = neighbours[id];
	
	voxel.neighbours = neighbours;
	
	voxel.level = lt_level_3_20(i_key.xy);
	
	/**/
	
	uint a = gl_InstanceID % u_tessellation;
	uint b = gl_InstanceID / u_tessellation;
	voxel.scales = vec3(s / float(u_tessellation));
	voxel.corner = c;
	int dir = 0;
	vec3 move = vec3(0);
	for(int i=0; i<3; i++)
	{
		if (side_neighbour[i] > 0)
			voxel.corner[i] += voxel.scales[i] * (u_tessellation - 1);
		if (side_neighbour[i] != 0)
		{
			move[(i+1) % 3] = a;
			move[(i+2) % 3] = b;
			dir = i;
		}
	}
	voxel.corner += move * voxel.scales;
	
	vec3 corners[8];
	loadCube(voxel.corner, voxel.scales, corners);
	vec3 voxel_corners[13];
	buildTransitionCell(side_neighbour.xyz, corners, voxel_corners);
	
	voxel.tr_code_id_cw[0] = gl_InstanceID;
	voxel.tr_code_id_cw[1] = getCode_transitionCell(voxel_corners);
	voxel.tr_code_id_cw[2] = texture(code_class_tr, float(voxel.tr_code_id_cw[1])/511.0).r;
	voxel.tr_code_id_cw[3] = voxel.tr_code_id_cw[2] >> 7;
	
	voxel.tr_code_id_cw[2] &= 0x7F;
	/**/

	indirect_draw current_cmd;
	tgl_vertices current_vertices;
	
	//voxel.tr_code_id_cw[2] = 0;
	if (voxel.tr_code_id_cw[2] != 0)
	{
		vec3 data[12];
		
		for(int v=0; v<4; v++)
		{
			uvec3 edges = texture(code_vertices_tr, vec2(float(voxel.tr_code_id_cw[1])/511.0, v*0.25 + 0.125)).rgb;
			
			vec3 off = vec3(0);
			vec3 pos = getVerticesPosition_transitionCell(voxel_corners, edges.r);
			if(pos.x == -1)
				break;
			data[v*3].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (isHalfSide(edges.r))
				if (do_offset(voxel.neighbours.xyz, pos-c, s))
					off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[v*3].xyz);
			current_vertices.vertices_list[v*3] = vec4(pos + off*voxel.scales, 1);

			off = vec3(0);
			pos = getVerticesPosition_transitionCell(voxel_corners, edges.g);
			if(pos.x == -1)
				break;
			data[v*3+1].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (isHalfSide(edges.g))
				if (do_offset(voxel.neighbours.xyz, pos-c, s))
					off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[v*3+1].xyz);
			current_vertices.vertices_list[v*3+1] = vec4(pos + off*voxel.scales, 1);

			off = vec3(0);
			pos = getVerticesPosition_transitionCell(voxel_corners, edges.b);
			if(pos.x == -1)
				break;
			data[v*3+2].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (isHalfSide(edges.b))
				if (do_offset(voxel.neighbours.xyz, pos-c, s))
					off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[v*3+2].xyz);
			current_vertices.vertices_list[v*3+2] = vec4(pos + off*voxel.scales, 1);
		}
		
		int nb = 0;
		for(int t=0; t<9; t++)
		{
			uvec4 indices = texture(class_triangles_tr, vec2(float(voxel.tr_code_id_cw[2])/55.0, t*0.11 + 0.055)).rgba;

			if (indices[0] == 37u)
				break;
			nb++;

			if (indices[1] == 37u)
				break;
			nb++;
			
			if (indices[2] == 37u)
				break;
			nb++;
			
			if (indices[3] == 37u)
				break;
			nb++;
		}

		current_cmd.vertex_count = nb;
		current_cmd.instance_count = 1;
		current_cmd.first_index = (voxel.tr_code_id_cw[2]-1)*max_indices;
		current_cmd.base_vertex = buffer_id*max_vertices;
		current_cmd.base_instance = 0;
	}
	else
	{
		//current_vertices.vertices_list[0] = vec4(voxel.corner, 1);
		current_cmd.vertex_count = 0;
		current_cmd.instance_count = 0;
		current_cmd.first_index = 0;
		current_cmd.base_vertex = 0;
		current_cmd.base_instance = 0;
	}
	
	verts[buffer_id] = current_vertices;
	cmd[buffer_id] = current_cmd;
}
#endif
