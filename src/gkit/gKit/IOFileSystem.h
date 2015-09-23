
#ifndef _IOFILESYSTEM_H
#define _IOFILESYSTEM_H

#ifdef APPLE_OSX
    #include <sys/stat.h>

#elif defined _MSC_VER
    #include <sys/types.h>
    #include <sys/stat.h>

#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

#include <cstdio>
#include <string>
#include <vector>


namespace gk {

//! representation des informations sur un fichier.
struct IOInfo
{
    size_t size;        //!< longueur en octets.
    size_t time;        //!< date du fichier.
    bool exists;        //!< faux si le fichier n'existe pas / erreur

    IOInfo( )
        :
        size(0),
        time(0),
        exists(false)
    {}

    bool operator == ( const IOInfo& b ) const
    {
        return (size == b.size && time == b.time);
    }

    bool operator != ( const IOInfo& b ) const
    {
        return !(*this == b);
    }
};


class IOFileSystem
{
    // non copyable
    IOFileSystem( ) : m_base_path("./") {};
    IOFileSystem( const IOFileSystem& );
    IOFileSystem operator=( const IOFileSystem& );

    std::string m_base_path;
    
public:
    const std::string& basePath( ) const { return m_base_path; }
    IOFileSystem& basePath( const char *path ) { printf("base path: %s\n", path); m_base_path= path; return *this; }
    
    static
    IOFileSystem& manager( )
    {
        static IOFileSystem object;
        return object;
    }
    
    //! renvoie le chemin d'acces a un fichier.

    /*! le chemin est toujours termine par /
     pathname("path/to/file") == "path/to/"
     pathname("file") == "./"
     */
    static
    std::string pathname( const std::string& filename )
    {
        //! \todo normaliser les separateurs.
        size_t slash = filename.find_last_of( '/' );    // separateur linux
        size_t bslash = filename.find_last_of( '\\' );  // separateur windows

        if ( slash == std::string::npos && bslash != std::string::npos )
            slash = bslash;
        else if ( slash != std::string::npos && bslash != std::string::npos && bslash > slash )
            slash = bslash;

        if ( slash != std::string::npos )
            return filename.substr( 0, slash +1 ); // inclus le slash
        else
            return "./";
    }

    //! renvoie le nom du fichier sans son extension (chemin inclus).
    static
    std::string basename( const std::string& filename )
    {
        size_t ext= filename.find_last_of('.');
        if(ext== std::string::npos)
            return filename;    // renvoie le nom tel quel
        else
            return filename.substr(0, ext);  // n'inclus pas le point
    }

    //! verifie que le fichier est bien du type represente par 'suffix'.
    static
    bool isType( const std::string& filename, const std::string& suffix )
    {
        size_t ext= filename.find_last_of('.');
        if(ext != std::string::npos)
            return (filename.substr(ext, std::string::npos).rfind(suffix) != std::string::npos);
        else
            return (filename.rfind(suffix) != std::string::npos);
    }

    //! change l'extension du fichier.
    static
    std::string changeType( const std::string& filename, const std::string& suffix )
    {
        size_t ext= filename.find_last_of('.');
        if(ext == std::string::npos)
            return filename + suffix;
        else
            return filename.substr(0, ext) + suffix;
    }

    //! verifie l'existance d'un fichier.
    static
    int exists( const std::string& filename )
    {
    #ifndef _MSC_VER
        struct stat info;
        if(stat(filename.c_str(), &info) < 0)
            return -1;

        // verifie aussi que c'est bien un fichier standard
        return (S_ISREG(info.st_mode)) ? 0 : -1;

    #else
        //! \todo a modifier pour compiler avec codeblock/mingw sous windows, utiliser la version linux.
        struct _stat64 info;
        if(_stat64(filename.c_str(), &info) < 0)
            return -1;

        // verifie aussi que c'est bien un fichier standard
        return (info.st_mode & _S_IFREG) ? 0 : -1;
    #endif
    }

    static
    bool isFilename( const std::string& filename )
    {
    #ifndef _MSC_VER
        struct stat info;
        if(stat(filename.c_str(), &info) < 0)
            return false;

        // verifie aussi que c'est bien un fichier standard
        return (S_ISREG(info.st_mode));
	#else
		struct _stat64 info;
        if(_stat64(filename.c_str(), &info) < 0)
            return false;
		return (info.st_mode & _S_IFREG);
    #endif
    }

    //! verifie l'existance d'un repertoire
    static
    bool isPathname( const std::string& pathname )
    {
    #ifndef _MSC_VER
        struct stat info;
        if(stat(pathname.c_str(), &info) < 0)
            return false;

        // verifie aussi que c'est bien un repertoire standard
        return (S_ISDIR(info.st_mode));

	#else
		struct _stat64 info;
        if(_stat64(pathname.c_str(), &info) < 0)
            return false;
		return (info.st_mode & _S_IFDIR);
    #endif
    }

    //! renvoie les informations taille et date sur un fichier.
    //! \param size peut etre NULL si l'information n'est pas souhaitee.
    //! \param time peut etre NULL si l'information n'est pas souhaitee.
    static
    int infos( const std::string& filename, size_t *size, size_t *time )
    {
    #ifndef _MSC_VER
        struct stat info;
        if(stat(filename.c_str(), &info) < 0)
            return -1;

        if(size != NULL)
            *size= info.st_size;
        if(time != NULL)
            *time= info.st_mtime;
        return 0;

    #else
        //! \todo a modifier pour compiler avec codeblock/mingw sous windows, utiliser la version linux.
        struct _stat64 info;
        if(_stat64(filename.c_str(), &info) < 0)
            return -1;

        if(size != NULL)
            *size= info.st_size;
        if(time != NULL)
            *time= info.st_mtime;
        return 0;
    #endif
    }

    //! renvoie les informations sur un fichier.
    //! renvoie -1 en cas d'erreur (le fichier n'existe pas, par exemple).
    static
    int infos( const std::string& filename, IOInfo& info )
    {
        int status= infos(filename, &info.size, &info.time);
        info.exists= (status == 0);
        return status;
    }

    //! renvoie 1 si le fichier a ete modifie depuis qu'il a ete lu, 0 sinon, et -1 en cas d'erreur.
    static
    int modified( const std::string& filename, const IOInfo& last_info, IOInfo *update= NULL )
    {
        IOInfo info;
        if(infos(filename, info) < 0)
            return -1;                          // erreur, le fichier n'existe pas
        if(update != NULL)
            *update= info;                      // renvoie la nouvelle version du fichier

        if(last_info.exists == false)
            return 1;                           // le 'vieux' fichier n'existe pas, mais la nouvelle version existe
        return (last_info != info) ? 1 : 0;     // compare les 2 versions du fichier
    }

    //! lit un fichier texte, et normalise les fins de lignes.
    static
    std::string readText( const std::string& filename )
    {
        std::string text;
        FILE *in= fopen(filename.c_str(), "rb");
        if(in == NULL)
            return text;

        char last_char= 0;
        char tmp[4096];
        for(;;)
        {
            if(fgets(tmp, sizeof(tmp), in) == NULL)
                break;

            // normalise les fins de lignes
            for(int i= 0; tmp[i] != 0; i++)
            {
                char c= tmp[i];

                if((c == '\r')    // windows + mac
                || (c == '\n' && last_char != '\r'))        // linux
                    text.push_back('\n');

                if(c != '\n')   // les fins de lignes sont deja emises, ne pas dupliquer...
                    text.push_back(c);

                last_char= c;
            }
        }
        
        // termine la derniere ligne du fichier, si necessaire.
        //~ if(text.back() != '\n') // c++11
        if(text.empty() == false && text[text.size() -1] != '\n')
            text.push_back('\n');
        
        fclose(in);
        return text;
    }

    //! ecrit un fichier texte
    static
    int writeText( const std::string& data, const std::string& filename )
    {
        FILE *out= fopen(filename.c_str(), "wb");
        if(out == NULL)
            return -1;

        fputs(data.c_str(), out);
        fclose(out);
        return 0;
    }

    //! lit un fichier.
    static
    std::vector<unsigned char> readBinary( const std::string& filename )
    {
        std::vector<unsigned char> data;
        FILE *in= fopen(filename.c_str(), "rb");
        if(in == NULL)
            return data;

        unsigned char buffer[64*1024];
        for(;;)
        {
            size_t status= fread(buffer, 1, sizeof(buffer), in);
            if(status == 0)
                break;
            data.insert(data.end(), &buffer[0], &buffer[status]);
        }

        fclose(in);
        return data;
    }

    //! recherche un fichier dans un ensemble de repertoires, renvoie le chemin d'acces complet, nom de fichier compris.
    static
    std::string findFile( const std::string& filename, 
        const std::vector<std::string>& global_paths= std::vector<std::string>(),
        const std::vector<std::string>& local_paths= std::vector<std::string>() )
    {
        if(filename.empty())
            return std::string();

        if(isFilename(filename))        // local file
            return filename;

        std::string tmp;
        tmp= manager().basePath();      // absolute path to local file
        tmp.append(filename);
        //~ printf("findFile: base path '%s'\n", tmp.c_str());
        if(isFilename(tmp))
            return tmp;
        
        // recherche dans la liste des dossiers
        for(unsigned int i= 0; i < global_paths.size(); i++)
        {
            tmp= manager().basePath();
            tmp.append(global_paths[i]).append("/").append(filename);
            //~ printf("findFile: global path '%s'\n", tmp.c_str());
            if(isFilename(tmp))
                return tmp;
        }

        for(unsigned int i= 0; i < local_paths.size(); i++)
        {
            tmp= manager().basePath();
            tmp.append(local_paths[i]).append("/").append(filename);
            //~ printf("findFile: local path '%s'\n", tmp.c_str());
            if(isFilename(tmp))
                return tmp;
        }

        return filename;        // not found
    }
};

}

#endif
