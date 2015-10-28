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
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform sampler1D u_spheresubdiv;
uniform int u_curv_val;

void goUp( inout ivec4 xyzk )
{
  xyzk[ 0 ] >>= 1;
  xyzk[ 1 ] >>= 1;
  xyzk[ 2 ] >>= 1;
  xyzk[ 3 ]  += 1;
}
void goDown( inout ivec4 xyzk )
{
  xyzk[ 0 ] <<= 1;
  xyzk[ 1 ] <<= 1;
  xyzk[ 2 ] <<= 1;
  xyzk[ 3 ]  -= 1;
}

void goNext(inout ivec4 xyzk, int max_lvl)
{
	while ( ( (xyzk[ 0 ] & 0x1)>0 ) && ( (xyzk[ 1 ] & 0x1)>0 ) && ( (xyzk[ 2 ] & 0x1)>0 ) )
		goUp( xyzk );
		
	if ( xyzk[ 3 ] == max_lvl ) { xyzk[ 3 ] = max_lvl+1; return; }
	
	if ( ( (xyzk[ 0 ] & 0x1) ) == 0 )  
		xyzk[ 0 ] += 1; 
	else 
	{
		xyzk[ 0 ] &= ~0x1;
		if ( ( (xyzk[ 1 ] & 0x1) ) == 0 )
			xyzk[ 1 ] += 1;
		else 
		{
			xyzk[ 1 ] &= ~0x1;
			xyzk[ 2 ] += 1;
		}
	}
}

void analytic_hierarch(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	int limit_lvl = int(lvl_tree);
	
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);
	
	float r = int(u_curv_radius);
	int max_lvl = int(log2(u_size_tex));//int( ceil(log2(2*r)) );
	
	vec3 ball_center = vertex_position*u_size_tex;
	
	vec3 first_point = ball_center - r;
	ivec4 xyzk = ivec4(0,0,0, max_lvl);

	do
    {
		ivec3 center_coord = 2*(xyzk.xyz) + 1;
		vec3 point_coord = center_coord << xyzk.w;
		point_coord /= 2.0;
		
		float d = length(point_coord - ball_center);
		float upper = -1.0;
		float delta = sqrt(3.0)/2.0;
		delta *= pow(2, xyzk.w);
		if( r >= delta )
			upper = r - delta;
		float lower = r + delta;
		
		if ( xyzk.w <= limit_lvl ) 
		{
			if (d <= u_curv_radius)
			{ // cell is completely inside
				float side = pow(2,xyzk.w);
				float val = textureLod(densities, point_coord/u_size_tex, xyzk.w).r * side*side*side;
				volume += val;
				xyz += textureLod(u_xyz_tex, point_coord/u_size_tex, xyzk.w).rgb;
				xyz2 += textureLod(u_xyz2_tex, point_coord/u_size_tex, xyzk.w).rgb;
				xy_yz_xz += textureLod(u_xy_yz_xz_tex, point_coord/u_size_tex, xyzk.w).rgb;
			}
			goNext( xyzk, max_lvl );
		}
		else
		{ // cell is completely inside
		    if ( d <= upper ) 
			{
				float side = pow(2,xyzk.w);
				float val = textureLod(densities, point_coord/u_size_tex, xyzk.w).r * side*side*side;
				volume += val;
				xyz += textureLod(u_xyz_tex, point_coord/u_size_tex, xyzk.w).rgb;
				xyz2 += textureLod(u_xyz2_tex, point_coord/u_size_tex, xyzk.w).rgb;
				xy_yz_xz += textureLod(u_xy_yz_xz_tex, point_coord/u_size_tex, xyzk.w).rgb;
				goNext( xyzk, max_lvl );
			}
			else if ( d > lower ) // cell is completely outside
				goNext( xyzk, max_lvl );
			else 
				goDown( xyzk );
		}
    }
	while ( xyzk[ 3 ] < max_lvl );
}

void fetch(vec3 p, float step, int l, inout float volume, inout vec3 xyz, inout vec3 xyz2, inout vec3 xy_yz_xz)
{
	float val = getPotential(p, u_time);//textureLod(densities, p, l).r;
	vec3 p2 = p*u_size_tex;
	val *= (step*step*step);
	
	volume += val;
	
	/*xyz += textureLod(u_xyz_tex, p, l).rgb;
	xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
	xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;*/
	float error = (step*step)/12.0;
	
	xyz += p2 * val;
	xyz2 += (p2*p2 + error) * val;
	xy_yz_xz += vec3(p2.x*p2.y, p2.y*p2.z, p2.x*p2.z) * val;
	
}

int getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);
	
	int nb_probe = 0;
	float nbfetch = texelFetch(u_spheresubdiv, 0, 0).r;
	nb_probe++;
	for(int i=1; i<=nbfetch; i++)
	{
		vec4 xyzk = texelFetch(u_spheresubdiv, i, 0).rgba;
		nb_probe++;
		
		vec3 probe = xyzk.xyz;
		
		vec3 init_p = vertex_position + probe;
		fetch(init_p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
		
		probe.x *= -1;
		vec3 p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		probe.x *= -1;
		
		probe.y *= -1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		
		probe.x *=-1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		probe.y *= -1;
		probe.x *= -1;
		
		probe.z *= -1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		
		probe.x *= -1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		probe.x *= -1;
		
		probe.y *= -1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		
		probe.x *=-1;
		p = vertex_position + probe;
		if (p.x != init_p.x || p.y != init_p.y || p.z != init_p.z)
		{
			fetch(p, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
		probe.y *= -1;
		probe.x *= -1;
		
		//fetch(vertex_position + xyzk.xyz, int(pow(2, xyzk.w)), int(xyzk.w), volume, xyz, xyz2, xy_yz_xz);
		//nb_probe++;
	}
	//analytic_hierarch(vertex_position, volume, xyz, xy_yz_xz, xyz2, lvl_tree);
	return nb_probe;
}


/*
uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

mat3 fromSymToMatrix(vec3 sym)
{
	vec3 m0 = vec3(0);
	m0[int(abs(sym[0])-1)] = 1;
	if (sym[0] < 0)
		m0[int(abs(sym[0])-1)] = -1;
	
	vec3 m1 = vec3(0);
	m1[int(abs(sym[1])-1)] = 1;
	if (sym[1] < 0)
		m1[int(abs(sym[1])-1)] = -1;
		
	vec3 m2 = vec3(0);
	m2[int(abs(sym[2])-1)] = 1;
	if (sym[2] < 0)
		m2[int(abs(sym[2])-1)] = -1;
	
	mat3 ret;
	ret[0] = vec3(m0.x, m1.x, m2.x);
	ret[1] = vec3(m0.y, m1.y, m2.y);
	ret[2] = vec3(m0.z, m1.z, m2.z);
	return ret;
}

void getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;
	float vol_boule = ((4*3.14159*(r*r*r))/3.0);

	vec3 symetries[24];
	int id = 0;
	for(int i=1; i<=3; i++)
	{
		for(int j=1; j<=3; j++)
		{
			if(j==i)
				continue;
			for(int k=1; k<=3; k++)
			{
				if (k==i || k== j || j > k)
					continue;
				
				for(int s0=1; s0>=-1; s0-=2)
				for(int s1=1; s1>=-1; s1-=2)
				for(int s2=1; s2>=-1; s2-=2)
					symetries[id++]=vec3(s0*i, s1*j, s2*k);
			}
		}
	}
	
	vec3 orig = vertex_position*u_size_tex;

	float k = log2((2.0*r/sqrt(3)));
	float size = pow(2.0, k);
	
	float total_volume = 0.0;
	
	volume += textureLod(densities, orig/u_size_tex, k).r * pow(size, 3);
	xyz += textureLod(u_xyz_tex, orig/u_size_tex, k).r;
	xyz2 += textureLod(u_xyz2_tex, orig/u_size_tex, k).r;
	xy_yz_xz += textureLod(u_xy_yz_xz_tex, orig/u_size_tex, k).r;

	total_volume += pow(size, 3);

	vec3 center = orig + vec3(size/2.0, 0, 0);
	while(k>=1)
	{
		size/=2.0;
		
		vec3 right_corner = center+vec3(size);
		
		vec3 current_voxel = right_corner;
		bool stay = true;
		while(true)
		{
			if (length(current_voxel - orig) > r)
				break;
			
			while(true)
			{
				if (length(current_voxel - orig) > r)
					break;
				
				for(int i=0; i<24; i++)
				{
					stay = false;
					vec3 pos = orig + fromSymToMatrix(symetries[i])*(current_voxel-vec3(size/2.0));
					volume += textureLod(densities, pos/u_size_tex, k).r * pow(size, 3);
					xyz += textureLod(u_xyz_tex, pos/u_size_tex, k).r;
					xyz2 += textureLod(u_xyz2_tex, pos/u_size_tex, k).r;
					xy_yz_xz += textureLod(u_xy_yz_xz_tex, pos/u_size_tex, k).r;
					total_volume += pow(size, 3);
				}
				current_voxel+= vec3(0, size, 0);
			}
			current_voxel.y = right_corner.y;
			current_voxel += vec3(0, 0, size);
		}
		
		if(!stay)
			center += vec3(size, 0, 0);
		
		k--;
	}
	
	volume *= vol_boule/total_volume;
}
*/