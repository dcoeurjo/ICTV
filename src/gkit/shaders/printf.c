#version 430

#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    layout (location= 0) in vec3 position;
    out vec3 vertex_position;

    void main( )
    {
        gl_Position= mvpMatrix * vec4(position, 1.0);
        vertex_position= position;
        
        WATCH1(position.x);
        ASSERT1(false);
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform vec4 color;
    in vec3 vertex_position;
    out vec4 fragment_color;
    
    void main( )
    {
        vec3 t= normalize(dFdx(vertex_position));
        V3WATCH2(t);
        
        vec3 b= normalize(dFdy(vertex_position));
        V3WATCH3(b);
        
        vec3 n= normalize(cross(t, b));
        V3WATCH4(n);
        
        fragment_color.rgb= color.rgb * abs(n.z);
        V3WATCH5(fragment_color.rgb);
        
        ASSERT1(length(dFdx(vertex_position)) > 1.0);
    }
#endif

