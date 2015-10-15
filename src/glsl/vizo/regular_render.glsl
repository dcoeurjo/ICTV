#version 430

uniform usampler1D code_class;
uniform usampler2D code_vertices;
uniform usampler2D class_triangles;

// ---------------------------------------------------------
// Vertex shader

struct tgl_vertices
{
	vec4 vertices_list[12];
};

struct indirect_draw
{
	uint vertex_count;
	uint instance_count;
	uint first_index;
	uint base_vertex;
	uint base_instance;
};

#ifdef VERTEX_SHADER
layout (location = 0) in uvec4 i_key; // translation + scale
layout (location = 1) in vec4 neighboursCell;

layout (binding = 2, std430) buffer vertices
{
	tgl_vertices verts[];
};
layout (binding = 3, std430) buffer indirect_draws
{
	indirect_draw cmd[];
};

uint getCode_regularCell(vec3 corners[8])
{
	uint caseCode = 0u;

	uint d = 1u;
	for(uint i=0; i<8; i++)
	{
		caseCode |= (isInSurface(corners[i], true) << i) & d;
		d*=2u;
	}

	return caseCode;
}

vec3 getVerticesPosition_regularCell(vec3 corners[8], uint edge)
{
    uint corner_ids = edge & 0xFFu;
    uint corner_id_2 = corner_ids & 0x0Fu;
    uint corner_id_1 = corner_ids >> 4u;
    
    if (corner_id_1 == corner_id_2 && corner_id_2 == 0)
        return vec3(-1, 0, 0);

    vec3 pos_orig = corners[corner_id_1];
    vec3 pos_end = corners[corner_id_2];
    
    float d0 = getVolume(corners[corner_id_1]);
    float d1 = getVolume(corners[corner_id_2]);
    
    float t = 0.5;//d1 / (d1 - d0);
    vec3 pos_displ = interpolate(t)*pos_end + (1.0-interpolate(t))*pos_orig;

    return pos_displ;
}

void main (void) 
{
	// extract octree position and scale
	vec3 c; //00 corner
	float s;
	lt_cell_3_20 (i_key.xy, c, s);
	
	/**Fetch Code, corners etc ... **/
	
	cell voxel;
	voxel.scales = vec3(s / float(u_tessellation));
	
	uint x = gl_InstanceID % u_tessellation;
	uint z = gl_InstanceID / (u_tessellation*u_tessellation);
	uint y = (gl_InstanceID % (u_tessellation*u_tessellation)) / u_tessellation;
	voxel.corner = c + uvec3(x, y, z)*voxel.scales;
	
	vec3 corners[8];
	loadCube(voxel.corner, voxel.scales, corners);
	
	voxel.tr_code_id_cw[0] = gl_InstanceID;
	voxel.tr_code_id_cw[1] = getCode_regularCell(corners);
	voxel.tr_code_id_cw[2] = texture(code_class, ( float(voxel.tr_code_id_cw[1])/255.0 )).r;
	voxel.level = lt_level_3_20(i_key.xy);
	voxel.neighbours = neighboursCell;
	voxel.block_info.xyz = c;
	voxel.block_info.w = s;
	
	int max_instance_id = u_tessellation*u_tessellation*u_tessellation;
	int buffer_id = gl_VertexID * max_instance_id + gl_InstanceID;
	
	int max_vertices = 12;
	int max_indices = 15; //5 tgl max * 3
	
	/** Determines Vertices and Indices **/
	
	indirect_draw current_cmd;
	tgl_vertices current_vertices;
	
	if (voxel.tr_code_id_cw[2] != 0)
	{
		vec4 data[12];
		int nb_indices = 0;
		
		/**Get Vertices**/
		for(int i=0; i<4; i++)
		{
			uvec3 edges = texture(code_vertices, vec2(float(voxel.tr_code_id_cw[1])/255.0, i*0.25 + 0.125)).rgb;

			vec3 off = vec3(0);
			vec3 pos = getVerticesPosition_regularCell(corners, edges.r); 
			if(pos.x == -1)
				break;
			data[i*3].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (do_offset(voxel.neighbours.xyz, pos-c, s))
				off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[i*3].xyz);
			current_vertices.vertices_list[i*3] = vec4(pos + off*voxel.scales, 1);

			off = vec3(0);
			pos = getVerticesPosition_regularCell(corners, edges.g); 
			if(pos.x == -1)
				break;
			data[i*3+1].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (do_offset(voxel.neighbours.xyz, pos-c, s))
				off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[i*3+1].xyz);
			current_vertices.vertices_list[i*3+1] = vec4(pos + off*voxel.scales, 1);
			
			off = vec3(0);
			pos = getVerticesPosition_regularCell(corners, edges.b); 
			if(pos.x == -1)
				break;
			data[i*3+2].xyz = vec3(0, 1, 0);//getNormal(pos, s);
			if (do_offset(voxel.neighbours.xyz, pos-c, s))
				off = compute_offset(voxel.neighbours.xyz, pos-c, s, data[i*3+2].xyz);
			current_vertices.vertices_list[i*3+2] = vec4(pos + off*voxel.scales, 1);
		}
		
		if (voxel.tr_code_id_cw[2] == 1)
			nb_indices = 3;
		else if (voxel.tr_code_id_cw[2] < 4)
			nb_indices = 6;
		else if (voxel.tr_code_id_cw[2] < 7)
			nb_indices = 9;
		else if (voxel.tr_code_id_cw[2] < 14)
			nb_indices = 12;
		else
			nb_indices = 15;
		
		/**Get indices count**/
		current_cmd.vertex_count = nb_indices;
		current_cmd.instance_count = 1;
		current_cmd.first_index = (voxel.tr_code_id_cw[2]-1)*max_indices;
		current_cmd.base_vertex = buffer_id*max_vertices;
		current_cmd.base_instance = 0;
		
	}
	else
	{
		current_cmd.vertex_count = 0;
		current_cmd.instance_count = 0;
		current_cmd.first_index = 0;
		current_cmd.base_vertex = 0;
		current_cmd.base_instance = 0;
	}
	
	verts[buffer_id] = current_vertices;
	cmd[buffer_id] = current_cmd;
}
#endif
