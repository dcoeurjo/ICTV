#version 330

#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    layout (location= 0) in vec3 position;
    out vec3 vertex_position;

    void main( )
    {
        gl_Position= mvpMatrix * vec4(position, 1.0);
        vertex_position= position;
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform vec4 diffuse_color;
    in vec3 vertex_position;
    out vec4 fragment_color;

    void main( )
    {
        vec3 t= normalize(dFdx(vertex_position));
        vec3 b= normalize(dFdy(vertex_position));
        vec3 n= normalize(cross(t, b));
        fragment_color.rgb= diffuse_color.rgb * abs(n.z);
        //~ fragment_color.rgb= diffuse_color.rgb;
        //~ fragment_color.rgb= abs(n.zzz);
    }
#endif

