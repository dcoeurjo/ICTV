
#ifndef _GK_SDLTTFPLATFORM_H
#define _GK_SDLTTFPLATFORM_H

#ifdef APPLE_OSX
    #include <SDL2_ttf/SDL_ttf.h>

#elif defined WIN32
    #include <SDL_ttf.h>

#else
    #include <SDL2/SDL_ttf.h>
#endif

#endif
