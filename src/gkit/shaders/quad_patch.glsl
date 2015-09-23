#version 430

#ifdef VERTEX_SHADER
in vec3 position;

void main( void )
{
    gl_Position= vec4(position, 1.f);
}
#endif

#ifdef CONTROL_SHADER
// nombre de point de controle a produire
layout(vertices= 4) out;

uniform vec3 eye;
uniform float edge_factor;

void main( void )
{
    gl_out[gl_InvocationID].gl_Position= gl_in[gl_InvocationID].gl_Position;
    
    // arete courante (id -1, id)
    vec3 a= gl_in[(gl_InvocationID +4 -1) % 4].gl_Position.xyz;
    vec3 b= gl_in[gl_InvocationID].gl_Position.xyz;
    
    float z= distance((a + b) * 0.5, eye);
    float l= length(b - a);
    float level= edge_factor *  l / (z+1.0);
    
    // parametre l'unite de decoupage sur l'arete courante
    gl_TessLevelOuter[gl_InvocationID]= level;
    
    barrier();
    if(gl_InvocationID == 0)
    {
        // parametre l'unite de decoupage sur l'interieur du patch
        gl_TessLevelInner[0]= max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
        gl_TessLevelInner[1]= max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
        
        // todo: frustum culling
    }
}
#endif

#ifdef EVALUATION_SHADER
uniform mat4 mvpMatrix;
uniform mat4 viewMatrix;

// parametre l'unite de decoupage
layout(quads, fractional_odd_spacing, ccw) in;

out vec2 evaluation_texcoords;

out vec3 evaluation_position;

void main( void )
{
    // recupere la position dans le domaine parametrique du sommet a traiter pour cette execution du shader
    float u= gl_TessCoord.x;
    float v= gl_TessCoord.y;
    
    // calcule la position du sommet dans le repere local du quad abcd,  
    vec3 p= (1.0-u) * (1.0-v) * gl_in[0].gl_Position.xyz
        + u * (1.0-v) * gl_in[1].gl_Position.xyz
        + (1.0-u) * v * gl_in[3].gl_Position.xyz
        + u * v * gl_in[2].gl_Position.xyz;
    
    gl_Position= mvpMatrix * vec4(p, 1.0);
    
    evaluation_position= vec3(viewMatrix * vec4(p, 1.0));
    evaluation_texcoords= gl_TessCoord.xy;
}
#endif

#ifdef FRAGMENT_SHADER
uniform vec4 diffuse_color;
in vec3 evaluation_position;
in vec2 evaluation_texcoords;

out vec4 fragment_color;

// #define BINDLESS_TEXTURES
#ifndef BINDLESS_TEXTURES
    #extension GL_NV_shader_thread_group : enable

    uniform sampler2DArray textures;
    
    void main( void )
    {
        vec4 texture_color= texture(textures, vec3(evaluation_texcoords, gl_PrimitiveID));
        
        vec3 color;
        bool helper= gl_HelperThreadNV;
        uint helperMask= ballotThreadNV(helper);
        if(!helper)
        {
            // uint quad= (gl_ThreadInWarpNV / 4) * 4; // == gl_ThreadInWarpNV & ~3 
            uint quadMask= (15 << (gl_ThreadInWarpNV & ~3));
            int count= bitCount(helperMask & quadMask);   // compte les helpers associes au quad : helper_id= 4*quad + i

            // autre solution :
            // float helperf= helper ? 1.0 : 0.0;
            // float count= quadSwizzle0NV(helperf) + quadSwizzle1NV(helperf) + quadSwizzle2NV(helperf) + quadSwizzle3NV(helperf);

            if(count < 3)
                color= vec3(float(count) / 2.0);
            else
                color= vec3(1.0, 0.0, 0.0); // 3 helpers threads sur le quad...

            /*
            // un seul thread du quad affiche les compteurs
            uint activeMask= gl_ThreadGtMaskNV & ~helperMask & quadMask;
            if(bitCount(activeMask) != 0)   // identifie le thread avec l'id la plus importante dans le quad
                color= vec3(0);
              */  

            fragment_color.rgb= color;
        }

        //fragment_color= diffuse_color * texture_color / texture_color.a;
    }
    
#else
    #extension GL_ARB_bindless_texture : require
    
    layout(binding= 0) readonly buffer handleBuffer 
    {
        layout(bindless_sampler) sampler2DArray handles[];
    };
    
    void main( void )
    {
        vec2 texcoords;
        texcoords= evaluation_texcoords;

        vec4 texture_color= texture(handles[gl_PrimitiveID / 2048], vec3(texcoords, float(gl_PrimitiveID % 2048)));
        //~ vec4 texture_color= texture(handles[gl_PrimitiveID / 2048], vec3(texcoords, float(gl_PrimitiveID % 2048))).rrra;

        vec3 t= normalize(dFdx(evaluation_position));
        vec3 b= normalize(dFdy(evaluation_position));
        vec3 n= normalize(cross(t, b));
        
        fragment_color= diffuse_color * texture_color / texture_color.a * abs(n.z);
        // fragment_color= diffuse_color * texture_color / texture_color.a ; //* abs(n.z);
    }
#endif
#endif
