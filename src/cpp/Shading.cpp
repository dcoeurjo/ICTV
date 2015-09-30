#include "Shading.h"

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
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_DENSITY],
			TEXTURE_DENSITY);
	glProgramUniform2f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_VIEWPORT],
			Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);

	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TAN_FOVY],
			tanf (Parameters::getInstance()->g_camera.fovy / 360.f * 3.14159f));
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SCALE],
			Parameters::getInstance()->g_scale);
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TIME],
			Parameters::getInstance()->g_time_elapsed);


	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_X],
			TEXTURE_COLOR_X);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_Y],
			TEXTURE_COLOR_Y);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_Z],
			TEXTURE_COLOR_Z);
	
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_X],
			TEXTURE_BUMP_X);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_Y],
			TEXTURE_BUMP_Y);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_Z],
			TEXTURE_BUMP_Z);

	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_WIREFRAME],
			Parameters::getInstance()->g_solid_wireframe);
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TEXTURED],
			Parameters::getInstance()->g_textured_data);

	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_FROMTEXTURE],
			Parameters::getInstance()->g_fromtexture);
	glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
			Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CAMERA],
			Parameters::getInstance()->g_camera.pos[0],
			Parameters::getInstance()->g_camera.pos[1],
			Parameters::getInstance()->g_camera.pos[2]
   			);

	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVRADIUS],
                    Parameters::getInstance()->g_curvradius);

    glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVMIN],
                    Parameters::getInstance()->g_curvmin);

    glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVMAX],
                    Parameters::getInstance()->g_curvmax);
	
	glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SKYBOX_TEXTURE],
                    TEXTURE_ENVMAP);

    glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_SHADING],
            Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_GROUNDTRUTH],
            (int)Parameters::getInstance()->g_ground_truth);

    glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_SHADING],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SIZETEX],
                            Parameters::getInstance()->g_sizetex);
}

void Shading::loadProgram()
{
	{
	GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SHADING];
    
    //const char* varyings[1] = { "trfeed_position" };

        fprintf (stderr, "loading shading program... "); fflush (stderr);
        gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("shading.glsl"));
        c.include(SHADER_PATH("noise.glsl") );
        c.include(SHADER_PATH("octree_common.glsl") );
        c.include(SHADER_PATH("ltree.glsl") );
        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        *program = tmp->name;
        
        //glTransformFeedbackVaryings (*program, 1, varyings, GL_SEPARATE_ATTRIBS);
        glLinkProgram (*program);
    
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SIZE] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_scene_size");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_DENSITY] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "densities");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_VIEWPORT] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_viewport");

	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TAN_FOVY] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_tan_fovy");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SCALE] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_scale");

	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_WIREFRAME] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "solid_wireframe");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TEXTURED] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "textured");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_FROMTEXTURE] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "fromtexture");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_TIME] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_time");
		
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_X] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texcolor_x");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_Y] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texcolor_y");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_COLOR_Z] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texcolor_z");
		
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_X] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texbump_x");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_Y] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texbump_y");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_BUMP_Z] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_texbump_z");
	
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CAMERA] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_camera_pos");
		
	Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVRADIUS] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_curv_radius");
        Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVMIN] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_kmin");
        Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_CURVMAX] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_kmax");

    Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_GROUNDTRUTH] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_ground_truth");

    Parameters::getInstance()->g_uniform_locations[LOCATION_SHADING_SIZETEX] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SHADING], "u_size_tex");

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


