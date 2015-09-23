#version 430 // compute shader, core profile

#ifdef COMPUTE_SHADER

// entrees
uniform sampler2D image0;       // image a filtrer
uniform sampler2D image1;       // filtre, taille 32x32

// parametres "supplementaires"
uniform vec3 alpha= vec3(1);
uniform float window= 32;        // le support du filtre

// sorties
layout( binding= 0 ) writeonly 
uniform image2D framebuffer;


#define PI 3.1415

float square( const float x ) { return x*x; }

float g( const float x, const float mean, const float var )
{
    return exp( - square(x - mean) / (2.0*var) ) / sqrt(2.0*PI*var);
}

vec3 gaussian( const vec2 p )
{
    return vec3( g(p.x, window/2, window) * g(p.y, window/2, window) );
}

vec3 gaussianx( const float x )
{
    return vec3( g(x, window/2, window*2) );
}
vec3 gaussiany( const float y )
{
    return vec3( g(y, window/2, window*2) );
}



// configuration du nombre de threads par groupe
layout( local_size_x= 32, local_size_y= 16 ) in;

shared float weights[gl_WorkGroupSize.x];

void main( )
{
    vec3 r;
    vec3 norm;
    for(int i= 0; i < window; i++)
    {
        // calculer les coefficients du filtre
        vec3 gx= gaussianx(i);
        
        if(gl_LocalInvocationID.y == 1)
            // ceratins font le calcul
            weights[gl_LocalInvocationID.x]= gx * gaussiany(gl_LocalInvocationID.x);

        // tous attendent
        barrier();
        
        for(int j= 0; j < window; j++)
        {
            // pixel associe au thread
            ivec2 pixel= ivec2(gl_GlobalInvocationID.xy);
            
            // lire un pixel dans l'image, i,j correspondent a un "deplacement" par rapport au centre du filtre.
            vec3 f= texelFetch(image0, pixel + ivec2(i -window/2, j -window/2), 0).rgb;
            //~ vec3 f= vec3(1);
            
            // lire un 'pixel' / coefficient du filtre, i,j correspondent a un "deplacement" par rapport au centre du filtre.
            //~ vec3 c= texelFetch(image1, ivec2(16 + i -window/2, 16 + j -window/2), 0).rgb;
            float c= weights[gl_LocalInvocationID.x];
            //~ vec3 c= gx * gaussiany(j);
            
            // filtrer
            r+= c * f * alpha;
            // sommer toutes les valeurs du filtre pour normaliser correctement
            norm+= c * alpha;
        }
    }

    // ecrire le pixel filtre dans l'image resultat
    imageStore(framebuffer, ivec2(gl_GlobalInvocationID.xy), vec4(r / norm, 1));
}


#endif
