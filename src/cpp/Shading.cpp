#include "Shading.h"
#include <boost/concept_check.hpp>

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

#include "ImageIO.h"
#include "Image.h"


void configure_shading(GLuint program, GLuint first_loc)
{
	glProgramUniform3f (program,
			Parameters::getInstance()->g_uniform_locations[first_loc],//LOCATION_SHADING_SIZE+first_loc],
			Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+1],
			TEXTURE_DENSITY);
	glProgramUniform2f (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+2],
			Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);

	glProgramUniform1f (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+3],
			tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
	glProgramUniform1f(program,
			Parameters::getInstance()->g_uniform_locations[first_loc+4],
			Parameters::getInstance()->g_scale);

	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+5],
			Parameters::getInstance()->g_solid_wireframe);

	glProgramUniform3f (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+6],
			Parameters::getInstance()->g_camera.pos[0],
			Parameters::getInstance()->g_camera.pos[1],
			Parameters::getInstance()->g_camera.pos[2]
   			);

	glProgramUniform1f (program,
                    Parameters::getInstance()->g_uniform_locations[first_loc+7],
                    Parameters::getInstance()->g_curvradius);

    glProgramUniform1f (program,
                    Parameters::getInstance()->g_uniform_locations[first_loc+8],
                    Parameters::getInstance()->g_curvmin);

    glProgramUniform1f (program,
                    Parameters::getInstance()->g_uniform_locations[first_loc+9],
                    Parameters::getInstance()->g_curvmax);

    glProgramUniform1i (program,
            Parameters::getInstance()->g_uniform_locations[first_loc+10],
            (int)Parameters::getInstance()->g_ground_truth);

    glProgramUniform1f(program,
            Parameters::getInstance()->g_uniform_locations[first_loc+11],
            Parameters::getInstance()->g_sizetex);
	
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+12],
			TEXTURE_X2Y2Z2);
	
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+13],
 			TEXTURE_XY_YZ_XZ);
	
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+14],
 			TEXTURE_XYZ);
	
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+15],
 			(int)Parameters::getInstance()->g_curv_dir);
	
	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+16],
 			(int)Parameters::getInstance()->g_curv_val);
	
	glProgramUniform1f (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+17],
 			Parameters::getInstance()->g_lvl);

	glProgramUniform1i (program,
			Parameters::getInstance()->g_uniform_locations[first_loc+18],
 			Parameters::getInstance()->g_k1k2_normals);
}

void load(GLuint program, GLuint first_loc)
{
	Parameters::getInstance()->g_uniform_locations[first_loc] =
		glGetUniformLocation (program, "u_scene_size");
	Parameters::getInstance()->g_uniform_locations[first_loc+1] =
		glGetUniformLocation (program, "densities");
	Parameters::getInstance()->g_uniform_locations[first_loc+2] =
		glGetUniformLocation (program, "u_viewport");

	Parameters::getInstance()->g_uniform_locations[first_loc+3] =
		glGetUniformLocation (program, "u_tan_fovy");
	Parameters::getInstance()->g_uniform_locations[first_loc+4] =
		glGetUniformLocation (program, "u_scale");

	Parameters::getInstance()->g_uniform_locations[first_loc+5] =
		glGetUniformLocation (program, "solid_wireframe");
	
	Parameters::getInstance()->g_uniform_locations[first_loc+6] =
		glGetUniformLocation (program, "u_camera_pos");
		
	Parameters::getInstance()->g_uniform_locations[first_loc+7] =
		glGetUniformLocation (program, "u_curv_radius");
    Parameters::getInstance()->g_uniform_locations[first_loc+8] =
        glGetUniformLocation (program, "u_kmin");
    Parameters::getInstance()->g_uniform_locations[first_loc+9] =
        glGetUniformLocation (program, "u_kmax");

    Parameters::getInstance()->g_uniform_locations[first_loc+10] =
        glGetUniformLocation (program, "u_ground_truth");

    Parameters::getInstance()->g_uniform_locations[first_loc+11] =
        glGetUniformLocation (program, "u_size_tex");
		
	Parameters::getInstance()->g_uniform_locations[first_loc+12] = 
		glGetUniformLocation(program, "u_xyz2_tex");

	Parameters::getInstance()->g_uniform_locations[first_loc+13] = 
		glGetUniformLocation(program, "u_xy_yz_xz_tex");
		
	Parameters::getInstance()->g_uniform_locations[first_loc+14] = 
		glGetUniformLocation(program, "u_xyz_tex");
		
	Parameters::getInstance()->g_uniform_locations[first_loc+15] = 
		glGetUniformLocation(program, "u_curv_dir");
	Parameters::getInstance()->g_uniform_locations[first_loc+16] = 
		glGetUniformLocation(program, "u_curv_val");
		
	Parameters::getInstance()->g_uniform_locations[first_loc+17] = 
		glGetUniformLocation(program, "u_lvl");
	Parameters::getInstance()->g_uniform_locations[first_loc+18] = 
		glGetUniformLocation(program, "u_k1k2_normals");
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

void Shading::configureProgram()
{
	configure_shading(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
}


void Shading::loadVA()
{
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_SHADING);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING]);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_TGL]);
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMIN]);
	glVertexAttribPointer(1, 4, GL_FLOAT, 0, 0, 0);
	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMAX]);
	glVertexAttribPointer(2, 4, GL_FLOAT, 0, 0, 0);
	
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_NORMALES]);
	glVertexAttribPointer(3, 4, GL_FLOAT, 0, 0, 0);
	
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


