
#ifndef _GL_BUFFER_H
#define _GL_BUFFER_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation d'un buffer openGL.
//! \ingroup OpenGL.
class GLBuffer : public GLResource
{
    // non copyable
    GLBuffer( const GLBuffer& );
    GLBuffer& operator= ( const GLBuffer& );

public:
    unsigned int length;        //!< taille du buffer (en octets)
    GLbitfield storage;         //!< buffer storage flags

    unsigned char *map;         //!< map= glBufferMap(map_target)

    //! constructeur par defaut.
    GLBuffer( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLBuffer( const char *_label ) : GLResource(_label) {}

    //! creation d'un buffer, eventuellement initialise.
    GLBuffer *create( const GLenum target, const unsigned int _length, const void *data,
        const GLenum usage= GL_STATIC_DRAW )
    {
        assert(name == 0 && "create buffer error");
        glGenBuffers(1, &name);
        if(name == 0)
            return this;

        length= _length;
        storage= 0;
        map= NULL;
        manage();       // insere l'objet dans le manager
        glBindBuffer(target, name);
        glBufferData(target, length, data, usage);
        return this;
    }

    //! creation d'un buffer, eventuellement initialise.
    GLBuffer *createStorage( const GLenum target, const unsigned int _length, const void *data,
        const GLbitfield _flags= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT )
    {
        assert(name == 0 && "create buffer storage error");
        glGenBuffers(1, &name);
        if(name == 0)
            return this;

        length= _length;
        storage= _flags;
        map= NULL;
        manage();       // insere l'objet dans le manager

        glBindBuffer(target, name);
        glBufferStorage(target, length, data, storage);
        return this;
    }

    //! destruction du buffer.
    void release( )
    {
        if(name != 0)
            glDeleteBuffers(1, &name);
        name= 0;
    }

    //! destructeur de l'objet c++, l'objet opengl doit etre deja detruit par release( ).
    ~GLBuffer( ) {}

    unsigned char* mapRange( const GLenum _target, const GLbitfield _flags, const GLintptr _offset= 0, const GLsizeiptr _length= 0 )
    {
        glBindBuffer(_target, name);

        // par defaut mappe la totalite du buffer
        GLintptr range_offset= 0;
        GLsizeiptr range_length= length;
        if(_offset > 0) range_offset =_offset;
        if(_length > 0) range_length= _length;

        map= (unsigned char *) glMapBufferRange(_target, range_offset, range_length, _flags);
        if(map == NULL)
            ERROR("buffer map failed.\n");
        return map;
    }

    int unmap( const GLenum _target )
    {
        if(map == NULL)
            return -1;

        glUnmapBuffer(_target);
        map= NULL;
        return 0;
    }

    //! renvoie un objet c++ associe au buffer 0.
    static
    GLBuffer *null( )
    {
        static GLBuffer object("null buffer");
        return &object;
    }
};


//! fonction utilitaire : creation simplifiee d'un buffer initialise. utilise glBufferData pour construire le buffer.
//! \ingroup OpenGL.
inline
GLBuffer *createBuffer( const GLenum target, const unsigned int length, const void *data,
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, length, data, usage);
}

//! fonction utilitaire : creation simplifiee d'un buffer storage initialise. utilise glBufferStorage pour construire le buffer.
//! \ingroup OpenGL.
inline
GLBuffer *createBufferStorage( const GLenum target, const unsigned int length, const void *data,
    const GLbitfield flags= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT )
{
    return (new GLBuffer())->createStorage(target, length, data, flags);
}

//! fonction utilitaire : creation simplifiee d'un buffer initialise. utilise glBufferData pour construire le buffer.
//! \ingroup OpenGL.
template < typename T >
GLBuffer *createBuffer( const GLenum target, const std::vector<T>& data,
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, data.size() * sizeof(T), &data.front(), usage);
}

//! fonction utilitaire : creation simplifiee d'un buffer storage initialise. utilise glBufferStorage pour construire le buffer.
//! \ingroup OpenGL.
template < typename T >
GLBuffer *createBufferStorage( const GLenum target, const std::vector<T>& data,
    const GLbitfield flags= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT )
{
    return (new GLBuffer())->createStorage(target, data.size() * sizeof(T), &data.front(), flags);
}

//! fonction utilitaire : creation simplifiee d'un buffer sans copie de donnees. utilise glBufferData(...NULL) pour construire le buffer.
//! \ingroup OpenGL.
inline
GLBuffer *createBuffer( const GLenum target, const unsigned int length,
    const GLenum usage= GL_STATIC_DRAW )
{
    return (new GLBuffer())->create(target, length, NULL, usage);
}

}       // namespace

#endif
