
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
    return sin(45 * p.x) * sin(45 * p.y) * sin(40 * p.z) * 0.04;
}

float object( const vec3 p )
{
    float d1= sphere(p, 0.75);
    float d2= displace(p);
    //~ float d2= 0;
    
    return d1+d2;
}

uniform mat4 mvpvInvMatrix;
uniform mat4 mvMatrix;

in vec3 position;

out vec4 fragment_color;

#define N 128

void main( )
{
    vec4 oh= mvpvInvMatrix * vec4(gl_FragCoord.xy, 0, 1);       // origine sur near
    vec4 eh= mvpvInvMatrix * vec4(gl_FragCoord.xy, 1, 1);       // extremite sur far
    
    vec3 o= oh.xyz / oh.w;                      // origine
    vec3 d= eh.xyz / eh.w - oh.xyz / oh.w;      // direction
    d= normalize(d);
    
    vec3 p= o;
    float t= 0;
    float distance= 0;
    for(int i= 0; i < N; i++)
    {
        p= o + t * d;
        
        distance= object(p);
        //~ if(distance < 0.0)
            //~ break;
        
        t= t + distance / 4.0;
    }
    
    vec4 q= mvMatrix * vec4(p, 1);
    q/= q.w;
    fragment_color.rgb= abs(cross(normalize(dFdx(q.xyz)), normalize(dFdy(q.xyz))));
    //~ fragment_color= vec4(0, 0, 0, 1);
    
}

#endif
