
#ifndef _GL_SAMPLER_H
#define _GL_SAMPLER_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation d'un sampler opengl.    
//! \ingroup OpenGL.
class GLSampler : public GLResource
{
    // non copyable
    GLSampler( const GLSampler& );
    GLSampler& operator= ( const GLSampler& );

public:
    //! constructeur par defaut.
    GLSampler( ) : GLResource() {}
    //! constructeur d'un objet nomme, cf khr_debug.
    GLSampler( const char *_label ) : GLResource(_label) {}
    
    //! creation du sampler opengl.
    GLSampler *create( const GLenum wrap, const GLenum min_filter, const GLenum mag_filter )
    {
        assert(name == 0 && "create sampler error");
        glGenSamplers(1, &name);
        if(name == 0) 
            return this;
        
        manage();       // insere l'objet dans le manager
        glSamplerParameteri(name, GL_TEXTURE_WRAP_S, wrap);
        glSamplerParameteri(name, GL_TEXTURE_WRAP_T, wrap);
        glSamplerParameteri(name, GL_TEXTURE_WRAP_R, wrap);
        glSamplerParameteri(name, GL_TEXTURE_MIN_FILTER, min_filter);
        glSamplerParameteri(name, GL_TEXTURE_MAG_FILTER, mag_filter);
        return this;
    }
    
    //! destruction du sampler opengl.
    void release( )
    {
        if(name != 0)
            glDeleteSamplers(1, &name);
        name= 0;
    }
    
    //! destructeur.
    ~GLSampler( ) {}
    
    //! 
    static 
    GLSampler *null( )
    {
        static GLSampler object("null sampler");
        return &object;
    }
};

//! fonction utilitaire : creation d'un sampler.
//! \ingroup OpenGL.
inline
GLSampler *createSampler( const GLenum wrap= GL_REPEAT, const GLenum min_filter= GL_LINEAR_MIPMAP_LINEAR, const GLenum mag_filter= GL_LINEAR )
{
    return (new GLSampler())->create(wrap, min_filter, mag_filter);
}

//! fonction utilitaire : creation d'un sampler avec filtrage lineaire (cf construire les niveaux de mipmaps des textures).
//! \ingroup OpenGL.
inline
GLSampler *createLinearSampler( const GLenum wrap= GL_REPEAT )
{
    GLSampler *sampler= (new GLSampler())->create(wrap, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    return sampler;
}

//! fonction utilitaire : creation d'un sampler avec fitlrage anisotrope.
//! \ingroup OpenGL.
inline
GLSampler *createAnisotropicSampler( const float filter, const GLenum wrap= GL_REPEAT, const GLenum min_filter= GL_LINEAR_MIPMAP_LINEAR, const GLenum mag_filter= GL_LINEAR )
{
    GLSampler *sampler= (new GLSampler())->create(wrap, min_filter, mag_filter);
    glSamplerParameterf(sampler->name, GL_TEXTURE_MAX_ANISOTROPY_EXT, filter);
    return sampler;
}

//! fonction utilitaire : creation d'un sampler pour une texture de profondeur GL_DEPTH_COMPONENT.
//! \ingroup OpenGL.
inline
GLSampler *createDepthSampler( const GLenum wrap= GL_CLAMP_TO_EDGE, const GLenum min_filter= GL_NEAREST_MIPMAP_NEAREST, const GLenum mag_filter= GL_NEAREST )
{
    GLSampler *sampler= (new GLSampler())->create(wrap, min_filter, mag_filter);
    glSamplerParameteri(sampler->name, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    return sampler;
}


//! fonction utilitaire : renvoie un sampler par defaut.
inline
GLSampler *defaultSampler( )
{
    static GLSampler *object= NULL;
    if(object == NULL)
        object= (new GLSampler("default sampler"))->create(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    
    return object;
}

}       // namespace

#endif
