#version 330

#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    uniform mat4 normalMatrix;
    
    layout(location= 0) in vec3 position;           //!< attribut
    layout(location= 1) in vec3 texcoord;           //!< attribut
    layout(location= 2) in vec3 normal;             //!< attribut
    
    out vec3 vertex_normal;
    out vec2 vertex_texcoord;

    void main( )
    {
        gl_Position= mvpMatrix * vec4(position, 1.0);
        vertex_normal= mat3(normalMatrix) * normal;
        vertex_texcoord= texcoord.st;
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform sampler2D texture0;
    uniform sampler2D texture1;
    uniform float time;
    
    in vec3 vertex_normal;
    in vec2 vertex_texcoord;
    
    out vec4 fragment_color;

    void main( )
    {
        //~ fragment_color.rgb= abs(normalize(vertex_normal).zzz) * texture(texture0, vertex_texcoord).rgb;
        fragment_color.rgb= abs(normalize(vertex_normal).zzz) * (0.8 * texture(texture0, vertex_texcoord).rgb + 0.4 * texture(texture1, vertex_texcoord).rgb);
        //~ fragment_color.rgb= 0.4 * texture(texture0, vertex_texcoord).rgb + texture(texture1, vertex_texcoord).rgb;
        //~ fragment_color.rgb= abs(normalize(vertex_normal).zzz);
    }
#endif

