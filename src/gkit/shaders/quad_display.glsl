#version 430

#ifdef VERTEX_SHADER
    void main( )
    {
        vec3 quad[4]= vec3[4]( 
            vec3(-1, -1, -1), vec3( 1, -1, -1), 
            vec3(-1,  1, -1), vec3( 1,  1, -1) );
        
        gl_Position= vec4(quad[gl_VertexID], 1.0);
    }
#endif


#ifdef FRAGMENT_SHADER
    layout(binding= 1, r32i) uniform iimage2D overdraw;
    uniform vec4 color;

    out vec4 fragment_color;
    
    void main( )
    {
        ivec2 quad= ivec2(gl_FragCoord.xy) / 2;
        int count= imageLoad(overdraw, quad);
        //~ uint count= imageLoad(overdraw, quad * 2);
        //~ uint count= 0;
        //~ count+= imageLoad(overdraw, quad * 2 + ivec2(1, 0));
        //~ count+= imageLoad(overdraw, quad * 2 + ivec2(0, 1));
        //~ count+= imageLoad(overdraw, quad * 2 + ivec2(1, 1));
        
        //~ count /= 4;
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
            vec3(255,0,0)
        );
        
        vec3 color;
        if(count < 10)
            color= colors[count] / vec3(255);
        else
            color= colors[9] / vec3(255);
        
        //~ fragment_color= vec4(count * color.rgb, 1.0);// * color.rgb;
        fragment_color.rgb= color.rgb;
    }
#endif
    
