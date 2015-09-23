#version 410

#ifdef VERTEX_SHADER
uniform mat4 mvpMatrix;    
in vec3 position;

void main( void )
{
    // transmet la position du sommet du triangle a decouper
    // et pre-transforme chaque sommet, une seule fois, 
    // au lieu de le faire pour chaque sommet du maillage subdivise, dans l'evaluation shader
    
    gl_Position= mvpMatrix * vec4(position, 1.f);
}
#endif

#ifdef CONTROL_SHADER
// declare le nombre de sommet a produire, 3 pour un triangle, 4 pour un quad, etc
layout(vertices= 4) out;

// parametres de decoupage des primitives
uniform vec2 inner_factor;
uniform vec4 edge_factor;

void main( void )
{
    // copie la position du sommet pour l'etape suivante du pipeline
    // gl_InvocationID est l'index du sommet a traiter pour cette execution du shader
    gl_out[gl_InvocationID].gl_Position= gl_in[gl_InvocationID].gl_Position;
    
    // parametre l'unite de decoupage
    gl_TessLevelInner[0]= inner_factor.x;     // niveau de decoupage pour l'interieur, sur la dimension u
    gl_TessLevelInner[1]= inner_factor.y;     // niveau de decoupage pour l'interieur, sur la dimension v
    
    gl_TessLevelOuter[0]= edge_factor.x;    // niveau de decoupage pour chaque arete
    gl_TessLevelOuter[1]= edge_factor.y;
    gl_TessLevelOuter[2]= edge_factor.z;
    gl_TessLevelOuter[3]= edge_factor.w;
}
#endif

#ifdef EVALUATION_SHADER
uniform mat4 mvpMatrix;

// parametre l'unite de decoupage
layout(quads, fractional_odd_spacing, ccw) in;

void main( void )
{
    // recupere la position dans le domaine parametrique du sommet a traiter pour cette execution du shader
    float u= gl_TessCoord.x;
    float v= gl_TessCoord.y;
    
    // calcule la position du sommet dans le repere local du quad abcd,  
    // interpolation barycentrique p(u, v, w)= a + (b - a) * u + (d - a) * v;
    // l'arete ab est utilisee comme axe u, et l'arete ad pour l'axe v.
    // les positions des sommets sont deja dans le repere projectif (homogene) de la camera, 
    // cf explications dans evaluation_simple_triangle
    //~ gl_Position= gl_in[0].gl_Position 
        //~ + u * (gl_in[1].gl_Position - gl_in[0].gl_Position) 
        //~ + v * (gl_in[3].gl_Position - gl_in[0].gl_Position);
        
    gl_Position= (1.0-u) * (1.0-v) * gl_in[0].gl_Position + u * (1.0-v) * gl_in[1].gl_Position + (1.0-u) * v * gl_in[3].gl_Position + u * v * gl_in[2].gl_Position;
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

void main( void )
{
    fragment_color= vec4(1.f, 1.f, 1.f, 1.f);
}
#endif
