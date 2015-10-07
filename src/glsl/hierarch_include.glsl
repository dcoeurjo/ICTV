uniform float u_curv_radius;
uniform float u_size_tex;
uniform sampler3D densities;
uniform sampler3D u_xyz2_tex;
uniform sampler3D u_xy_yz_xz_tex;
uniform sampler3D u_xyz_tex;
uniform int u_curv_val;

mat3 fromSymToMatrix(vec3 sym)
{
	vec3 m0 = vec3(0);
	m0[int(abs(sym[0])-1)] = 1;
	if (sym[0] < 0)
		m0[int(abs(sym[0])-1)] = -1;
	
	vec3 m1 = vec3(0);
	m1[int(abs(sym[1])-1)] = 1;
	if (sym[1] < 0)
		m1[int(abs(sym[1])-1)] = -1;
		
	vec3 m2 = vec3(0);
	m2[int(abs(sym[2])-1)] = 1;
	if (sym[2] < 0)
		m2[int(abs(sym[2])-1)] = -1;
	
	mat3 ret;
	ret[0] = vec3(m0.x, m1.x, m2.x);
	ret[1] = vec3(m0.y, m1.y, m2.y);
	ret[2] = vec3(m0.z, m1.z, m2.z);
	return ret;
}

void getVolumeMoments(in vec3 vertex_position, out float volume, out vec3 xyz, out vec3 xy_yz_xz, out vec3 xyz2)
{
	volume = 0.0;
	xyz2 = vec3(0);
	xy_yz_xz = vec3(0);
	xyz = vec3(0);

	float r = u_curv_radius;
	float vol_boule = ((4*3.14159*(r*r*r))/3.0);

	vec3 symetries[24];
	int id = 0;
	for(int i=1; i<=3; i++)
	{
		for(int j=1; j<=3; j++)
		{
			if(j==i)
				continue;
			for(int k=1; k<=3; k++)
			{
				if (k==i || k== j || j > k)
					continue;
				
				for(int s0=1; s0>=-1; s0-=2)
				for(int s1=1; s1>=-1; s1-=2)
				for(int s2=1; s2>=-1; s2-=2)
					symetries[id++]=vec3(s0*i, s1*j, s2*k);
			}
		}
	}
	
	vec3 orig = vertex_position*u_size_tex;

	float k = log2((2.0*r/sqrt(3)));
	float size = pow(2.0, k);
	
	float total_volume = 0.0;
	
	volume += textureLod(densities, orig/u_size_tex, k).r * pow(size, 3);
	xyz += textureLod(u_xyz_tex, orig/u_size_tex, k).r;
	xyz2 += textureLod(u_xyz2_tex, orig/u_size_tex, k).r;
	xy_yz_xz += textureLod(u_xy_yz_xz_tex, orig/u_size_tex, k).r;

	total_volume += pow(size, 3);

	vec3 center = orig + vec3(size/2.0, 0, 0);
	while(k>=1)
	{
		size/=2.0;
		
		vec3 right_corner = center+vec3(size);
		
		vec3 current_voxel = right_corner;
		bool stay = true;
		while(true)
		{
			if (length(current_voxel - orig) > r)
				break;
			
			while(true)
			{
				if (length(current_voxel - orig) > r)
					break;
				
				for(int i=0; i<24; i++)
				{
					stay = false;
					vec3 pos = orig + fromSymToMatrix(symetries[i])*(current_voxel-vec3(size/2.0));
					volume += textureLod(densities, pos/u_size_tex, k).r * pow(size, 3);
					xyz += textureLod(u_xyz_tex, pos/u_size_tex, k).r;
					xyz2 += textureLod(u_xyz2_tex, pos/u_size_tex, k).r;
					xy_yz_xz += textureLod(u_xy_yz_xz_tex, pos/u_size_tex, k).r;
					total_volume += pow(size, 3);
				}
				current_voxel+= vec3(0, size, 0);
			}
			current_voxel.y = right_corner.y;
			current_voxel += vec3(0, 0, size);
		}
		
		if(!stay)
			center += vec3(size, 0, 0);
		
		k--;
	}
	
	volume *= vol_boule/total_volume;
}