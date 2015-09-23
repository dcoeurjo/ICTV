
#include "SDLImagePlatform.h"

#include "Image.h"
#include "ImageArray.h"
#include "ImageIO.h"

#include "IOFileSystem.h"

#include "Logger.h"

#ifdef __cplusplus
extern "C" {
#include "rgbe.h"
}
#endif

#ifdef GK_OPENEXR
#include <ImfRgbaFile.h>
#include <ImfChannelList.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imf;
using namespace Imath;
#endif


namespace gk {

namespace ImageIO {

static
bool is_color_image( const std::string& filename )
{
    return ((filename.rfind(".bmp") != std::string::npos)
        || (filename.rfind(".png") != std::string::npos)
        || (filename.rfind(".jpg") != std::string::npos)
        || (filename.rfind(".jpeg") != std::string::npos)
        || (filename.rfind(".tga") != std::string::npos));
}

static
bool is_hdr_image( const std::string& filename )
{
    return (filename.rfind(".hdr") != std::string::npos);
}

#ifdef GK_OPENEXR
static
bool is_exr_image( const std::string& filename )
{
    return (filename.rfind(".exr") != std::string::npos);
}
#endif


Image *readImage( const std::string& filename, const int _channels, const unsigned int _type )
{
    Image *image= NULL;
    if(is_color_image(filename))
    {
        MESSAGE("loading color image '%s'...\n", filename.c_str());

        // importer le fichier en utilisant SDL_image
        SDL_Surface *surface= IMG_Load(filename.c_str());
        if(surface == NULL)
        {
            ERROR("loading color image '%s'... failed.\n", filename.c_str());
            return NULL;
        }

        // verifier le format, rgb ou rgba
        const SDL_PixelFormat format= *surface->format;
        if(format.BitsPerPixel != 24 && format.BitsPerPixel != 32)
        {
            ERROR("loading color image '%s'... failed.\n", filename.c_str());
            SDL_FreeSurface(surface);
            return NULL;
        }

        int height= surface->h;
        int width= surface->w;
        int channels= (format.BitsPerPixel == 32) ? 4 : 3;
        if(_channels > 0) channels= _channels;
        unsigned int type= Image::UNSIGNED_BYTE;
        if(_type > 0) type= _type;

        image= (new Image(filename))->create(width, height, channels, type);
        if(image == NULL)
            return NULL;

        // converti les donnees en pixel rgba, retourne l'image, openGL utilise une origine en bas a gauche.
        Uint8 *p= (Uint8 *) surface->pixels;
        int py= 0;

        if(format.BitsPerPixel == 32)
        {
            for(int y= height -1; y >= 0; y--, py++)
            {
                p= (Uint8 *) surface->pixels + py * surface->pitch;
                for(int x= 0; x < width; x++, p+= format.BytesPerPixel)
                {
                    const Uint8 r= p[format.Rshift / 8];
                    const Uint8 g= p[format.Gshift / 8];
                    const Uint8 b= p[format.Bshift / 8];
                    const Uint8 a= p[format.Ashift / 8];

                    image->setPixel(x, y, VecColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f));
                }
            }
        }

        else if(format.BitsPerPixel == 24)
        {
            for(int y= height -1; y >= 0; y--, py++)
            {
                p= (Uint8 *) surface->pixels + py * surface->pitch;
                for(int x= 0; x < width; x++, p+= format.BytesPerPixel)
                {
                    const Uint8 r= p[format.Rshift / 8];
                    const Uint8 g= p[format.Gshift / 8];
                    const Uint8 b= p[format.Bshift / 8];

                    image->setPixel(x, y, VecColor(r / 255.f, g / 255.f, b / 255.f, 1));
                }
            }
        }

        SDL_FreeSurface(surface);
    }

    else if(is_hdr_image(filename))
    {
        MESSAGE("loading hdr image '%s'...\n", filename.c_str());

        FILE *in= fopen(filename.c_str(), "rb");
        if(in == NULL)
        {
            ERROR("loading hdr image '%s'... failed.\n", filename.c_str());
            return NULL;
        }

        rgbe_header_info info;
        int width, height;
        if(RGBE_ReadHeader(in, &width, &height, &info) != RGBE_RETURN_SUCCESS)
        {
            fclose(in);

            ERROR("loading hdr image '%s'... failed.\n", filename.c_str());
            return NULL;
        }

        Image *flip= (new Image())->create(width, height, 3, Image::FLOAT);
        if(RGBE_ReadPixels_RLE(in, (float *) flip->data, width, height) != RGBE_RETURN_SUCCESS)
        {
            fclose(in);
            delete flip;

            ERROR("loading hdr image '%s'... failed.\n", filename.c_str());
            return NULL;
        }

        fclose(in);

        // converti les donnees en pixels hdr, retourne l'image, openGL utilise une origine en bas a gauche.
        int channels= 3;
        if(_channels > 0) channels= _channels;
        unsigned int type= Image::FLOAT;
        if(_type > 0) type= _type;

        image= (new Image(filename))->create(width, height, channels, type);
        if(image == NULL)
            return NULL;

        for(int y= 0; y < image->height; y++)
        for(int x= 0; x < image->width; x++)
            image->setPixel(x, image->height - y -1, flip->pixel(x, y));

        delete flip;
    }

#ifdef GK_OPENEXR
    else if(is_exr_image(filename))
    {
        MESSAGE("loading exr image '%s'...\n", filename.c_str());

        RgbaInputFile file(filename.c_str());
        Box2i dw= file.dataWindow();
        int width= dw.max.x - dw.min.x + 1;
        int height= dw.max.y - dw.min.y + 1;

        MESSAGE("%d x %d pixels, ", width, height);

        {
            // verifie que l'image peut etre chargee
            const ChannelList &channels= file.header().channels();
            int channelCount= 0;
            {
                for(ChannelList::ConstIterator i= channels.begin(); i != channels.end(); ++i)
                    channelCount++;
            }

            MESSAGE("%d channels:", channelCount);
            for(ChannelList::ConstIterator i= channels.begin(); i != channels.end(); ++i)
            {
                const Channel &channel= i.channel();

                if(channel.type == UINT)
                    MESSAGE(" '%s' uint", i.name());
                else if(channel.type == HALF)
                    MESSAGE(" '%s' half", i.name());
                else if(channel.type == FLOAT)
                    MESSAGE(" '%s' float", i.name());
                else
                    MESSAGE(" '%s' type 0x%x", i.name(), channel.type);
            }
            MESSAGE("\n");
            
            if(channelCount > 4)
            {
                ERROR("loading exr image '%s'... failed. not an rgb image.\n", filename.c_str());
                return NULL;
            }
        }

        Array2D<Rgba> pixels;
        pixels.resizeErase(height, width);
        file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels(dw.min.y, dw.max.y);

        int channels= 4;
        if(_channels > 0) channels= _channels;
        unsigned int type= Image::FLOAT;
        if(_type > 0) type= _type;

        image= (new Image(filename))->create(width, height, channels, type);
        if(image == NULL)
            return NULL;

        Rgba *p= pixels[0];
        for(int y= height -1; y >= 0; y--)
        for(int x= 0; x < width; x++, p++)
            image->setPixel(x, y, VecColor((float) p->r, (float) p->g, (float) p->b, (float) p->a));
    }
#endif

    else
        WARNING("loading image '%s'... not a valid format. failed.\n", filename.c_str());
    
    return image;
}


ImageArray *readImageArray( const std::string& format, const int _channels, const unsigned int _type, const unsigned int size )
{
    if(format.size() == 0)
        return NULL;

    char tmp[1024];
    ImageArray *array= new ImageArray();

    for(unsigned int i= 0; i < size; i++)
    {
        sprintf(tmp, format.c_str(), i+1);
        if(IOFileSystem::isFilename(tmp) == false)
            break;

        array->push_back( readImage(tmp, _channels, _type) );
    }

    if(array->images.size() == 0)
        WARNING("loading %u images '%s': no image. failed.\n", size, format.c_str());

    return array;
}


int writeImage( const std::string& filename, Image *image )
{
    if(image == NULL)
        return -1;

    if(filename.rfind(".bmp") != std::string::npos || filename.rfind(".png") != std::string::npos )
    {
        MESSAGE("writing color image '%s'...\n", filename.c_str());

        // flip de l'image : Y inverse entre GL et BMP
        Image *flip= (new Image())->create(image->width, image->height, 4, Image::UNSIGNED_BYTE);
        for(int y= 0; y < image->height; y++)
        for(int x= 0; x < image->width; x++)
            flip->setPixel(x, image->height - y -1, image->pixel(x, y));

        SDL_Surface *bmp= SDL_CreateRGBSurfaceFrom((void *) flip->data,
            image->width, image->height,
            32, image->width * 4,
    #if 0
            0xFF000000,
            0x00FF0000,
            0x0000FF00,
            0x000000FF
    #else
            0x000000FF,
            0x0000FF00,
            0x00FF0000,
            0xFF000000
    #endif
        );

        int code= 0;
        if(filename.rfind(".bmp") != std::string::npos)
            code= SDL_SaveBMP(bmp, filename.c_str());

        else if(filename.rfind(".png") != std::string::npos)
            code= IMG_SavePNG(bmp, filename.c_str());
        
        else
            ERROR("writing color image '%s'... invalid image format. failed.\n", filename.c_str());

        SDL_FreeSurface(bmp);
        delete flip;
        return code;
    }

    else if(is_hdr_image(filename))
    {
        if(image == NULL)
            return -1;

        MESSAGE("writing hdr image '%s'...\n", filename.c_str());

        FILE *out= fopen(filename.c_str(), "wb");
        if(out == NULL)
        {
            ERROR("writing hdr image '%s'... failed.\n", filename.c_str());
            return -1;
        }

        if(RGBE_WriteHeader(out, image->width, image->height, NULL) != RGBE_RETURN_SUCCESS)
        {
            fclose(out);

            ERROR("writing hdr image '%s'... failed.\n", filename.c_str());
            return -1;
        }

        Image *flip= (new Image())->create(image->width, image->height, 3, Image::FLOAT);
        for(int y= 0; y < image->height; y++)
        for(int x= 0; x < image->width; x++)
            flip->setPixel(x, image->height - y -1, image->pixel(x, y));

        int code= RGBE_WritePixels_RLE(out, (const float *) flip->data, image->width, image->height);
        fclose(out);
        delete flip;
        if(code != RGBE_RETURN_SUCCESS)
        {
            ERROR("writing hdr image '%s'... failed.\n", filename.c_str());
            return -1;
        }

        return 0;
    }
    
#ifdef GK_OPENEXR
    else if(is_exr_image(filename))
    {
        if(image == NULL)
            return -1;
        
        MESSAGE("writing exr image '%s'...\n", filename.c_str());
        
        RgbaOutputFile file(filename.c_str(), image->width, image->height, WRITE_RGBA);
        Array2D<Rgba> pixels(image->width, image->height);
        file.setFrameBuffer(&pixels[0][0], 1, image->width);
        
        Rgba *p= pixels[0];
        for(int y= image->height -1; y >= 0; y--)
        for(int x= 0; x < image->width; x++, p++)
        {
            gk::VecColor pixel= image->pixel(x, y);
            p->r= pixel.r;
            p->g= pixel.g;
            p->b= pixel.b;
            p->a= pixel.a;
        }
        
        file.writePixels(image->height);
        
        return 0;
    }
#endif

    ERROR("invalid image format writing '%s'. failed.\n", filename.c_str());
    return -1;
}

int writeImageArray( const std::string& format, ImageArray *array )
{
    if(format.size() == 0)
        return -1;
    if(array == NULL || array->images.size() == 0)
        return -1;

    char tmp[1024];
    for(unsigned int i= 0; i < array->images.size(); i++)
    {
        sprintf(tmp, format.c_str(), i+1);
        if(writeImage(tmp, array->images[i]) < 0)
            return -1;
    }
    
    MESSAGE("writing '%s'... %d images. done\n", format.c_str(), array->images.size());
    return 0;
}

Image *defaultImage( )
{
    static Image *object= NULL;
    if(object == NULL)
        object= readImage("debug.png");

    return object;
}

}       // namespace

}       // namespace
