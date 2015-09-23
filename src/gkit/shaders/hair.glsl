#version 330

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;

in vec3 position;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1.0);
}
#endif

#ifdef GEOMETRY_SHADER
// todo construire un buffer avec les bonnes valeurs plutot que d'utiliser un geometry shader
layout(lines) in;
layout(line_strip) out;
layout(max_vertices= 2) out;

uniform vec2 halfview;
uniform float znear;
uniform float zfar;

out float t;
out float z;
out vec2 a;
out vec2 b;

void main( )
{
    t= 0;
    z= 1.0 / gl_in[0].gl_Position.z;
    
    a= gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w * halfview + halfview;
    b= gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w * halfview + halfview;
    gl_Position= gl_in[0].gl_Position;
    EmitVertex();
    
    t= 1;
    z= 1.0 / gl_in[1].gl_Position.z;
    
    a= gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w * halfview + halfview;
    b= gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w * halfview + halfview;
    gl_Position= gl_in[1].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec4 color;
uniform float znear;
uniform float zfar;

in float t;
in float z;
in vec2 a;
in vec2 b;

out vec4 fragment_color;

void main( )
{
    vec2 p= (1 - t) * a + t * b;
    float d= length(p - gl_FragCoord.xy);
    float s= z / znear;
    fragment_color= vec4(color.rgb, s * 20.0);
    //~ if(d < s)
        //~ fragment_color= vec4(color.rgb, 1);
    //~ else
        //~ fragment_color= vec4(color.rgb, 2.0 / s);
}
#endif
