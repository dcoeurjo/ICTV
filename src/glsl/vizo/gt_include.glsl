uniform float u_curv_radius;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform sampler1D u_spheresubdiv;
uniform int u_curv_val;

bool isincube(in vec3 pos)
{
	return ((pos[0]>= 0) && (pos[0]<=1) && (pos[1]>= 0)&& (pos[1]<=1) && (pos[2]>= 0)&& (pos[2]<=1));
}

int getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;
	
	int l = 0;
	float step = pow(2, l);
	
	int nb_probe = 0;
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
		
		nb_probe += 8;
	}
	
	return nb_probe;
}