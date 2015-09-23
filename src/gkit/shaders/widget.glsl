    #version 330
    
#ifdef VERTEX_SHADER
    uniform mat4 projection;
    
    layout(location= 0) in vec2 position;
    layout(location= 1) in vec3 texcoord;
    
    out vec3 vertex_texcoord;
    
    void main( )
    {
        vertex_texcoord= texcoord;
        gl_Position= projection * vec4(position, 0.0, 1.0);
    }
#endif
    
#ifdef FRAGMENT_SHADER
    uniform vec4 fillColor;
    uniform vec4 borderColor;
    uniform vec2 zones;
    
    in vec3 vertex_texcoord;
    out vec4 fragment_color;
    
    void main()
    {
        float doTurn = float(vertex_texcoord.y > 0);
        float radiusOffset = doTurn * abs( vertex_texcoord.z );
        float turnDir = sign( vertex_texcoord.z );
        vec2 uv = vec2(vertex_texcoord.x + turnDir*radiusOffset, vertex_texcoord.y);
        float l = abs( length(uv) - radiusOffset );
        float a = clamp( l - zones.x, 0.0, 2.0);
        float b = clamp( l - zones.y, 0.0, 2.0);
        b = exp2(-2.0*b*b);
        fragment_color= ( fillColor * b + (1.0-b)*borderColor );
        fragment_color.a *= exp2(-2.0*a*a);
    }
#endif
