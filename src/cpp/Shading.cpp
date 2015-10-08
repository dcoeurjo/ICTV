#include "Shading.h"
#include <boost/concept_check.hpp>

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

#include "ImageIO.h"
#include "Image.h"


void Shading::configureProgram()
{
	glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SIZE],
                            Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
}

void load(GLuint program, GLuint first_loc)
{
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SIZE] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_scene_size");
}

void Shading::loadProgram()
{
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SHADING];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("shading.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
	}
	
	configureProgram();
}

void Shading::loadVA()
{
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_SHADING);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_TRIANGULATION]);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
	glBindVertexArray(0);
}

void Shading::init()
{
	loadProgram();
	loadVA();
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_TRIANGLES]);
}

void Shading::run(GLuint nb_triangles_regular)
{
	configureProgram();

	glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_SHADING]);
	
	int res = (int)Parameters::getInstance()->g_tessel;
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING]);
	glDrawArrays(
		GL_TRIANGLES,
		0,
		nb_triangles_regular*3);
	glBindVertexArray (0);
	
	glUseProgram(0);
}


