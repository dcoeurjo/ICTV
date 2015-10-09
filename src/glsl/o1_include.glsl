uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

void getVolumeMoments(vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2)
{
	float r = u_curv_radius;
	float vol_boule = ((4*3.14159*(r*r*r))/3.0);
	float vol_cube = (2*r)*(2*r)*(2*r);
	volume = 0.0;
	
	float lvl = log2(2*r);
	volume = textureLod(densities, vertex_position, lvl).r * (2.0*vol_boule/vol_cube);
	
	
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);
	
	int nb_probe = 0;
	float size_obj = u_size_tex;
	for(float i=-r; i<r; i++)
	for(float j=-r; j<r; j++)
	for(float k=-r; k<r; k++)
	{
		vec3 probe = vec3(i+0.5, j+0.5, k+0.5);
		if (length(probe) <= r)
		{
			//volume += textureLod(densities, vertex_position + (probe/size_obj), 0).r;
			xyz += textureLod(u_xyz_tex, vertex_position + (probe/size_obj), 0).rgb;
			xyz2 += textureLod(u_xyz2_tex, vertex_position + (probe/size_obj), 0).rgb;
			xy_yz_xz += textureLod(u_xy_yz_xz_tex, vertex_position + (probe/size_obj), 0).rgb;
			nb_probe ++;
		}
	}
}