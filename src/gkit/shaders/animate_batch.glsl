// simple shader program
#version 330


#ifdef VERTEX_SHADER
    uniform mat4 mvpMatrix;
    uniform mat4 normalMatrix;
    uniform float t;
    
    layout (location= 0) in vec3 position;
    //~ layout (location= 1) in vec3 texcoord; 
    layout (location= 2) in vec3 normal; 
    
    layout (location= 3) in vec3 position1; 
    //~ layout (location= 4) in vec3 normal1; 
    
    out vec3 vertex_normal;

    void main( )
    {
        vec3 p= (1.0 - t) * position + t * position1;
        //~ vec3 n=  (1.0 - t) * normal + t * normal1;

        gl_Position= mvpMatrix * vec4(p, 1.0);
        vertex_normal= mat3(normalMatrix) * normal;
    }
#endif

#ifdef FRAGMENT_SHADER
    uniform vec4 color;
    
    in vec3 vertex_normal;
    out vec4 fragment_color;

    void main( )
    {
        fragment_color= color * abs(normalize(vertex_normal).z);
    }
#endif

