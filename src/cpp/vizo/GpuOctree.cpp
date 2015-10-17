#include "GpuOctree.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

int sizekey = 2;

//Cube data, used for cells drawing
const GLfloat g_cubeVertices[] = {  0.5f, -0.5f, -0.5f,   // 0 
					0.5f,  0.5f, -0.5f,   // 1
					0.5f,  0.5f,  0.5f,   // 2
					0.5f, -0.5f,  0.5f,   // 3
					-0.5f, -0.5f, -0.5f,   // 4
					-0.5f,  0.5f, -0.5f,   // 5
					-0.5f,  0.5f,  0.5f,   // 6
					-0.5f, -0.5f,  0.5f }; // 7
					   
const GLushort g_cubeIndexes[] = { 0,1,1,2,2,3,3,0,   // front
					1,5,5,6,6,2,2,1,   // right
					5,4,4,7,7,6,6,5,   // back
					4,0,0,3,3,7,7,4,   // left
					2,3,3,7,7,6,6,2,   // top
					0,4,4,5,5,1,1,0 }; // bottom
							

void streamFrustum (void) 
{
        #define STREAM_BUFFER_BYTE_SIZE (1 << 8)
        const int mapflags = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
        const int stream_byte_size = sizeof (Parameters::getInstance()->g_transforms);
        static int buffer_byte_offset = STREAM_BUFFER_BYTE_SIZE;
        static bool first = true;
        int buffer;
        float *map;

        // upload to GPU
        glGetIntegerv (GL_UNIFORM_BUFFER_BINDING, &buffer);
        glBindBuffer (GL_UNIFORM_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM]);
        if (first) { // first initialisation
                glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM]);
                glBindBuffer (GL_UNIFORM_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM]);
                glBufferData (GL_UNIFORM_BUFFER, STREAM_BUFFER_BYTE_SIZE, NULL, GL_STREAM_DRAW);
                first = false;
        }
        if (buffer_byte_offset + stream_byte_size > STREAM_BUFFER_BYTE_SIZE) {
                buffer_byte_offset = 0; // orphan
        }
        map = (float*)glMapBufferRange (GL_UNIFORM_BUFFER, buffer_byte_offset,
                                stream_byte_size, mapflags);
        memcpy (map, Parameters::getInstance()->g_frustum, stream_byte_size);
        glUnmapBuffer (GL_UNIFORM_BUFFER);
        glBindBuffer (GL_UNIFORM_BUFFER, buffer);
        glBindBufferRange (GL_UNIFORM_BUFFER,
                           BUFFER_FRUSTUM,
                           Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM],
                           buffer_byte_offset, stream_byte_size);
        buffer_byte_offset+= stream_byte_size;
}


void GPUOctree::loadBuffers()
{
        const int root[] = {0,0};
        //const size_t cap = 1 << 28;
		
		size_t full_tree = 1;
		long int side = std::pow(2, 8); //lvl 8 max
		full_tree += side*side*side; 
		
		size_t full_tree_cap = full_tree * sizeof(float) * 2*sizekey; //1 vec2 for each
		
		printf("LOD CAPACITY %lu (%lu)\n", full_tree, full_tree_cap);
		
		size_t tr_cells = 1;
		tr_cells = full_tree/100.0; //a lot less than tree cells
		printf("TR CAPACITY %lu\n", tr_cells);
		
		size_t tr_cells_cap = tr_cells * sizeof(float) * 2*sizekey; //1 vec2 for each
		
		size_t tr_neighbours = tr_cells * sizeof(float) * 4.0; //1 vec4 for each
		size_t full_neighbours = full_tree * sizeof(float) * 4.0; //1 vec4 for each
		
		glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM]);
			glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_FRUSTUM]);
			glBufferData (
					GL_ARRAY_BUFFER,
					8*4,
					NULL,
					GL_DYNAMIC_COPY
			);
		glBindBuffer (GL_ARRAY_BUFFER, 0);

        glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
			glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
			glBufferData (
					GL_ARRAY_BUFFER,
					full_tree_cap,
					NULL,
					GL_DYNAMIC_COPY
			);
			glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(root), root);
		glBindBuffer (GL_ARRAY_BUFFER, 0);

        glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]);
			glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]);
			glBufferData (
					GL_ARRAY_BUFFER,
					full_tree_cap,
					NULL,
					GL_DYNAMIC_COPY
			);
			glBindBuffer (GL_ARRAY_BUFFER, 0);
		
		glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1_TR]);
        glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1_TR]);
			glBufferData (
					GL_ARRAY_BUFFER,
					tr_cells_cap,
					NULL,
					GL_DYNAMIC_COPY
			);
			glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(root), root);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	
        glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2_TR]);
        glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2_TR]);
			glBufferData (
					GL_ARRAY_BUFFER,
					tr_cells_cap,
					NULL,
					GL_DYNAMIC_COPY
			);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
	
		glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_NEIGHBOURS]);
			glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_NEIGHBOURS]);
			glBufferData (
					GL_ARRAY_BUFFER,
					tr_neighbours,
					NULL,
					GL_DYNAMIC_COPY
			);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	
		glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_CODE]);
			glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_CODE]);
			glBufferData (
					GL_ARRAY_BUFFER,
					full_neighbours,
					NULL,
					GL_DYNAMIC_COPY
			);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	//To draw the octree's cells
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
	glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
	glBufferData (GL_ARRAY_BUFFER, sizeof (g_cubeVertices), g_cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDEX_CUBE]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_CUBE]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (g_cubeIndexes), g_cubeIndexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GPUOctree::loadVertexArrays()
{
	if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_EMPTY]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_EMPTY);
        if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE1]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_UPDATE1);
        if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE2]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_UPDATE2);
        if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER1]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER1);
        if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER2);
	if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER1_TR]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER1_TR);
        if (glIsVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2_TR]))
                glDeleteVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER2_TR);
	
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_EMPTY);
        glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_EMPTY]);
        glBindVertexArray (0);

        glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_UPDATE1);
        glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE1]);
                glEnableVertexAttribArray (0);
                glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
                glVertexAttribIPointer (0, sizekey, GL_UNSIGNED_INT, 0, 0);
	glBindVertexArray (0);
        glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_UPDATE2);
        glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE2]);
                glEnableVertexAttribArray (0);
                glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]);
                glVertexAttribIPointer (0, sizekey, GL_UNSIGNED_INT, 0, 0);
        glBindVertexArray (0);

        glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER1);
        glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER1]);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
                glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_CODE]);
                glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, 0);
	glBindVertexArray (0);
        
        glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER2);
        glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2]);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]);
                glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_CODE]);
                glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, 0);
	glBindVertexArray (0);
        
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER1_TR);
        glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER1_TR]);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1_TR]);
                glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_NEIGHBOURS]);
                glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, 0);
	glBindVertexArray (0);
        
        glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_LTREE_RENDER2_TR);
        glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2_TR]);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2_TR]);
                glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glEnableVertexAttribArray(1);
                glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_NEIGHBOURS]);
                glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, 0);
        glBindVertexArray(0);
	
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_OCTREE_RENDER1);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_OCTREE_RENDER1]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
		glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glVertexAttribDivisor(0, 1);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
		glVertexAttribPointer (1, 3, GL_FLOAT, 0, 0, 0);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_CUBE]);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
		glVertexAttribPointer (1, 3, GL_FLOAT, 0, 0, 0);
	glBindVertexArray(0);
	
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_OCTREE_RENDER2);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_OCTREE_RENDER2]);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]);
		glVertexAttribIPointer(0, sizekey, GL_UNSIGNED_INT, 0, 0);
		glVertexAttribDivisor(0, 1);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
		glVertexAttribPointer (1, 3, GL_FLOAT, 0, 0, 0);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_CUBE]);
	glBindVertexArray(0);
}

void GPUOctree::loadFeedback()
{
	glGenTransformFeedbacks(1, &(Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE1]));
	glGenTransformFeedbacks(1, &(Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE2]));
}

void GPUOctree::configurePrograms()
{
	/**LOD**/
        glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SCENE_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
        glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_TAN_FOVY],
                            tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SCALE],
                            Parameters::getInstance()->g_scale);
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_DENSITY],
                            TEXTURE_DENSITY);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], 
			    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_FROMTEXTURE], 
			    (int)Parameters::getInstance()->g_fromtexture);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], 
			    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_METRIC], 
			    (int)Parameters::getInstance()->g_radial_length);
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_TIME],
                            Parameters::getInstance()->g_time_elapsed);
    glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], 
                Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_REGULAR], 
                (int)Parameters::getInstance()->g_regular);

    glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SIZETEX],
                            Parameters::getInstance()->g_sizetex);
    

	/**CULLING**/
        glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SCENE_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TAN_FOVY],
                            tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SCALE],
                            Parameters::getInstance()->g_scale);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_DENSITY],
                            TEXTURE_DENSITY);
	glProgramUniform1i(Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TESSEL],
                            (int)Parameters::getInstance()->g_tessel);
        glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_ISOSURFACE],
                            Parameters::getInstance()->g_isosurface);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], 
			    Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_FROMTEXTURE], 
			    (int)Parameters::getInstance()->g_fromtexture);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], 
			    Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_METRIC], 
			    (int)Parameters::getInstance()->g_radial_length);
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TIME],
                            Parameters::getInstance()->g_time_elapsed);
    glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SIZETEX],
                            Parameters::getInstance()->g_sizetex);

	
	
	/**DISPLAY**/
	glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_DRAW_SCENE_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_DRAW_DENSITY],
                            TEXTURE_DENSITY);
        glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_TAN_FOVY],
                            tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
        glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_SCALE],
                            Parameters::getInstance()->g_scale);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], 
			    Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_FROMTEXTURE], 
			    (int)Parameters::getInstance()->g_fromtexture);
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_TIME],
                            Parameters::getInstance()->g_time_elapsed);

    
}

void GPUOctree::loadPrograms()
{
	/**LOD**/
	{
        const GLchar *varyings[] = {"o_data"};
        GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD];

        fprintf (stderr, "Loading octree LOD program... "); fflush (stderr);        //struct program_args_t *args = program_args_create ();
	
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("octree_lod.glsl"));
	   c.include(SHADER_PATH("noise.glsl") );
	   c.include(SHADER_PATH("octree_common.glsl") );
	   c.include(SHADER_PATH("ltree.glsl") );

        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        *program = tmp->name;
        glTransformFeedbackVaryings (*program, 1, varyings, GL_SEPARATE_ATTRIBS);
        glLinkProgram (*program);

        Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SCENE_SIZE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_scene_size");
        Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_TAN_FOVY] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_tan_fovy");
	    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SCALE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_scale");
        Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_DENSITY] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "densities");
	    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_FROMTEXTURE] =
		        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "fromtexture");
	    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_METRIC] =
		        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_radial");
	    Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_TIME] =
		        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_time");
        Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_REGULAR] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_regular");
        Parameters::getInstance()->g_uniform_locations[LOCATION_LOD_SIZETEX] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD], "u_size_tex");
        
		
	fprintf (stderr, "Success\n");

	}

	/**CULL**/
	{
        const GLchar *varyings[] = {"o_data", "gl_NextBuffer", "o_data_tr", "gl_NextBuffer", "o_code", "gl_NextBuffer", "o_neighbours"};
        GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL];

        fprintf (stderr, "Loading octree cull program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("octree_cull.glsl") );
    	c.include(SHADER_PATH("noise.glsl") );
    	c.include(SHADER_PATH("octree_common.glsl") );
    	c.include(SHADER_PATH("ltree.glsl") );

        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        *program = tmp->name;
        glTransformFeedbackVaryings (*program, 7, varyings, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram (*program);

        Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SCENE_SIZE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_scene_size");
	 Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_DISABLED] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "culling_disabled");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TAN_FOVY] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_tan_fovy");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SCALE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_scale");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TESSEL] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_tessellation");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_DENSITY] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "densities");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_ISOSURFACE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_isosurface");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_FROMTEXTURE] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "fromtexture");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_METRIC] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_radial");
	Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_TIME] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_time");
    Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_SIZETEX] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL], "u_size_tex");
		
	fprintf (stderr, "Success\n");
				
	}
	
	{
	   GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW];
	
	   fprintf (stderr, "Loading cell draw program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("cell_draw.glsl"));
    	c.include(SHADER_PATH("noise.glsl") );
    	c.include(SHADER_PATH("octree_common.glsl") );
    	c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        *program = tmp->name;
	
    	Parameters::getInstance()->g_uniform_locations[LOCATION_DRAW_SCENE_SIZE] =
                    glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "u_scene_size");
    	Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_TAN_FOVY] =
                    glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "u_tan_fovy");
    	Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_SCALE] =
                    glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "u_scale");
    	Parameters::getInstance()->g_uniform_locations[LOCATION_DRAW_DENSITY] =
                    glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "densities");
    	Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_FROMTEXTURE] =
    		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "fromtexture");
    	Parameters::getInstance()->g_uniform_locations[LOCATION_CELL_TIME] =
    		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW], "u_time");
	
	fprintf (stderr, "Success\n");
	}
	
	configurePrograms();
}

void GPUOctree::init()
{	
	loadBuffers();
	loadFeedback();
	loadVertexArrays();
	loadPrograms();
	
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_LOD]);
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_REGULAR]);
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_TRANSITION]);
	
	//load affine
	Parameters::getInstance()->g_geometry.affine =
            gk::Translate(gk::Vector(0, -Parameters::getInstance()->g_geometry.scale[2] * 1e-5, -Parameters::getInstance()->g_geometry.scale[2] * 1.1f));
        Parameters::getInstance()->g_geometry.affine = Parameters::getInstance()->g_geometry.affine * gk::RotateX(3.14159f * 0.025f);
  
    // pre process quadtree
	glEnable (GL_RASTERIZER_DISCARD);
	glUseProgram (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD]);
	glBindTransformFeedback (GL_TRANSFORM_FEEDBACK,
								Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE1]);
	glBindBufferBase (
                GL_TRANSFORM_FEEDBACK_BUFFER,
                0u,
                Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2]
	);
	glBeginTransformFeedback (GL_POINTS);
			glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE1]);
			glDrawArrays (GL_POINTS, 0, 1);
	glEndTransformFeedback ();
	glDisable (GL_RASTERIZER_DISCARD);

	glFinish (); // for AMD only ?
}

void GPUOctree::runLod(GLuint* unmovedCells, GLuint* queryResult_lod)
{			
	
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Parameters::getInstance()->g_query[QUERY_LOD]);
		glUseProgram (Parameters::getInstance()->g_programs[PROGRAM_LTREE_LOD]);
		glBindTransformFeedback (GL_TRANSFORM_FEEDBACK, Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE1 + Parameters::getInstance()->g_geometry.pingpong]);
		glBindBufferBase (
			GL_TRANSFORM_FEEDBACK_BUFFER,
			0u,
			Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA2 -  Parameters::getInstance()->g_geometry.pingpong]
		);
		    
	GLuint atomicsBuffer;
	glGenBuffers(1, &atomicsBuffer);
	// bind the buffer and define its initial storage capacity
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0u, atomicsBuffer);
	GLuint a = 0;
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &a, GL_DYNAMIC_DRAW);
	// unbind the buffer
		    
	glBeginTransformFeedback (GL_POINTS);
		glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE1 + Parameters::getInstance()->g_geometry.pingpong]);
		glDrawTransformFeedback (GL_POINTS, Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE2 - Parameters::getInstance()->g_geometry.pingpong]);
		glEndTransformFeedback ();
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	
	glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), unmovedCells);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	
	glDeleteBuffers(1, &atomicsBuffer);
	
	glGetQueryObjectuiv(Parameters::getInstance()->g_query[QUERY_LOD], GL_QUERY_RESULT, queryResult_lod);
}

void GPUOctree::runCull(GLuint* queryResult_regular, GLuint* queryResult_transition, GLuint64* sync_time )
{	                    
	if (Parameters::getInstance()->g_culling)
	{
		glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
			Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_DISABLED],
			0);
	}
	else
	{
		glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL],
			Parameters::getInstance()->g_uniform_locations[LOCATION_CULL_DISABLED],
			1);
	}
	glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_LTREE_CULL]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE2 - Parameters::getInstance()->g_geometry.pingpong]);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		0u,
		Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1 +  Parameters::getInstance()->g_geometry.pingpong]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		1u,
		Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1_TR +  Parameters::getInstance()->g_geometry.pingpong]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		2u,
		Parameters::getInstance()->g_buffers[BUFFER_CODE]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		3u,
		Parameters::getInstance()->g_buffers[BUFFER_NEIGHBOURS]
	);
	glBeginQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0, Parameters::getInstance()->g_query[QUERY_REGULAR]);
	glBeginQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 1, Parameters::getInstance()->g_query[QUERY_TRANSITION]);
	glBeginTransformFeedback(GL_POINTS);
		glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_UPDATE2 - Parameters::getInstance()->g_geometry.pingpong]);
		glDrawTransformFeedback(GL_POINTS, Parameters::getInstance()->g_feedbacks[FEEDBACK_LTREE1 + Parameters::getInstance()->g_geometry.pingpong]);
	glEndTransformFeedback();
	glEndQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0);
	glEndQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 1);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	
	glDisable(GL_RASTERIZER_DISCARD);
	
	GLint64 start, stop;
	glGetInteger64v(GL_TIMESTAMP, &start);
	glGetQueryObjectuiv(Parameters::getInstance()->g_query[QUERY_REGULAR], GL_QUERY_RESULT, queryResult_regular);
	glGetQueryObjectuiv(Parameters::getInstance()->g_query[QUERY_TRANSITION], GL_QUERY_RESULT, queryResult_transition);
	glGetInteger64v(GL_TIMESTAMP, &stop);
	*sync_time = stop - start;
}

void GPUOctree::runDisplay(int nb)
{
	glLineWidth(2.0);

	//draw cells
	glUseProgram (Parameters::getInstance()->g_programs[PROGRAM_CELL_DRAW]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_OCTREE_RENDER2 - Parameters::getInstance()->g_geometry.pingpong]);

	glDrawElementsInstanced (GL_LINES, 48, GL_UNSIGNED_SHORT, 0, nb);

	glBindVertexArray(0);
	glUseProgram(0);
	
	glLineWidth(1.0);
}


