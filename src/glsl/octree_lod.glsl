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

// ---------------------------------------------------------
// Vertex shader

#ifdef VERTEX_SHADER
layout (location = 0) in uvec2 i_data;
layout (location = 0) out uvec2 o_data;
void main() {
	o_data = i_data;
}
#endif

// ---------------------------------------------------------
// Geometry shader

#ifdef GEOMETRY_SHADER
layout (points) in;
layout (points, max_vertices = 8) out;

layout (location = 0) in uvec2 i_data[];
layout (location = 0) out uvec2 o_data;

layout (binding = 0, offset = 0) uniform atomic_uint unchanged;

uniform int u_regular;

void main() {
	// get position
	vec3 node, parent;
	float node_size;
	
	vec3 c;
	float s;
	lt_cell_3_20 (i_data[0].xy, c, s, parent);
	
	node_size = s;
	
	node = c;
	c += vec3(node_size*0.5);

	// translate tree
	node += node_size * 0.5 - 0.5;
	parent += node_size - 0.5;

	// scale tree
	node_size *= length(u_scene_size);
	
	node.x *= u_scene_size.x;
	node.y *= u_scene_size.y;
	node.z *= u_scene_size.z;
	
	parent.x *= u_scene_size.x;
	parent.y *= u_scene_size.y;
	parent.z *= u_scene_size.z;
	
	//int lvl = max_level;

	// distance from node centers in view space
	node = (u_transforms.modelview * vec4 (node, 1)).xyz;
	parent = (u_transforms.modelview * vec4 (parent, 1)).xyz;
	float dn = getLength(node) * u_tan_fovy;
	float dp = getLength(parent) * u_tan_fovy; // XXX fix the 0.5 factor

	// merge
	bool regular = (u_regular == 1);
	
	if ( (regular && lt_level_3_20(i_data[0].xy) > max_level) || 
		(!regular && (!lt_is_root_3_20 (i_data[0].xy) && (u_scale * 2.0 * node_size) < dp)) )
	{
		atomicCounterIncrement(unchanged);
		
		// make sure we generate the root node only once
		if (lt_is_front_upper_left_3_20 (i_data[0].xy)) {
			o_data.xy = lt_parent_3_20 (i_data[0].xy);
			EmitVertex ();
			EndPrimitive ();
		}
		
	} else if ( (regular && lt_level_3_20(i_data[0].xy) < max_level) ||
		( !regular && ( lt_level_3_20(i_data[0].xy) < max_level && (u_scale * node_size) > dn) ) )// split //!lt_is_leaf_3_20(i_data[0].xy) //lt_level_3_20(i_data[0].xy) < 10u
	{
		//if (!isEmptyCell(c, s))
		//{
			atomicCounterIncrement(unchanged);
			uvec2 children[8];
			lt_children_3_20 (i_data[0].xy, children);
			for (int i = 0; i < 8; ++i) {
				o_data.xy = children[i];
				EmitVertex ();
				EndPrimitive ();
			}
		
		/*}else{
			o_data.xy = i_data[0].xy;
			EmitVertex ();
			EndPrimitive ();
		}*/
	} else { // keep
		o_data.xy = i_data[0].xy;
		EmitVertex ();
		EndPrimitive ();
	}
}
#endif

