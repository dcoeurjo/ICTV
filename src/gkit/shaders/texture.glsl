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
    uniform vec4 diffuse_color;
    uniform sampler2D diffuse_texture;
    
    in vec3 vertex_normal;
    in vec2 vertex_texcoord;
    
    out vec4 fragment_color;

    void main( )
    {
	vec4 diffuse= texture(diffuse_texture, vertex_texcoord);
	if(diffuse.a < 0.1)
		discard;
	
        fragment_color.rgb= diffuse_color.rgb * abs(normalize(vertex_normal).z) * diffuse.rgb;
        //~ fragment_color.rgb= abs(normalize(vertex_normal));
        //~ fragment_color= diffuse;
        //~ fragment_color.rg= abs(normalize(vertex_texcoord));
    }
#endif

