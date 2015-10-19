uniform float u_time;
uniform float u_size_tex;

uniform sampler3D densities;

float getPotential(vec3 position, float t)
{
	float ret = textureLod(densities, position, 0).r;
	
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
	/*float size = 20.0;
	vec3 p = (position-0.5)*size;
	
	//SinCard
	float y = 1.0;
	float d = (abs(sin(0.5*t))+0.5) * sqrt( p.x*p.x + p.z*p.z );
	if (d!=0)
		y = sin(d)/(d);
	y*=10.0;
	
	float ret = 0;
	if (p.y < y)
		ret = 1;*/
	
	return ret;
}