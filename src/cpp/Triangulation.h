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


#ifndef TRANSVOXELS_H_
#define TRANSVOXELS_H_

#include "Parameters.h"
#include "TriangulationTables.h"

class RegularTriangulation
{
	
private:
	void loadTables();
	void loadBuffers();
	
public:
	void init();
	
	void configurePrograms();
	void loadPrograms();
	
	void runTriangulation(GLuint nbCells);
};

class TransitionTriangulation
{	
private:
	void loadTables();
	void loadBuffers();
	
public:
	void init();
	
	void configurePrograms();
	void loadPrograms();
	
	void runTriangulation(GLuint nbCells);
};

class Triangulation
{
private:
	RegularTriangulation rt;
	TransitionTriangulation tt;
	
	void loadVA();
public:
	void init();
	
	void loadPrograms() { rt.loadPrograms(); tt.loadPrograms(); }
	void runRegular(GLuint nbCells);
	void runTransition(GLuint nbCells);
};

#endif