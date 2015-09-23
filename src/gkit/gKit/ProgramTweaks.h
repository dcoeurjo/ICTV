
#ifndef _PROGRAM_TWEAKS_H
#define _PROGRAM_TWEAKS_H

#include <map>

#include "GL/GLProgram.h"
#include "Widgets/nvWidgets.h"
#include "Widgets/nvSDLContext.h"


namespace gk {
    
class GLProgramUniforms;

class TweakManager
{
    // non copyable
    TweakManager( const TweakManager& );
    TweakManager& operator= ( const TweakManager& );
    
    TweakManager( );
    ~TweakManager( );
    
    std::map<GLProgram *, GLProgramUniforms *> programs;
    
    nv::Rect tweaks_panel;
    bool tweaks_unfold;
    
public:
    int tweak( GLProgram *program );
    int doTweaks( nv::UIContext *widgets, GLProgram *program );
    
    // singleton
    static TweakManager& manager( ) 
    {
        static TweakManager object;
        return object;
    }
};


inline
int tweakProgram( GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return TweakManager::manager().tweak(program);
}

inline
int doTweaks( nv::UIContext *widgets, GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return TweakManager::manager().doTweaks(widgets, program);
}

inline
int doTweaks( nv::SdlContext& widgets, GLProgram *program )
{
    if(program == NULL || program == GLProgram::null()) return -1;
    return TweakManager::manager().doTweaks(&widgets, program);
}

} // namespace

#endif
