#ifndef CURVATURE_H_
#define CURVATURE_H_


#include "Parameters.h"

class Curvature
{
private:
	void configureProgram();
	void loadTextures();
	void loadVA();
public:
	void loadProgram();
	
	void init();
	void run(GLuint nbcells_reg, GLuint nbcells_tr, GLuint* nb_triangles_regular, GLuint* nb_triangles_transition, GLuint64* sync_time);
};

#endif