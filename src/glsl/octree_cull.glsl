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

uniform int culling_disabled;

// ---------------------------------------------------------
// Vertex shader

#ifdef VERTEX_SHADER
layout (location = 0) in uvec2 i_data;
layout (location = 0) out uvec2 o_data;

void main (void) {
	o_data = i_data;
}
#endif //VERTEX_SHADER

// ---------------------------------------------------------
// Geometry shader

#ifdef GEOMETRY_SHADER
layout (points) in;
layout (points, max_vertices = 16) out;

layout (location = 0) in uvec2 i_data[];

layout (stream = 0) out uvec2 o_data;
layout (stream = 0) out vec4 o_code;
layout (stream = 1) out uvec2 o_data_tr;
layout (stream = 1) out vec4 o_neighbours;

//determine if a cell is bordered by other cells of a higher resolution (smaller in scale)
vec3 getSideTransitions(vec3 min_c, float s)
{
	vec3 max_c = min_c + s;
	vec3 node = ((min_c + max_c) * 0.5).xyz;

	vec3 neighbours[6];
	neighbours[0] = node + s*vec3(0, 0, 1);
	neighbours[1] = node + s*vec3(0, 0, -1);
	neighbours[2] = node + s*vec3(0, 1, 0);
	neighbours[3] = node + s*vec3(0, -1, 0);
	neighbours[4] = node + s*vec3(1, 0, 0);
	neighbours[5] = node + s*vec3(-1, 0, 0);

	vec3 ret = vec3(0, 0, 0);
	for(int i=0; i<6; i++)
	{
		vec4 n = vec4(neighbours[i], 1);
		n -= 0.5;
		n.x *= u_scene_size.x;
		n.y *= u_scene_size.y;
		n.z *= u_scene_size.z;
		n.w = 1;
		
		vec3 mv = (u_transforms.modelview * n).xyz;
		float d = getLength(mv) * u_tan_fovy;
		if ( d <= u_scale*s*length(u_scene_size))
		{
			ret += ((neighbours[i] - node) * 1.0/s);
		}
	}

	return ret;
}

uint getCode_regularCell(vec3 min, float s, out vec3 corners[8])
{
	uint caseCode = 0u;

	uint d = 1u;
	for(uint i=0; i<8; i++)
	{
		uint x = i & 0x01;
		uint z = (i & 0x02) >> 1u;
		uint y =(i & 0x04) >> 2u;
		corners[i] = min + vec3(x, y, z)*s;
		caseCode |= (isInSurface(corners[i], true) << i) & d;
		d*=2u;
	}

	return caseCode;
}

bool isEmptyCell(vec3 min_c, float s, float level)
{
	float tess = u_tessellation;
	
	float step = s / tess;
	
	uint reference = isInSurface(min_c, false);
	
	for(float x = 0; x<=tess; x += 1)
	{
		for(float y = 0; y<=tess; y += 1)
		{
			for(float z = 0; z<=tess; z += 1)
			{
				vec3 corner = min_c + vec3(x, y, z)*step;
				if ( isInSurface(corner, false) != reference )
					return false;
			}
		}
	}
	return true;
	
}

void main (void) {
	// get position
	vec3 c;
	float s;
	lt_cell_3_20 (i_data[0].xy, c, s);

	// compute aabb
	vec3 node = (c - 0.5);
	node.x *= u_scene_size.x;
	node.y *= u_scene_size.y;
	node.z *= u_scene_size.z;

	vec3 node_min = node;
	vec3 node_max = node + s * u_scene_size;

	// emit if intersection or inside
	if (octree_frustum_test (node_min, node_max) || culling_disabled != 0) 
	{
		uint level = lt_level_3_20(i_data[0].xy);
		
		vec3 neighbours = vec3(0);
		if (level < max_level)
			neighbours = getSideTransitions(c, s);
		o_code = vec4( neighbours, 1 );
		
		if ( ! isEmptyCell(c, s, level) )
		{
			o_data = i_data[0];
			
			EmitStreamVertex (0);
			EndStreamPrimitive (0);
		}
		
		
		if (level <= max_level)
		{
			o_data_tr = i_data[0];
			for(int i=0; i<3; i++)
			{
				if (neighbours[i] != 0)
				{
					o_neighbours.xyz = neighbours;
					o_neighbours[3]= i;

					EmitStreamVertex (1);
					EndStreamPrimitive (1);
				}
			}
		}
	}
}
#endif //GEOMETRY_SHADER

