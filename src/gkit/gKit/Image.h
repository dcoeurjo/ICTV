
#ifndef _IMAGE_H
#define _IMAGE_H


#include "Vec.h"
#include "IOResource.h"


namespace gk {

//! representation d'une image.
class Image : public IOResource
{
    Image( const Image & );
    Image& operator=( const Image& );

protected:
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int offset, const VecColor& color )
    {
        switch(type)
        {
            case UNSIGNED_BYTE: 
            {
                unsigned char *p= (unsigned char *) (data + offset);
                switch(channels)        // un seul test au lieu de 3, devrait etre plus rapide
                {
                    case 4: p[3]= color.a * 255.f;
                    case 3: p[2]= color.b * 255.f;
                    case 2: p[1]= color.g * 255.f;
                    case 1: p[0]= color.r * 255.f;      
                        break;
                    default:
                        assert(0 && "invalid image channel count");
                }
                
                //~ p[0]= color.r * 255.f;
                //~ if(channels > 1) p[1]= color.g * 255.f;
                //~ if(channels > 2) p[2]= color.b * 255.f;
                //~ if(channels > 3) p[3]= color.a * 255.f;
            }
            break;
            
            case FLOAT:
            {
                float *p= (float *) (data + offset);
                switch(channels)
                {
                    case 4: p[3]= color.a;
                    case 3: p[2]= color.b;
                    case 2: p[1]= color.g;
                    case 1: p[0]= color.r;
                        break;
                    default:
                        assert(0 && "invalid image channel count");
                }
                
                //~ p[0]= color.r;
                //~ if(channels > 1) p[1]= color.g;
                //~ if(channels > 2) p[2]= color.b;
                //~ if(channels > 3) p[3]= color.a;
            }
            break;
            
            default:
                assert(0 && "invalid image type");
        }
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int offset ) const
    {
        VecColor color(0, 0, 0, 1);
        switch(type)
        {
            case UNSIGNED_BYTE:
            {
                unsigned char *p= (unsigned char *) (data + offset);
                switch(channels)        // un seul test au lieu de 3, devrait etre plus rapide
                {
                    case 4: color.a= (float) p[3] / 255.f;
                    case 3: color.b= (float) p[2] / 255.f;
                    case 2: color.g= (float) p[1] / 255.f;
                    case 1: color.r= (float) p[0] / 255.f;
                        break;
                    default:
                        assert(0 && "invalid image channel count");
                }
                
                //~ color.r= (float) p[0] / 255.f;
                //~ if(channels > 1) color.g= (float) p[1] / 255.f;
                //~ if(channels > 2) color.b= (float) p[2] / 255.f;
                //~ if(channels > 3) color.a= (float) p[3] / 255.f;
            }
            break;
            
            case FLOAT:
            {
                float *p= (float *) (data + offset);
                switch(channels)
                {
                    case 4: color.a= (float) p[3];
                    case 3: color.b= (float) p[2];
                    case 2: color.g= (float) p[1];
                    case 1: color.r= (float) p[0];
                        break;
                    default:
                        assert(0 && "invalid image channel count");
                }
                
                //~ float *p= (float *) (data + offset);
                //~ color.r= (float) p[0];
                //~ if(channels > 1) color.g= (float) p[1];
                //~ if(channels > 2) color.b= (float) p[2];
                //~ if(channels > 3) color.a= (float) p[3];
            }
            break;
            
            default:
                assert(0 && "invalid image type");
        }
        
        return color;
    }
    
public:
    int width;
    int height;
    int depth;
    int channels;
    unsigned int type;
    int pixel_sizeof;
    unsigned char *data;
    bool reference_data;
    //! \todo ajouter TextureFormat format ...

    enum {
        // undef= 0,
        UNSIGNED_BYTE= 1,
        FLOAT
    };
    
    //! constructeur par defaut.
    Image( ) 
        : 
        IOResource(), 
        width(0), height(0), depth(0), channels(0), type(0), pixel_sizeof(0), data(NULL), reference_data(false)
    {}
    
    //! constructeur d'une image nommee / associee a un fichier.
    Image( const std::string& filename ) 
        : 
        IOResource(filename), 
        width(0), height(0), depth(0), channels(0), type(0), pixel_sizeof(0), data(NULL), reference_data(false)
    {}
    
    //! destructeur.
    ~Image( )
    {
        release();
    }
    
    //! construction d'une image de dimension wxh. 
    Image *create( const int _w, const int _h, const int _channels, const unsigned int _type, const unsigned int _length= 0, void *_data= NULL );
    
    //! construction d'une image de dimension wxhxd. 
    Image *create( const int _w, const int _h, const int _d, const int _channels, const unsigned int _type, const unsigned int _length= 0, void *_data= NULL );
    
    Image *reference( Image *image, const unsigned int offset= 0u, const int width= 0, const int height= 0, const int depth= 0 );
    
    //! destruction des donnees de l'image / de la ressource.
    void release( );
    
    //! adressage lineaire d'un pixel (x,y).
    unsigned int offset( const unsigned int x, const unsigned int y ) const
    {
        assert(x < (unsigned int) width);
        assert(y < (unsigned int) height);
        // z= 0;
        return (y * width + x) * channels * pixel_sizeof;
    }
    
    //! adressage lineaire d'un pixel (x,y, z).
    unsigned int offset( const unsigned int x, const unsigned int y, const unsigned int z ) const
    {
        assert(x < (unsigned int) width);
        assert(y < (unsigned int) height);
        assert(z < (unsigned int) depth);
        return (z * width * height + y * width + x) * channels * pixel_sizeof;
    }
    
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int x, const unsigned int y, const VecColor& color )
    {
        setPixel(offset(x,y), color);
    }
    
    //! modifie la valeur d'un pixel.
    void setPixel( const unsigned int x, const unsigned int y, const unsigned int z, const VecColor& color )
    {
        setPixel(offset(x, y, z), color);
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int x, const unsigned int y ) const
    {
        return pixel(offset(x, y));
    }
    
    //! renvoie la valeur d'un pixel.
    VecColor pixel( const unsigned int x, const unsigned int y, const unsigned int z ) const
    {
        return pixel(offset(x, y, z));
    }    
};


//! fonction utilitaire : simplifie la creation d'une image "standard".
inline
Image *createImage( const int w, const int h, const int channels= 4, const unsigned int type= Image::FLOAT )
{
    return (new Image())->create(w, h, channels, type);
}

//! fonction utilitaire : simplifie la creation d'une image de couleur constante.
Image *createImage( const int w, const int h, const VecColor& color, const int channels= 4, const unsigned int type= Image::FLOAT );


}       // namespace

#endif
