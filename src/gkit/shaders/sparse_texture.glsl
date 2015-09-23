#version 420

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
    uniform vec4 diffuse_color;
    //~ uniform sampler2D diffuse_texture;
    uniform sampler2DArray diffuse_texture;
    uniform float time;
    
    in vec3 vertex_normal;
    in vec2 vertex_texcoord;
    
    out vec4 fragment_color;

    void main( )
    {
        //~ fragment_color.rgb= 0.4 * texture(texture0, vertex_texcoord).rgb + texture(texture1, vertex_texcoord).rgb;
        //~ fragment_color.rgb= vec3(abs(normalize(vertex_normal).z)) * texture(texture0, vertex_texcoord).rgb;
        //~ fragment_color.rgb= texture(texture1, vertex_texcoord).rgb;
        //~ fragment_color.rgb= 2 * diffuse_color.rgb * abs(normalize(vertex_normal).z) * texture(diffuse_texture, vertex_texcoord).rgb;
        //~ fragment_color.rgb= 2 * diffuse_color.rgb * abs(normalize(vertex_normal).z) * texture(diffuse_texture, vertex_texcoord).rgb;

        //~ fragment_color.rg= abs(dFdx(vertex_texcoord * 1024)) + abs(dFdy(vertex_texcoord * 1024));
        //~ fragment_color.rgb= textureQueryLod(diffuse_texture, vertex_texcoord).xxx;
        
        //~ vec3 colors[11]= vec3[11](
            //~ vec3(40,40,80), 
            //~ vec3(12,17,115),
            //~ vec3(28,121,255),
            //~ vec3(31,255,255),
            //~ vec3(130,255,17),
            //~ vec3(255,255,14),
            //~ vec3(255,112,22),
            //~ vec3(251,0,20),
            //~ vec3(113,1,14),
            //~ vec3(113,1,14),
            //~ vec3(255,0,0)
        //~ );
        
        //~ int level= int(textureQueryLod(diffuse_texture, vec3(vertex_texcoord, 0)).x);
        //~ vec3 color;
        //~ if(level < 11)
            //~ color= colors[level] / vec3(255);
        //~ else
            //~ color= colors[10] / vec3(255);
    
        //~ fragment_color= vec4(color, 1.0);

        //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * texture(diffuse_texture, vec3(vertex_texcoord, 0)).rgb;
        //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * texture(diffuse_texture, vec3(vertex_texcoord, 0)).rgb;
        //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * textureGrad(diffuse_texture, vec3(vertex_texcoord, 0), dFdx(vertex_texcoord), dFdy(vertex_texcoord)).rgb;
        
        //~ float scale= min(length(dFdx(vertex_texcoord) * 2048), length(dFdy(vertex_texcoord) * 2048));   // linear sampler
        //~ float scale= min(length(dFdx(vertex_texcoord) * 2048), length(dFdy(vertex_texcoord) * 2048)) / 8;       // aniso 16 sampler
        //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * textureGrad(diffuse_texture, vertex_texcoord, scale * dFdx(vertex_texcoord), scale * dFdy(vertex_texcoord)).rgb;
        
        //~ float lod_min= 4;
        //~ if(scale >= lod_min)
            fragment_color.rgb= abs(normalize(vertex_normal).z) * texture(diffuse_texture, vec3(vertex_texcoord, 0)).rgb;
            //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * textureGrad(diffuse_texture, vec3(vertex_texcoord, 0), dFdx(vertex_texcoord), dFdy(vertex_texcoord)).rgb;
        //~ else
            //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * vec3(1, 0, 0);
            //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * textureGrad(diffuse_texture, vec3(vertex_texcoord, 0), dFdx(vertex_texcoord) / scale * lod_min, dFdy(vertex_texcoord) / scale * lod_min).rgb;
            //~ fragment_color.rgb= abs(normalize(vertex_normal).z) * textureLod(diffuse_texture, vec3(vertex_texcoord, 0), 3).rgb;
        
    }
#endif

