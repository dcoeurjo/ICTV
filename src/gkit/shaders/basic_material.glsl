#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec3 texcoords;
layout(location= 2) in vec3 normal;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec3 vertex_texcoords;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1.0);
    vertex_position= vec3(mvMatrix * vec4(position, 1.0));
    vertex_normal= mat3(normalMatrix) * normal;
    vertex_texcoords= texcoords;
}
#endif

#ifdef FRAGMENT_SHADER
    #ifdef USE_AMBIENT_TEXTURE
    uniform sampler2D ambient_texture;
    #endif
    #ifdef USE_DIFFUSE_TEXTURE
    uniform sampler2D diffuse_texture;
    #endif
    #ifdef USE_SPECULAR_TEXTURE
    uniform sampler2D specular_texture;
    #endif
    #ifdef USE_ENVMAP_LIGHT
    uniform samplerCube envmap_texture;
    #endif

uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float ns;

uniform vec3 light_position;    // view 
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

in vec3 vertex_position;        // view
in vec3 vertex_normal;          // view
in vec3 vertex_texcoords;

out vec4 fragment_color;

#define PI 3.1415
#define saturation 1.0
#define compression 2.2


void main( ) 
{
#ifdef USE_AMBIENT_TEXTURE
    vec4 ambient= texture(ambient_texture, vertex_texcoords.xy);
#else
    vec4 ambient= vec4(1.0);
#endif
    
#ifdef USE_DIFFUSE_TEXTURE
    vec4 diffuse= texture(diffuse_texture, vertex_texcoords.xy);
#else
    vec4 diffuse= vec4(1.0);
#endif
    
    vec3 n= normalize(vertex_normal);
#ifndef USE_ENVMAP_LIGHT
    vec3 l= normalize(light_position - vertex_position);
#else
    vec3 l= reflect(normalize(vertex_position), n);
    //~ l.y= -l.y;
#endif
    vec3 h= normalize(n + l);
    
#ifndef USE_BOTH_SIDES
    float cos_theta= max(0.0, dot(n, l));
    float blinn= pow( max(0.0, dot(n, h)), ns ) * (ns + 2) / (2 * PI);
#else
    float cos_theta= abs(dot(n, l));
    float blinn= pow( abs(dot(n, h)), ns ) * (ns + 2) / (2 * PI);
#endif

#ifdef USE_SPECULAR_TEXTURE
    vec4 specular= texture(specular_texture, vertex_texcoords.xy);
#else
    vec4 specular= vec4(1.0);
#endif
    specular.rgb= specular.rgb * blinn;
    
#ifdef USE_ENVMAP_LIGHT
    //! utilise "Plausible Blinn-Phong Reflection of Standard Cube MIP-Maps"
    //! http://graphics.cs.williams.edu/papers/EnvMipReport2013/paper.pdf
    
    int size= textureSize(envmap_texture, 0).x;
    float level_diffuse= log2(size);
    //~ float level_specular= log2(size * sqrt(3.0)) - 0.5 * log2(ns + 1);
    float level_specular= 0;
    
    vec4 env_diffuse= textureLod(envmap_texture, n, level_diffuse);
    //~ vec4 env_diffuse= texture(envmap_texture, n, level_diffuse);
    vec4 env_ambient= light_diffuse;
    vec4 env_specular= textureLod(envmap_texture, l, level_specular);
    //~ vec4 env_specular= texture(envmap_texture, l, level_specular);
#else
    vec4 env_diffuse= vec4(1.0);
    vec4 env_ambient= vec4(1.0);
    vec4 env_specular= vec4(1.0);
#endif

    fragment_color= vec4(0, 0, 0, 1);   // opaque
    
    fragment_color= light_ambient * ka * ambient_color * ambient / PI * abs(dot(n, l))
        + env_diffuse * kd * diffuse_color * diffuse * cos_theta 
        + env_specular * ks * specular_color * specular * cos_theta;
    //~ fragment_color= (env_ambient + light_ambient) * ka * ambient_color * ambient / PI * abs(dot(n, l))
        //~ + (env_diffuse + light_diffuse) * kd * diffuse_color * diffuse * cos_theta 
        //~ + (env_specular + light_specular) * ks * specular_color * specular * cos_theta;
        
    //~ fragment_color.rgb= vec3( env_specular * ks * specular_color * specular * cos_theta );
    //~ fragment_color.rgb= vec3(env_specular);
    //~ fragment_color.rgb= vec3(env_diffuse);

    
    vec3 f= fragment_color.rgb * pow( 2.0, saturation );          // exposure
    f= pow( f, vec3( 1.0 / compression ) );  // gamma
    fragment_color= vec4(f, 1.0);    
}
#endif
