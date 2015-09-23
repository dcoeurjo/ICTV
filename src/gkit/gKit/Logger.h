
#ifndef _GK_LOGGER_H
#define _GK_LOGGER_H

#include <cstdio>
#include <string>
#include <set>
#include <vector>


namespace gk {

//! affiche une seule fois chaque message.
class Log
{
    // non copyable
    Log( const Log& );
    Log& operator= ( const Log& );

    std::set<std::string> m_slots;
    FILE *m_output;
    unsigned int m_level;
    
public:
    
    //! plusieurs types de messages.
    enum {
        ERROR= 0,       //!< pas bon !
        WARNING,        //!< oops
        MESSAGE,        //!< information
        DEBUGLOG        //!< information supplementaire
    };

    //! constructeur par defaut.
    Log( );
    //! destructeur.
    ~Log( );
    
    //! utilsation interne. filtre un message formate / printf.
    //! utiliser les macros MESSAGE(), WARNING() et ERROR() a la place.
    void write( const unsigned int type, const char *file, const int line, const char *function, const char *format, ... );

    //! redirige les messages vers un fichier texte.
    int setOutputFile( const char *filename );
    
    //! selectionne le type de message a afficher, dans l'ordre ERROR, WARNING, MESSAGE, DEBUGLOG.
    int setOutputLevel( const unsigned int level );
    
    //! singleton.
    static 
    Log& manager()
    {
        static Log logger;
        return logger;
    }
};

#ifndef NDEBUG
#define DEBUGLOG(format, ...) gk::Log::manager().write(gk::Log::DEBUGLOG, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__ )
#define MESSAGE(format, ...) gk::Log::manager().write(gk::Log::MESSAGE, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__ )
#define WARNING(format, ...) gk::Log::manager().write(gk::Log::WARNING, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__ )
#define ERROR(format, ...) gk::Log::manager().write(gk::Log::ERROR, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__ )

#else
// supprime toutes les sorties en mode release, sauf les erreurs
#define DEBUGLOG(format, ...) 
#define MESSAGE(format, ...) 
#define WARNING(format, ...) 
#define ERROR(format, ...) gk::Log::manager().write(gk::Log::ERROR, __FILE__, __LINE__, __FUNCTION__, format, ## __VA_ARGS__ )
#endif

}       // namespace

#endif
