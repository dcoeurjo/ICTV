uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;
uniform float u_time;

float getPotential(vec3 position)
{
	//float ret = textureLod(densities, position, 0).r;
	
	
	float size = 128;
	
	float r = sin(0.05*u_time)*10 + 20;
	vec3 c = vec3(0.5);
	
	float ret = 0;
	if( length((position-c)*size) < r )
		ret = 1;
		
	
	return ret;
}



void fetch(vec3 p, float step, float l, inout float volume, inout vec3 xyz, inout vec3 xyz2, inout vec3 xy_yz_xz)
{
	float val = getPotential(p);//textureLod(densities, p, l).r;
	vec3 p2 = p*u_size_tex;
	val *= (step*step*step);
	
	volume += val;
	
	/*xyz += textureLod(u_xyz_tex, p, l).rgb;
	xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
	xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
	*/
	xyz += p2 * val;
	xyz2 += p2*p2 * val;
	xy_yz_xz += vec3(p2.x*p2.y, p2.y*p2.z, p2.x*p2.z) * val;
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