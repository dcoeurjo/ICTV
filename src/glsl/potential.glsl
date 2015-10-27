		uniform float u_time;
 		uniform float u_size_tex;
 		uniform sampler3D densities;
 		
		float getPotential(vec3 position, float t, int lvl) {
		
		float ret = 0;
		float size = 20.0;
		vec3 p = (position-0.5)*size;
		float x = p.x; float y = p.y; float z = p.z;
		float h = sin(0.5*x);
				if (y < h) ret = 1;
		return ret;

 		}
