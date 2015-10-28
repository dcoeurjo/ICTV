/*
 * Copyright 2015 
 * Hélène Perrier <helene.perrier@liris.cnrs.fr>
 * Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Jacques-Olivier Lachaud <jacques-olivier.lachaud@univ-savoie.fr>
 * Jean-Philippe Farrugia <jean-philippe.farrugia@liris.cnrs.fr>
 * Jean-Claude Iehl <jean-claude.iehl@liris.cnrs.fr>
 * 
 * This file is part of ICTV.
 * 
 * ICTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ICTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ICTV.  If not, see <http://www.gnu.org/licenses/>
 */


#include "PrimitiveDraw.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"
#include "Parameters.h"

void SphereDraw::configureProgram()
{
	glProgramUniform1f (Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_CURVRADIUS],
                    Parameters::getInstance()->g_curvradius);
	glProgramUniform1f(Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_SIZETEX],
                    Parameters::getInstance()->g_sizetex);
	glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW],
                    Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_SCENE_SIZE],
                    Parameters::getInstance()->g_geometry.scale[0], Parameters::getInstance()->g_geometry.scale[1], Parameters::getInstance()->g_geometry.scale[2]);
}

void SphereDraw::loadProgram()
{
	GLuint *program = &Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW];
    
    fprintf (stderr, "loading sphere drawing program... "); fflush (stderr);
    gk::GLCompiler& c = gk::loadProgram( SHADER_PATH("drawsphere.glsl"));
	c.include(SHADER_PATH("potential.glsl"));
    c.include(SHADER_PATH("noise.glsl") );
    c.include(SHADER_PATH("octree_common.glsl") );
    c.include(SHADER_PATH("ltree.glsl") );
    GLProgram* tmp = c.make();
    if (tmp->errors)
        exit(-1);
    *program = tmp->name;

    Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_SCENE_SIZE] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW], "u_scene_size");
    Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_CURVRADIUS] =
		glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW], "u_curv_radius");
	Parameters::getInstance()->g_uniform_locations[LOCATION_SPHEREDRAW_SIZETEX] =
        glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW], "u_size_tex");

    glLinkProgram (*program);

    configureProgram();
}

void SphereDraw::loadBuffers()
{
	//Cube data, used for cells drawing
	
	nb_vertices = 0;
	int u = 30;
	int v = 30;

	GLfloat* g_sphereVertices = (GLfloat*)malloc(sizeof(GLfloat)*(u+1)*(v+1)*3);

	float pi = 3.14159;
	for(float j=0; j<=pi; j+=pi/(float)v)
	for(float i=0; i<=pi*2; i+=(pi*2)/(float)u)
	{
		float x = cos(i)*sin(j);
		float z = sin(i)*sin(j);
		float y = cos(j);

		g_sphereVertices[nb_vertices*3] = x;
		g_sphereVertices[nb_vertices*3+1] = y;
		g_sphereVertices[nb_vertices*3+2] = z;

		nb_vertices++;
	}

	printf("Loaded sphere with %d vertices\n", nb_vertices);

	GLushort* g_sphereIndexes = (GLushort*)malloc(sizeof(GLushort)*nb_vertices*6);

	nb_indices = 0;
	nb_triangles = 0;
	for (int i=0; i<=u; i++)
    for (int j=0; j<v; j++)
    {

                int first = (i * v) + j;
                int second = first + v;
                
                g_sphereIndexes[nb_indices++] = first;
                g_sphereIndexes[nb_indices++] = second+1;
                g_sphereIndexes[nb_indices++] = first+1;

                g_sphereIndexes[nb_indices++] = second+1;
                g_sphereIndexes[nb_indices++] = second;
                g_sphereIndexes[nb_indices++] = first;

                nb_triangles += 2;
    }

	printf("and %d triangles\n", nb_triangles);

	//DrawSphere

	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_VERTEX_SPHERE]);
	glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_SPHERE]);
	glBufferData (GL_ARRAY_BUFFER, sizeof(GLfloat)*nb_vertices*3, g_sphereVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers (1, &Parameters::getInstance()->g_buffers[BUFFER_INDEX_SPHERE]);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_SPHERE]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*nb_indices, g_sphereIndexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void SphereDraw::loadVA()
{
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_SPHERE_DRAW);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SPHERE_DRAW]);
		glEnableVertexAttribArray(0);
		glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_SPHERE]);
		glVertexAttribPointer (0, 3, GL_FLOAT, 0, 0, 0);
		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_SPHERE]);
	glBindVertexArray(0);
}

void SphereDraw::init()
{
	loadBuffers();
	loadVA();
	loadProgram();
}

void SphereDraw::run()
{
	configureProgram();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(Parameters::getInstance()->g_programs[PROGRAM_SPHEREDRAW]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_SPHERE_DRAW]);

	glDrawElements(GL_TRIANGLES, nb_triangles*3, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

