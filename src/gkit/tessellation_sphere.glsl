#version 410

#ifdef VERTEX_SHADER
in vec4 position;       // x, y, z, radius

void main( void )
{
    gl_Position= position;
}
#endif

#ifdef CONTROL_SHADER
// declare le nombre de sommet a produire pour traiter le patch
layout(vertices= 1) out;

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
// domaine parametrique : triangle, quad ou ligne
layout(quads, equal_spacing, ccw) in;

void main( void )
{
    // recupere la position dans le domaine parametrique du sommet a traiter pour cette execution du shader
    float theta= gl_TessCoord.x * 3.14159 * 0.5;        // theta entre 0 et pi/2
    float phi= gl_TessCoord.y * 3.14159 * 2.0;  // phi entre 0 et 2pi
    // peut mieux faire comme subdivision
    
    // recuperer les parametres de la sphere
    float radius= gl_in[0].gl_Position.w;
    vec3 center= gl_in[0].gl_Position.xyz;
    
    vec3 p= center + vec3(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta)) * radius;
    gl_Position= mvpMatrix * vec4(p, 1.0);
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

void main( void )
{
    fragment_color= vec4(1.f, 1.f, 1.f, 1.f);
}
#endif
