#version 430

#ifdef VERTEX_SHADER

in vec4 position;
out vec4 vertex_position;

uniform float u_curv_radius;

void main( )
{
	vec3 corner_pos = (position.xyz + 0.5);
	vec3 scales = (u_scene_size/u_size_tex)*u_curv_radius;
	corner_pos *= scales;
	corner_pos.x += u_scene_size.x*0.5;

	/*
	mat4 scaleMat;
	scaleMat[0] = vec4(scales.x, 0, 0, 0);
	scaleMat[1] = vec4(0, scales.y, 0, 0);
	scaleMat[2] = vec4(0, 0, scales.z, 0);
	scaleMat[3] = vec4(0, 0, 0, 1);
	*/

    vertex_position = u_transforms.modelviewprojection * vec4(corner_pos.xyz, 1);

    gl_Position = vertex_position;
}
#endif

#ifdef FRAGMENT_SHADER

in vec4 vertex_position;

out vec4 fragment_color;

void main( )
{
	fragment_color = vec4(0, 0, 1, 1);
}

#endif
