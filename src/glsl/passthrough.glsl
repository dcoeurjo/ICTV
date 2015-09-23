#version 430

#ifdef VERTEX_SHADER

uniform mat4 mvpMatrix;

in vec3 position;
in vec3 color;

flat out vec4 vertex_color;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1.0);
    vertex_color = vec4(color, 1);
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec4 color;

flat in vec4 vertex_color;
out vec4 fragment_color;

void main( )
{
    fragment_color= vertex_color;
}
#endif
