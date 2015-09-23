
#include <algorithm>

#include "SDLPlatform.h"
#include "ProgramManager.h"


namespace gk {
    
ProgramManager::ProgramManager( ) : m_compilers(), m_paths() {}

ProgramManager::~ProgramManager( )
{
    for(unsigned int i= 0; i < m_compilers.size(); i++)
        delete m_compilers[i];
}

int ProgramManager::searchPath( const std::string& path )
{
    if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
        m_paths.push_back(path);
    return 0;
}

GLProgram *ProgramManager::createProgram( const std::string& label, const std::string& vertex, const std::string& fragment )
{
    GLCompiler *compiler= new GLCompiler(label);
    m_compilers.push_back(compiler);
    
    compiler->loadVertex(IOFileSystem::findFile(vertex, m_paths));
    compiler->loadFragment(IOFileSystem::findFile(fragment, m_paths));
    return compiler->make();
}

GLCompiler &ProgramManager::loadProgram( const std::string& source, const std::string& common )
{
    GLCompiler *compiler= new GLCompiler(source);      // nomme le programme == nom du fichier source
    m_compilers.push_back(compiler);
    
    compiler->loadCommon(IOFileSystem::findFile(common, m_paths));
    return compiler->load(IOFileSystem::findFile(source, m_paths));
}

GLProgram *ProgramManager::createProgram( const std::string& source, const std::string& common )
{
    return loadProgram(source, common).make();
}

GLProgram *ProgramManager::program( const std::string& label, const unsigned int version )
{
    for(unsigned int i= 0; i < m_compilers.size(); i++)
        if(m_compilers[i]->program->label == label
        && m_compilers[i]->program->version == version)
            return m_compilers[i]->program;
    
    return GLProgram::null();
}

int ProgramManager::reload( )
{
    for(unsigned int i= 0; i < m_compilers.size(); i++)
        m_compilers[i]->reload();
    
    return 0;
}

GLProgram *defaultProgram( )
{
    static GLProgram *object= NULL;
    if(object == NULL)
        object= createProgram("shaders/default.glsl");
    
    return object;
}

}       // namespace
