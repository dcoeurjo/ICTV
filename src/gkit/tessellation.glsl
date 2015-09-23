#version 410

#ifdef VERTEX_SHADER 
    // vertex simple + normale
    uniform float t;
    in vec3 position0;
    in vec3 position1;
    in vec3 normal0;
    in vec3 normal1;
    
    out vec3 vertex_normal;
    
    void main( void )
    {
        vec3 position= (1.0 - t) * position0 + t * position1;
        vec3 normal=  (1.0 - t) * normal0 + t * normal1;
        
        gl_Position= vec4(position, 1.0);
        vertex_normal= normal;
    }
#endif

#ifdef CONTROL_SHADER
    // control shader pour mailler des quad

    // declare le nombre de sommet a produire, 3 pour un triangle, 4 pour un quad, etc
    layout(vertices= 4) out;
    
    // parametres de decoupage des primitives
    uniform vec2 inner_factor;
    uniform vec4 edge_factor;
    
    in vec3 vertex_normal[];
    out vec3 control_normal[];

    void main( void )
    {
        // copie la position du sommet pour l'etape suivante du pipeline
        // gl_InvocationID est l'index du sommet a traiter pour cette execution du shader
        gl_out[gl_InvocationID].gl_Position= gl_in[gl_InvocationID].gl_Position;
        control_normal[gl_InvocationID]= vertex_normal[gl_InvocationID];
        
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
    // phong tessellation 

    uniform mat4 mvMatrix;
    uniform mat4 normalMatrix;
    uniform mat4 mvpMatrix;
    
    // parametre l'unite de decoupage
    //~ layout(quads, fractional_odd_spacing, ccw) in;
    layout(quads, equal_spacing, ccw) in;

    in vec3 control_normal[];
    out vec3 evaluation_normal;
    out vec3 evaluation_tangent;
    out vec3 evaluation_bitangent;
    out vec3 view_position;
    
    // dq / dv
    vec3 pt_dqdu(const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3, const float v ) {
        return (v-1.0) * p0 + (1.0-v) * p1 - v * p3 + v * p2;
    }
 
    // dq / dv
    vec3 pt_dqdv(const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3, const float u ) {
        return (u-1.0) * p0 - u * p1 + (1.0-u) * p3 + u * p2;
    }
   
    // pi
    vec3 pt_pi( const vec3 q, const vec3 p, const vec3 n ) {
        return q - dot(q - p, n) * n;
    }

    // dpi / du
    vec3 pt_dpidu( const vec3 dqdu, const vec3 p, const vec3 n ) {
        vec3 gradx = vec3( 1.0 - n.x * n.x, -n.y * n.x, -n.z * n.x );
        vec3 grady = vec3( -n.x * n.y, 1.0 - n.y * n.y, -n.z * n.y );
        vec3 gradz = vec3( -n.x * n.z, -n.y * n.z, 1.0 - n.z * n.z );
        return mat3(gradx,grady,gradz) * dqdu;
    }
   
    // dpi / dv
    vec3 pt_dpidv( const vec3 dqdv, const vec3 p, const vec3 n ) {
        vec3 gradx = vec3( 1.0 - n.x * n.x, -n.y * n.x, -n.z * n.x );
        vec3 grady = vec3( -n.x * n.y, 1.0 - n.y * n.y, -n.z * n.y );
        vec3 gradz = vec3( -n.x * n.z, -n.y * n.z, 1.0 - n.z * n.z );
        return mat3(gradx,grady,gradz) * dqdv;
    }
   
    // q
    vec3 pt_q( const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3, const float u, const float v ) {
       return (1.0-u) * (1.0-v) * p0 + u * (1.0-v) * p1 + (1.0-u) * v * p3 + u * v * p2;
    }
   
    // n
    vec3 pt_n( const vec3 n0, const vec3 n1, const vec3 n2, const vec3 n3, const float u, const float v ) {
       return (1.0-u) * (1.0-v) * n0 + u * (1.0-v) * n1 + (1.0-u) * v * n3 + u * v * n2;
    }
    
    vec3 project( const vec3 q, const vec3 p, const vec3 n )
    {
        return q - dot(q - p, n) * n;
    }
    
    void main( void )
    {
        // recupere la position dans le domaine parametrique du sommet a traiter pour cette execution du shader
        float u= gl_TessCoord.x;
        float v= gl_TessCoord.y;
        
        vec3 p0 = gl_in[0].gl_Position.xyz;
        vec3 p1 = gl_in[1].gl_Position.xyz;
        vec3 p2 = gl_in[2].gl_Position.xyz;
        vec3 p3 = gl_in[3].gl_Position.xyz;
       
        vec3 n0 = control_normal[0];
        vec3 n1 = control_normal[1];
        vec3 n2 = control_normal[2];
        vec3 n3 = control_normal[3];
       
        // interpolated position
        vec3 q= pt_q(p0,p1,p2,p3,u, v);
       
        // interpolated normal
        vec3 n= pt_n(n0,n1,n2,n3,u,v);

        // position
        vec3 p= (1.0-u) * (1.0-v) * pt_pi(q, p0, n0)
            + u * (1.0-v) * pt_pi(q, p1, n1)
            + (1.0-u) * v * pt_pi(q, p3, n3)
            + u * v * pt_pi(q, p2, n2);

        // tangent
        vec3 dqdu= pt_dqdu(p0,p1,p2,p3,v);
        vec3 t = (v-1.0) * pt_pi(q, p0, n0) + (1.0-u) * (1.0-v) * pt_dpidu(dqdu, p0, n0)
               + (1.0-v) * pt_pi(q, p1, n1) + u * (1.0-v) * pt_dpidu(dqdu, p1, n1)
               - v * pt_pi(q, p3, n3) + v * (1.0-u) * pt_dpidu(dqdu, p3, n3)
               + v * pt_pi(q, p2, n2) + u * v * pt_dpidu(dqdu, p2, n2);        
        
        // bitangent
        vec3 dqdv= pt_dqdv(p0,p1,p2,p3,u);
        vec3 b = (u-1.0) * pt_pi(q, p0, n0) + (1.0-u) * (1.0-v) * pt_dpidv(dqdv, p0, n0)
               - u * pt_pi(q, p1, n1) + u * (1.0-v) * pt_dpidv(dqdv, p1, n1)
               + (1.0-u) * pt_pi(q, p3, n3) + v * (1.0-u) * pt_dpidv(dqdv, p3, n3)
               + u * pt_pi(q, p2, n2) + u * v * pt_dpidv(dqdv, p2, n2);        
        
        gl_Position= mvpMatrix * vec4(p, 1.0);
       
        evaluation_tangent= normalize(t);
        evaluation_bitangent= normalize(b);
        evaluation_normal= n;
    }
#endif

#ifdef FRAGMENT_SHADER
    // fragment simple
    
    in vec3 evaluation_normal;
    in vec3 evaluation_tangent;
    in vec3 evaluation_bitangent;
    
    out vec4 fragment_color;
    
    void main( void )
    {
        vec3 gt= normalize(evaluation_tangent);
        vec3 gb= normalize(evaluation_bitangent);
        vec3 gn= normalize(evaluation_normal);
        fragment_color= vec4( vec3(0.0, 0.8, 1.0) * abs(evaluation_normal.zzz), 1.f );
        
        fragment_color.rgb= cross(gt,gb);
        fragment_color.rgb= gn; 
    }
#endif

