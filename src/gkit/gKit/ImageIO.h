
#ifndef _IMAGE_IO_H
#define _IMAGE_IO_H

#include <string>


namespace gk {

class Image;
class ImageArray;


//! chargement / enregistrement d'une image, d'un ensemble de d'images. version "bas niveau" sans cache, ni gestion memoire.
namespace ImageIO {


//! charge une image.
    //! \todo renvoyer Image::null() en cas d'echec
Image *readImage( const std::string& filename, const int _channels= 0, const unsigned int _type= 0 );

//! charge une sequence d'images numerotees de 1 a n et construit un ImageArray.
/*! exemple :
    \code
    // charge les images envmap*.bmp 
    const char *format= "images/envmap_%05d.bmp";
    gk::ImageArray *envmap= gk::ImageIO::readImageArray(format);
    
    // parcours les n images chargees
    for(unsigned int i= 0; i < envmap->images.size(); i++)
        ... faire quelquechose avec envmap->images[i] 
    \endcode
*/
    //! \todo renvouer Image::null() en cas d'echec
ImageArray *readImageArray( const std::string& format, const int _channels= 0, const unsigned int _type= 0, const unsigned int size= -1u );

//! enregistre une image.
int writeImage( const std::string& filename, Image *image );
    
//! enregistre toutes les image du ImageArray dans une sequence d'images numerotees.
/*! exemple :
    \code
    gk::ImageArray *array= ... ;
    const char *format= "images/resultats_%05d.png";
    gk::ImageIO::writeImageArray(format, array);
    \endcode
*/
int writeImageArray( const std::string& format, ImageArray *array );

//! renvoie une image par defaut ("debug.png"). utile en cas d'erreur de lecture d'une autre image, par exemple.
Image *defaultImage( );

}       // namespace

}       // namespace

#endif
