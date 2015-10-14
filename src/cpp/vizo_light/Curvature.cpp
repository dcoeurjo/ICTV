#include "Curvature.h"
#include <boost/concept_check.hpp>

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

#include "ImageIO.h"
#include "Image.h"


void configure_curv(GLuint program, GLuint first_loc)
{
	/*
	LOCATION_SHADING_SIZE,
	LOCATION_SHADING_DENSITY,
	LOCATION_SHADING_VIEWPORT,
	LOCATION_SHADING_TAN_FOVY,
	LOCATION_SHADING_SCALE,
	LOCATION_SHADING_WIREFRAME,
	LOCATION_SHADING_CAMERA,
	LOCATION_SHADING_CURVRADIUS,
	LOCATION_SHADING_CURVMIN,
	LOCATION_SHADING_CURVMAX,
	LOCATION_SHADING_GROUNDTRUTH,
	LOCATION_SHADING_SIZETEX,
	*/
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

void Curvature::configureProgram()
{
	//configure_curv(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
	configure_curv(Parameters::getInstance()->g_programs[PROGRAM_GTCURV], LOCATION_GTCURV_SIZE);
	configure_curv(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV], LOCATION_HIERARCHCURV_SIZE);
	configure_curv(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV], LOCATION_APPROXCURV_SIZE);
}

void load_curv(GLuint program, GLuint first_loc)
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

void Curvature::loadProgram()
{
	/*{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SHADING];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("shading.glsl"));
        //c.include(SHADER_PATH("noise.glsl") );
        //c.include(SHADER_PATH("octree_common.glsl") );
        //c.include(SHADER_PATH("ltree.glsl") );
		c.include(SHADER_PATH("eigendecomposition.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load_curv(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
	}*/
	
	int nb_var = 7;
	const GLchar *varyings[] = {"geometry_position", "gl_NextBuffer", 
								//"geometry_k1_k2", "gl_NextBuffer",
								"geometry_min_dir", "gl_NextBuffer",
								"geometry_max_dir", "gl_NextBuffer",
								"geometry_normale"};// "geometry_egv", "geometry_covmatDiag", "geometry_covmatUpper"};
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV];

        fprintf (stderr, "loading O(1) curv program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature.glsl"));
        c.include(SHADER_PATH("o1_include.glsl") );
		c.include(SHADER_PATH("eigendecomposition.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
		
		glTransformFeedbackVaryings (*program, nb_var, varyings, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram (*program);
		
		load_curv(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV], LOCATION_APPROXCURV_SIZE);
	}
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_GTCURV];

        fprintf (stderr, "loading GT curv program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature.glsl"));
        c.include(SHADER_PATH("gt_include.glsl") );
		c.include(SHADER_PATH("eigendecomposition.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;

		glTransformFeedbackVaryings (*program, nb_var, varyings, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram (*program);
		
		load_curv(Parameters::getInstance()->g_programs[PROGRAM_GTCURV], LOCATION_GTCURV_SIZE);
	}
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV];

        fprintf (stderr, "loading hierarchic curv program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature.glsl"));
        c.include(SHADER_PATH("hierarch_include.glsl") );
		c.include(SHADER_PATH("eigendecomposition.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
		
		glTransformFeedbackVaryings (*program, nb_var, varyings, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram (*program);
		
		load_curv(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV], LOCATION_HIERARCHCURV_SIZE);
	}
        
	configureProgram();
}

void Curvature::loadVA()
{
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_CURVATURE);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_CURVATURE]);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES]);
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
    
	glEnableVertexAttribArray(1);       
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES]);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
	
	glBindVertexArray(0);
        
    glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_CURVATURE_TR);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_CURVATURE_TR]);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES_TR]);
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
    
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES_TR]);
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
	
	glBindVertexArray(0);
}

void Curvature::init()
{
	loadProgram();
	loadVA();
	loadBuffers();
	loadTransformFeedbacks();
	
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_TRIANGLES]);
}

void Curvature::run(GLuint nbcells_reg, GLuint nbcells_tr, GLuint* nb_triangles_regular, GLuint* nb_triangles_transition, GLuint64* sync_time)
{
	configureProgram();

	if((int)Parameters::getInstance()->g_ground_truth == 1)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_GTCURV]);
	else if((int)Parameters::getInstance()->g_ground_truth == 2)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV]);
	else if((int)Parameters::getInstance()->g_ground_truth == 3)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV]);

	glBeginQuery(GL_PRIMITIVES_GENERATED, Parameters::getInstance()->g_query[QUERY_TRIANGLES]);
	
	int res = (int)Parameters::getInstance()->g_tessel;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_CURVATURE]);

	
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, Parameters::getInstance()->g_feedbacks[FEEDBACK_TRIANGULATION]);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		0u,
		Parameters::getInstance()->g_buffers[BUFFER_EXPORT_TGL]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		1u,
		Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMIN]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		2u,
		Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMAX]
	);
	glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		3u,
		Parameters::getInstance()->g_buffers[BUFFER_EXPORT_NORMALES]
	);
	/*glBindBufferBase (
		GL_TRANSFORM_FEEDBACK_BUFFER,
		4u,
		Parameters::getInstance()->g_buffers[BUFFER_EXPORT_NORMALES]
	);*/
	glBeginTransformFeedback(GL_TRIANGLES);
	
	
	glMultiDrawElementsIndirect(
		GL_TRIANGLES,
		GL_UNSIGNED_INT,
		NULL,
		nbcells_reg*res*res*res,
		0);
	glEndQuery(GL_PRIMITIVES_GENERATED);
	
	
	GLint64 start, stop;
	glGetInteger64v(GL_TIMESTAMP, &start);
	glGetQueryObjectuiv(Parameters::getInstance()->g_query[QUERY_TRIANGLES], GL_QUERY_RESULT, nb_triangles_regular);
	glGetInteger64v(GL_TIMESTAMP, &stop);

	*sync_time = stop - start;
	
	glBeginQuery(GL_PRIMITIVES_GENERATED, Parameters::getInstance()->g_query[QUERY_TRIANGLES]);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS_TR]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_CURVATURE_TR]);
	glMultiDrawElementsIndirect(
		GL_TRIANGLES,
		GL_UNSIGNED_INT,
		NULL,
		nbcells_tr*res*res,
		0);
		
	glEndQuery(GL_PRIMITIVES_GENERATED);
	
	glGetInteger64v(GL_TIMESTAMP, &start);
	glGetQueryObjectuiv(Parameters::getInstance()->g_query[QUERY_TRIANGLES], GL_QUERY_RESULT, nb_triangles_transition);
	glGetInteger64v(GL_TIMESTAMP, &stop);
	
	
	glEndTransformFeedback();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	
	glUseProgram(0);
	
	*sync_time += (stop - start);
}

void Curvature::loadTransformFeedbacks()
{
	glGenTransformFeedbacks(1, &(Parameters::getInstance()->g_feedbacks[FEEDBACK_TRIANGULATION]));
}

void Curvature::loadBuffers()
{
	int res = (int)Parameters::getInstance()->g_tessel;
	long long int nb_cells = 300000*res*res*res;
	int export_data = 4;
	
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_EXPORT_TGL]);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_TGL]);
		glBufferData (
				GL_ARRAY_BUFFER,
				nb_cells*12*export_data*sizeof(float), //12 triangles max for each cell, each made of 3 vec3
				NULL,
				GL_DYNAMIC_COPY
		);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	export_data = 4;
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMIN]);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMIN]);
		glBufferData (
				GL_ARRAY_BUFFER,
				nb_cells*12*export_data*sizeof(float), //12 triangles max for each cell, each made of 3 vec3
				NULL,
				GL_DYNAMIC_COPY
		);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	export_data = 4;
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMAX]);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_DIRMAX]);
		glBufferData (
				GL_ARRAY_BUFFER,
				nb_cells*12*export_data*sizeof(float), //12 triangles max for each cell, each made of 3 vec3
				NULL,
				GL_DYNAMIC_COPY
		);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	
	export_data = 4;
	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_EXPORT_NORMALES]);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_EXPORT_NORMALES]);
		glBufferData (
				GL_ARRAY_BUFFER,
				nb_cells*12*export_data*sizeof(float), //12 triangles max for each cell, each made of 3 vec3
				NULL,
				GL_DYNAMIC_COPY
		);
	glBindBuffer (GL_ARRAY_BUFFER, 0);
}



