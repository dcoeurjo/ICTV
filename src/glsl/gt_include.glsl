uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

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
	
	float nb_probe = 0;
	float size_obj = u_size_tex;
	for(float i=-r; i<r; i+=1)
	for(float j=-r; j<r; j+=1)
	for(float k=-r; k<r; k+=1)
	{
		vec3 probe = vec3(i+0.5, j+0.5, k+0.5);
		if ((length(probe) < r))
		{
			vec3 p = vertex_position*size_obj + probe;
			float val = textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			volume += val;
			xyz+=p.xyz*val;
			xyz2+=vec3(p.x*p.x, p.y*p.y, p.z*p.z)*val;
			xy_yz_xz+=vec3(p.x*p.y, p.y*p.z, p.x*p.z)*val;
			//xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			//xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			//xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
		}
	}
	
	/*
	float nb_probe = 0;
	float size_obj = u_size_tex;
	float step = pow(2, lvl_tree);
	for(float i=0; i<r; i+=step)
	for(float j=0; j<r; j+=step)
	for(float k=0; k<r; k+=step)
	{
		vec3 probe = vec3(i+0.5*step, j+0.5*step, k+0.5*step);
		if ((length(probe) <= r))
		{
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(-(i+0.5*step), j+0.5*step, k+0.5*step);
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(i+0.5*step, -(j+0.5*step), k+0.5*step);
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(-(i+0.5*step), -(j+0.5*step), k+0.5*step);
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(i+0.5*step, j+0.5*step, -(k+0.5*step));
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(-(i+0.5*step), j+0.5*step, -(k+0.5*step));
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(i+0.5*step, -(j+0.5*step), -(k+0.5*step));
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			probe = vec3(-(i+0.5*step), -(j+0.5*step), -(k+0.5*step));
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			
			nb_probe+=8;
		}
	}
	*/
	
	//volume += 10;
}