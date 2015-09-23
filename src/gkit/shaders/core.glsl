#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;

in vec3 position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec4 color;

out vec4 fragment_color;

void main( )
{
    fragment_color= color;
}
#endif
