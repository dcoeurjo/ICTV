uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

void getVolumeMoments(vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	/*float r = u_curv_radius;
	float lvl = log2(2*u_curv_radius);
	
	float vol_boule = ((4*3.14159*(r*r*r))/3.0);
	float vol_cube = (2*r)*(2*r)*(2*r);
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);
	
	float l = pow(2, lvl-1)/2.0;
	float diag = sqrt((l*l)+(l*l));
	vec3 dpl = vec3(0);

	float size_obj = u_size_tex;
	/*for(int i=0; i<8; i++)
	{
		vec3 dpl = vec3(diag);
		if (i%2 == 0)
			dpl[0] *= -1;
		if (i%4>=2)
			dpl[1] *= -1;
		if (i>=4)
			dpl[2] *= -1;*/
		/*volume = textureLod(densities, vertex_position+(dpl/size_obj), lvl).r * (2.0*r*2.0*r*2.0*r);
		xyz = textureLod(u_xyz_tex, vertex_position+(dpl/size_obj), lvl).rgb;
		xyz2 = textureLod(u_xyz2_tex, vertex_position+(dpl/size_obj), lvl).rgb;
		xy_yz_xz = textureLod(u_xy_yz_xz_tex, vertex_position+(dpl/size_obj), lvl).rgb;*/
	/*}*/
	
	//volume /= 8.0;
	//volume *= vol_cube;
	
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;
	
	int l = int(lvl_tree);
	float step = pow(2, l);
	
	float nb_probe = 0;
	float size_obj = u_size_tex;
	for(float i=0; i<r; i+=step)
	for(float j=0; j<r; j+=step)
	for(float k=0; k<r; k+=step)
	{
		vec3 probe = vec3(i + step/2.0, j + step/2.0, k + step/2.0);
		if ((length(probe) < r))
		{
			probe /= size_obj;
			
			vec3 p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			
			probe.x *= -1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			probe.x *= -1;
			
			probe.y *= -1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			
			probe.x *=-1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			probe.y *= -1;
			probe.x *= -1;
			
			probe.z *= -1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			
			probe.x *= -1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			probe.x *= -1;
			
			probe.y *= -1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			
			probe.x *=-1;
			p = vertex_position + probe;
			volume += textureLod(densities, p, l).r * (step*step*step);
			xyz += textureLod(u_xyz_tex, p, l).rgb;
			xyz2 += textureLod(u_xyz2_tex, p, l).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, l).rgb;
			probe.y *= -1;
			probe.x *= -1;
		}
	}
}