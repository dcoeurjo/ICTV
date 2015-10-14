#ifndef DRAW_SPHERE_H
#define DRAW_SPHERE_H

class SphereDraw
{
private:
	int nb_vertices;
	int nb_indices;
	int nb_triangles;

	void configureProgram();
	void loadVA();
	void loadBuffers();

public:
	void loadProgram();
	
	void init();
	void run();
};

#endif