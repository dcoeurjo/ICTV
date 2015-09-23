
#ifndef _GL_TEXTURE_H
#define _GL_TEXTURE_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

//! representation du format des pixels d'une texture.
//! \ingroup OpenGL.
struct TextureFormat
{
    GLenum internal;
    GLenum data_format;
    GLenum data_type;
    int samples;
    bool fixed_samples;
    
    TextureFormat( )
        :
        internal(GL_RGBA),
        data_format(GL_RGBA),
        data_type(GL_UNSIGNED_BYTE),
        samples(0),
        fixed_samples(false)
    {}
    
    TextureFormat( const GLenum _internal, const GLenum _format, 
        const GLenum _type, const int _samples= 0, const bool _fixed= false )
        :
        internal(_internal),
        data_format(_format),
        data_type(_type),
        samples(_samples),
        fixed_samples(_fixed)
    {}
};

extern TextureFormat TextureBGRA;

extern TextureFormat TextureRGBA;
extern TextureFormat TextureRGB16F;
extern TextureFormat TextureRGBA16F;
extern TextureFormat TextureRGB32F;
extern TextureFormat TextureRGBA32F;
extern TextureFormat TextureDepth;
extern TextureFormat TextureDepth24;
extern TextureFormat TextureDepth32;

extern TextureFormat TextureR32UI;
extern TextureFormat TextureRG32UI;
extern TextureFormat TextureR16UI;
extern TextureFormat TextureRG16UI;
extern TextureFormat TextureR32F;
extern TextureFormat TextureRG32F;
extern TextureFormat TextureR16F;
extern TextureFormat TextureRG16F;

extern TextureFormat TextureR32I;

extern TextureFormat TextureRGBA_MS4;
extern TextureFormat TextureDepth_MS4;
extern TextureFormat TextureRGBA_MS8;
extern TextureFormat TextureDepth_MS8;

class Image;
class ImageArray;

//! representation d'une texture.
//! \ingroup OpenGL.
class GLTexture : public GLResource
{
    GLTexture( const GLTexture& );
    GLTexture& operator=( const GLTexture& );
    
public:
    enum {
        UNIT0= 0,
        UNIT1,
        UNIT2,
        UNIT3,
        UNIT4,
        UNIT5,
        UNIT6,
        UNIT7,
    };
    
    TextureFormat format;       //!< format, nombre de canaux, type de chaque canal.
    GLenum target;              //!< type de la texture 2d, 3d, etc.
    int width;                  //!< largeur.
    int height;                 //!< hauteur.
    int depth;                  //!< profondeur ou 1.

    //! constructeur par defaut.
    GLTexture( ) : GLResource( ), format(), target(GL_TEXTURE_2D), width(0), height(0), depth(0)  {}
    //! constructeur d'une texture nommee, cf khr_debug.
    GLTexture( const char *_label ) : GLResource(_label), format(), target(0), width(0), height(0), depth(0) {}
    
    //! desctructeur.
    ~GLTexture( ) {}
    
    //! creation de la texture opengl.
    GLTexture *create( const int _unit, 
        const GLenum _target, const int _width, const int _height, const int _depth, const TextureFormat& _format )
    {
        assert(name == 0 && "texture creation error");
        glGenTextures(1, &name);
        if(name == 0)
            return this;
        
        format= _format;
        target= _target;
        width= _width;
        height= _height;
        depth= _depth;
        manage();
        
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(target, name);
        
        return this;
    }
    
    //! utilisation exeperimentale. construit un objet GLTexture a partir d'une texture opengl deja cree.
    GLTexture *attach( const int _unit, 
        const GLenum _target, GLuint _name, const int _width, const int _height, const int _depth, const TextureFormat& _format )
    {
        assert(name == 0 && "texture creation error");
        if(_name == 0)
            return this;
        
        name= _name;
        format= _format;
        target= _target;
        width= _width;
        height= _height;
        depth= _depth;
        manage();
        
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(target, name);
        
        return this;
    }
    
    //! destruction de la texture opengl.
    void release( )
    {
        if(name != 0)
            glDeleteTextures(1, &name);
        name= 0;
    }
    
    //! creation d'une texture 2d, vide.
    GLTexture *createTexture2D( const int _unit, 
        const int _width, const int _height, const TextureFormat& _format= TextureRGBA );
    //! creation d'une texture 2d avec le contenu de l'image.
    GLTexture *createTexture2D( const int _unit, Image *image, const TextureFormat& _format= TextureRGBA );
    
    //! creation d'un texture 2d array, vide.
    GLTexture *createTexture2DArray( const int _unit, 
        const int _width, const int _height, const int _count, const TextureFormat& _format= TextureRGBA );
    //! creation d'un texture 2d array avec le contenu des images.
    GLTexture *createTexture2DArray( const int _unit, ImageArray *array, const TextureFormat& _format= TextureRGBA );
    
    //! creation d'une texture cube map, vide.
    GLTexture *createTextureCube( const int _unit, 
        const int _width, const int _height, const TextureFormat& _format= TextureRGBA );
    //! creation d'une texture cube map avec le contenu de 6 images.
    GLTexture *createTextureCube( const int _unit, ImageArray *array, const TextureFormat& _format= TextureRGBA );
    
    //! creation d'une texture 3d, vide.
    GLTexture *createTexture3D( const int _unit, 
        const int _width, const int _height, const int _depth, const TextureFormat& _format= TextureRGBA );
    //! creation d'une texture 3d, avec le contenu d'une image multi couche.
    GLTexture *createTexture3D( const int _unit, Image *image, const TextureFormat& _format= TextureRGBA );
    //! creation d'une texture 3d, avec le contenu des images.
    GLTexture *createTexture3D( const int _unit, ImageArray *array, const TextureFormat& _format= TextureRGBA );
    
    //! utilisation interne. renvoie le contenu de la texture.
    Image *image( const int _unit, const GLenum _target, const int _level= 0 );
    //! renvoie le contenu de la texture.
    Image *image( const int unit );
    //! renvoie le contenu de la texture, ensemble d'images.
    ImageArray *imageArray( const int unit );
    
    //! renvoie un objet c++ associe a la texture 0.
    static
    GLTexture *null( )
    {
        static GLTexture object("null texture");
        return &object;
    }
};

//! \ingroup OpenGL.
inline
GLTexture *createTexture2D( const int unit, const int width, const int height, const TextureFormat& format= TextureRGBA )
{
    return (new GLTexture())->createTexture2D(unit, width, height, format);
}

//! \ingroup OpenGL.
inline
GLTexture *createTexture2D( const int unit, Image *image, const TextureFormat& format= TextureRGBA )
{
    //! \todo nommer les textures avec leur filename : return (new GLTexture(image->resource->filename))->createTexture2D(unit, image, format);
    return (new GLTexture())->createTexture2D(unit, image, format);
}

//! \ingroup OpenGL.
inline
GLTexture *createTexture2DArray( const int unit, ImageArray *array, const TextureFormat& format= TextureRGBA )
{
    //! \todo nommer les textures avec leur filename
    return (new GLTexture())->createTexture2DArray(unit, array, format);
}

//! \ingroup OpenGL.
inline
GLTexture *createTextureCube( const int unit, ImageArray *array, const TextureFormat& format= TextureRGBA )
{
    //! \todo nommer les textures avec leur filename
    return (new GLTexture())->createTextureCube(unit, array, format);
}

//! \ingroup OpenGL.
inline
GLTexture *createTexture3D( const int unit, Image *image, const TextureFormat& format= TextureRGBA )
{
    //! \todo nommer les textures avec leur filename
    return (new GLTexture())->createTexture3D(unit, image, format);
}

//! enregistre le framebuffer.
//! \ingroup OpenGL.
int writeFramebuffer( const std::string& filename );

}       // namespace


#endif
