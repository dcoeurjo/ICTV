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