
#ifndef _GL_VAO_H
#define _GL_VAO_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation d'un buffer openGL.    
//! \ingroup OpenGL.
class GLVertexArray : public GLResource
{
    // non copyable
    GLVertexArray( const GLVertexArray& );
    GLVertexArray& operator= ( const GLVertexArray& );
    
public:
    //! constructeur par defaut.
    GLVertexArray( ) : GLResource() {}
    //! constructeur d'un vertex array nomme, cf khr_debug.
    GLVertexArray( const char *_label ) : GLResource(_label) {}

    //! creation du vertex array opengl.
    GLVertexArray *create( )
    {
        assert(name == 0 && "vertex array creation error");
        
        glGenVertexArrays(1, &name);
        if(name == 0)
            return this;
        
        manage();
        glBindVertexArray(name);
        return this;
    }
    
    //! destruction du vertex array opengl.
    void release( )
    {
        if(name != 0)
            glDeleteVertexArrays(1, &name);
        name= 0;
    }
    
    //! destructeur.
    ~GLVertexArray( ) {}
    
    //! renvoie le vertex array opengl 0.
    static
    GLVertexArray *null( )
    {
        static GLVertexArray object("null vertex array");
        return &object;
    }
};

//! fonction utilitaire :  creation d'un vertex array.
//! \ingroup OpenGL.
inline
GLVertexArray *createVertexArray( )
{
    return (new GLVertexArray())->create();
}

//! fonction utilitaire :  creation d'un vertex array.
//! \ingroup OpenGL.
inline
GLVertexArray *createVertexArray( const char *_label )
{
    return (new GLVertexArray(_label))->create();
}

}       // namespace

#endif
