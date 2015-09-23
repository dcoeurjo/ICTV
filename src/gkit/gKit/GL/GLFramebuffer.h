
#ifndef _GL_FRAMEBUFFER_H
#define _GL_FRAMEBUFFER_H

#include "Logger.h"

#include "GL/GLPlatform.h"
#include "GLResource.h"

#include "GL/GLTexture.h"


namespace gk {

//! representation d'un framebuffer opengl.
//! \ingroup OpenGL.
class GLFramebuffer : public GLResource
{
    // non copyable
    GLFramebuffer( const GLFramebuffer& );
    GLFramebuffer& operator= ( const GLFramebuffer& );
    
public:
    //! identification des buffers associees au framebuffer.
    enum 
    {
        COLOR0= 0,
        COLOR1= 1,
        COLOR2= 2,
        COLOR3= 3,
        COLOR4= 4,
        COLOR5= 5,
        COLOR6= 6,
        COLOR7= 7,
        FRAMEBUFFER_LAST_COLOR= 8,
        DEPTH= 8,
        FRAMEBUFFER_LAST= 9
    };

    //! creation des textures par le framebuffer : identification des textures/draw buffers a initialiser.
    enum
    {
        COLOR0_BIT= 1<<COLOR0,
        COLOR1_BIT= 1<<COLOR1,
        COLOR2_BIT= 1<<COLOR2,
        COLOR3_BIT= 1<<COLOR3,
        COLOR4_BIT= 1<<COLOR4,
        COLOR5_BIT= 1<<COLOR5,
        COLOR6_BIT= 1<<COLOR6,
        COLOR7_BIT= 1<<COLOR7,
        DEPTH_BIT= 1<<DEPTH
    };

    std::vector< GLTexture * > buffers;
    GLTexture *depth_buffer;
    int width;
    int height;
    
    //! constructeur par defaut.
    GLFramebuffer( ) : GLResource(), buffers(FRAMEBUFFER_LAST, GLTexture::null()), depth_buffer(GLTexture::null()), width(0), height(0) {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLFramebuffer( const char *_label ) : GLResource(_label), buffers(FRAMEBUFFER_LAST, GLTexture::null()), depth_buffer(GLTexture::null()), width(0), height(0) {}
    
    //! creation et activation d'un framebuffer. \param _target est GL_DRAW_FRAMEBUFFER ou GL_READ_FRAMEBUFFER.
    GLFramebuffer *create( const GLenum _target )
    {
        assert(name == 0 && "create framebuffer error");
        glGenFramebuffers(1, &name);
        if(name == 0) 
            return this;
        
        manage();       // insere l'objet dans le manager
        glBindFramebuffer(_target, name);
        return this;
    }
    
    //! destruction du framebuffer.
    void release( )
    {
        if(name != 0)
            glDeleteFramebuffers(1, &name);
        name= 0;
    }
    
    //! desctruction de l'objet c++. l'objet opengl doit etre deja detruit par release().
    ~GLFramebuffer( ) {}
    
    //! creation, activation et configuration d'un framebuffer de dimensions width * height. 
    //! les textures designees par buffer_bits sont crees et attachees au framebuffer. 
    //! les textures couleurs sont crees avec le format color_format et la texture de profondeur est cree avec le format depth_format.
    //! \param buffer_bits est une combinaison des flags COLOR0_BIT | COLOR1_BIT | DEPTH_BIT, etc.
    GLFramebuffer *create( const GLenum _target, const int _width, const int _height, const unsigned int buffer_bits, 
        const TextureFormat& color_format= TextureRGBA, const TextureFormat& depth_format= TextureDepth )
    {
        create(_target);
        if(name == 0)
            return this;
        
        width= _width;
        height= _height;
        
        for(unsigned int i= COLOR0; i < FRAMEBUFFER_LAST_COLOR; i++)
        {
            if((buffer_bits & (1<<i)) == 0)
                continue;
            
            GLTexture *color= (new GLTexture())->createTexture2D(GLTexture::UNIT0, width, height, color_format);
            buffers[i]= color;
            
            glFramebufferTexture(_target, GL_COLOR_ATTACHMENT0 + i, color->name, 0);
            glBindTexture(color->target, 0);
        }
        
        if(buffer_bits & DEPTH_BIT)
        {
            depth_buffer= (new GLTexture())->createTexture2D(GLTexture::UNIT0, width, height, depth_format);
            
            glFramebufferTexture(_target, GL_DEPTH_ATTACHMENT, depth_buffer->name, 0);
            glBindTexture(depth_buffer->target, 0);
        }
        
        return this;
    }
    
    //! attache une texture au draw buffer du framebuffer et selectionne le framebuffer sur _target.
    GLFramebuffer *attach( const GLenum _target, const unsigned int buffer, GLTexture *texture )
    {
        if(texture == NULL || texture == GLTexture::null())
            return this;
        
        if(width == 0 || height == 0)
        {
            width= texture->width;
            height= texture->height;
        }
        
        if(texture->width != width || texture->height != height)
            return this;
        
        glBindFramebuffer(_target, name);
        if(buffer == DEPTH)
        {
            depth_buffer= texture;
            glFramebufferTexture(_target, GL_DEPTH_ATTACHMENT, texture->name, 0);
        }
        else if(buffer < FRAMEBUFFER_LAST_COLOR)
        {
            buffers[buffer]= texture;
            glFramebufferTexture(_target, GL_COLOR_ATTACHMENT0 + buffer, texture->name, 0);
        }
        
        return this;
    }
    
    //! renvoie la texture attachee au framebuffer, si elle existe ou GL::Texture::null().
    //! exemple: framebuffer->texure(COLOR0), ou framebuffer->texture(DEPTH).
    GLTexture *texture( const unsigned int buffer )
    {
        if(buffer == DEPTH)
            return depth_buffer;
        else if(buffer < FRAMEBUFFER_LAST_COLOR)
            return buffers[buffer];
        
        else
            return GLTexture::null();
    }
    
    //! renvoie la texture de profondeur attachee au framebuffer, si elle existe ou GL::Texture::null().
    //! equivalent a framebuffer->texture(DEPTH)
    GLTexture *depth( )
    {
        return depth_buffer;
    }
    
    //! configure les draw buffers indiques par buffer_bits (tous par defaut).
    int drawBuffers( const unsigned int buffer_bits= -1u )
    {
        GLenum tmp[FRAMEBUFFER_LAST_COLOR];
        int n= 0;
        
        for(unsigned int i= COLOR0; i < FRAMEBUFFER_LAST_COLOR; i++)
        {
            if(buffers[i] == NULL || buffers[i] == GLTexture::null())
                continue;
            
            if(buffer_bits & (1<<i))
            {
                tmp[n]= GL_COLOR_ATTACHMENT0 + n;
                n++;
            }
        }
        
        glDrawBuffers(n, tmp);
        return (n > 0) ? 0 : -1;        // renvoie -1 si aucun buffer n'est selectionne
    }
};


inline
GLFramebuffer *createFramebuffer( const GLenum _target, const int _width, const int _height, const unsigned int buffer_bits, 
    const TextureFormat& color_format= TextureRGBA, const TextureFormat& depth_format= TextureDepth )
{
    return (new GLFramebuffer())->create(_target, _width, _height, buffer_bits, color_format, depth_format);
}

}       // namespace
#endif
