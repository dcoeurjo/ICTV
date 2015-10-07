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
	
	float lvl = log2(r)+1;
	volume = textureLod(densities, vertex_position, lvl).r * (r*r*r);
	
	//float vol_boule = ((4*3.14159*(0.5*0.5*0.5))/3.0);
	
	xyz2 = textureLod(u_xyz2_tex, vertex_position, lvl).rgb;
	xy_yz_xz = textureLod(u_xy_yz_xz_tex, vertex_position, lvl).rgb;
	xyz = textureLod(u_xyz_tex, vertex_position, lvl).rgb;
}