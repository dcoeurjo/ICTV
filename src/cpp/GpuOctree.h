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


#ifndef OCTREEHANDLER_H_
#define OCTREEHANDLER_H_

#include "Parameters.h"

#include <string.h> //memcpy

//Camera Data for culling
void streamFrustum();

class GPUOctree
{
private:
  /* Buffers */
  void loadBuffers();
  
  /* VA */
  void loadVertexArrays();
  
  /* Feedback */
  void loadFeedback();
  
public:
  GPUOctree() {};
  ~GPUOctree() {};
  
  void init();
  
  /* Programs */
  void loadPrograms();
  void configurePrograms();
  
  /*Run*/
  void runLod(GLuint* unmovedCells, GLuint* queryResult_Lod);
  void runCull(GLuint* queryResult_regular, GLuint* queryResult_transition, GLuint64* sync_time );
  void runDisplay(int nb);
};

#endif