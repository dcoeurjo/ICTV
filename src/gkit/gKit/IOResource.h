#ifndef _IORESOURCE_H
#define _IORESOURCE_H

#include <string>

#include "IOFile.h"


namespace gk {

//! classe de base des objets crees a partir d'un fichier.
class IOResource
{
    // non copyable
    IOResource( const IOResource& );
    IOResource& operator=( const IOResource& );
    
public:
    //! constructeur par defaut.
    IOResource( ) : resource(), released(false) {}
    //! constructeur.
    IOResource( const std::string& filename ) : resource(filename), released(false) {}
    
    //! constructeur par copie.
    IOResource( const IOFile& info ) : resource(info), released(false) {}
    
    //! destructeur.
    virtual ~IOResource( ) {}
    
    IOFile resource;    //!< taille et date du fichier charge.
    bool released;      //!< vrai si les donnees ne sont plus disponibles. cf. ImageManager::reload() par exemple.
};

}       // namespace

#endif
