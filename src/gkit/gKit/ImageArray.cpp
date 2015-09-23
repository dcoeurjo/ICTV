
#include "ImageArray.h"

namespace gk {

int ImageArray::validate( int& width, int& height )
{
    if(images.empty()) return -1;
    if(images[0] == NULL) return -1;
    
    int w= images[0]->width;
    int h= images[0]->height;
    for(unsigned int i= 1; i < images.size(); i++)
        if(images[i] == NULL || images[i]->width != w || images[i]->height != h)
            return -1;
    
    width= w;
    height= h;
    return 0;
}


ImageLevels *ImageLevels::create( const int _width, const int _height, const int _channels, const unsigned int _type )
{
    width= _width;
    height= _height;

    unsigned int pixel_sizeof= 0;
    switch(_type)
    {
        case Image::UNSIGNED_BYTE: pixel_sizeof= sizeof(unsigned char); break;
        case Image::FLOAT: pixel_sizeof= sizeof(float); break;
        default:
            assert(0 && "invalid image type");
    }
    
    // determine le nombre de mipmaps
    length= 0;
    int level= 0;
    for(; level < 16; level++)
    {
        int w= std::max(1, _width / (1<< level));
        int h= std::max(1, _height / (1<< level));
        
        length+= w * h * _channels * pixel_sizeof;
        if(w == 1 && h == 1)
            break;
    }
    
    // alloue la premiere image avec un buffer suffisant pour stocker tous les mipmaps 
    Image *level0= (new Image())->create(_width, _height, _channels, _type, length, NULL);
    if(level0->data == NULL) return this;
    data= level0->data;

    // insere les references sur les autres mipmaps
    levels.push_back(level0);
    unsigned int offset= _width * _height * _channels * pixel_sizeof;
    for(int i= 1; i < level +1; i++)
    {
        int w= std::max(1, _width / (1<< i));
        int h= std::max(1, _height / (1<< i));
        
        levels.push_back( (new Image())->reference(level0, offset, w, h, 1) );
        
        offset+= w * h * _channels * pixel_sizeof;
    }
    
    return this;
}

ImageLevels *ImageLevels::create( Image *image )
{
    if(image == NULL)
        return this;
    
    return create(image->width, image->height, image->channels, image->type);
}

    
}       // namespace
