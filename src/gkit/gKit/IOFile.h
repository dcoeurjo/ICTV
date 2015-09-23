
#ifndef _IOFILE_H
#define _IOFILE_H

#include <cassert>
#include "IOFileSystem.h"


namespace gk {
    
//! representation de l'etat (taille + date) d'un fichier charge par l'application.
struct IOFile
{
    std::string filename;       //!< nom du fichier.
    IOInfo info;                //!< taille + date fichier.

    IOFile( ) :  filename(), info() {}
    
    IOFile( const IOFile& file ) : filename(file.filename), info(file.info) {}
    
    IOFile( const std::string& _filename ) : filename(_filename), info()
    {
        IOFileSystem::infos(filename, info);
    }
    
    IOFile& operator=( const IOFile& file )
    {
        filename= file.filename;
        info= file.info;
        return *this;
    }
    
    bool exists( )
    {
        if(filename.empty())
            return false;  // pas de fichier 
        return (info.exists && info.size > 0);
    }
    
    std::string readText( )
    {
        if(exists() == false)
            return "";
        return IOFileSystem::readText(filename);
    }
    
    int modified( )
    {
        if(exists() == false)
            return -1;  // pas de fichier 
        return IOFileSystem::modified(filename, info);
    }
    
    int reloadText( std::string& text )
    {
        if(exists() == false)
            return -1;  // pas de fichier 
        
        IOInfo update;
        int status= IOFileSystem::modified(filename, info, &update);
        if(status != 1)
            return status;
        
        info= update;
        text= IOFileSystem::readText(filename);
        return 1;
    }
};

}       // namespace gk

#endif
