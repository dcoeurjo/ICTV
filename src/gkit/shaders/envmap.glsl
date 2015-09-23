#version 330

#ifdef VERTEX_SHADER
uniform mat4 vpInverse;

out vec3 vertex_direction;

void main( )
{
    vec2 quad[4]= vec2[4]( vec2(-1, -1), vec2( 1, -1), vec2(-1,  1), vec2( 1,  1) );
    vec2 position= quad[gl_VertexID];

    gl_Position= vec4(position, 1, 1);  // far plane
    
    vec4 a= vpInverse * vec4(position, -1, 1);
    vec4 b= vpInverse * vec4(position, 1, 1);
    vertex_direction= normalize(b.xyz / b.w - a.xyz / a.w);
}
#endif

#ifdef FRAGMENT_SHADER_TEST
uniform samplerCube envmap;
uniform sampler2DArray envarray;
in vec3 vertex_direction;

out vec4 fragment_color;


// cf https://developer.nvidia.com/sites/default/files/akamai/gamedev/docs/Borderless%20Ptex.pdf
// pour les matrices de transformations entre faces adjacentes.

void main( )
{
    float bias = 2;
    
    // selection de la face 
    vec4 stmi;
    vec3 r= vertex_direction;
    vec3 ra= abs(vertex_direction);
    if(ra.x > ra.y && ra.x > ra.z)      // x dominant
    {
        if(r.x > 0)
        {
            stmi= vec4(-r.z, -r.y, ra.x, 0);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            vec4 color0= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color0;
            
            // 0 0 == 2 1
            mat3 m01= transpose(mat3(
                vec3(0, 1, 1), 
                vec3(-1, 0, 1), 
                vec3(0, 0, 1)));
            vec4 color2= textureLod(envarray, vec3(vec2(m01 * uv1), 2), bias);
            fragment_color+= color2;
            
            // 0 1 == 5 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color5= textureLod(envarray, vec3(vec2(m13 * uv1), 5), bias);
            fragment_color+= color5;
            
            // 0 2 == 3 1
            mat3 m21= transpose(mat3(
                vec3(0, -1, 2),
                vec3(1, 0, 0),
                vec3(0, 0, 1)));
            vec4 color3= textureLod(envarray, vec3(vec2(m21 * uv1), 3), bias);
            fragment_color+= color3;
            
            // 0 3 == 4 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color4= textureLod(envarray, vec3(vec2(m31 * uv1), 4), bias);
            fragment_color+= color4;
        }
        else
        {
            stmi= vec4( r.z, -r.y, ra.x, 1);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            vec4 color1= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color1;
            
            // 1 0 == 2 3
            mat3 m03= transpose(mat3(
                vec3(0, -1, 0), 
                vec3(1, 0, 0), 
                vec3(0, 0, 1)));
            vec4 color2= textureLod(envarray, vec3(vec2(m03 * uv1), 2), bias);
            fragment_color+= color2;
            
            // 1 1 == 4 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color4= textureLod(envarray, vec3(vec2(m13 * uv1), 4), bias);
            fragment_color+= color4;
            
            // 1 2 == 3 3
            mat3 m23= transpose(mat3(
                vec3(0, 1, -1),
                vec3(-1, 0, 1),
                vec3(0, 0, 1)));
            vec4 color3= textureLod(envarray, vec3(vec2(m23 * uv1), 3), bias);
            fragment_color+= color3;
            
            // 1 3 == 5 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color5= textureLod(envarray, vec3(vec2(m31 * uv1), 5), bias);
            fragment_color+= color5;
        }
    }
    else if(ra.y > ra.x && ra.y > ra.z) // y dominant
    {
        if(r.y > 0)
        {
            stmi= vec4( r.x,  r.z, ra.y, 2);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            vec4 color2= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color2;
            
            // 2 0 == 5 0
            mat3 m00= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 0), 
                vec3(0, 0, 1)));
            vec4 color5= textureLod(envarray, vec3(vec2(m00 * uv1), 5), bias);
            fragment_color+= color5;
            
            // 2 1 == 0 0
            mat3 m10= transpose(mat3(
                vec3(0, -1, 1),
                vec3(1, 0, -1),
                vec3(0, 0, 1)));
            vec4 color0= textureLod(envarray, vec3(vec2(m10 * uv1), 0), bias);
            fragment_color+= color0;

            // 2 2 == 4 0
            mat3 m20= transpose(mat3(
                vec3(1, 0, 0),
                vec3(0, 1, -1),
                vec3(0, 0, 1)));
            vec4 color4= textureLod(envarray, vec3(vec2(m20 * uv1), 4), bias);
            fragment_color+= color4;
            
            // 2 3 == 1 0
            mat3 m30= transpose(mat3(
                vec3(0, 1, 0),
                vec3(-1, 0, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureLod(envarray, vec3(vec2(m30 * uv1), 1), bias);
            fragment_color+= color1;
        }
        else
        {
            stmi= vec4( r.x, -r.z, ra.y, 3);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            vec4 color3= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color3;
            
            // 3 0 == 4 2
            mat3 m02= transpose(mat3(
                vec3(1, 0, 0), 
                vec3(0, 1, 1), 
                vec3(0, 0, 1)));
            vec4 color4= textureLod(envarray, vec3(vec2(m02 * uv1), 4), bias);
            fragment_color+= color4;
            
            // 3 1 == 0 2
            mat3 m12= transpose(mat3(
                vec3(0, 1, 0),
                vec3(-1, 0, 2),
                vec3(0, 0, 1)));
            vec4 color0= textureLod(envarray, vec3(vec2(m12 * uv1), 0), bias);
            fragment_color+= color0;
            
            // 3 2 == 5 2
            mat3 m22= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 2),
                vec3(0, 0, 1)));
            vec4 color5= textureLod(envarray, vec3(vec2(m22 * uv1), 5), bias);
            fragment_color+= color5;
            
            // 3 3 == 1 2
            mat3 m32= transpose(mat3(
                vec3(0, -1, 1),
                vec3(1, 0, 1),
                vec3(0, 0, 1)));
            vec4 color1= textureLod(envarray, vec3(vec2(m32 * uv1), 1), bias);
            fragment_color+= color1;
        }
    }
    else // if(ra.z > ra.x && ra.z > ra.y)      // z dominant
    {
        if(r.z > 0)
        {
            stmi= vec4( r.x, -r.y, ra.z, 4);
            
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            vec4 color4= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color4;
            
            // 4 0 == 2 2
            mat3 m02= transpose(mat3(
                vec3(1, 0, 0), 
                vec3(0, 1, 1), 
                vec3(0, 0, 1)));
            vec4 color2= textureLod(envarray, vec3(vec2(m02 * uv1), 2), bias);
            fragment_color+= color2;
            
            // 4 1 == 0 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color0= textureLod(envarray, vec3(vec2(m13 * uv1), 0), bias);
            fragment_color+= color0;
            
            // 4 2 == 3 0
            mat3 m20= transpose(mat3(
                vec3(1, 0, 0),
                vec3(0, 1, -1),
                vec3(0, 0, 1)));
            vec4 color3= textureLod(envarray, vec3(vec2(m20 * uv1), 3), bias);
            fragment_color+= color3;
            
            // 4 3 == 1 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureLod(envarray, vec3(vec2(m31 * uv1), 1), bias);
            fragment_color+= color1;
        }
        else
        {
            stmi= vec4(-r.x, -r.y, ra.z, 5);
            
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            vec4 color5= textureLod(envarray, vec3(uv, stmi.w), bias);
            fragment_color.rgba= color5;
            
            // 5 0 == 2 0
            mat3 m00= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 0), 
                vec3(0, 0, 1)));
            vec4 color2= textureLod(envarray, vec3(vec2(m00 * uv1), 2), bias);
            fragment_color+= color2;
            
            // 5 1 == 1 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureLod(envarray, vec3( vec2(m13 * uv1), 1), bias);
            fragment_color+= color1;
            
            // 5 2 == 3 2
            mat3 m22= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 2),
                vec3(0, 0, 1)));
            vec4 color3= textureLod(envarray, vec3( vec2(m22 * uv1), 3), bias);
            fragment_color+= color3;
            
            // 5 3 == 0 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color0= textureLod(envarray, vec3( vec2(m31 * uv1), 0), bias);
            fragment_color+= color0;
        }
    }
    
    fragment_color/= fragment_color.a;
    
    //~ fragment_color= texture(envmap, vertex_direction, bias);
    //~ fragment_color= texture(envmap, vec3(1, 1, 1));
    //~ fragment_color= vec4(1, 1, 0, 1);
    
}
#endif

#ifdef FRAGMENT_SHADER
uniform samplerCube envmap;
uniform sampler2DArray envarray;
in vec3 vertex_direction;

out vec4 fragment_color;

void main( )
{
    float bias = 2;
    
    vec3 dpdx= vec3(1, 0, 0)/1000;
    vec3 dpdy= vec3(0, 400, 0)/1000;
    
    
    // selection de la face 
    vec4 stmi;
    vec3 r= vertex_direction;
    vec3 ra= abs(vertex_direction);
    if(ra.x > ra.y && ra.x > ra.z)      // x dominant
    {
        if(r.x > 0)
        {
            stmi= vec4(-r.z, -r.y, ra.x, 0);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color0= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color0;
            
            // 0 0 == 2 1
            mat3 m01= transpose(mat3(
                vec3(0, 1, 1), 
                vec3(-1, 0, 1), 
                vec3(0, 0, 1)));
            vec4 color2= textureGrad(envarray, vec3(vec2(m01 * uv1), 2), vec2(m01 * dpdx), vec2(m01 * dpdy));
            fragment_color+= color2;
            
            // 0 1 == 5 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color5= textureGrad(envarray, vec3(vec2(m13 * uv1), 5), vec2(m13 * dpdx), vec2(m13 * dpdy));
            fragment_color+= color5;
            
            // 0 2 == 3 1
            mat3 m21= transpose(mat3(
                vec3(0, -1, 2),
                vec3(1, 0, 0),
                vec3(0, 0, 1)));
            vec4 color3= textureGrad(envarray, vec3(vec2(m21 * uv1), 3), vec2(m21 * dpdx), vec2(m21 * dpdy));
            fragment_color+= color3;
            
            // 0 3 == 4 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color4= textureGrad(envarray, vec3(vec2(m31 * uv1), 4), vec2(m31 * dpdx), vec2(m31 * dpdy));
            fragment_color+= color4;
        }
        else
        {
            stmi= vec4( r.z, -r.y, ra.x, 1);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color1= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color1;
            
            // 1 0 == 2 3
            mat3 m03= transpose(mat3(
                vec3(0, -1, 0), 
                vec3(1, 0, 0), 
                vec3(0, 0, 1)));
            vec4 color2= textureGrad(envarray, vec3(vec2(m03 * uv1), 2), vec2(m03 * dpdx), vec2(m03 * dpdy));
            fragment_color+= color2;
            
            // 1 1 == 4 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color4= textureGrad(envarray, vec3(vec2(m13 * uv1), 4), vec2(m13 * dpdx), vec2(m13 * dpdy));
            fragment_color+= color4;
            
            // 1 2 == 3 3
            mat3 m23= transpose(mat3(
                vec3(0, 1, -1),
                vec3(-1, 0, 1),
                vec3(0, 0, 1)));
            vec4 color3= textureGrad(envarray, vec3(vec2(m23 * uv1), 3),  vec2(m23 * dpdx), vec2(m23 * dpdy));
            fragment_color+= color3;
            
            // 1 3 == 5 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color5= textureGrad(envarray, vec3(vec2(m31 * uv1), 5),  vec2(m31 * dpdx), vec2(m31 * dpdy));
            fragment_color+= color5;
        }
    }
    else if(ra.y > ra.x && ra.y > ra.z) // y dominant
    {
        if(r.y > 0)
        {
            stmi= vec4( r.x,  r.z, ra.y, 2);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color2= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color2;
            
            // 2 0 == 5 0
            mat3 m00= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 0), 
                vec3(0, 0, 1)));
            vec4 color5= textureGrad(envarray, vec3(vec2(m00 * uv1), 5),  vec2(m00 * dpdx), vec2(m00 * dpdy));
            fragment_color+= color5;
            
            // 2 1 == 0 0
            mat3 m10= transpose(mat3(
                vec3(0, -1, 1),
                vec3(1, 0, -1),
                vec3(0, 0, 1)));
            vec4 color0= textureGrad(envarray, vec3(vec2(m10 * uv1), 0),  vec2(m10 * dpdx), vec2(m10 * dpdy));
            fragment_color+= color0;

            // 2 2 == 4 0
            mat3 m20= transpose(mat3(
                vec3(1, 0, 0),
                vec3(0, 1, -1),
                vec3(0, 0, 1)));
            vec4 color4= textureGrad(envarray, vec3(vec2(m20 * uv1), 4),  vec2(m20 * dpdx), vec2(m20 * dpdy));
            fragment_color+= color4;
            
            // 2 3 == 1 0
            mat3 m30= transpose(mat3(
                vec3(0, 1, 0),
                vec3(-1, 0, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureGrad(envarray, vec3(vec2(m30 * uv1), 1),  vec2(m30 * dpdx), vec2(m30 * dpdy));
            fragment_color+= color1;
        }
        else
        {
            stmi= vec4( r.x, -r.z, ra.y, 3);
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color3= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color3;
            
            // 3 0 == 4 2
            mat3 m02= transpose(mat3(
                vec3(1, 0, 0), 
                vec3(0, 1, 1), 
                vec3(0, 0, 1)));
            vec4 color4= textureGrad(envarray, vec3(vec2(m02 * uv1), 4),  vec2(m02 * dpdx), vec2(m02 * dpdy));
            fragment_color+= color4;
            
            // 3 1 == 0 2
            mat3 m12= transpose(mat3(
                vec3(0, 1, 0),
                vec3(-1, 0, 2),
                vec3(0, 0, 1)));
            vec4 color0= textureGrad(envarray, vec3(vec2(m12 * uv1), 0),  vec2(m12 * dpdx), vec2(m12 * dpdy));
            fragment_color+= color0;
            
            // 3 2 == 5 2
            mat3 m22= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 2),
                vec3(0, 0, 1)));
            vec4 color5= textureGrad(envarray, vec3(vec2(m22 * uv1), 5),  vec2(m22 * dpdx), vec2(m22 * dpdy));
            fragment_color+= color5;
            
            // 3 3 == 1 2
            mat3 m32= transpose(mat3(
                vec3(0, -1, 1),
                vec3(1, 0, 1),
                vec3(0, 0, 1)));
            vec4 color1= textureGrad(envarray, vec3(vec2(m32 * uv1), 1),  vec2(m32 * dpdx), vec2(m32 * dpdy));
            fragment_color+= color1;
        }
    }
    else // if(ra.z > ra.x && ra.z > ra.y)      // z dominant
    {
        if(r.z > 0)
        {
            stmi= vec4( r.x, -r.y, ra.z, 4);
            
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color4= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color4;
            
            // 4 0 == 2 2
            mat3 m02= transpose(mat3(
                vec3(1, 0, 0), 
                vec3(0, 1, 1), 
                vec3(0, 0, 1)));
            vec4 color2= textureGrad(envarray, vec3(vec2(m02 * uv1), 2),  vec2(m02 * dpdx), vec2(m02 * dpdy));
            fragment_color+= color2;
            
            // 4 1 == 0 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color0= textureGrad(envarray, vec3(vec2(m13 * uv1), 0),  vec2(m13 * dpdx), vec2(m13 * dpdy));
            fragment_color+= color0;
            
            // 4 2 == 3 0
            mat3 m20= transpose(mat3(
                vec3(1, 0, 0),
                vec3(0, 1, -1),
                vec3(0, 0, 1)));
            vec4 color3= textureGrad(envarray, vec3(vec2(m20 * uv1), 3),  vec2(m20 * dpdx), vec2(m20 * dpdy));
            fragment_color+= color3;
            
            // 4 3 == 1 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureGrad(envarray, vec3(vec2(m31 * uv1), 1),  vec2(m31 * dpdx), vec2(m31 * dpdy));
            fragment_color+= color1;
        }
        else
        {
            stmi= vec4(-r.x, -r.y, ra.z, 5);
            
            vec2 uv= vec2(0.5 * (stmi.x / stmi.z + 1.0), 0.5 * (stmi.y / stmi.z + 1.0));
            vec3 uv1= vec3(uv, 1);
            
            //~ fragment_color= vec4(uv, 0, 1);
            //~ return;
            
            vec4 color5= textureGrad(envarray, vec3(uv, stmi.w), vec2(dpdx), vec2(dpdy));
            fragment_color.rgba= color5;
            
            // 5 0 == 2 0
            mat3 m00= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 0), 
                vec3(0, 0, 1)));
            vec4 color2= textureGrad(envarray, vec3(vec2(m00 * uv1), 2),  vec2(m00 * dpdx), vec2(m00 * dpdy));
            fragment_color+= color2;
            
            // 5 1 == 1 3
            mat3 m13= transpose(mat3(
                vec3(1, 0, -1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color1= textureGrad(envarray, vec3( vec2(m13 * uv1), 1),  vec2(m13 * dpdx), vec2(m13 * dpdy));
            fragment_color+= color1;
            
            // 5 2 == 3 2
            mat3 m22= transpose(mat3(
                vec3(-1, 0, 1), 
                vec3(0, -1, 2),
                vec3(0, 0, 1)));
            vec4 color3= textureGrad(envarray, vec3( vec2(m22 * uv1), 3),  vec2(m22 * dpdx), vec2(m22 * dpdy));
            fragment_color+= color3;
            
            // 5 3 == 0 1
            mat3 m31= transpose(mat3(
                vec3(1, 0, 1),
                vec3(0, 1, 0),
                vec3(0, 0, 1)));
            vec4 color0= textureGrad(envarray, vec3( vec2(m31 * uv1), 0),  vec2(m31 * dpdx), vec2(m31 * dpdy));
            fragment_color+= color0;
        }
    }
    
    //~ fragment_color= 0.5*fragment_color.aaaa;
    fragment_color/= fragment_color.a;
    fragment_color.rgb*= 1.5;
    //~ fragment_color= texture(envmap, vertex_direction, bias);
    //~ fragment_color= texture(envmap, vec3(1, 1, 1));
    //~ fragment_color= vec4(1, 1, 0, 1);
    
}
#endif
