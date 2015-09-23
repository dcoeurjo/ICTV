
#version 330

#ifdef VERTEX_SHADER

out vec3 position;

void main( )
{
    vec3 quad[4]= vec3[4]( vec3(-1, -1, 0), vec3( 1, -1, 0), 
                           vec3(-1,  1, 0), vec3( 1,  1, 0) );
    
    position= quad[gl_VertexID];
    gl_Position= vec4(quad[gl_VertexID], 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

float sphere( const vec3 p, const float radius )
{
    return length(p) - radius;
}

float displace( const vec3 p )
{
    return sin(35 * p.x) * sin(35 * p.y) * sin(30 * p.z) * 0.04;
}

float object( const vec3 p )
{
    float d1= sphere(p, 0.75);
    float d2= displace(p);
    //~ float d2= 0;
    
    return d1+d2;
}

in vec3 position;

out vec4 fragment_color;

void main( )
{
    vec3 o= vec3(position.xy, -1.0);
    vec3 d= vec3(0, 0, 2);
    
    for(int i= 0; i < 64; i++)
    {
        float t= i / 64.0;
        vec3 p= o + t * d;
        
        float distance= object(p);
        if(distance < 0.0)
        {
            fragment_color= vec4(t *2.5, t * 1.5, t , 1);
            return;
        }
    }
    
    fragment_color= vec4(0, 0, 0, 1);
}

#endif
