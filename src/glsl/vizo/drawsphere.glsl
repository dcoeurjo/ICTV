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

#ifdef VERTEX_SHADER

in vec4 position;
out vec4 vertex_position;

uniform float u_curv_radius;

void main( )
{
	vec3 corner_pos = position.xyz;
	vec3 scales = (u_scene_size/u_size_tex)*u_curv_radius;
	corner_pos *= scales;
	corner_pos.x += u_scene_size.x*0.5 + u_size_tex/2.0;

	/*
	mat4 scaleMat;
	scaleMat[0] = vec4(scales.x, 0, 0, 0);
	scaleMat[1] = vec4(0, scales.y, 0, 0);
	scaleMat[2] = vec4(0, 0, scales.z, 0);
	scaleMat[3] = vec4(0, 0, 0, 1);
	*/

    vertex_position = u_transforms.modelviewprojection * vec4(corner_pos.xyz, 1);

    gl_Position = vertex_position;
}
#endif

#ifdef FRAGMENT_SHADER

in vec4 vertex_position;

out vec4 fragment_color;

void main( )
{
	fragment_color = vec4(0, 0, 1, 1);
}

#endif
