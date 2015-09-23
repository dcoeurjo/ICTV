#version 330	// core profile
    
#ifdef VERTEX_SHADER
    uniform mat4 projection;
    
    layout(location= 0) in vec2 position;
    layout(location= 1) in vec3 texcoord;
    out vec3 vertex_texcoord;
    
    void main( )
    {
        vertex_texcoord= texcoord;
        gl_Position= projection * vec4(position, 0.0, 1.0);
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform vec4 color;
    uniform sampler2D font;
    
    in vec3 vertex_texcoord;
    out vec4 fragment_color;
    
    void main( )
    {
        fragment_color= color * texture(font, vertex_texcoord.st);
    }
#endif
