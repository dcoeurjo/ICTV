#ifndef BLITFRAMEBUFFER_H_
#define BLITFRAMEBUFFER_H_

#include "Parameters.h"

void updateFramebufferRegion();

class BlitFramebuffer
{
private:
  void loadProgram();
  void loadFramebuffers();
  void loadFramebuffersTextures();
  
public:
  BlitFramebuffer() {}
  ~BlitFramebuffer() {}
  
  void init() { loadFramebuffersTextures(); loadFramebuffers(); loadProgram(); }
  void blit();
};

#endif