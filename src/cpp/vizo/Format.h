#ifndef FORMAT_H_
#define FORMAT_H_

#include "Widgets/nvSDLContext.h"

//! classe utilitaire : permet de construire une chaine de caracteres formatee. cf sprintf.
struct Format
{
    char text[1024];

    Format( const char *_format, ... )
    {
        text[0]= 0;     // chaine vide

        // recupere la liste d'arguments supplementaires
        va_list args;
        va_start(args, _format);
        vsnprintf(text, sizeof(text), _format, args);
        va_end(args);
    }

    ~Format( ) {}

    // conversion implicite de l'objet en chaine de caracteres stantard
    operator const char *( )
    {
        return text;
    }
};

#endif