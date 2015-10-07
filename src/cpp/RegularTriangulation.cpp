#include "Triangulation.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

void RegularTriangulation::loadTables()
{
	//Code to Class (256 -> 18)
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CODE_CLASS]);
	glBindTexture(GL_TEXTURE_1D, Parameters::getInstance()->g_textures[TEXTURE_CODE_CLASS]);

	std::vector<char> table_id(256, 0);
	
	for(int i=0; i<256; i++)
		table_id[i] = regularCellClass[i];
	
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, 256, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &(table_id[0]));

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_1D, 0);
	
	
	//Code to vertices
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CODE_VERTICES]);	
	glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_CODE_VERTICES]);

	std::vector<unsigned short int> table_vertices(256*4*3, 0);
	
	for(int i=0; i<256*3; i+=3)
	{
		int class_code = regularCellClass[i/3];
		int nb_vertices = (regularCellData[class_code].geometryCounts >> 4);

		for(int v=0; v<nb_vertices; v++)
		{
		int row = v/3;
		table_vertices[i + row*(256*3) + v%3] = regularVertexData[i/3][v];
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16UI, 256, 4, 0, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, &(table_vertices[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	//Class to triangulation
	glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_CLASS_TRIANGLES]);
	glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_CLASS_TRIANGLES]);

	std::vector<unsigned char> table_triangles(17*5*5, 15);
	
	for(int i=0; i<16*4; i+=4)
	{
		int nb_triangles = (regularCellData[i/4].geometryCounts & 0x0F);

		for(int t=0; t<nb_triangles*3; t++)
		{
			int row = t/4;
			table_triangles[i + row*(16*4) + t%4] = regularCellData[i/4].vertexIndex[t];
		}
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, 16, 4, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &(table_triangles[0]));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RegularTriangulation::configurePrograms()
{
        glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_SCENE_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
        glProgramUniform2f (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_VIEWPORT],
                            Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);
        glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TAN_FOVY],
                            tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_DENSITY],
                            TEXTURE_DENSITY);
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CODE_CLASS],
                            TEXTURE_CODE_CLASS);
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CODE_VERTICES],
                            TEXTURE_CODE_VERTICES);
        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CLASS_TRIANGLES],
                            TEXTURE_CLASS_TRIANGLES);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TESSEL],
			    (int)Parameters::getInstance()->g_tessel);
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_SCALE],
                            Parameters::getInstance()->g_scale);
        glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
			    Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_ISOSURFACE],
                            Parameters::getInstance()->g_isosurface);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
			    Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_FROMTEXTURE],
			    Parameters::getInstance()->g_fromtexture);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
			    Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_METRIC],
			    Parameters::getInstance()->g_radial_length);
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TIME],
                            Parameters::getInstance()->g_time_elapsed);
}

void RegularTriangulation::loadPrograms()
{
	GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW];

	fprintf (stderr, "Loading regular draw program... "); fflush (stderr);
	gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("regular_render.glsl"));
	c.include(SHADER_PATH("noise.glsl") );
	c.include(SHADER_PATH("octree_common.glsl") );
	c.include(SHADER_PATH("ltree.glsl") );

	GLProgram* tmp = c.make();
	if (tmp->errors)
		exit(-1);

	*program = tmp->name;

	glLinkProgram (*program);

	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_SCENE_SIZE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_scene_size");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_VIEWPORT] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_viewport");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TAN_FOVY] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_tan_fovy");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_DENSITY] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "densities");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CODE_CLASS] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "code_class");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CODE_VERTICES] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "code_vertices");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_CLASS_TRIANGLES] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "class_triangles");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TESSEL] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_tessellation");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_SCALE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_scale");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_ISOSURFACE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_isosurface");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_FROMTEXTURE] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "fromtexture");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_METRIC] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_radial");
	Parameters::getInstance()->g_uniform_locations[LOCATION_REGULAR_TIME] =
			glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW], "u_time");
	
			
	configurePrograms();

	fprintf (stderr, "Success\n");
}

void list_indices(std::vector<unsigned int>& id_list)
{
    if (id_list.size() < 15*(16-1))
    {
        id_list.clear();
        id_list.resize(15*(16-1), 16);
    }
    
    for(int c=0; c<15; c++) //Pour chaque classe
    {
        RegularCellData d = regularCellData[c+1];
        for(int id=0; id<d.GetTriangleCount()*3; id++)
        {
            id_list[c*15 + id] = d.vertexIndex[id];
        }
    }
}
void RegularTriangulation::loadBuffers()
{
	int res = (int)Parameters::getInstance()->g_tessel;
	long long int nb_cells = 500000*res*res*res;
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glBufferData (GL_ARRAY_BUFFER, nb_cells*5*sizeof(unsigned int), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDICES]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES]);
        std::vector<unsigned int> indices_listing;
        list_indices(indices_listing);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indices_listing.size()*sizeof(unsigned int), &indices_listing[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
        //Emitted triangles from regular cells
        glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_VERTICES]);
        glBindBuffer (GL_TRANSFORM_FEEDBACK_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES]);
        glBufferData (GL_TRANSFORM_FEEDBACK_BUFFER, nb_cells*4*12*sizeof(float), NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
}

void RegularTriangulation::runTriangulation(GLuint nbCells)
{
	configurePrograms();
	
	glEnable(GL_RASTERIZER_DISCARD);
	
	glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_REGULAR_DRAW]);

	int res = (int)Parameters::getInstance()->g_tessel;
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_LTREE_RENDER2 - Parameters::getInstance()->g_geometry.pingpong]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Parameters::getInstance()->g_buffers[BUFFER_VERTICES]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glDrawArraysInstanced(
			GL_POINTS,
			0,
			nbCells, res*res*res);
	glUseProgram(0);
	
	glDisable(GL_RASTERIZER_DISCARD);
}


void RegularTriangulation::init()
{
	loadTables();
	loadBuffers();
	loadPrograms();
}



