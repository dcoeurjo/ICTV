
#version 330

#ifdef VERTEX_SHADER
out vec2 vertex_texcoord;

void main(void)
{
    vec3 quad[4]= vec3[4]( 
        vec3(-1, -1, -1), vec3( 1, -1, -1), 
        vec3(-1,  1, -1), vec3( 1,  1, -1) );
    
    gl_Position= vec4(quad[gl_VertexID], 1.0);
    vertex_texcoord= quad[gl_VertexID].xy * 0.5 + 0.5;
}
#endif


#ifdef FRAGMENT_SHADER
uniform float compression;
uniform float saturation;
uniform float heat;
uniform int image_index;
uniform vec4 channels;

uniform sampler2DArray images;
uniform sampler2D image;
uniform sampler2D colors;

in vec2 vertex_texcoord;
out vec4 fragment_color;

void main(void)
{
    const vec3 rgby= vec3(0.3, 0.59, 0.11);
    float k1= 1.0 / pow(saturation, 1.0 / compression); // normalisation : saturation == blanc
    
    vec4 color;
    if(image_index == 0)        // stupid workaround
        color= texture(image, vertex_texcoord);
    else
        color= texture(images, vec3(vertex_texcoord, float(image_index)));
    
    float y= dot(color.rgb, rgby);  // normalisation de la couleur : (color / y) == teinte
    
    if(y > saturation)
        color= vec4(y, y, y, color.a);
    
    if(heat > 0.0)
        // applique une fausse couleur
        color= texture(colors, vec2(saturation - y / saturation, 0.5));
    else
        // applique la compression (gamma)
        color= (color / y) * k1 * pow(y, 1.0 / compression);

    //~ color *= pow( 2.0, saturation );          // exposure
    //~ color= pow( color, vec4( 1.0 / compression ) );  // gamma
    if(channels != vec4(0, 0, 0, 1))
        fragment_color= clamp( channels * color, vec4(0.0), vec4(1.0) );
    else
        // visualisation du canal alpha seul
        fragment_color= clamp( color.aaaa, vec4(0.0), vec4(1) );
    
}
#endif
