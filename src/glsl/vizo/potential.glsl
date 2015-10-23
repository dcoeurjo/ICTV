uniform float u_time;
uniform float u_size_tex;

uniform sampler3D densities;

float getPotential(vec3 position, float t)
{
	//float ret = textureLod(densities, position, 0).r;
	
	//Wave
	/*float s = 20;
	float A = s * 0.1;
    float w = 2;
    float theta = 3.14159 / 4.0;
	float sin_t = sin(theta);
	float cos_t = cos(theta);
	
	float time = t*0.5;
	
	vec2 k = vec2(cos_t, sin_t);
	
	float phi = dot(k,(position.xz)*s) - w*time;
	
    float y = A * cos(phi);
	
	float ret = 0;
	if ((position.y - 0.5)*s < y)
		ret = 1;*/
	
	//Sinc
	float size = 20.0;
	vec3 p = (position-0.5)*size;
	
	float y = 1.0;
	float d = (abs(sin(0.5*t))+0.5) * sqrt( p.x*p.x + p.z*p.z );
	if (d!=0)
		y = sin(d)/(d);
	y*=10.0;
	
	float ret = 0;
	if (p.y < y)
		ret = 1;
		
	//Goursat
	/*vec3 abc = vec3(0, abs(sin(u_time))*-2, -1);
	float size = 4.0;
	
	vec3 xyz = (position.xyz-0.5)*size;
	vec3 xyz2 = vec3(xyz.x*xyz.x, xyz.y*xyz.y, xyz.z*xyz.z);
	vec3 xyz4 = vec3(xyz2.x*xyz2.x, xyz2.y*xyz2.y, xyz2.z*xyz2.z);
	
	float y = xyz4.x + xyz4.y + xyz4.z;
	y += abc.x * (xyz2.x + xyz2.y + xyz2.z) * (xyz2.x + xyz2.y + xyz2.z);
	y += abc.y *  (xyz2.x + xyz2.y + xyz2.z);
	y += abc.z;
	
	float ret = 1;
	if (xyz.y < y)
		ret = 0;*/
		
	//Leopold
	/*float a = abs(sin(u_time))*30000;
	float size = 1.0;
	
	vec3 xyz = (position.xyz-0.5)*size;
	float y = a * xyz.x*xyz.x*xyz.y*xyz.y*xyz.z*xyz.z;
	y += 3*xyz.x*xyz.x;
	y += 3*xyz.y*xyz.y;
	y += xyz.z*xyz.z;
	
	float ret = 1;
	if (xyz.y < y)
		ret = 0;*/
	
	return ret;
}