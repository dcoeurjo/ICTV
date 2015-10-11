uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

void getVolumeMoments(vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2, in float lvl_tree)
{
	float lvl = log2(2*u_curv_radius);
	float r = pow(2.0, lvl);
	
	float vol_boule = ((4*3.14159*(r*r*r))/3.0);
	float vol_cube = (2*r)*(2*r)*(2*r);
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);
	
	volume = textureLod(densities, vertex_position, lvl).r * vol_cube;
	xyz = textureLod(u_xyz_tex, vertex_position, lvl).rgb;
	xyz2 = textureLod(u_xyz2_tex, vertex_position, lvl).rgb;
	xy_yz_xz = textureLod(u_xy_yz_xz_tex, vertex_position, lvl).rgb;

}