#ifndef SHADING_H_
#define SHADING_H_


#include "Parameters.h"

class Shading
{
private:
	void configureProgram();
	void loadTextures();
	void loadVA();
public:
	void loadProgram();
	
	void init();
	void run(GLuint nb_triangles_regular);
};

#endif