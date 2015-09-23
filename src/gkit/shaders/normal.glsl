// simple shader program
#version 330


#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    uniform mat4 normalMatrix;
    
    layout (location= 0) in vec3 position;
    layout (location= 1) in vec3 normal; 
    in vec3 translation;
    
    out vec3 vertex_normal;
    out vec3 vertex_position;

    void main( )
    {
        gl_Position= mvpMatrix * vec4(position + translation, 1.0);
        vertex_normal= mat3(normalMatrix) * normal;
        vertex_position= position + translation;
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform vec4 color;
    
    in vec3 vertex_position;
    in vec3 vertex_normal;
    
    out vec4 fragment_color;

    void main( )
    {
        //~ vec3 t= normalize(dFdx(vertex_position));
        //~ vec3 b= normalize(dFdy(vertex_position));
        //~ vec3 n= normalize(cross(t, b));
        //~ fragment_color.rgb= color.rgb * abs(n.z);
        
        fragment_color= color * max(0.0, normalize(vertex_normal).z);
    }
#endif

