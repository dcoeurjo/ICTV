
#include <cassert>
#include <cstring>

#include "Image.h"


namespace gk {

//! construction d'une image de dimension wxh. 
Image *Image::create( const int _w, const int _h, const int _channels, const unsigned int _type, const unsigned int _length, void *_data )
{
    width= _w;
    height= _h;
    depth= 1;
    channels= _channels;
    type= _type;
    released= false;
    
    switch(type)
    {
        case UNSIGNED_BYTE: pixel_sizeof= sizeof(unsigned char); break;
        case FLOAT: pixel_sizeof= sizeof(float); break;
        
        default:
            //~ printf("image: type %d, channels %d\n", _type, _channels);
            assert(0 && "invalid image type");
    }
    
    int length= width * height * pixel_sizeof * channels;
    if(_length > 0) length= _length;
    data= new unsigned char[length];
    if(_data != NULL)
        memcpy(data, _data, length);
    
    return this;
}

//! construction d'une image de dimension wxhxd. 
Image *Image::create( const int _w, const int _h, const int _d, const int _channels, const unsigned int _type, const unsigned int _length, void *_data )
{
    width= _w;
    height= _h;
    depth= _d;
    channels= _channels;
    type= _type;
    released= false;
    
    switch(type)
    {
        case UNSIGNED_BYTE: pixel_sizeof= sizeof(unsigned char); break;
        case FLOAT: pixel_sizeof= sizeof(float); break;
        default:
            assert(0 && "invalid image type");
    }
    
    int length= width * height * depth * pixel_sizeof * channels;
    if(_length > 0) length= _length;
    data= new unsigned char[length];
    if(_data != NULL)
        memcpy(data, _data, length);
    
    return this;
}

Image *Image::reference( Image *image, const unsigned int offset, const int _width, const int _height, const int _depth )
{
    if(image == NULL)
        return this;
    
    width= image->width;
    if(_width > 0) width= _width;
    height= image->height;
    if(_height > 0) height= _height;
    depth= image->depth;
    if(_depth > 0) depth= _depth;
    
    channels= image->channels;
    type= image->type;
    pixel_sizeof= image->pixel_sizeof;
    data= image->data + offset;
    reference_data= true;
    released= false;
    
    return this;
}

//! destruction des donnees de l'image / de la ressource.
void Image::release( )
{
    if(released) return;
    
    if(reference_data == false)
        delete [] data;
    data= NULL;
    released= true;
}

    
Image *createImage( const int w, const int h, const VecColor& color, const int channels, const unsigned int type )
{
    Image *image= createImage(w, h, channels, type);
    
    // remplit la premiere ligne
    for(int x= 0; x < image->width; x++)
        image->setPixel(x, 0, color);
    
    // duplique la premiere ligne sur les suivantes
    for(int y= 1; y < image->height; y++)
        memcpy(image->data + image->offset(0, y), image->data, image->width * image->channels * image->pixel_sizeof);
    
    return image;
}


} // namespace
