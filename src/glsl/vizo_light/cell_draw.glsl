#version 430

// ---------------------------------------------------------
// Vertex shader

#ifdef VERTEX_SHADER
layout (location = 0) in uvec2 i_key; // translation + scale
layout (location = 1) in vec3 position; //cube vertex
layout (location = 0) out float dist_to_camera;
layout (location = 1) out vec3 color;

#define COLOR_TRANSITION 0
#define COLOR_LEVEL 1
#define COLOR_DENSITY 0
#define NO_COLOR 0

vec3 getSideTransitions(vec3 min_c, vec3 parent, float s, float level)
{
	vec3 max_c = min_c + s;
	vec3 node = ((min_c + max_c) * 0.5).xyz;

	vec3 neighbours[6];
	neighbours[0] = node + s*vec3(0, 0, 1);
	neighbours[1] = node + s*vec3(0, 0, -1);
	neighbours[2] = node + s*vec3(0, 1, 0);
	neighbours[3] = node + s*vec3(0, -1, 0);
	neighbours[4] = node + s*vec3(1, 0, 0);
	neighbours[5] = node + s*vec3(-1, 0, 0);

	vec3 ret = vec3(0, 0, 0);
	for(int i=0; i<6; i++)
	{
		vec4 n = vec4(neighbours[i], 1);
		n -= 0.5;
		n.x *= u_scene_size.x;
		n.y *= u_scene_size.y;
		n.z *= u_scene_size.z;
		n.w = 1;
		
		vec4 np = vec4( parent + s - 0.5, 1);	
		np.x *= u_scene_size.x;
		np.y *= u_scene_size.y;
		np.z*= u_scene_size.z;
		np.w = 1;
		
		vec3 mv = (u_transforms.modelview * np).xyz;
		float d = getLength(mv) * u_tan_fovy;
		if ( d < 2*u_scale*s*length(u_scene_size)) //if smaller neighbour
		{
			/*float val = textureLod(densities, node, clamp(max_tex - level, 0, max_tex)).r;
			if (val == 0 || val == 1)
				ret += ((neighbours[i] - node) * 1.0/s);*/
			ret += ((neighbours[i] - node) * 1.0/s);
		}
		else
		{
			//ret += ((neighbours[i] - node) * 1.0/s);
		}
	}

	return ret;
}


void main (void) {

	// extract octree position and scale
	vec3 c, pc; //00 corner
	float s;
	lt_cell_3_20 (i_key.xy, c, s, pc);
	
	//s *= 0.5;
	//c = c+vec3(s);
	
	float level = lt_level_3_20(i_key.xy);
	
#if COLOR_DENSITY
	//if (c.z != 0)
	//	s = 0;

	float i;
	float d = getVolume(c + (position+0.5)*s);//textureLod(densities, c + vec3(0.5*s), max_tex - 5).r;

	if (d > 0)
		color = vec3(0, 1, 0);
	else if (d == 0)
		color = vec3(0, 0, 1);
	else
		color = vec3(1, 0, 0);
	
	//s*=0.5;
	
	/*
	if ( d == 0 || d == 1)
		color = vec3(0);
	else
		color = vec3(1, 0, 0)*d + vec3(0, 1, 0) * (1-d);
	*/
#endif

	// set up varyings
	vec3 min_corner = (c - 0.5) * u_scene_size;
	vec3 vert = min_corner + (position+0.5)* s *u_scene_size;
	dist_to_camera = max(length(u_transforms.modelview * vec4(vert, 1)) - 130, 0);
    
	gl_Position = u_transforms.modelviewprojection  * vec4(vert, 1);
	
#if COLOR_TRANSITION
	color = vec3(0);
	for(int i=1; i<=4; i*=2)
	{
		color = abs( getSideTransitions(c, pc, s, level-1) );
	}
#endif
	
	
#if COLOR_LEVEL
	/*vec3 color_levels[12] = vec3[12] (
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (1, 0, 0),
		vec3 (0.8, 0, 0.2),
		vec3 (0.6, 0, 0.4),
		vec3 (0.4, 0, 0.6),
		vec3 (0.2, 0, 0.8),
		vec3 (0, 0, 1),
		vec3 (0, 0, 1),
		vec3 (0, 0, 1)
	);
	color = color_levels[12 - uint(level)];*/
	
	/*vec3 color_levels[6] = vec3[6] (
		vec3 (1, 0, 0),
		vec3 (0.8, 0, 0.2),
		vec3 (0.6, 0, 0.4),
		vec3 (0.4, 0, 0.6),
		vec3 (0.2, 0, 0.8),
		vec3 (0, 0, 1)
	);*/
	
	vec3 color_levels[6] = vec3[6] (
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1),
		vec3 (1, 0, 0),
		vec3 (0, 1, 0),
		vec3 (0, 0, 1)
	);
	color = color_levels[uint(level)%6];
#endif

#if NO_COLOR
	color = vec3(0);
#endif
	
}
#endif

// ---------------------------------------------------------
// Fragment shader

#ifdef FRAGMENT_SHADER
layout(location = 0) in float dist;
layout(location = 1) in vec3 color;
layout(location = 0) out vec4 o_colour;

void main (void) {
	o_colour = vec4(color, 1);
}
#endif // FRAGMENT_SHADER

