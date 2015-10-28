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

out vec2 tex_coord;

void main (void) {
	vec2 p = vec2 (gl_VertexID & 1, gl_VertexID >> 1 & 1);
        gl_Position = vec4 (p * 2.0 - 1.0, 0.0, 1.0);
        tex_coord = p;
}
#endif

// ---------------------------------------------------------
// Fragment shader

#ifdef FRAGMENT_SHADER
layout(location = 0) out vec4 o_colour;
in vec2 tex_coord;

uniform sampler2D u_texture;

void main (void) {
	o_colour = vec4(texture(u_texture, tex_coord).rgb, 1.0);
}
#endif // FRAGMENT_SHADER

