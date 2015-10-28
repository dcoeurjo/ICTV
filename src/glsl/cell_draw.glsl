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
layout (location = 0) in uvec2 i_key; // translation + scale
//layout (location = 1) in vec3 position; //cube vertex
//layout (location = 0) out float dist_to_camera;
layout (location = 0) out vec3 color;

#define COLOR_TRANSITION 0
#define COLOR_LEVEL 1
#define COLOR_DENSITY 0
#define NO_COLOR 0

vec3 getSideTransitions(vec3 min_c, vec3 parent, float s, float level)
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
		
		vec4 np = vec4( parent + s - 0.5, 1);	
		np.x *= u_scene_size.x;
		np.y *= u_scene_size.y;
		np.z*= u_scene_size.z;
		np.w = 1;
		
		vec3 mv = (u_transforms.modelview * np).xyz;
		float d = getLength(mv) * u_tan_fovy;
		if ( d < 2*u_scale*s*length(u_scene_size)) //if smaller neighbour
		{
			/*float val = textureLod(densities, node, clamp(max_tex - level, 0, max_tex)).r;
			if (val == 0 || val == 1)
				ret += ((neighbours[i] - node) * 1.0/s);*/
			ret += ((neighbours[i] - node) * 1.0/s);
		}
		else
		{
			//ret += ((neighbours[i] - node) * 1.0/s);
		}
	}

	return ret;
}


void main (void) {

	// extract octree position and scale
	vec3 c; //00 corner
	float s;
	lt_cell_3_20 (i_key.xy, c, s);
	
	//s *= 0.5;
	//c = c+vec3(s);
	
	float level = lt_level_3_20(i_key.xy);

	// set up varyings
	vec3 min_corner = ((c-0.5) + vec3(s/2.0)) * u_scene_size;
	vec3 vert = min_corner;// + (position+0.5)* s *u_scene_size;
	//dist_to_camera = max(length(u_transforms.modelview * vec4(vert, 1)) - 130, 0);
    
	gl_Position = u_transforms.modelviewprojection  * vec4(vert, 1);

#if COLOR_LEVEL
	/*vec3 color_levels[12] = vec3[12] (
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (0.8, 0, 0.2),
		vec3 (0.6, 0, 0.4),
		vec3 (0.4, 0, 0.6),
		vec3 (0.2, 0, 0.8),
		vec3 (0, 0, 1),
		vec3 (0, 0, 1),
		vec3 (0, 0, 1)
	);
	color = color_levels[12 - uint(level)];*/
	
	/*vec3 color_levels[6] = vec3[6] (
		vec3 (1, 0, 0),
		vec3 (0.8, 0, 0.2),
		vec3 (0.6, 0, 0.4),
		vec3 (0.4, 0, 0.6),
		vec3 (0.2, 0, 0.8),
		vec3 (0, 0, 1)
	);*/
	
	vec3 color_levels[6] = vec3[6] (
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1),
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1)
	);
	color = color_levels[uint(level)%6];
#endif

}
#endif

// ---------------------------------------------------------
// Fragment shader

#ifdef FRAGMENT_SHADER
//layout(location = 0) in float dist;
layout(location = 0) in vec3 color;
layout(location = 0) out vec4 o_colour;

void main (void) {
	o_colour = vec4(color, 1);
}
#endif // FRAGMENT_SHADER

