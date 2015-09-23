
#version 330

#ifdef VERTEX_SHADER
out vec2 texcoord;

void main( )
{
    vec2 quad[4]= vec2[4]( vec2(-1, -1), vec2( 1, -1), vec2(-1,  1), vec2( 1,  1) );
    
    texcoord= quad[gl_VertexID] * 0.5 + 0.5;
    gl_Position= vec4(quad[gl_VertexID], 0, 1);
}
#endif

#ifdef FRAGMENT_SHADER
uniform sampler2D data;
in vec2 texcoord;

out vec4 fragment_color;

void main( )
{
    vec4 color= texture(data, texcoord);
    fragment_color= color;
}

#endif
