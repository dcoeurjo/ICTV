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

void getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;
	
	float nb_probe = 0;
	float size_obj = u_size_tex;
	for(float i=-r; i<r; i++)
	for(float j=-r; j<r; j++)
	for(float k=-r; k<r; k++)
	{
		vec3 probe = vec3(i+0.5, j+0.5, k+0.5);
		if ((length(probe) <= r))
		{
			volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			nb_probe++;
		}
	}
	
	volume += 10;
}