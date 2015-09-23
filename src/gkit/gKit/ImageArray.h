
#ifndef _IMAGE_ARRAY_H
#define _IMAGE_ARRAY_H

#include <vector>

#include "Image.h"


namespace gk {

//! representation d'un ensemble d'images.
class ImageArray
{
    ImageArray( const ImageArray& );
    ImageArray& operator= ( const ImageArray& );
    
protected:

public:
    std::vector< Image * > images;      //!< vecteur d'images.
    int width;                          //!< largeur commune des images.
    int height;                         //!< hauteur commune des images.
    
    //! constructeur par defaut.
    ImageArray( ) : images(), width(0), height(0) {}
    
    //! destructeur.
    ~ImageArray( )
    {
        for(unsigned int i= 0; i < images.size(); i++)
            delete images[i];
    }
    
    //! detruit l'ensemble d'images.
    void clear( ) { images.clear(); width= 0; height= 0; }
    
    //! insere une image dans l'ensemble.
    int push_back( Image *image )
    {
        images.push_back(image);
        return images.size() -1;
    }
    
    //! verifie que toutes les images ont les memes dimensions.
    int validate( int& width, int& height );
    
    //! renvoie la ieme image de l'ensemble. ou NULL si elle n'existe pas.
    const Image *operator[] ( const unsigned int index ) const
    {
        if(index >= (unsigned int) images.size()) return NULL;
        return images[index];
    }
    
    //! renvoie la ieme image de l'ensemble. ou NULL si elle n'existe pas.
    Image *& operator[] ( const unsigned int index )
    {
        assert(index < (unsigned int) images.size());
        return images[index];
    }
};


class ImageLevels
{
    ImageLevels( const ImageLevels& );
    ImageLevels& operator= ( const ImageLevels& );
    
public:
    std::vector<Image *> levels;
    int width;
    int height;
    unsigned long int length;
    unsigned char *data;
    
    ImageLevels( ) : levels(), width(0), height(0), length(0), data(NULL) {}
    
    ~ImageLevels( )
    {
        for(unsigned int i= 0; i < levels.size(); i++)
            delete levels[i];
    }
    
    ImageLevels *create( const int w, const int h, const int _channels, const unsigned int _type );
    
    ImageLevels *create( Image *image );
    
    //! renvoie la ieme image de l'ensemble. ou NULL si elle n'existe pas.
    const Image *operator[] ( const unsigned int index ) const
    {
        if(index >= (unsigned int) levels.size())
            return NULL;
        
        return levels[index];
    }
    
    //! renvoie la ieme image de l'ensemble. ou NULL si elle n'existe pas.
    Image *& operator[] ( const unsigned int index )
    {
        assert(index < (unsigned int) levels.size());
        return levels[index];
    }
};

}

#endif
