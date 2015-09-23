
#ifndef _IMAGE_MANAGER_H
#define  _IMAGE_MANAGER_H

#include <string>

#include "ImageIO.h"


namespace gk {
    
//! cache et gestion memoire simplifiee pour charger des images. utilise les fonctionnalites d' ImageIO.
class ImageManager
{
    // non copyable
    ImageManager( const ImageManager& );
    ImageManager& operator= (const ImageManager& );
    
protected:
    std::vector<Image *> m_images;
    std::vector<std::string> m_paths;
    
    ImageManager( );
    ~ImageManager( );

public:
    //! ajoute un repertoire pour charger les images.
    int searchPath( const std::string& path );
    
    //! recherche une image dans les repertoires associes au manager. renvoie le chemin d'acces complet de l'image.
    std::string search( const std::string& filename );
    
    //! renvoie l'image, et la charge ma premiere fois.
    Image *read( const std::string& filename );
    
    //! renvoie une image par defaut.
    Image *defaultImage( );
    
    //! recherche une image d'apres son nom. renvoie l'image, si elle est disponible dans le cache.
    Image *find( const std::string& filename );

    //! ajoute une image au cache.
    Image *insert( Image *object );
    
    //! recharge une image modifiee (depuis sa mise en  cache).
    int reload( Image *object );
    
    //! recharge toutes les images modifiees (depuis leur mise en cache).
    int reload( );
    
    //! renvoie une instance unqiue de l'objet 'cache'.
    static 
    ImageManager& manager( ) 
    {
        static ImageManager object;
        return object;
    }
};

//! utilitaire: acces simplifie au singleton ImageManager.
inline
int imagePath( const std::string& path )
{
    return ImageManager::manager().searchPath(path);
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline
std::string searchImage( const std::string& filename )
{
    return ImageManager::manager().search(filename);
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline
Image *readImage( const std::string& filename )
{
    return ImageManager::manager().read(filename);
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline
int writeImage( const std::string& filename, Image *image )
{
    return ImageIO::writeImage(filename, image);
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline
Image *defaultImage( )
{
    return ImageManager::manager().defaultImage();
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline
Image *findImage( const std::string& filename )
{
    return ImageManager::manager().defaultImage();
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline 
int reloadImage( Image *image )
{
    return ImageManager::manager().reload(image);
}

//! utilitaire: acces simplifie au singleton ImageManager.
inline 
int reloadImages( )
{
    return ImageManager::manager().reload();
}

//! fonction utilitaire, importee de ImageIO. 
//! charge une sequence d'images numerotees de 1 a n et construit un ImageArray. les images chargees sont mises en cache.
/*! exemple :
    \code
    // charge les images envmap[1..n].bmp 
    const char *format= "images/envmap_%05d.bmp";
    gk::ImageArray *envmap= gk::readImageArray(format);
    
    // parcours les n images chargees
    for(unsigned int i= 0; i < envmap->images.size(); i++)
        ... faire quelquechose avec envmap->images[i] 
    \endcode
*/
ImageArray *readImageArray( const std::string& format, const unsigned int size= -1u );

//! fonction utilitaire, importee de ImageIO. 
//! enregistre toutes les image du ImageArray dans une sequence d'images numerotees.
/*! exemple :
    \code
    gk::ImageArray *array= ... ;
    const char *format= "images/resultats_%05d.png";
    gk::writeImageArray(format, array);
    \endcode
*/
int writeImageArray( const std::string& format, ImageArray *array );

}       // namespace

#endif
