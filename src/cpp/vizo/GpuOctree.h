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