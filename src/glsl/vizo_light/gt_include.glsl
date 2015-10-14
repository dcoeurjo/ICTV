uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

void fetch(in vec3 p, inout float volume, inout vec3 xyz, inout vec3 xyz2, inout vec3 xy_yz_xz)
{
	float val = textureLod(densities, p, 0).r;
	volume += val;
	xyz += textureLod(u_xyz_tex, p, 0).rgb * val;
	xyz2 += textureLod(u_xyz2_tex, p, 0).rgb * val;
	xy_yz_xz += textureLod(u_xy_yz_xz_tex, p, 0).rgb * val;
}

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
	for(float i=0; i<r; i+=1)
	for(float j=0; j<r; j+=1)
	for(float k=0; k<r; k+=1)
	{
		vec3 probe = vec3(i + 0.5, j + 0.5, k + 0.5);
		if ((length(probe) < r))
		{
			probe /= size_obj;
			
			vec3 p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			
			probe.x *= -1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			probe.x *= -1;
			
			probe.y *= -1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			
			probe.x *=-1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			probe.y *= -1;
			probe.x *= -1;
			
			probe.z *= -1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			
			probe.x *= -1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			probe.x *= -1;
			
			probe.y *= -1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			
			probe.x *=-1;
			p = vertex_position + probe;
			fetch(p, volume, xyz, xyz2, xy_yz_xz);
			probe.y *= -1;
			probe.x *= -1;
		}
	}
}