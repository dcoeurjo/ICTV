#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;

in vec3 position;

void main( )
{
    //~ gl_Position= mvpMatrix * vec4(position, 1.0);
    gl_Position= vec4(position, 1.0);
}
#endif

#ifdef GEOMETRY_SHADER
layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices= 3) out;

uniform mat4 mvpMatrix;

out vec4 gl_Position;

void main( )
{
    for(int i= 0; i < gl_in.length(); i++)
    {
        //~ gl_Position= mvpMatrix * gl_in[i].gl_Position;
        gl_Position= mvpMatrix * (gl_in[i].gl_Position + vec4(1, 0, 0, 0));
        EmitVertex();
    }
    EndPrimitive();
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
