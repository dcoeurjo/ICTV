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

uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

void fetch(vec3 p, float step, float l, inout float volume, inout vec3 xyz, inout vec3 xyz2, inout vec3 xy_yz_xz)
{
	float val = textureLod(densities, p, l).r;
	vec3 p2 = p*u_size_tex;
	float scale = step*step*step; // => h^3
	float error = (step*step)/12.0; // Error term introduce by estimating 2nd order moments (except cross moments) only with the center of cell.

	volume += val * scale;
	xyz += p2 * scale * val;
	xyz2 += ( p2 * p2 + error) * scale * val;
	xy_yz_xz += vec3(p2.x*p2.y, p2.y*p2.z, p2.x*p2.z) * scale * val;
}

bool isincube(in vec3 pos)
{
	return ((pos[0]>= 0) && (pos[0]<=1) && (pos[1]>= 0)&& (pos[1]<=1) && (pos[2]>= 0)&& (pos[2]<=1));
}

void getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;

	int l = int(lvl_tree);
	float step = pow(2, l);

	float nb_probe = 0;
	float size_obj = u_size_tex;
	for(float i=0; i<=r; i+=step)
	for(float j=0; j<=r; j+=step)
	for(float k=0; k<=r; k+=step)
	{
		vec3 probe = vec3(i + step/2.0, j + step/2.0, k + step/2.0);
		if ((length(probe) <= r))
		{
			probe /= size_obj;

			vec3 p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);

			probe.x *= -1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);
			probe.x *= -1;

			probe.y *= -1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);

			probe.x *=-1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);
			probe.y *= -1;
			probe.x *= -1;

			probe.z *= -1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);

			probe.x *= -1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);
			probe.x *= -1;

			probe.y *= -1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);

			probe.x *=-1;
			p = vertex_position + probe;
			fetch(p, step, l, volume, xyz, xyz2, xy_yz_xz);
			probe.y *= -1;
			probe.x *= -1;
		}
	}
}
