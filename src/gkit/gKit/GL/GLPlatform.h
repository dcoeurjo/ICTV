
#ifndef _GK_GLPLATFORM_H
#define _GK_GLPLATFORM_H

#define GLEW_NO_GLU

#ifdef APPLE_OSX
    #include <GL/glew.h>
//    #include <OpenGL/gl.h>

#elif defined WIN32
    #include <GL/glew.h>
//    #include <GL/gl.h>

#else
    #include "GL/glew.h"
//    #include <GL/gl.h>
#endif

#ifdef GLDEBUGGER    
    //~ #include "Debugger/GLDebugger.h"
    //~ #include "Debugger/AppDebugger.h"
    
    #define glDrawArrays(...) gk::AppDebugger::drawArrays(  __VA_ARGS__ )
    #define glDrawElements(...) gk::AppDebugger::.drawElements(  __VA_ARGS__ )
#endif

#if defined GK_OPENGL3 && !defined GLEW_VERSION_3_3
    #error glew does not support openGL 3.3
#endif

#if defined GK_OPENGL4 && !defined GLEW_VERSION_4_2
    #error glew does not support openGL 4.2
#endif

#if defined GK_OPENGL4 && !defined GLEW_VERSION_4_3
    #warning glew does not support openGL 4.3
#endif

#if defined GK_OPENGL4 && !defined GLEW_VERSION_4_4
    #warning glew does not support openGL 4.4
#endif

#endif
