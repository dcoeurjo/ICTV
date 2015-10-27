#include "Triangulation.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

void TransitionTriangulation::loadTables()
{
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CODE_CLASS_TR]);
	glBindTexture(GL_TEXTURE_1D, Parameters::getInstance()->g_textures[TEXTURE_CODE_CLASS_TR]);

	std::vector<unsigned char> table_id_tr(512, 0);
	
	for(int i=0; i<512; i++)
		table_id_tr[i] = transitionCellClass[i];
	
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, 512, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &(table_id_tr[0]));

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_1D, 0);

	//Transition Code to vertices
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CODE_VERTICES_TR]);
	glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_CODE_VERTICES_TR]);

	std::vector<unsigned short int> table_vertices_tr(512*4*3, 0);
	
	for(int i=0; i<512*3; i+=3)
	{
		unsigned char class_code = transitionCellClass[i/3];
		class_code &= 0x7F;
		int nb_vertices = (transitionCellData[class_code].geometryCounts >> 4);

		for(int v=0; v<nb_vertices; v++)
		{
			int row = v/3;
			table_vertices_tr[i + row*(512*3) + v%3] = transitionVertexData[i/3][v];
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16UI, 512, 4, 0, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, &(table_vertices_tr[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	//Transition Class to triangulation
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CLASS_TRIANGLES_TR]);
	glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_CLASS_TRIANGLES_TR]);

        //56 classes d'équivalence comportant au max 12 triangles = 12*3 vertex = 36
        //Paye tes puissances de 2 !
	std::vector<unsigned char> table_triangles_tr(56*9*4, 42);
	
	for(int i=0; i<56*4; i+=4)
	{
		int nb_triangles = (transitionCellData[i/4].geometryCounts & 0x0F);

		for(int t=0; t<nb_triangles*3; t++)
		{
			int row = t/4;
			table_triangles_tr[i + row*(56*4) + t%4] = transitionCellData[i/4].vertexIndex[t];
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, 56, 9, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &(table_triangles_tr[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TransitionTriangulation::configurePrograms()
{
	glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_SCENE_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
        glProgramUniform2f (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_VIEWPORT],
                            Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);
        glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TAN_FOVY],
                            tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
        glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_SCALE],
                            Parameters::getInstance()->g_scale);
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_DENSITY],
                            TEXTURE_DENSITY);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CODE_CLASS_TR],
                            TEXTURE_CODE_CLASS_TR);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CODE_VERTICES_TR],
                            TEXTURE_CODE_VERTICES_TR);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CLASS_TRIANGLES_TR],
                            TEXTURE_CLASS_TRIANGLES_TR);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TESSEL],
			    (int)Parameters::getInstance()->g_tessel);
        glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_ISOSURFACE],
                            Parameters::getInstance()->g_isosurface);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
			    Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_FROMTEXTURE],
                            Parameters::getInstance()->g_fromtexture);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_METRIC],
                            Parameters::getInstance()->g_radial_length);
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TIME],
                            Parameters::getInstance()->g_time_elapsed);
}

void TransitionTriangulation::loadPrograms()
{
	GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW];

	fprintf (stderr, "loading transition draw program... "); fflush (stderr);
	gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("transition_render.glsl") );

	c.include(SHADER_PATH("noise.glsl") );
	c.include(SHADER_PATH("potential.glsl") );
	c.include(SHADER_PATH("octree_common.glsl") );
	c.include(SHADER_PATH("ltree.glsl") );
	
	GLProgram* tmp = c.make();
	if (tmp->errors)
		exit(-1);

	*program = tmp->name;
	
	glLinkProgram (*program);

	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_SCENE_SIZE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_scene_size");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_VIEWPORT] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_viewport");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TAN_FOVY] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_tan_fovy");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_DENSITY] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "densities");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CODE_CLASS_TR] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "code_class_tr");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CODE_VERTICES_TR] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "code_vertices_tr");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_CLASS_TRIANGLES_TR] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "class_triangles_tr");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TESSEL] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_tessellation");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_SCALE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_scale");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_ISOSURFACE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_isosurface");  
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_FROMTEXTURE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "fromtexture");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_METRIC] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_radial");
	Parameters::getInstance()->g_uniform_locations[LOCATION_TRANSITION_TIME] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW], "u_time");
	
	configurePrograms();

	fprintf (stderr, "success\n");
}

void list_indices_tr(std::vector<unsigned int>& id_list)
{
    if (id_list.size() < 36*(56-1))
    {
        id_list.clear();
        id_list.resize(36*(56-1), 37);
    }
    
    for(int c=0; c<55; c++) //Pour chaque classe
    {
        TransitionCellData d = transitionCellData[c+1];
        for(int id=0; id<d.GetTriangleCount()*3; id++)
        {
            id_list[c*36 + id] = d.vertexIndex[id];
        }
    }
}
void TransitionTriangulation::loadBuffers()
{
	int res = (int)Parameters::getInstance()->g_tessel;
	long long int nb_cells = 10000*res*res*res;
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS_TR]);
	glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS_TR]);
	glBufferData (GL_ARRAY_BUFFER, nb_cells*5*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDICES_TR]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES_TR]);
        std::vector<unsigned int> indices_listing_tr;
        list_indices_tr(indices_listing_tr);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices_listing_tr.size()*sizeof(unsigned int), &indices_listing_tr[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_VERTICES_TR]);
        glBindBuffer (GL_TRANSFORM_FEEDBACK_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES_TR]);
        glBufferData (GL_TRANSFORM_FEEDBACK_BUFFER, nb_cells*4*12*sizeof(float), NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	
}

void TransitionTriangulation::runTriangulation(GLuint nbCells)
{
	configurePrograms();
	
	glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_TRANSITION_DRAW]);

	int res = (int)Parameters::getInstance()->g_tessel;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS_TR]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2_TR - Parameters::getInstance()->g_geometry.pingpong]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Parameters::getInstance()->g_buffers[BUFFER_VERTICES_TR]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS_TR]);
	glDrawArraysInstanced(
		GL_POINTS,
		0,
		nbCells, res*res);
	
	glUseProgram(0);
}

void TransitionTriangulation::init()
{
	loadTables();
	loadBuffers();
	loadPrograms();
}