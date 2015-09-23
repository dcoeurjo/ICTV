
#ifndef _GL_COMPILER_H
#define _GL_COMPILER_H

#include "Logger.h"

#include "IOFile.h"
#include "IOFileSystem.h"

#include "GL/GLProgram.h"


namespace gk {

//! utilisation interne. representation d'une partie du source d'un shader program.
//! \ingroup OpenGL.
struct SourceSection
{
    std::string definitions;    //!< #define USE_IT
    std::string source;         //!< contenu du fichier
    std::string build;          //!< texte pre a compiler
    IOFile file;                //!< infos sur le fichier

    SourceSection( ) : definitions(), source(), build(), file() {}
    SourceSection( const IOFile& _file, const std::string& _source ) : definitions(), source(_source), build(), file(_file) {}
    SourceSection( const std::string& filename ) : definitions(), source(), build(), file() { load(filename); }
    
    //! definit une valeur : #define what value.
    SourceSection& define( const std::string& what, const std::string& value= "" )
    {
        std::string d;
        d.append("#define ").append(what);
        if(value.empty() == false)
            d.append(" ").append(value);
        
        definitions.append(d).append("\n");
        return *this;
    }
    
    //! charge le fichier texte filename.
    SourceSection& load( const std::string& filename )
    {
        file= IOFile(filename);
        source= file.readText();
        build.clear();
        return *this;
    }
    
    //! recharge le fichier texte, si necessaire.
    SourceSection& reload( )
    {
        if(file.reloadText(source) > 0)
            build.clear();
        return *this;
    }
};


//! assemble les sources des shaders, les parametres, les compile et produit un shader program utilisable.
//! \ingroup OpenGL.
class GLCompiler
{
    GLCompiler( const GLCompiler& );
    GLCompiler& operator= ( const GLCompiler& );
    
    //! construit le source pret a compiler d'un shader.
    std::string build_source( unsigned int shader );
    
public:
    GLProgram *program;
    std::string program_label;

    std::vector<SourceSection> includes;
    std::vector<SourceSection> sources;
    
    GLCompiler( const std::string& label );
    ~GLCompiler( );
    
    //! charge un source commun a tous les shaders du program. en gros un fichier include.
    GLCompiler& loadCommon( const std::string& filename );
    
    //! ajoute un source commun a tous les shaders du program. 
    GLCompiler& include( const std::string& filename );
    
    //! charge un seul fichier contenant les sources de tous les shaders a compiler.
    //! le source de chaque shader est defini par une directive du pre processeur. VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER, etc.
    GLCompiler& load( const std::string& filename );
    
    //! charge le source du vertex shader.
    GLCompiler& loadVertex( const std::string& filename );
    //! charge le source du control shader.
    GLCompiler& loadControl( const std::string& filename );
    //! charge le source de l'evaluation shader.
    GLCompiler& loadEvaluation( const std::string& filename );
    //! charge le source du geometry shader.
    GLCompiler& loadGeometry( const std::string& filename );
    //! charge le source du fragment shader.
    GLCompiler& loadFragment( const std::string& filename );
    //! charge le source d'un compute shader.
    GLCompiler& loadCompute( const std::string& filename );
    
    //! vertex shader : definit une valeur #define what value.
    GLCompiler& defineVertex( const std::string& what, const std::string& value= "" );
    //! control shader : definit une valeur #define what value.
    GLCompiler& defineControl( const std::string& what, const std::string& value= "" );
    //! evaluation shader : definit une valeur #define what value.
    GLCompiler& defineEvaluation( const std::string& what, const std::string& value= "" );
    //! geometry shader : definit une valeur #define what value.
    GLCompiler& defineGeometry( const std::string& what, const std::string& value= "" );
    //! fragment shader : definit une valeur #define what value.
    GLCompiler& defineFragment( const std::string& what, const std::string& value= "" );
    //! compute shader : definit une valeur #define what value.
    GLCompiler& defineCompute( const std::string& what, const std::string& value= "" );
    
    //! compile les shaders et les linke dans un program.
    GLProgram *make( );
    
    //! compile une 'version' des shaders et les linke dans un program. 
    //! utilise par gk::ProgramManager pour identifier plusieurs variantes du meme programme (compilation avec #define).
    GLProgram *makeVersion( const unsigned int _version )
    {
        GLProgram *program= make();
        if(program != GLProgram::null()) program->version= _version;
        return program;
    }
    
    //! recharge les sources et recompile le program, si necessaire.
    GLProgram *reload( );
};

}       // namespace

#endif
