
#ifndef _GL_RESOURCE_H
#define _GL_RESOURCE_H

#include <vector>
#include <string>
#include <cassert>


#include "GL/GLPlatform.h"
#include "Logger.h"


namespace gk {

class GLResource;

//! \defgroup OpenGL representation et gestions des objets openGL.
//@{

//! ensemble d'objets openGL crees par l'application.
class GLManager
{
    GLManager( ) : m_resources() {}
    
    GLManager( const GLManager& );
    GLManager& operator=( const GLManager& );
    
protected:
    std::vector<GLResource *> m_resources;
    
public:
    ~GLManager( );
    
    //! utilisation interne. conserve un objet opengl. 
    void insert( GLResource *resource );
    
    //! detruit tous les objets opengl crees par l'application.
    void release( );
    
    //! singleton.
    static 
    GLManager& manager( )
    {
        static GLManager m;
        return m;
    }
};


//! representation d'un objet openGL, propriete du contexte openGL.
class GLResource
{
    // non copyable
    GLResource( const GLResource& );
    GLResource& operator=( const GLResource& );
    
protected:
    GLResource( ) : name(0), label() {}
    GLResource( GLuint _name, const std::string& _label ) : name(_name), label(_label) {}
    GLResource( const std::string& _label ) : name(0), label(_label) {}
    
    void manage( );

public:
    GLuint name;        //!< id opengl.
    std::string label;  //!< nom "lisible" de l'objet. cf debug output / log
    
    virtual void release( ) { assert(0 && "GLResource::release() called..."); };
    virtual ~GLResource( ) {}
};

//@}


// GLManager inline impl
inline
GLManager::~GLManager( )
{
    release();
}

inline
void GLManager::insert( GLResource *resource )
{
    if(resource != NULL)
        m_resources.push_back(resource);
}

inline
void GLManager::release( )
{
    for(unsigned int i= 0; i < m_resources.size(); i++)
    {
        if(m_resources[i] != NULL)
            m_resources[i]->release();
        delete m_resources[i];
    }
    
    m_resources.clear();
}


// GLResource inline impl
inline
void GLResource::manage( )
{
    GLManager::manager().insert(this);
}

} // namespace

#endif
