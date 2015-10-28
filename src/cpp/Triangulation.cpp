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


#include "Triangulation.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"

void Triangulation::loadVA()
{
	//Drawing the octree
        //As the buffers pingpong between LoD and Culling, it requires two VAO to pingpong with
	glGenVertexArrays (1, Parameters::getInstance()->g_vertex_arrays + VERTEX_ARRAY_OCTREE_RENDER1);
	glBindVertexArray(Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_OCTREE_RENDER1]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_LTREE_DATA1]);
	glVertexAttribIPointer(0, 4, GL_UNSIGNED_INT, 0, 0);
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
	glVertexAttribIPointer(0, 4, GL_UNSIGNED_INT, 0, 0);
	glVertexAttribDivisor(0, 1);
	glBindBuffer (GL_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_VERTEX_CUBE]);
	glVertexAttribPointer (1, 3, GL_FLOAT, 0, 0, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, Parameters::getInstance()->g_buffers[BUFFER_INDEX_CUBE]);
	glBindVertexArray(0);
}


void Triangulation::init()
{
	rt.init();
	tt.init();
	loadVA();
}

void Triangulation::runRegular(GLuint nbCells)
{
	rt.runTriangulation(nbCells);
}

void Triangulation::runTransition(GLuint nbCells)
{
	tt.runTriangulation(nbCells);
}


