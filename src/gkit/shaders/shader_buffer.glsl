#version 430 

#ifdef VERTEX_SHADER

struct Astruct
{
    vec4 a;
    vec4 b;
};

layout(std140) buffer ABuffer
{
    Astruct array[];
};

layout(std140) buffer BBuffer
{
    Astruct array2[];
};

void main( )
{
    gl_Position= vec4(array[gl_VertexID].a + array[gl_VertexID].b + array2[gl_VertexID].a + array2[gl_VertexID].b);
}
#endif


#ifdef FRAGMENT_SHADER

vec4 fragment_color;

void main( )
{
    fragment_color= vec4(1);
}

#endif
