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


#ifndef DATALOADER_H_
#define DATALOADER_H_

#include "Parameters.h"

#include <cstdlib>

class DataLoader
{
protected:
	unsigned int sizex;
	unsigned int sizey;
	unsigned int sizez;
	
	float* data;
	unsigned char* data_char;
public:
	virtual void loadFile(char* file) { sizex = sizey = sizez = 0; }
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	
	int getSize() { return sizex; }
	
	virtual void loadData32BGpu();
	virtual void loadData8BGpu();
	/*virtual void loadx2y2z2();
	virtual void loadxyyzxz();
	virtual void loadxyz();*/
	
	virtual void releaseData() { free(data); }
};

/*class DataTerrain : public DataLoader
{
public:	
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	virtual void loadFile(char* file);
};*/

class DataRaw : public DataLoader
{
public:	
	DataRaw(int size) { sizex=sizey=sizez=size; }
	virtual float* get32BData() { return data; }
	virtual unsigned char* get8BData() { return data_char; }
	virtual void loadFile(char* file);
};


#endif