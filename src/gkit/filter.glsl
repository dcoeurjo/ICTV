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

// configuration du nombre de threads par groupe
layout( local_size_x= 16, local_size_y= 16 ) in;

void main( )
{
    vec3 r;
    vec3 norm;
    for(int i= 0; i < window; i++)
    for(int j= 0; j < window; j++)
    {
        // lire un pixel dans l'image, i,j correspondent a un "deplacement" par rapport au centre du filtre.
        vec3 pixel= texelFetch(image0, ivec2(gl_GlobalInvocationID.xy) + ivec2(i -window/2, j -window/2), 0).rgb;
        //~ vec3 pixel= vec3(1);
        
        // lire un 'pixel' / coefficient du filtre, i,j correspondent a un "deplacement" par rapport au centre du filtre.
        vec3 c= texelFetch(image1, ivec2(16 + i -window/2, 16 + j -window/2), 0).rgb;
        //~ vec3 c= vec3(1);
        
        // filtrer
        r+= c * pixel * alpha;
        // sommer toutes les valeurs du filtre pour normaliser correctement
        norm+= c * alpha;
    }
    
    // ecrire le pixel filtre dans l'image resultat
    imageStore(framebuffer, ivec2(gl_GlobalInvocationID.xy), vec4(r / norm, 1));
}


#endif
