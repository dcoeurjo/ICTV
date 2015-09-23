
#include "Image.h"
#include "ImageArray.h"
#include "GL/GLTexture.h"
#include "ImageIO.h"


namespace gk {

TextureFormat TextureBGRA= TextureFormat( GL_RGBA8, GL_BGRA_EXT, GL_UNSIGNED_BYTE );
    
TextureFormat TextureRGBA= TextureFormat( GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE );
TextureFormat TextureRGB16F= TextureFormat( GL_RGB16F, GL_RGB, GL_FLOAT );
TextureFormat TextureRGBA16F= TextureFormat( GL_RGBA16F, GL_RGBA, GL_FLOAT );
TextureFormat TextureRGB32F= TextureFormat( GL_RGB32F, GL_RGB, GL_FLOAT );
TextureFormat TextureRGBA32F= TextureFormat( GL_RGBA32F, GL_RGBA, GL_FLOAT );

TextureFormat TextureDepth= TextureFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT );
TextureFormat TextureDepth24= TextureFormat( GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT );
TextureFormat TextureDepth32= TextureFormat( GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT );

TextureFormat TextureR32F= TextureFormat( GL_R32F, GL_RED, GL_FLOAT );
TextureFormat TextureRG32F= TextureFormat( GL_RG32F, GL_RG, GL_FLOAT );
TextureFormat TextureR16F= TextureFormat( GL_R16F, GL_RED, GL_FLOAT );
TextureFormat TextureRG16F= TextureFormat( GL_RG16F, GL_RG, GL_FLOAT );
    
TextureFormat TextureR32UI= TextureFormat( GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT );
TextureFormat TextureRG32UI= TextureFormat( GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT );
TextureFormat TextureR16UI= TextureFormat( GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT );
TextureFormat TextureRG16UI= TextureFormat( GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT );

TextureFormat TextureR32I= TextureFormat( GL_R32I, GL_RED_INTEGER, GL_INT );

TextureFormat TextureRGBA_MS4= TextureFormat( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 4 );
TextureFormat TextureDepth_MS4= TextureFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 4 );
TextureFormat TextureRGBA_MS8= TextureFormat( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 8 );
TextureFormat TextureDepth_MS8= TextureFormat( GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 8 );


GLTexture *GLTexture::createTexture2D( const int _unit, 
    const int _width, const int _height, const TextureFormat& _format )
{
    if(_format.samples == 0)
    {
        // creation d'une texture standard
        create(_unit, GL_TEXTURE_2D, _width, _height, 1, _format);
        if(name == 0)
            return this;
        
    #if 0
        //! BIG \todo initialiser proprement les filtres et le nombre de mipmaps a la creation des textures...
        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    #endif

        // initialise la texture a 0
        // alloue un buffer assez gros pour tous les formats
        std::vector<unsigned int> zeros(width * height * 4, 0);
        
        glTexImage2D(target, 0, 
            format.internal, width, height, 0,
            format.data_format, format.data_type, &zeros.front());
        
        glGenerateMipmap(target);
    }
    else
    {
        // creation d'un buffer de rendu msaa, utilisation avec un framebuffer.
        create(_unit, GL_TEXTURE_2D_MULTISAMPLE, _width, _height, 1, _format);
        if(name == 0)
            return this;
        
        glTexImage2DMultisample(target, format.samples, 
            format.internal, width, height, format.fixed_samples ? GL_TRUE : GL_FALSE);
    }
    
    return this;
}

GLTexture *GLTexture::createTexture2D( const int _unit, Image *image, const TextureFormat& _format )
{
    if(image == NULL)
        return this;

    create(_unit, GL_TEXTURE_2D, image->width, image->height, 1, _format);
    if(name == 0)
        return this;
    
    GLenum data_format= 0;
    switch(image->channels)
    {
        case 1: data_format= GL_RED; break;
        case 2: data_format= GL_RG; break;
        case 3: data_format= GL_RGB; break;
        case 4: data_format= GL_RGBA; break;
        default: assert(0 && "invalid image format");
    }
    
    GLenum data_type= GL_UNSIGNED_BYTE;
    switch(image->type)
    {
        case Image::UNSIGNED_BYTE: data_type= GL_UNSIGNED_BYTE; break;
        case Image::FLOAT: data_type= GL_FLOAT; break;
        default: assert(0 && "invalid image format");
    }
    
    glTexImage2D(target, 0, 
        format.internal, width, height, 0,
        data_format, data_type, image->data);
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTexture2DArray( const int _unit, 
    const int _width, const int _height, const int _count, const TextureFormat& _format )
{
    create(_unit, GL_TEXTURE_2D_ARRAY, _width, _height, _count, _format);
    if(name == 0)
        return this;
    
    // alloue un buffer assez gros pour tous les formats
    std::vector<unsigned int> zeros(width * height * depth * 4, 0);
    
    glTexImage3D(target, 0, 
        format.internal, width, height, depth, 0,
        format.data_format, format.data_type, &zeros.front());
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTexture2DArray( const int _unit, ImageArray *array, const TextureFormat& _format )
{
    if(array == NULL || array->images.size() == 0)
        return this;
    
    int w, h;
    if(array->validate(w, h) < 0)
        return this;
    
    create(_unit, GL_TEXTURE_2D_ARRAY, w, h, array->images.size(), _format);
    if(name == 0)
        return this;
    
    glTexImage3D(target, 0, 
        format.internal, width, height, depth, 0,
        format.data_format, format.data_type, NULL);
    
    for(unsigned int i= 0; i < array->images.size(); i++)
    {
        GLenum data_format= 0;
        switch(array->images[i]->channels)
        {
            case 1: data_format= GL_RED; break;
            case 2: data_format= GL_RG; break;
            case 3: data_format= GL_RGB; break;
            case 4: data_format= GL_RGBA; break;
            default: assert(0 && "invalid image format");
        }
        
        GLenum data_type= GL_UNSIGNED_BYTE;
        switch(array->images[i]->type)
        {
            case Image::UNSIGNED_BYTE: data_type= GL_UNSIGNED_BYTE; break;
            case Image::FLOAT: data_type= GL_FLOAT; break;
            default: assert(0 && "invalid image format");
        }
        
        glTexSubImage3D(target, 0, 
            0, 0, i,  
            width, height, 1, 
            data_format, data_type, array->images[i]->data);
    }
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTextureCube( const int _unit, 
    const int _width, const int _height, const TextureFormat& _format )
{
    create(_unit, GL_TEXTURE_CUBE_MAP, _width, _height, 1, _format);
    if(name == 0)
        return this;
    
    // alloue un buffer assez gros pour tous les formats
    std::vector<unsigned int> zeros(width * height * 4, 0);
    
    for(unsigned int face= 0; face < 6; face++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 
            format.internal, width, height, 0,
            format.data_format, format.data_type, &zeros.front());
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTextureCube( const int _unit, ImageArray *array, const TextureFormat& _format )
{
    if(array == NULL || array->images.size() != 6)
    {
        ERROR("texture cube creation error: 6 images required.\n");
        return this;
    }
    
    int w, h;
    if(array->validate(w, h) < 0)
        return this;
    
    create(_unit, GL_TEXTURE_CUBE_MAP, w, h, 1, _format);
    if(name == 0)
        return this;
    
    for(unsigned int face= 0; face < 6; face++)
    {
        GLenum data_format= 0;
        switch(array->images[face]->channels)
        {
            case 1: data_format= GL_RED; break;
            case 2: data_format= GL_RG; break;
            case 3: data_format= GL_RGB; break;
            case 4: data_format= GL_RGBA; break;
            default: assert(0 && "invalid image format");
        }
        
        GLenum data_type= GL_UNSIGNED_BYTE;
        switch(array->images[face]->type)
        {
            case Image::UNSIGNED_BYTE: data_type= GL_UNSIGNED_BYTE; break;
            case Image::FLOAT: data_type= GL_FLOAT; break;
            default: assert(0 && "invalid image format");
        }
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 
            format.internal, width, height, 0,
            data_format, data_type, array->images[face]->data);
    }
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTexture3D( const int _unit, 
    const int _width, const int _height, const int _depth, const TextureFormat& _format )
{
    create(_unit, GL_TEXTURE_3D, _width, _height, _depth, _format);
    if(name == 0)
        return this;
    
    glTexImage3D(target, 0, 
        format.internal, width, height, depth, 0,
        format.data_format, format.data_type, NULL);
    
    glGenerateMipmap(target);
    return this;
}

GLTexture *GLTexture::createTexture3D( const int _unit, ImageArray *array, const TextureFormat& _format )
{
    if(array == NULL || array->images.size() == 0)
        return this;
    
    int w, h;
    if(array->validate(w, h) < 0)
        return this;
    
    create(_unit, GL_TEXTURE_3D, w, h, array->images.size(), _format);
    if(name == 0)
        return this;
    
    glTexImage3D(target, 0, 
        format.internal, width, height, depth, 0,
        format.data_format, format.data_type, NULL);
    
    for(unsigned int i= 0; i < array->images.size(); i++)
    {
        GLenum data_format= 0;
        switch(array->images[i]->channels)
        {
            case 1: data_format= GL_RED; break;
            case 2: data_format= GL_RG; break;
            case 3: data_format= GL_RGB; break;
            case 4: data_format= GL_RGBA; break;
            default: assert(0 && "invalid image format");
        }
        
        GLenum data_type= GL_UNSIGNED_BYTE;
        switch(array->images[i]->type)
        {
            case Image::UNSIGNED_BYTE: data_type= GL_UNSIGNED_BYTE; break;
            case Image::FLOAT: data_type= GL_FLOAT; break;
            default: assert(0 && "invalid image format");
        }
        
        glTexSubImage3D(target, 0, 
            0, 0, i,  
            width, height, 1, 
            data_format, data_type, array->images[i]->data);
    }
    
    glGenerateMipmap(target);
    return this;
}


GLTexture *GLTexture::createTexture3D( const int _unit, Image *image, const TextureFormat& _format )
{
    if(image == NULL || image->depth < 2)
        return this;
    
    create(_unit, GL_TEXTURE_3D, image->width, image->height, image->depth, _format);
    if(name == 0)
        return this;
    
    glTexImage3D(target, 0, 
        format.internal, width, height, depth, 0,
        format.data_format, format.data_type, NULL);
    
    for(int i= 0; i < image->depth; i++)
    {
        GLenum data_format= 0;
        switch(image->channels)
        {
            case 1: data_format= GL_RED; break;
            case 2: data_format= GL_RG; break;
            case 3: data_format= GL_RGB; break;
            case 4: data_format= GL_RGBA; break;
            default: assert(0 && "invalid image format");
        }
        
        GLenum data_type= GL_UNSIGNED_BYTE;
        switch(image->type)
        {
            case Image::UNSIGNED_BYTE: data_type= GL_UNSIGNED_BYTE; break;
            case Image::FLOAT: data_type= GL_FLOAT; break;
            default: assert(0 && "invalid image format");
        }
        
        glTexSubImage3D(target, 0, 
            0, 0, i,  
            width, height, 1, 
            data_format, data_type, image->data + image->offset(0, 0, i) );
    }
    
    glGenerateMipmap(target);
    return this;
}

Image *GLTexture::image( const int _unit, const GLenum _target, const int _level )
{
    int channels= 0;
    GLenum glchannels= 0;
    switch(format.internal)
    {
        case GL_RED:
        case GL_R8:
        case GL_R16F:
        case GL_R32F:
        case GL_R16UI:
        case GL_R32UI:
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
        case GL_DEPTH_COMPONENT32F:
            channels= 1;
            glchannels= GL_RED;
            break;
        
        case GL_RG:
        case GL_RG8:
        case GL_RG16F:
        case GL_RG32F:
        case GL_RG16UI:
        case GL_RG32UI:
            channels= 2;
            glchannels= GL_RG;
            break;
        
        case GL_RGB:
        case GL_RGB8:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_RGB16UI:
        case GL_RGB32UI:
            channels= 3;
            glchannels= GL_RGB;
            break;
        
        case GL_RGBA:
        case GL_RGBA8:
        case GL_RGBA16F:
        case GL_RGBA32F:
        case GL_RGBA16UI:
        case GL_RGBA32UI:
            channels= 4;
            glchannels= GL_RGBA;
            break;
        
        default:
            assert(0 && "invalid texture format");
    }
    
    unsigned int type= 0;
    GLenum gltype= 0;
    switch(format.internal)
    {
        case GL_RED:
        case GL_R8:
        case GL_RG:
        case GL_RG8:
        case GL_RGB:
        case GL_RGB8:
        case GL_RGBA:
        case GL_RGBA8:
            type= Image::UNSIGNED_BYTE;
            gltype= GL_UNSIGNED_BYTE;
            break;
            
        case GL_R16F:
        case GL_RG16F:
        case GL_RGB16F:
        case GL_RGBA16F:
        
        case GL_R32F:
        case GL_RG32F:
        case GL_RGB32F:
        case GL_RGBA32F:
        
        case GL_R16UI:
        case GL_RG16UI:
        case GL_RGB16UI:
        case GL_RGBA16UI:
        
        case GL_R32UI:
        case GL_RG32UI:
        case GL_RGB32UI:
        case GL_RGBA32UI:
        
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32:
        case GL_DEPTH_COMPONENT32F:
            type= Image::FLOAT;
            gltype= GL_FLOAT;
            break;
        
        default:
            assert(0 && "invalid texture format");
    }
    
    Image *image= (new Image())->create(std::max(1, width / (1<< _level)), std::max(1, height / (1<< _level)), std::max(1, depth / (1<< _level)), channels, type);
    if(image == NULL)
        return NULL;
    
    glActiveTexture(GL_TEXTURE0 + _unit);
    glBindTexture(target, name);
    
    #ifndef NDEBUG
    {
        GLint w= 0; glGetTexLevelParameteriv(_target, _level, GL_TEXTURE_WIDTH, &w);
        GLint h= 0; glGetTexLevelParameteriv(_target, _level, GL_TEXTURE_HEIGHT, &h);
        if(w != image->width || h != image->height)
            ERROR("GetTexImage: wrong dimensions %dx%d, level %d, %dx%d\n", image->width, image->height, _level, w, h);
    }
    #endif
    
    glGetTexImage(_target, _level, glchannels, gltype, image->data);
    
    return image;
}

Image *GLTexture::image( const int _unit )
{
    return image(_unit, target);
}

ImageArray *GLTexture::imageArray( const int _unit )
{
    ImageArray *array= new ImageArray();
    
    if(target == GL_TEXTURE_CUBE_MAP)
    {
        for(unsigned int i= 0; i < 6; i++)
            array->push_back( image(_unit, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i) );
    }
    else
    {
        Image *tmp= image(_unit);
        if(tmp == NULL)
            return NULL;
        
        array->push_back(tmp);
        for(int i= 1; i < tmp->depth; i++)
            array->push_back( (new Image())->reference(tmp, tmp->offset(0, 0, i)) );
    }
    
    return array;
}


int writeFramebuffer( const std::string& filename )
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    Image *image= (new Image())->create(viewport[2], viewport[3], 4, Image::UNSIGNED_BYTE);
    if(image == NULL)
        return -1;
    
    glFinish();
    glReadBuffer(GL_BACK);
    glReadPixels(viewport[0], viewport[1], viewport[2], viewport[3], 
        GL_RGBA, GL_UNSIGNED_BYTE, image->data);
    
    int code= ImageIO::writeImage(filename, image);
    delete image;
    return code;
}

}       // namespace gk
