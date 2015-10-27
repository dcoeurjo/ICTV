#define TRANSFORMS_BINDING 0
#define FRUSTUM_BINDING 1

#ifndef SQRT_2
#define SQRT_2 1.414213562
#endif
#ifndef PI
#	define PI 3.14159265
#endif

/** Density **/
//if not read from a texture, choose the function to apply
#define SPHERE 0
#define PLANE 0
#define COSSIN_PLANE 0
#define COS_PLANE 0
#define HIGH_FREQ_PLANE 0
#define PLANET 0
#define PINK_NOISE 0
#define CONVOLUTION_TEST 0
#define JULIA 1
#define BALLS 0

#define TR_SIZE 0.4

// ---------------------------------------------------------
// uniforms 

layout (std140, binding = FRUSTUM_BINDING)
uniform ViewFrustumPlanes {
	vec4 u_planes[6];         // frustum planes of the camera
};

layout (std140, binding = TRANSFORMS_BINDING)
uniform Transforms {
	mat4 modelview;
	mat4 projection;
	mat4 modelviewprojection;
	mat4 invmodelview;
} u_transforms;

struct cell
{
	uvec4 tr_code_id_cw;
	vec3 corner;
	vec3 scales;
	vec4 neighbours;
	uint level;
	vec4 block_info;
};

uniform vec3 u_scene_size; // scale of the scene
uniform float u_tan_fovy;   // tan of half the fovy
uniform vec2 u_viewport; // viewport width and height
uniform int fromtexture;

uniform float u_isosurface; //isovalue
uniform int u_tessellation; //voxels to blocks of u_tessellation^3
uniform float u_scale; //octree levels length
uniform int u_radial; //use radial distance for LoD

uint max_level = min( uint( floor(log2(u_size_tex)) ), 8u ); //max octree level; cannot be splitted more than this level
float max_tex = max_level;


// -------------------------------------------------
// Utils

void loadCube(vec3 corner, vec3 scales, out vec3 corners[8])
{
	for(int i=0; i<8; i++)
	{
		uint x = i & 0x01u;
		uint z = (i & 0x02u) >> 1u;
		uint y = (i & 0x04u) >> 2u;

		vec3 move = vec3(x, y, z);

		corners[i] = vec3(move * scales) + corner.xyz;
	}
}

mat3 loadRotationMatrix(vec3 u, vec3 v)
{
	mat3 res;
	
	u = normalize(u);
	v = normalize(v);
	
	vec3 a = cross(u, v);
	float s = a.length();
	float c = dot(u, v);
	a = normalize(a);
	
	res[0] = vec3(a.x*a.x + (1 - a.x*a.x)*c, a.x*a.y*(1-c) - a.z*s, a.x*a.z*(1-c) + a.y*s);
	res[1] = vec3(a.x*a.y*(1-c) - a.z*s, a.y*a.y + (1 - a.y*a.y)*c, a.y*a.z*(1-c) + a.x*s);
	res[2] = vec3(a.x*a.z*(1-c) - a.y*s, a.y*a.z*(1-c) + a.x*s, a.z*a.z + (1 - a.z*a.z)*c);
	
	return res;
}

// ---------------------------------------------------------
// Frustum culling

vec3 nvertex (vec3 m, vec3 M, vec3 n) {
	bvec3 s = greaterThanEqual (n, vec3 (0));
	return mix (m, M, s);
}
// cull http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
bool octree_frustum_test (vec3 bmin, vec3 bmax) {
	int i = 0;
	float a = 1.0;
	vec3 n;

	for (; i<6 && a>=0.0; ++i) {
		// compute negative vertex
		n = nvertex (bmin, bmax, u_planes[i].xyz);
		a = dot (u_planes[i].xyz, n) + u_planes[i].w;
	}
	return (a >= 0.0); // true if visible, false otherwise
}

//-----------------------------------------------------------
// Interpolation
float linear_interpolation( float mu )
{
	return mu;
}
float gaussian_interpolation( float mu )
{
	return exp( -((1-mu)*(1-mu)) / (2*0.3*0.3));
}
float cosine_interpolation( float mu )
{
	return (1 - cos(mu*3.14159))/2.0;
}
float interpolate( float mu )
{
	return linear_interpolation(mu);
}

//----------------------------------------------------------------
// Offset handling
float getLength(vec3 n) //TODO parameterer changement metrique
{
	float angle = dot(normalize(n), normalize(vec3(0, 0, -1))); // entre -1 et 1
	
	float rad_fovy = atan(u_tan_fovy)*2;
	float rad_angle = abs( acos(angle) );
	
	float l = length(n);
	float l_off = 0;
	
	float w = 6;
	
	if ( (u_radial == 1) && rad_angle > rad_fovy) //if out of frustum
		l_off = w*l*(rad_angle-rad_fovy);
	
	return l + l_off;
}

//-----------------------------------------------------------------
// Get the texture level corresponding to the octree level at one point
float getLevel(vec3 pts)
{
	//computes point position
	pts = pts - 0.5;
	pts.x *= u_scene_size.x;
	pts.y *= u_scene_size.y;
	pts.z *= u_scene_size.z;
	
	//Computes octree level at this point
	vec3 mv = (u_transforms.modelview * vec4 (pts, 1)).xyz;
	float d = getLength(mv) * u_tan_fovy;
	float level_x = log2((u_scene_size.x * u_scale) / d);
	float level_y = log2((u_scene_size.y * u_scale) / d);
	float level_z = log2((u_scene_size.z * u_scale) / d);
	float level = min(level_x, min(level_y, level_z)) + 1;
	if (level < 0)
		return 0;
	
	return level + 2;
}

float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}
float seawaves(vec3 p)
{
	p.x *= u_scene_size.x;
	p.y -= 0.5;
	p.y *= u_scene_size.y;
	p.z *= u_scene_size.z;
	mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);
	float freq = 0.16;
	float amp = 0.6;
	float choppy = 4.0;
	vec2 uv = p.xz; uv.x *= 0.75;
	float d, h = 0.0;    
	for(int i = 0; i < 5; i++) {        
		d = sea_octave((uv+u_time)*freq,choppy);
		d += sea_octave((uv-u_time)*freq,choppy);
		h += d * amp;        
		uv *= octave_m; freq *= 2; amp *= 0.25;
		choppy = mix(choppy,1.0,0.2);
	}
	return p.y - h;
}


float hash1( float n )
{
    return fract(sin(n)*43758.5453123);
}

vec2 hash2( float n )
{
    return fract(sin(vec2(n,n+1.0))*vec2(43758.5453123,22578.1459123));
}

vec3 hash3( float n )
{
    return fract(sin(vec3(n,n+1.0,n+2.0))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

float getVolume(vec3 position) //returns the value for a point in space fetching it from mipmapped texture
{
	float isovalue = 0;
	float kernel = getPotential(position, u_time, 0);//, isovalue, true);
	float ret = ( kernel - isovalue );
	return kernel;
}

float getVolume0(vec3 position) //returns the value for a point in space fetching it from the lowest mipmap of the texture
{
	float isovalue = 0;
	float kernel = getPotential(position, u_time, 0);// isovalue, false);
	float ret = ( kernel - isovalue );
	return ret;
}

uint isInSurface(vec3 position, bool tessel)
{    
    float d = getVolume(position);
    
    if (d <= 0)
        return 1u;
    else
        return 0u;
}

// --------------------------------------------------------
// Ambiant Occlusion

//USABLE WHEN THE TEXTURE ONLY CONTAINS O OR 1 FOR INSIDE AND OUTSIDE
float getVolumeAmbiant(vec3 position) 
{
	uint mip = uint(max_tex);
	vec3 p = clamp(position, vec3(0), vec3(1));
	
	float tree_level = getLevel(position) + 1;
	tree_level = clamp(tree_level, 0, mip-2);

	float level = 0;//mip - tree_level;
	
	float ao5 = textureLod(densities, p, level+5).r;
	float ao4 = textureLod(densities, p, level+4).r;
	float ao3 = textureLod(densities, p, level+3).r; // caves
	float ao2 = textureLod(densities, p, level+2).r;
	float ao1 = textureLod(densities, p, level+1).r; // steps
	//float ao0 = textureLod(densities, p, level).r;
	
	ao4 *= 3;
	float ret = clamp( ao4*ao4, 0, 1 );
	ret += ao1 * (1-ao4);
	
	ret = clamp( 1-ret, 0, 1 );
	
	return ret;
}

// -----------------------------------------------------------
// Get the normal from Finite differences

vec3 getNormal(vec3 p, float s)
{
	float  l = getLevel(p);
	float epsilon = s;
	vec3 nxt, prv;
	
	nxt.x = getVolume0(vec3(p.x+epsilon, p.y, p.z));
	nxt.y = getVolume0(vec3(p.x, p.y+epsilon, p.z));
	nxt.z = getVolume0(vec3(p.xy, p.z+epsilon));

	prv.x = getVolume0(vec3(p.x-epsilon, p.yz));
	prv.y = getVolume0(vec3(p.x, p.y-epsilon, p.z));
	prv.z = getVolume0(vec3(p.xy, p.z-epsilon));

	vec3 grad = (nxt - prv) / (2.0*epsilon);
	
	return normalize(grad);
	
	/*vec3 n;
	float eps = 0.00001;//clamp( 0.0001 * s, 0., 0.1);
	n.y = seawaves(p);    
	n.x = seawaves(vec3(p.x+eps,p.y,p.z)) - n.y;
	n.z = seawaves(vec3(p.x,p.y,p.z+eps)) - n.y;
	n.y = eps;
	return normalize(n);*/
}

// --------------------------------------------------------------------------------
// Transition Cells utils

void buildTransitionCell(vec3 neighboursSizes, vec3 voxel[8], out vec3 transition_cell[13])
{
	if (neighboursSizes.x == -1)
	{
		transition_cell[0] = voxel[0];
		transition_cell[2] = voxel[2];
		transition_cell[6] = voxel[4];
		transition_cell[8] = voxel[6];
	}
	else if (neighboursSizes.x == 1)
	{
		transition_cell[0] = voxel[3];
		transition_cell[2] = voxel[1];
		transition_cell[6] = voxel[7];
		transition_cell[8] = voxel[5];
	}

	if (neighboursSizes.y == -1)
	{
		transition_cell[0] = voxel[3];
		transition_cell[2] = voxel[2];
		transition_cell[6] = voxel[1];
		transition_cell[8] = voxel[0];
	}
	else if (neighboursSizes.y == 1)
	{
		transition_cell[0] = voxel[5];
		transition_cell[2] = voxel[4];
		transition_cell[6] = voxel[7];
		transition_cell[8] = voxel[6];
	}

	if (neighboursSizes.z == -1)
	{
		transition_cell[0] = voxel[1];
		transition_cell[2] = voxel[0];
		transition_cell[6] = voxel[5];
		transition_cell[8] = voxel[4];

	}
	else if (neighboursSizes.z == 1)
	{
		transition_cell[0] = voxel[2];
		transition_cell[2] = voxel[3];
		transition_cell[6] = voxel[6];
		transition_cell[8] = voxel[7];
	}

	transition_cell[9] = transition_cell[0];
	transition_cell[10] = transition_cell[2];
	transition_cell[11] = transition_cell[6];
	transition_cell[12] = transition_cell[8];

	transition_cell[1] = (transition_cell[0] + transition_cell[2]) * 0.5;
	transition_cell[3] = (transition_cell[0] + transition_cell[6]) * 0.5;
	transition_cell[5] = (transition_cell[2] + transition_cell[8]) * 0.5;
	transition_cell[7] = (transition_cell[6] + transition_cell[8]) * 0.5;

	transition_cell[4] = (transition_cell[0] + transition_cell[8]) * 0.5;
}

uint getCode_transitionCell(vec3 transition_cell[13])
{
    uint code = 0;
    
    //full resolution side
    //012
    code = (1u & isInSurface(transition_cell[0].xyz, true));
    code = code | (2u & (isInSurface(transition_cell[1].xyz, true) << 1u));
    code = code | (4u & (isInSurface(transition_cell[2].xyz, true) << 2u));
    //345
    code = code | (8u & (isInSurface( transition_cell[5].xyz, true) << 3u));
    code = code | (16u & (isInSurface( transition_cell[8].xyz, true) << 4u));
    code = code | (32u & (isInSurface( transition_cell[7] .xyz, true) << 5u));
    //678
    code = code | (64u & (isInSurface( transition_cell[6].xyz, true) << 6u));
    code = code | (128u & (isInSurface( transition_cell[3].xyz, true) << 7u));
    code = code | (256u & (isInSurface( transition_cell[4].xyz, true) << 8u));
    
    return code;
}

// ------------------------------------------------------------------------
// Cells deformation with transition cells insertion

vec3 near_in_block(vec3 pos, float s)
{
	vec3 n = vec3(0);
	for(int i=0; i<3; i++)
	{
		if (pos[i] == 0)
			n[i] = -1;
		else if (abs (pos[i] - s) < 0.00001)
			n[i] = 1;
	}
	return n;
}

bool do_offset(vec3 neighbours, vec3 pos, float s)
{
	vec3 n = near_in_block(pos, s);
	for(int i=0; i<3; i++)
	{
		if (n[i] != 0 && n[i] != neighbours[i])
			return false;
	}
	return true;
}

vec3 compute_offset(vec3 neighbours, vec3 pos, float s, vec3 N)
{
	float size = TR_SIZE;
	return vec3(0);

	vec3 n = near_in_block(pos, s);
	vec3 off = vec3(0);
	for(int i=0; i<3; i++)
	{
		if (n[i] < 0)
			off[i] = size;
		else if(n[i] > 0)
			off[i] = -size;
	}

	mat3 m;
	m[0] = vec3(1 - N.x*N.x, -N.x*N.y, -N.x*N.z);
	m[1] = vec3(- N.x*N.y, 1 -N.y*N.y, -N.y*N.z);
	m[2] = vec3( N.x*N.z, -N.y*N.z, 1 -N.z*N.z);

	return m*off;
}
