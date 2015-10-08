#version 430

#ifdef VERTEX_SHADER

#define TRANSFORMS_BINDING 0

layout (std140, binding = TRANSFORMS_BINDING)
uniform Transforms {
	mat4 modelview;
	mat4 projection;
	mat4 modelviewprojection;
	mat4 invmodelview;
} u_transforms;
uniform vec3 u_scene_size;

in vec3 position;

void main( )
{
    gl_Position = u_transforms.modelviewprojection * vec4((position-0.5)*u_scene_size, 1 );
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 fragment_color;

void main( )
{
	fragment_color = vec4(1, 0.5, 0.5, 1);
}

#endif
