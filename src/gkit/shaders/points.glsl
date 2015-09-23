
#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;
uniform float scale;
uniform int mode;

layout(location= 0) in vec3 position;
layout(location= 1) in float overlap;
layout(location= 2) in float nodes;
layout(location= 3) in float leafs;

flat out float vertex_count;

void main( )
{
    switch(mode)
    {
        case 0: vertex_count= overlap / scale; break;
        case 1: vertex_count= nodes / scale; break;
        case 2: vertex_count= leafs / scale; break;
    }
    
    gl_Position= mvpMatrix * vec4(position, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
flat in float vertex_count;
out vec4 fragment_color;

void main( )
{
    vec3 colors[10]= vec3[10](
        vec3(0,0,0), 
        vec3(12,17,115),
        vec3(28,121,255),
        vec3(31,255,255),
        vec3(130,255,17),
        vec3(255,255,14),
        vec3(255,112,22),
        vec3(251,0,20),
        vec3(113,1,14),
        vec3(113,1,14)
    );
    
    vec3 color;
    if(vertex_count < 10)
        color= colors[int(vertex_count)] / vec3(255);
    else
        color= colors[9] / vec3(255);
    
    fragment_color= vec4(color, 1.0);
}
#endif
