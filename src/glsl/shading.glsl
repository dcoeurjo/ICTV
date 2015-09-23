#version 430

#ifdef VERTEX_SHADER

in vec4 position;
out vec3 vertex_position;

void main( )
{
    vertex_position = position.xyz;
    gl_Position = position;
}
#endif

#ifdef GEOMETRY_SHADER

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vertex_position[];
out vec3 geometry_position;
out vec3 geometry_normal;
out vec3 geometry_view;
out vec3 geometry_distance;
out vec3 geometry_color;

void main()
{
	vec3 pts[3];
	pts = vec3[3](( vertex_position[0] - 0.5 ) * u_scene_size, ( vertex_position[1] - 0.5 ) * u_scene_size, ( vertex_position[2] - 0.5 ) * u_scene_size);
	
	vec3 pts_abs[3];
	pts_abs = vec3[3](( vertex_position[0]) * u_scene_size, ( vertex_position[1]) * u_scene_size, ( vertex_position[2] ) * u_scene_size);

	vec4 transformed[3];
	transformed[0] = u_transforms.modelviewprojection * vec4(pts[0], 1 );
	transformed[1] = u_transforms.modelviewprojection * vec4(pts[1], 1 );
	transformed[2] = u_transforms.modelviewprojection * vec4(pts[2], 1 );
	
	vec2 p0 = u_viewport * transformed[0].xy / transformed[0].w;
	vec2 p1 = u_viewport * transformed[1].xy / transformed[1].w;
	vec2 p2 = u_viewport * transformed[2].xy / transformed[2].w;
	
	vec2 v[3] = vec2[3](p2 - p1, p2 - p0, p1 - p0);
	float area = abs (v[1].x*v[2].y - v[1].y*v[2].x);
	
	for(int i =0; i<3; i++)
	{
		geometry_distance = vec3(0);
		geometry_distance[i] = area * inversesqrt (dot (v[i],v[i]));
		gl_Position = transformed[i];
		geometry_position = vertex_position[i].xyz;
		geometry_view = (u_transforms.modelview * vec4( ((vertex_position[i]-0.5)*u_scene_size), 1 )).xyz;
		vec3 center = (pts_abs[1] + pts_abs[2] + pts_abs[0]) / 3;
		gl_PrimitiveID = int( length(center)*1000 );
		EmitVertex();
	}
	
	EndPrimitive();
}

#endif

#ifdef FRAGMENT_SHADER

layout(early_fragment_tests) in;

in vec3 geometry_position;
in vec3 geometry_distance;
in vec3 geometry_view;

out vec4 fragment_color;

uniform int textured;
uniform int solid_wireframe;

uniform sampler2D u_texcolor_x;
uniform sampler2D u_texcolor_y;
uniform sampler2D u_texcolor_z;

uniform sampler2D u_texbump_x;
uniform sampler2D u_texbump_y;
uniform sampler2D u_texbump_z;

uniform vec3 u_camera_pos;

float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (3.1415 * 8.0);
    return pow(max(dot(normalize(reflect(e,n)),normalize(l)),0.0),s) * nrm;
}
float diffuse(vec3 n,vec3 l,float p) {
    return pow(clamp(dot(normalize(n),normalize(l)), 0.3, 1.0), p);
}

vec3 perturb_normal(vec3 surf_pos, vec3 normal, float h)
{
	vec3 sigmaS = dFdx(surf_pos);
	vec3 sigmaT = dFdy(surf_pos);
	vec3 vn = normal;
	
	vec3 vr1 = cross(sigmaT, vn);
	vec3 vr2 = cross(vn, sigmaS);
	
	float det = dot(sigmaS, vr1);
	
	float dBs = dFdx( h );
	float dBt = dFdy( h );
	
	vec3 grad = sign(det) * (dBs * vr1 + dBt * vr2);
	return normalize( abs(det)*vn - grad );
	
}

void main( )
{
	/**Normal**/
	vec3 geometry_normal = getNormal(geometry_position, 0.005); 
	//vec3 geometry_normal = 1*getNormal(geometry_position, (1.0/(512.0*500))*length(geometry_view));
	//vec3 geometry_normal = -1 * normalize(cross( dFdx(geometry_position.xyz), dFdy(geometry_position.xyz)));

	/**Color**/
	float h = 0;
	
	/*float brightness = 0.2;
	vec3 color_levels[6] = vec3[6] (
		vec3 (1, brightness, brightness),
		vec3 (brightness, 1, brightness),
		vec3 (brightness, brightness, 1),
		vec3 (brightness, 1, 1),
		vec3 (1, 1, brightness),
		vec3 (1, brightness, 1)
	);
	vec4 tmp_color = vec4(color_levels[uint(gl_PrimitiveID)%6], 1);// vec4(0.4, 0.8, 0.7, 1);*/
	
	vec4 tmp_color = vec4(geometry_normal, 1);
	if (textured == 1)
	{
		float tex_factor = 20;
		float bump_factor = 2;
		vec2 tex_x, tex_y, tex_z;
		vec3 weights = vec3(0);
		
		tex_x.x = geometry_position.y;
		tex_y.x = geometry_position.x;
		tex_z.x = geometry_position.x;
		
		tex_x.y = geometry_position.z;
		tex_y.y = geometry_position.z;
		tex_z.y = geometry_position.y;
		
		float delta = 0.1;
		float mw = 1;
		float normalization = 0;

		if (geometry_position.x < 0)
		        tex_x.x *= -1;
		if (geometry_position.y >= 0)
		        tex_y.x *= -1;
		if (geometry_position.x < 0)
		        tex_z.x *= -1;
		        
		weights.x = pow( max(abs(geometry_normal.x) - delta, 0.0), mw);
		weights.y = pow( max(abs(geometry_normal.y) - delta, 0.0), mw);
		weights.z = pow( max(abs(geometry_normal.z) - delta, 0.0), mw);
		
		normalization = weights.x + weights.y + weights.z;
		weights /= normalization;
		
		/*h =  texture(u_texbump_x, tex_x*bump_factor).r * weights.x;
		h += texture(u_texbump_y, tex_x*bump_factor).r * weights.x;
		h += texture(u_texbump_z, tex_z*bump_factor).r * weights.z;
		
		geometry_normal = perturb_normal(geometry_position, geometry_normal, h);
		
		weights.x = pow( max(abs(geometry_normal.x) - delta, 0.0), mw);
		weights.y = pow( max(abs(geometry_normal.y) - delta, 0.0), mw);
		weights.z = pow( max(abs(geometry_normal.z) - delta, 0.0), mw);
		
		normalization = weights.x + weights.y + weights.z;
		weights /= normalization;*/
	  
		tmp_color =  texture(u_texcolor_y, tex_x*tex_factor) * weights.x;
		tmp_color += texture(u_texcolor_z, tex_y*tex_factor) * weights.y;
		tmp_color += texture(u_texcolor_y, tex_z*tex_factor) * weights.z;
	}
	
	if (fromtexture == 1)
	{
		vec3 light_dir = vec3(1, -1, -1);
		float w = 0.5;
		fragment_color = pow( clamp(dot(normalize(light_dir), normalize(geometry_normal)), 0.0, 1.0), 1) * w * tmp_color + tmp_color * (1-w);
	}
	else
	{
	
	/**Ocean**/
	/*https://www.shadertoy.com/view/Ms2SD1*/
	
	/*const vec3 SEA_BASE = vec3(0.2,0.4,0.4);
	const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6);
	
	vec3 sun_pos = vec3(0.5, 2, -2);
	vec3 real_pos = (geometry_position - 0.5) * u_scene_size;
	vec3 cam_dir = normalize(geometry_view);
	vec3 light_dir = normalize(sun_pos - geometry_position);
	
	float fresnel = 1.0 - max(dot(geometry_normal,-cam_dir),0.0);
	fresnel = pow(fresnel,3.0) * 0.65;
		
	vec3 reflected = vec3(1);//getSkyColor(reflect(eye,n));    
	vec3 refracted = SEA_BASE + diffuse(geometry_normal,light_dir,20.0) * SEA_WATER_COLOR * 0.12; 
	
	vec3 color = mix(refracted,reflected,fresnel);
	color += SEA_WATER_COLOR * 0.18 * (geometry_position.y - 0.6);
	color += vec3(specular(geometry_normal,light_dir,cam_dir,20.0));
	
	fragment_color = vec4(color, 1);*/
	
	
	/**Metaballs**/
	
		vec3 real_pos = (geometry_position - 0.5) * u_scene_size;
		vec3 cam_dir = normalize(geometry_view);
		vec3 light_dir = vec3(1);
			
		vec3 color = 0.7 * diffuse(geometry_normal, light_dir, 1) * vec3(1, 0, 0);
		color += 0.5 * vec3(specular(geometry_normal,light_dir,cam_dir,20.0));
		
		fragment_color = vec4(color, 1);
	}


	if (solid_wireframe == 1)
	{
		const float wirescale = 0.5; // scale of the wire
		vec3 d2 = geometry_distance * geometry_distance;
		float nearest = min (min (d2.x, d2.y), d2.z);
		float f = exp2 (-nearest / wirescale);
		fragment_color = mix (fragment_color, vec4(0), f);
	}
}
#endif
