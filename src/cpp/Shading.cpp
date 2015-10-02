#include "Shading.h"
#include <boost/concept_check.hpp>

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

#include "ImageIO.h"
#include "Image.h"

void configure(GLuint program, GLuint first_loc)
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
}

void Shading::configureProgram()
{
	configure(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
	configure(Parameters::getInstance()->g_programs[PROGRAM_GTCURV], LOCATION_GTCURV_SIZE);
	configure(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV], LOCATION_HIERARCHCURV_SIZE);
	configure(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV], LOCATION_APPROXCURV_SIZE);
	
	glProgramUniform1i (PROGRAM_SKYBOX,
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SKYBOX_TEXTURE],
                    TEXTURE_ENVMAP);
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
}

void Shading::loadProgram()
{
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SHADING];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("shading.glsl"));
        c.include(SHADER_PATH("noise.glsl") );
        c.include(SHADER_PATH("octree_common.glsl") );
        c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load(Parameters::getInstance()->g_programs[PROGRAM_SHADING], LOCATION_SHADING_SIZE);
	}
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature_o1.glsl"));
        c.include(SHADER_PATH("noise.glsl") );
        c.include(SHADER_PATH("octree_common.glsl") );
        c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV], LOCATION_APPROXCURV_SIZE);
	}
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_GTCURV];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature_gt.glsl"));
        c.include(SHADER_PATH("noise.glsl") );
        c.include(SHADER_PATH("octree_common.glsl") );
        c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load(Parameters::getInstance()->g_programs[PROGRAM_GTCURV], LOCATION_GTCURV_SIZE);
	}
	
	{
		GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV];

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("curvature_hierarchique.glsl"));
        c.include(SHADER_PATH("noise.glsl") );
        c.include(SHADER_PATH("octree_common.glsl") );
        c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);
        *program = tmp->name;
        glLinkProgram (*program);
		
		load(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV], LOCATION_HIERARCHCURV_SIZE);
	}
	
	{
	GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SKYBOX];

        fprintf (stderr, "loading skybox program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram(SHADER_PATH("skybox.glsl"));
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        *program = tmp->name;
        glLinkProgram (*program);

        Parameters::getInstance()->g_uniform_locations[LOCATION_SKYBOX_TEXTURE] =
                        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SKYBOX], "u_texture");
	}
        
	configureProgram();
}

void Shading::loadTextures()
{
    /** Fetch assets **/
	
    gk::Image* texture_yz = gk::ImageIO::readImage(DATA_PATH("texture/rock_texture.jpg"));
    if (texture_yz == NULL)
        exit(1);
    gk::Image* texture_xz = gk::ImageIO::readImage(DATA_PATH("texture/rock_texture.jpg"));
    if (texture_xz == NULL)
        exit(1);
    gk::Image* texture_xy = gk::ImageIO::readImage(DATA_PATH("texture/grass_texture.jpg"));
    if (texture_xy == NULL)
        exit(1);
    
    /*
    gk::Image* texture_yz = gk::ImageIO::readImage(DATA_PATH("texture/bone.jpg"));
    if (texture_yz == NULL)
        exit(1);
    gk::Image* texture_xz = gk::ImageIO::readImage(DATA_PATH("texture/bone.jpg"));
    if (texture_xz == NULL)
        exit(1);
    gk::Image* texture_xy = gk::ImageIO::readImage(DATA_PATH("texture/bone.jpg"));
    if (texture_xy == NULL)
        exit(1);
    */
    
    /*gk::Image* bump_yz = gk::ImageIO::readImage(DATA_PATH("texture/rock_bumpmap.jpg"));
    if (texture_yz == NULL)
        exit(1);
    gk::Image* bump_xz = gk::ImageIO::readImage(DATA_PATH("texture/rock_bumpmap.jpg"));
    if (texture_xz == NULL)
        exit(1);
    gk::Image* bump_xy = gk::ImageIO::readImage(DATA_PATH("texture/sand_bumpmap.jpg"));
    if (texture_xy == NULL)
        exit(1);*/
    
    gk::Image* texture_envmap = gk::ImageIO::readImage(DATA_PATH("texture/envmap3.jpg"));
    if (texture_envmap == NULL)
        exit(1);
    
    /**Load textures**/
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_COLOR_X]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_COLOR_X]);

    if (texture_yz->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_yz->width, texture_yz->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_yz->data);
    else if  (texture_yz->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_yz->width, texture_yz->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_yz->data);
    else
    {
        printf("Error: %d channels on texture YZ\n", texture_yz->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_COLOR_Y]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_COLOR_Y]);

    if (texture_xz->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_xz->width, texture_xz->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_xz->data);
    else if  (texture_xz->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_xz->width, texture_xz->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_xz->data);
    else
    {
        printf("Error: %d channels on texture XZ\n", texture_xz->channels);
        exit(1);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_COLOR_Z]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_COLOR_Z]);

    if (texture_xy->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_xy->width, texture_xy->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_xy->data);
    else if  (texture_xy->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_xy->width, texture_xy->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_xy->data);
    else
    {
        printf("Error: %d channels on texture XY\n", texture_xy->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    /** Load Bumpmaps **/
    
    /*glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_BUMP_X]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_BUMP_X]);

    if (bump_yz->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bump_yz->width, bump_yz->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump_yz->data);
    else if  (bump_yz->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bump_yz->width, bump_yz->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bump_yz->data);
    else
    {
        printf("Error: %d channels on bump YZ\n", bump_yz->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_BUMP_Y]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_BUMP_Y]);

    if (bump_xz->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bump_xz->width, bump_xz->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump_xz->data);
    else if  (bump_xz->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bump_xz->width, bump_xz->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bump_xz->data);
    else
    {
        printf("Error: %d channels on bump XZ\n", bump_xz->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_BUMP_Z]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_BUMP_Z]);

    if (bump_xy->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bump_xy->width, bump_xy->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bump_xy->data);
    else if  (bump_xy->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bump_xy->width, bump_xy->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bump_xy->data);
    else
    {
        printf("Error: %d channels on bump XY\n", bump_xy->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);*/

    /** Load Envmap **/
    
    glGenTextures(1, &Parameters::getInstance()->g_textures[TEXTURE_ENVMAP]);	
    glBindTexture(GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_ENVMAP]);

    if (texture_envmap->channels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_envmap->width, texture_envmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_envmap->data);
    else if  (texture_envmap->channels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_envmap->width, texture_envmap->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_envmap->data);
    else
    {
        printf("Error: %d channels on skybox texture\n", texture_envmap->channels);
        exit(1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Shading::loadVA()
{
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_SHADING);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES]);
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(1);       
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES]);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
	glBindVertexArray(0);
        
    glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_SHADING_TR);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING_TR]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTICES_TR]);
	glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDICES_TR]);
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
	glBindVertexArray(0);
}

void Shading::init()
{
	loadProgram();
	//loadTextures();
	loadVA();
	glGenQueries (1, &Parameters::getInstance()->g_query[QUERY_TRIANGLES]);
}

void Shading::run(GLuint nbcells_reg, GLuint nbcells_tr, GLuint* nb_triangles_regular, GLuint* nb_triangles_transition, GLuint64* sync_time)
{
	configureProgram();

	if((int)Parameters::getInstance()->g_ground_truth == 1)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_GTCURV]);
	else if((int)Parameters::getInstance()->g_ground_truth == 2)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_HIERARCHCURV]);
	else if((int)Parameters::getInstance()->g_ground_truth == 3)
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_APPROXCURV]);
	else
		glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_SHADING]);
	
	glBeginQuery(GL_PRIMITIVES_GENERATED, Parameters::getInstance()->g_query[QUERY_TRIANGLES]);
	
	int res = (int)Parameters::getInstance()->g_tessel;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDIRECT_DRAWS]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING]);
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
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SHADING_TR]);
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
	
	glUseProgram(0);
	
	*sync_time += (stop - start);
}


