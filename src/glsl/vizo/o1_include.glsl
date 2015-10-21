uniform float u_curv_radius;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform sampler1D u_spheresubdiv;
uniform int u_curv_val;

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

	float r = u_curv_radius;
	
	int l = int(lvl_tree);
	int step = int(pow(2, l));

	/*
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
	*/
	
	int nb_probe = 0;
	vec3 c = vertex_position*u_size_tex;
	for(int m=int(ceil(c.x-r)); m<=int(floor(c.x+r)); m +=step)
	for(int n=int(ceil(c.y-r)); n<=int(floor(c.y+r)); n +=step)
	for(int o=int(ceil(c.z-r)); o<=int(floor(c.z+r)); o +=step)
	{
		vec3 pixel_p = vec3( float(m), float(n), float(o) );		
		if (length(pixel_p - c) <= r)
		{
			fetch(pixel_p/u_size_tex, float(step), l, volume, xyz, xyz2, xy_yz_xz);
			nb_probe++;
		}
	}
	
	return nb_probe;
}