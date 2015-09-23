
#ifndef _PROGRAM_MANAGER_H
#define _PROGRAM_MANAGER_H

#include "GL/GLProgram.h"
#include "GL/GLCompiler.h"


namespace gk {

//! conserve l'ensemble des shader programs crees par l'application.
//! \ingroup OpenGL.
class ProgramManager
{
    // non copyable
    ProgramManager( const ProgramManager& );
    ProgramManager& operator= ( const ProgramManager& );
    
protected:
    std::vector<GLCompiler *> m_compilers;
    std::vector<std::string> m_paths;

    ProgramManager( );
    ~ProgramManager( );

public:
    //! ajoute un repertoire pour chercher les sources de shaders.
    int searchPath( const std::string& path );
    
    //! construit un programme (vertex + fragment) a partir des sources des shaders.
    //! le programme est nomme 'label'.
    GLProgram *createProgram( const std::string& label, const std::string& vertex, const std::string& fragment );
    
    //! construit un programme a partir des sections du fichier source. commom est un fichier include.
    //! le programme est nomme 'source'.
    //! exemple: createProgram("dir/shader.glsl") constuit un programme nomme 'dir/shader.glsl'.
    GLProgram *createProgram( const std::string& source, const std::string& common= "" );

    //! prepare les sources d'un programme a partir des sections du fichier source, common est un fichier include. 
    //! renvoie le GLCompiler charge de construire le programe. permet de modifier les sources avant la compilation.
    GLCompiler& loadProgram( const std::string& source, const std::string& common= "" );

    //! renvoie un programme nomme ou GLProgram::null() s'il n'existe pas. 
    GLProgram *program( const std::string& label, const unsigned int version= 0 );
    
    //! recharge les sources modifies et recompile les programmes modifies.
    //! les objets c++ ne sont pas detruits (les pointeurs sont toujours valides), seuls les objets opengl sont modifies.
    int reload( );
    
    static
    ProgramManager& manager( )
    {
        static ProgramManager m;
        return m;
    }
};

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
int programPath( const std::string& path )
{
    return ProgramManager::manager().searchPath(path);
}

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
GLProgram *createProgram( const std::string& label, const std::string& vertex, const std::string& fragment )
{
    return ProgramManager::manager().createProgram(label, vertex, fragment);
}

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
GLProgram *createProgram( const std::string& source, const std::string& common= "" )
{
    return ProgramManager::manager().createProgram(source, common);
}

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
GLCompiler &loadProgram( const std::string& source, const std::string& common= "" )
{
    return ProgramManager::manager().loadProgram(source, common);
}

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
GLProgram *findProgram( const std::string& label, const unsigned int version= 0 )
{
    return ProgramManager::manager().program(label, version);
}

//! utilitaire: acces simplifie au singleton ProgramManager.
//! \ingroup OpenGL.
inline
int reloadPrograms( )
{
    return ProgramManager::manager().reload();
}

//! compile un programme de base, peut etre utilise en cas d'echec de chargement d'un programme.
//! \ingroup OpenGL.
GLProgram *defaultProgram( );

}       // namespace

#endif
