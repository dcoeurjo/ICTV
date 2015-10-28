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

#ifndef MSAA_FACTOR
#error MSAA_FACTOR must be specified by the application
#endif

#if MSAA_FACTOR > 0
uniform sampler2DMS u_framebuffer_sampler;
#else
uniform sampler2D   u_framebuffer_sampler;
#endif
uniform vec3 u_viewport;

#ifdef VERTEX_SHADER
void main () {
	vec2 p = vec2 (gl_VertexID & 1, gl_VertexID >> 1 & 1);
	gl_Position = vec4 (p * 2.0 - 1.0, 0.0, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
layout (location=0) out vec4 o_colour;

void main () {
	ivec2 st = ivec2 (gl_FragCoord.xy / u_viewport.z + u_viewport.xy);
#if MSAA_FACTOR > 0
	o_colour = vec4 (0);
	for (int i = 0; i < MSAA_FACTOR; ++i) {
		o_colour += texelFetch (u_framebuffer_sampler, st, i);
	}
	o_colour/= vec4 (MSAA_FACTOR);
#else
	o_colour = texelFetch (u_framebuffer_sampler, st, 0);
#endif
}
#endif

