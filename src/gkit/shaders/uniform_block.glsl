#version 330

#ifdef VERTEX_SHADER

uniform vec4 p;
layout(std140) uniform Ablock
{
    vec4 a;
    vec4 b;
};

uniform vec4 q;

void main( )
{
    gl_Position= vec4(a+b+p+q);
}
#endif


#ifdef FRAGMENT_SHADER

vec4 fragment_color;

void main( )
{
    fragment_color= vec4(1);
}

#endif
