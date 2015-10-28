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

layout (location = 0) out vec3 color;
layout (location = 1) out vec4 center;

void main (void) {

	// extract octree position and scale
	vec3 c; //00 corner
	float s;
	lt_cell_3_20 (i_key.xy, c, s);
	
	float level = lt_level_3_20(i_key.xy);
	
	center = vec4(c, s);

	// set up varyings
	vec3 color_levels[6] = vec3[6] (
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1),
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1)
	);
	color = color_levels[uint(level)%6];
}
#endif

/*
//Cube data, used for cells drawing
const GLfloat g_cubeVertices[] = {  0.5f, -0.5f, -0.5f,   // 0 
					0.5f,  0.5f, -0.5f,   // 1
					0.5f,  0.5f,  0.5f,   // 2
					0.5f, -0.5f,  0.5f,   // 3
					-0.5f, -0.5f, -0.5f,   // 4
					-0.5f,  0.5f, -0.5f,   // 5
					-0.5f,  0.5f,  0.5f,   // 6
					-0.5f, -0.5f,  0.5f }; // 7
					   
const GLushort g_cubeIndexes[] = { 0,1,1,2,2,3,3,0,   // front
					1,5,5,6,6,2,2,1,   // right
					5,4,4,7,7,6,6,5,   // back
					4,0,0,3,3,7,7,4,   // left
					2,3,3,7,7,6,6,2,   // top
					0,4,4,5,5,1,1,0 }; // bottom
*/
					
#ifdef GEOMETRY_SHADER
layout (points) in;
layout (line_strip, max_vertices = 30) out;

layout (location = 0) in vec3 color[];
layout (location = 1) in vec4 center[];

layout (location = 0) out vec3 o_color;

void main() {
	
	vec3 c = center[0].xyz; //00 corner
	float s = center[0].w;

	c = (c-0.5) * u_scene_size;
	//vec3 vert = min_corner;// + (position+0.5)* s *u_scene_size;
	//dist_to_camera = max(length(u_transforms.modelview * vec4(vert, 1)) - 130, 0);
    
   vec3 vertices[8] = { c + (vec3( 0.5, -0.5, -0.5)+0.5) * s * u_scene_size,   // 0 
						c + (vec3( 0.5,  0.5, -0.5)+0.5) * s * u_scene_size,   // 1
						c + (vec3( 0.5,  0.5,  0.5)+0.5) * s * u_scene_size,   // 2
						c + (vec3( 0.5, -0.5,  0.5)+0.5) * s * u_scene_size,   // 3
						c + (vec3(-0.5, -0.5, -0.5)+0.5) * s * u_scene_size,   // 4
						c + (vec3(-0.5,  0.5, -0.5)+0.5) * s * u_scene_size,   // 5
 						c + (vec3(-0.5,  0.5,  0.5)+0.5) * s * u_scene_size,   // 6
						c + (vec3(-0.5, -0.5,  0.5)+0.5) * s * u_scene_size }; // 7
	//front face
    o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[0], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[1], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[2], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[3], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[0], 1);
	EmitVertex();
	EndPrimitive();
	
	//back face
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[5], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[4], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[7], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[6], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[5], 1);
	EmitVertex();
	EndPrimitive();
	
	//midlines face
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[0], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[4], 1);
	EmitVertex();
	EndPrimitive();
	
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[1], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[5], 1);
	EmitVertex();
	EndPrimitive();
	
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[2], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[6], 1);
	EmitVertex();
	EndPrimitive();
	
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[3], 1);
	EmitVertex();
	o_color = color[0];
	gl_Position = u_transforms.modelviewprojection  * vec4(vertices[7], 1);
	EmitVertex();
	EndPrimitive();
}
#endif

// ---------------------------------------------------------
// Fragment shader

#ifdef FRAGMENT_SHADER
//layout(location = 0) in float dist;
layout(location = 0) in vec3 o_color;
layout(location = 0) out vec4 fragment_color;

void main (void) {
	fragment_color = vec4(o_color, 1);
}
#endif // FRAGMENT_SHADER

