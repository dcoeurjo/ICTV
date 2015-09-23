#version 430


#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    uniform mat4 normalMatrix;
    
    layout (location= 0) in vec3 position;
    layout (location= 2) in vec3 normal; 
    
    out vec3 vertex_normal;

    void main( )
    {
        gl_Position= mvpMatrix * vec4(position, 1.0);
        vertex_normal= mat3(normalMatrix) * normal;
    }
#endif

#ifdef GEOMETRY_SHADER
    layout (triangles) in;
    layout (triangle_strip, max_vertices= 3) out;

    in vec3 vertex_normal[];
    out vec3 geometry_normal;
    out vec3 geometry_tesscoord;
    
    void main( )
    {
        gl_Position= gl_in[0].gl_Position;
        geometry_normal= vertex_normal[0];
        geometry_tesscoord= vec3(1, 0, 0);
        EmitVertex();
        
        gl_Position= gl_in[1].gl_Position;
        geometry_normal= vertex_normal[1];
        geometry_tesscoord= vec3(0, 1, 0);
        EmitVertex();
        
        gl_Position= gl_in[2].gl_Position;
        geometry_normal= vertex_normal[2];
        geometry_tesscoord= vec3(0, 0, 1);
        EmitVertex();
    }
#endif
    
#ifdef FRAGMENT_SHADER
    #extension GL_ARB_shader_group_vote : enable
    
    uniform vec4 color;
    
    in vec3 geometry_normal;
    in vec3 geometry_tesscoord;
    
    out vec4 fragment_color;

#if 0
    // use anyInvocationARB()
    void main( )
    {
        fragment_color= color * abs(normalize(geometry_normal).z);
        //~ fragment_color.rgb= geometry_tesscoord;
        bvec3 n= lessThan(geometry_tesscoord, vec3(0));
        bvec3 p= greaterThan(geometry_tesscoord, vec3(1));
        if(anyInvocationARB(any(n) || any(p)))
        //~ if(any(n) || any(p))
            fragment_color.rgb= vec3(1, 0, 0);
    }
#endif

    layout(r32ui, binding= 0) uniform coherent uimage2D lock;
    layout(r32i, binding= 1) uniform coherent iimage2D overdraw;
    
#if 1
    // use dFdx / dFdy
    void main( )
    {
        ivec2 quad= ivec2(gl_FragCoord.xy) / 2;
        imageAtomicAdd(overdraw, quad * 2, 1);
        float red= abs(normalize(geometry_normal).z);
        fragment_color= color * red;
        
        vec3 tesscoord_helperx= geometry_tesscoord + dFdx(geometry_tesscoord);
        bvec3 nx= lessThan(tesscoord_helperx, vec3(0));
        bvec3 px= greaterThan(tesscoord_helperx, vec3(1));
        if(any(nx) || any(px))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 0), 1);
        }
        
        vec3 tesscoord_helperxx= geometry_tesscoord - dFdx(geometry_tesscoord);
        bvec3 nxx= lessThan(tesscoord_helperxx, vec3(0));
        bvec3 pxx= greaterThan(tesscoord_helperxx, vec3(1));
        if(any(nxx) || any(pxx))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 0), 1);
        }
        
        vec3 tesscoord_helpery= geometry_tesscoord + dFdy(geometry_tesscoord);
        bvec3 ny= lessThan(tesscoord_helpery, vec3(0));
        bvec3 py= greaterThan(tesscoord_helpery, vec3(1));
        if(any(ny) || any(py))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2+ ivec2(0, 1), 1);
        }
        
        vec3 tesscoord_helperyy= geometry_tesscoord - dFdy(geometry_tesscoord);
        bvec3 nyy= lessThan(tesscoord_helperyy, vec3(0));
        bvec3 pyy= greaterThan(tesscoord_helperyy, vec3(1));
        if(any(nyy) || any(pyy))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2+ ivec2(0, 1), 1);
        }
        
        #if 0
        vec3 tesscoord_helperw= tesscoord_helpery + dFdx(tesscoord_helpery);
        bvec3 nw= lessThan(tesscoord_helperw, vec3(0));
        bvec3 pw= greaterThan(tesscoord_helperw, vec3(1));
        if(any(nw) || any(pw))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 1), 1);
        }
        
        vec3 tesscoord_helperww= tesscoord_helpery - dFdx(tesscoord_helpery);
        bvec3 nww= lessThan(tesscoord_helperww, vec3(0));
        bvec3 pww= greaterThan(tesscoord_helperww, vec3(1));
        if(any(nww) || any(pww))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 1), 1);
        }
        
        vec3 tesscoord_helperwww= tesscoord_helperyy + dFdx(tesscoord_helperyy);
        bvec3 nwww= lessThan(tesscoord_helperwww, vec3(0));
        bvec3 pwww= greaterThan(tesscoord_helperwww, vec3(1));
        if(any(nwww) || any(pwww))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 1), 1);
        }
        
        vec3 tesscoord_helperwwww= tesscoord_helperyy - dFdx(tesscoord_helperyy);
        bvec3 nwwww= lessThan(tesscoord_helperwwww, vec3(0));
        bvec3 pwwww= greaterThan(tesscoord_helperwwww, vec3(1));
        if(any(nwwww) || any(pwwww))
        {
            fragment_color.rgb= vec3(red, 0, 0);
            imageAtomicAdd(overdraw, quad * 2 + ivec2(1, 1), 1);
        }
        #endif
    }
    #endif
    
#if 0
    // use atomic lock
    void main( )
    {
        ivec2 quad= ivec2(gl_FragCoord.xy) / 2;
        float red= abs(normalize(geometry_normal).z);
        fragment_color= color * red;
        
        //~ for(int i= 0; i < 16; i++)
        //~ bool done= false;
        //~ while(!done)
        {
            uint primitiveID= imageAtomicCompSwap(lock, quad, 0u, uint(gl_PrimitiveID));
            if(uint(primitiveID) == 0u)
            {
                // lock
                //~ imageAtomicAdd(overdraw, quad, 4);        // 4 fragments par quad
                fragment_color= vec4(1, 1, 0, 1);
                //~ imageAtomicExchange(lock, quad, 0u);
                //~ done= true;
            }
            else if(primitiveID == uint(gl_PrimitiveID))
            {
                //~ imageAtomicAdd(overdraw, quad, -1);  // 1 fragment actif sur le quad
                fragment_color= vec4(0, 0, 1, 1);
                //~ done= true;
            }
        }
    }
#endif


#endif

