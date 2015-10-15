#version 430

#ifdef VERTEX_SHADER

in vec4 position;

out vec3 vertex_position;
out vec3 vertex_color;

out float curv_value;
out vec3 curv_dir_min;
out vec3 curv_dir_max;
out vec3 curv_normale;
out vec3 eigenvalues;
out vec3 covmatDiag;
out vec3 covmatUpper;
out vec3 vertex_k1_k2;

uniform vec3 u_scene_size;
uniform float u_lvl;

void main( )
{
    vertex_position = position.xyz;

	/*curvature from regular integration*/
	float volume = 0.0;
	vec3 xyz2 = vec3(0);
	vec3 xy_yz_xz = vec3(0);
	vec3 xyz = vec3(0);

	float volume_approx = 0.0;
	getVolumeMoments(vertex_position, volume, xyz, xy_yz_xz, xyz2, u_lvl);

	float k1;
	float k2;
	curv_dir_min = vec3(0, 0, 1);
	curv_dir_max = vec3(0, 0, 1);
	curv_normale = vec3(0);
	computeK1K2(volume, u_curv_radius,
				xyz2, xy_yz_xz, xyz,
				curv_dir_min, curv_dir_max, curv_normale, eigenvalues, k1, k2);
	
	float r = u_curv_radius;
	float fact83r = 8.0/(3.0*r);
	float fact4pir4 = 4.0 / (3.14159*r*r*r*r);
	float curvature = fact83r - fact4pir4*volume;
	
	curv_value = 0;
	if(u_curv_val == 1)
		curv_value = (k1+k2)/2.0;
	else if(u_curv_val == 2)
		curv_value = (k1*k2);
	else if(u_curv_val == 3)
		curv_value = k1;
	else if(u_curv_val == 4)
		curv_value = k2;
		
	vertex_k1_k2.x = curvature;
	vertex_k1_k2.y = k1;
	vertex_k1_k2.z = k2;
	
	/*
	covmatDiag[0] = xyz2.x - (xyz.x*xyz.x/volume); 
	covmatDiag[1] = xyz2.y - (xyz.y*xyz.y/volume);
	covmatDiag[2] = xyz2.z - (xyz.z*xyz.z/volume);
	
	float covxy = xy_yz_xz.x - (xyz.x*xyz.y/volume);
	float covyz = xy_yz_xz.y - (xyz.y*xyz.z/volume);
	float covxz = xy_yz_xz.z - (xyz.x*xyz.z/volume);
	covmatUpper[0] = covxy; 
	covmatUpper[1] = covyz;
	covmatUpper[2] = covxz;
	*/
		
	/*float r = u_curv_radius;
	float fact83r = 8.0/(3.0*r);
	float fact4pir4 = 4.0 / (3.14159*r*r*r*r);
	float curvature = fact83r - fact4pir4*volume;
	curv_value = curvature;*/

	//xyz = textureLod(u_xyz_tex, vertex_position, 0).rgb;
	//vertex_color = vec3(volume/10000.0, 0, 1);
	//vertex_color = xyz2/(65.0*65.0);
	
	//vertex_color = vec3(volume, 0, 1);
	vertex_color = xyz;
	
    gl_Position = position;
}
#endif

#ifdef GEOMETRY_SHADER

#define TRANSFORMS_BINDING 0

layout (triangles) in;
layout (triangle_strip, max_vertices = 10) out;

uniform int u_curv_dir;

in vec3 vertex_k1_k2[];
in vec3 vertex_position[];
in vec3 vertex_color[];
in vec3 curv_dir_max[];
in vec3 curv_dir_min[];
in vec3 curv_normale[];
in float curv_value[];
/*
in vec3 eigenvalues[];
in vec3 covmatDiag[];
in vec3 covmatUpper[];
*/

out vec3 geometry_k1_k2;

out vec4 geometry_min_dir;
out vec4 geometry_max_dir;
out vec4 geometry_position;
out vec4 geometry_normale;

out vec3 geometry_egv;
out vec3 geometry_covmatDiag;
out vec3 geometry_covmatUpper;

out vec3 geometry_distance;
out vec3 geometry_color;
out float geometry_curv_value;
out flat int geometry_curvdir;

layout (std140, binding = TRANSFORMS_BINDING)
uniform Transforms {
	mat4 modelview;
	mat4 projection;
	mat4 modelviewprojection;
	mat4 invmodelview;
} u_transforms;

uniform vec3 u_scene_size;
uniform vec2 u_viewport;

/*
void setPoint(vec3 point, vec3 color)
{
	geometry_curvdir = 1;
	gl_Position = u_transforms.modelviewprojection * vec4(point, 1 );
	geometry_position = vec4(point.xyz, 1);
	geometry_color = color;
}

void drawTetra( vec3 pts[3], vec3 mean_dir, vec3 color )
{
	vec3 normale = normalize(cross(normalize(pts[0]-pts[1]).xyz, normalize(pts[0]-pts[2]).xyz));
	float decalage = 3;
	float width = 0.1;
	
	vec3 p0 = pts[0].xyz + decalage*normale;
	vec3 p1 = p0 + mean_dir*5;
	vec3 p2 =pts[0].xyz  + (width*(pts[0].xyz - pts[1].xyz)) + decalage*normale;
	vec3 p3 = pts[0].xyz + width*(pts[0].xyz  - pts[2].xyz) + decalage*normale;
	
	setPoint(p0, color);
	EmitVertex();
	setPoint(p2, color);
	EmitVertex();
	setPoint(p1, color);
	EmitVertex();
	setPoint(p3, color);
	EmitVertex();
	
	EndPrimitive();
	
	p1 = p0 - mean_dir*5;
	
	setPoint(p0, color);
	EmitVertex();
	setPoint(p2, color);
	EmitVertex();
	setPoint(p1, color);
	EmitVertex();
	setPoint(p3, color);
	EmitVertex();
	
	EndPrimitive();
}


vec3 reorientNormal(vec3 pts[3], vec3 given_normale)
{
	vec3 normale = normalize(cross(normalize(pts[0]-pts[1]).xyz, normalize(pts[0]-pts[2]).xyz));
	if (dot(normalize(given_normale), normalize(normale)) < 0)
		given_normale *= -1;
	return given_normale;
}

void drawParallelpitruc( vec3 pts[3], vec3 mean_dir, vec3 color )
{		
		vec3 center_face = (pts[0].xyz+pts[1].xyz+pts[2].xyz)/3.0;
		vec3 normale = normalize(cross(normalize(pts[0]-pts[1]).xyz, normalize(pts[0]-pts[2]).xyz));
		vec3 tan_dir = normalize(pts[0]-pts[1]);
		vec3 depth = normalize(cross(normalize(tan_dir), normalize(mean_dir)));
		
		float l = 0.1* max( max(length(pts[0]-pts[2]), length(pts[0]-pts[1])), length(pts[1]-pts[2]) );
		float L = 10*l;
		float p = l;
		
		vec3 middle_geom = center_face + 0.5*L*normale;
		vec3 up_geom = middle_geom+mean_dir*0.5*L;
		vec3 bottom_geom = middle_geom-mean_dir*0.5*L;
		vec3 right_geom = middle_geom+tan_dir*0.5*l;
		vec3 left_geom = middle_geom-tan_dir*0.5*l;
		
		vec3 c0 = (right_geom+up_geom)/2.0;
		vec3 c1 = (left_geom+up_geom)/2.0;
		vec3 c2 = (right_geom+bottom_geom)/2.0;
		vec3 c3 = (left_geom+bottom_geom)/2.0;
		
		vec3 c4 = c0+p*depth;
		vec3 c5 = c1+p*depth;
		vec3 c6 = c2+p*depth;
		vec3 c7 = c3+p*depth;
		
		vec3 shade = color;
		setPoint(c0, shade);
		EmitVertex();
		setPoint(c1, shade);
		EmitVertex();
		setPoint(c2, shade);
		EmitVertex();
		setPoint(c3, shade);
		EmitVertex();
		
		setPoint(c7, shade);
		EmitVertex();
		setPoint(c1, shade);
		EmitVertex();
		setPoint(c5, shade);
		EmitVertex();
		setPoint(c4, shade);
		EmitVertex();
		
		setPoint(c7, shade);
		EmitVertex();
		setPoint(c6, shade);
		EmitVertex();
		setPoint(c2, shade);
		EmitVertex();
		
		setPoint(c4, shade);
		EmitVertex();
		setPoint(c0, shade);
		EmitVertex();
		setPoint(c1, shade);
		EmitVertex();
		
		
		EndPrimitive();
}
*/

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
		
		geometry_color = vertex_color[i];
		geometry_curv_value = curv_value[i];
		geometry_curvdir = 0;
		
		geometry_k1_k2 = vertex_k1_k2[i];
		
		geometry_min_dir = vec4(curv_dir_min[i], vertex_k1_k2[i].y);
		geometry_max_dir = vec4(curv_dir_max[i], vertex_k1_k2[i].z);
		
		geometry_position = vec4(vertex_position[i], vertex_k1_k2[i].x);
		geometry_normale = vec4(curv_normale[i], 1);//vec4(reorientNormal(pts, curv_normale[i]), 1);
		/*geometry_covmatDiag = covmatDiag[i];
		geometry_covmatUpper = covmatUpper[i];
		geometry_egv = eigenvalues[i];*/
		
		gl_Position = transformed[i];
		EmitVertex();
	}
	
	EndPrimitive();
	
	/*
	if (u_curv_dir == 1 || u_curv_dir == 3)
	{
		drawParallelpitruc(pts, curv_dir_min[0], vec3(0, 0, 1));
	}
	
	if (u_curv_dir == 2 || u_curv_dir == 3)
	{
		drawParallelpitruc(pts, curv_dir_max[0], vec3(1, 0, 0));
	}
	
	if (u_curv_dir == 4)
	{
		drawParallelpitruc(pts, curv_normale[0], curv_normale[0]);
	}
	*/
}

#endif


#ifdef FRAGMENT_SHADER

#define TRANSFORMS_BINDING 0

layout(early_fragment_tests) in;

in vec2 geometry_k1_k2;
in vec4 geometry_min_dir;
in vec4 geometry_max_dir;
in vec4 geometry_position;
in vec4 geometry_normale;

in vec3 geometry_distance;
in vec3 geometry_color;
in float geometry_curv_value;
in flat int geometry_curvdir;

uniform float u_kmin;
uniform float u_kmax;
uniform int solid_wireframe;
uniform int u_k1k2_normals;

out vec4 fragment_color;

layout (std140, binding = TRANSFORMS_BINDING)
uniform Transforms {
	mat4 modelview;
	mat4 projection;
	mat4 modelviewprojection;
	mat4 invmodelview;
} u_transforms;

vec3 HSVtoRGB(vec3 hsv)
{
  int i;
  double f, p, q, t;
  if( hsv.y == 0 ) {                     // achromatic (gray)
    return vec3(hsv.z);
  }
  i = int( floor( hsv.x / 60 ) );
  f = ( hsv.x / 60 ) - i;                        // factorial part of h
  p = hsv.z * ( 1.0 - hsv.y );
  q = hsv.z * ( 1.0 - hsv.y * f );
  t = hsv.z * ( 1.0 - hsv.y * ( 1.0 - f ) );

  if (i==0)
  	return vec3(hsv.z, t, p);
  if (i==1)
  	return vec3(q, hsv.z, p);
  if (i==2)
  	return vec3(p, hsv.z, t);
  if (i==3)
  	return vec3(p, q, hsv.z);
  if (i==4)
  	return vec3(t, p, hsv.z);

  return vec3(hsv.z, p, q);
}

vec3 colormap(float scale)
{
  float cycles = 1;
  const double hue = 360 * ( scale * cycles - floor(scale * cycles));
  return HSVtoRGB( vec3(hue, 0.9, 1.0) );
}

vec3 gradientmap( float scale )
{
  const int intervals = 2; //3 colors
  int upper_index = int( ceil( intervals * scale ) );
  if ( upper_index == 0 ) // Special case when value == min.
    upper_index = 1;
    
  vec3 colors[3];
  colors[0] = vec3(0, 0, 1);
  colors[1] = vec3(1, 0, 0);
  colors[2] = vec3(1, 1, 0);
  
  
  vec3 firstColor = colors[upper_index-1];
  vec3 lastColor = colors[upper_index];
	
  scale = ( scale * intervals ) - (upper_index - 1);

  return firstColor + scale * (lastColor - firstColor);
}

vec3 colorFromCurv(float c)
{
	vec3 color;
	float gt_curvature = c;
	if(u_kmax > u_kmin)
		gt_curvature = (c-u_kmin) / (u_kmax-u_kmin);

	if ((gt_curvature<0) || (gt_curvature>1)) color= vec3(0.5,0.5,0.5);
	else
		color= gradientmap(gt_curvature);
	
	return color;
}

void main( )
{
	vec3 color;
	if (geometry_curvdir == 0)
	{
		if (u_curv_val != 0)
			color = colorFromCurv(geometry_curv_value);
		else
			color = abs(geometry_color);
	}
	else
		color = abs(geometry_color);
	
	vec3 normale;
	if( u_k1k2_normals == 0 )
		normale = normalize(cross( dFdx(geometry_position.xyz), dFdy(geometry_position.xyz)));
	else
		normale = geometry_normale.xyz;
	
	//Phong
	vec3 light_dir = vec3(1, 1, 1);
	normale = (u_transforms.modelview * vec4(normale, 0)).xyz;
	
	float shadow_weight = 0;
	float dotnormal = clamp(dot(normalize(normale), normalize(light_dir.xyz)), 0, 1);
	fragment_color = vec4( shadow_weight * color * dotnormal + (1-shadow_weight) * color, 1);

	if (geometry_curvdir == 0 && solid_wireframe == 1)
	{
		const float wirescale = 0.5; // scale of the wire
		vec3 d2 = geometry_distance * geometry_distance;
		float nearest = min (min (d2.x, d2.y), d2.z);
		float f = exp2 (-nearest / wirescale);
		fragment_color = mix (fragment_color, vec4(0), f);
	}
}
#endif
