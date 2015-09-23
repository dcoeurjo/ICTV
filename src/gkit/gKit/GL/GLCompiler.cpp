
#include <algorithm>

#include "GL/GLCompiler.h"



namespace gk {

GLCompiler::GLCompiler( const std::string& label )
    :
    program(GLProgram::null()),
    program_label(label),
    includes(),
    sources(GLProgram::SHADERTYPE_LAST)
{}
    
GLCompiler::~GLCompiler( ) {}

GLCompiler& GLCompiler::loadCommon( const std::string& filename )
{
    if(filename.empty() == true) return *this;
    
    includes.push_back( SourceSection(filename) );
    return *this;
}


GLCompiler& GLCompiler::include( const std::string& filename )
{
    if(filename.empty() == true) return *this;
    
    includes.push_back( SourceSection(filename) );
    return *this;
}


GLCompiler& GLCompiler::load( const std::string& filename )
{
    IOFile file(filename);
    if(file.exists() == false)
    {
        ERROR("error reading program '%s'. failed.\n", filename.c_str());
        return *this;
    }
    
    const char *stages[]= {
        "VERTEX_SHADER",
        "FRAGMENT_SHADER",
        "GEOMETRY_SHADER",
        "CONTROL_SHADER",
        "EVALUATION_SHADER",
        "COMPUTE_SHADER",
        ""
    };
    
    MESSAGE("program '%s'...\n", filename.c_str());
    
    std::string source= file.readText();
    for(unsigned int i= 0; i < GLProgram::SHADERTYPE_LAST; i++)
    {
        if(source.find(stages[i]) != std::string::npos)
        {
            MESSAGE("  %s shader...\n", GLProgram::labels[i]);
            sources[i]= SourceSection(file, source);
            sources[i].define(stages[i]);
        }
    }
    
    return *this;
}

GLCompiler& GLCompiler::loadVertex( const std::string& filename )
{
    sources[GLProgram::VERTEX].load(filename);
    if(sources[GLProgram::VERTEX].file.exists() == false)
        ERROR("error reading vertex shader '%s'. failed.\n", filename.c_str());
    
    sources[GLProgram::VERTEX].define("VERTEX_SHADER");
    return *this;
}

GLCompiler& GLCompiler::loadControl( const std::string& filename )
{
    sources[GLProgram::CONTROL].load(filename);
    if(sources[GLProgram::CONTROL].file.exists() == false)
        ERROR("error reading control shader '%s'. failed.\n", filename.c_str());

    sources[GLProgram::CONTROL].define("CONTROL_SHADER");
    return *this;
}

GLCompiler& GLCompiler::loadEvaluation( const std::string& filename )
{
    sources[GLProgram::EVALUATION].load(filename);
    if(sources[GLProgram::EVALUATION].file.exists() == false)
        ERROR("error reading evaluation shader '%s'. failed.\n", filename.c_str());
    
    sources[GLProgram::EVALUATION].define("EVALUATION_SHADER");
    return *this;
}

GLCompiler& GLCompiler::loadGeometry( const std::string& filename )
{
    sources[GLProgram::GEOMETRY].load(filename);
    if(sources[GLProgram::GEOMETRY].file.exists() == false)
        ERROR("error reading geometry shader '%s'. failed.\n", filename.c_str());
    
    sources[GLProgram::GEOMETRY].define("GEOMETRY_SHADER");
    return *this;
}

GLCompiler& GLCompiler::loadFragment( const std::string& filename )
{
    sources[GLProgram::FRAGMENT].load(filename);
    if(sources[GLProgram::FRAGMENT].file.exists() == false)
        ERROR("error reading fragment shader '%s'. failed.\n", filename.c_str());
    
    sources[GLProgram::FRAGMENT].define("FRAGMENT_SHADER");
    return *this;
}

GLCompiler& GLCompiler::loadCompute( const std::string& filename )
{
    sources[GLProgram::COMPUTE].load(filename);
    if(sources[GLProgram::COMPUTE].file.exists() == false)
        ERROR("error reading compute shader '%s'. failed.\n", filename.c_str());
    
    sources[GLProgram::COMPUTE].define("COMPUTE_SHADER");
    return *this;
}


GLCompiler& GLCompiler::defineVertex( const std::string& what, const std::string& value )
{
    sources[GLProgram::VERTEX].define(what, value);
    return *this;
}

GLCompiler& GLCompiler::defineControl( const std::string& what, const std::string& value )
{
    sources[GLProgram::CONTROL].define(what, value);
    return *this;
}

GLCompiler& GLCompiler::defineEvaluation( const std::string& what, const std::string& value )
{
    sources[GLProgram::EVALUATION].define(what, value);
    return *this;
}

GLCompiler& GLCompiler::defineGeometry( const std::string& what, const std::string& value )
{
    sources[GLProgram::GEOMETRY].define(what, value);
    return *this;
}

GLCompiler& GLCompiler::defineFragment( const std::string& what, const std::string& value )
{
    sources[GLProgram::FRAGMENT].define(what, value);
    return *this;
}

GLCompiler& GLCompiler::defineCompute( const std::string& what, const std::string& value )
{
    sources[GLProgram::COMPUTE].define(what, value);
    return *this;
}

GLProgram *GLCompiler::reload( )
{
    MESSAGE("reload program '%s'... ", program->label.c_str());
    
#if 0
    const char *stages[]= {
        "VERTEX_SHADER",
        "FRAGMENT_SHADER",
        "GEOMETRY_SHADER",
        "CONTROL_SHADER",
        "EVALUATION_SHADER",
        "COMPUTE_SHADER",
        ""
    };
#endif
    
    bool update= false;
    for(unsigned int i= 0; i < GLProgram::SHADERTYPE_LAST; i++)
        // recharge les sources des shaders deja compiles
        if(sources[i].file.modified() == 1)
        {
            MESSAGE("%s shader ", GLProgram::labels[i]);
            sources[i].reload();
            
        #if 0
            // verifie que le source modifie n'inclut pas un nouveau type de shader
            for(unsigned int s= i+1; s < GLProgram::SHADERTYPE_LAST; s++)
                if(sources[i].source.find(stages[s]) != std::string::npos)
                {
                    MESSAGE("%s shader ", GLProgram::labels[s]);
                    sources[s]= SourceSection(sources[i].file, sources[i].source);
                    sources[s].define(stages[s]);
                }
        #endif
            
            update= true;
        }
    
    if(update == false)
    {
        MESSAGE("no modifications.\n");
        return program;
    }
    
    MESSAGE("\n");
    return make();  // rebuild program
}


static
int getLine( std::string& buffer, const std::string& source, int& string_id, int& line_id )
{
    buffer.clear();
    int line= 1;
    for(unsigned int i= 0; i < source.size(); i++)
    {
        if(line == line_id)
            buffer.push_back(source[i]);
        
        if(source[i] == '\n')
        {
            line++;
            if(line > line_id)
                return 0;
        }
    }
    
    if(buffer.empty() == false)
        return 0;       // trouve
    
    // non trouve, iteration sur les sources
    //~ if(source.empty() == false && source[source.size() -1] != '\n')
        //~ line++;
        
    string_id++;
    line_id= line_id - line +1;
    return -1;
}
    
static
void printErrors( const char *log, const SourceSection& source )
{
    // affiche la ligne du source + l'erreur associee
    int last_string= -1;
    int last_line= -1;
    
    for(int i= 0; log[i] != 0; i++)
    {
        int string_id= 0, line_id= 0, position= 0;
        if(sscanf(&log[i], "%d ( %d ) : %n", &string_id, &line_id, &position) == 2        // nvidia syntax
        || sscanf(&log[i], "ERROR : %d : %d %n", &string_id, &line_id, &position) == 2  // ati syntax
        || sscanf(&log[i], "WARNING : %d : %d %n", &string_id, &line_id, &position) == 2)  // ati syntax
        {
            if(string_id != last_string || line_id != last_line)
            {
                MESSAGE("\n");
                std::string line; 
                if(getLine(line, source.build, string_id, line_id) == 0)        // affiche la ligne correspondante dans le source
                    MESSAGE("%s\n", line.c_str());
            }
        }
        
        // affiche l'erreur
        //~ for(i+= position; log[i] != 0; i++)
        for(; log[i] != 0; i++)
        {
            //~ MESSAGE("%c", log[i]);
            printf("%c", log[i]);
            if(log[i] == '\n') break;
        }
        
        last_string= string_id;
        last_line= line_id;
    }
}


std::string GLCompiler::build_source( unsigned int shader )
{
    if(shader >= GLProgram::SHADERTYPE_LAST) return "";
    if(sources[shader].source.empty()) return "";
    
    // extraire la directive #version pour la repositionner en debut de source
    std::string version;
    {
        unsigned long int b= sources[shader].source.find("#version");
        if(b != std::string::npos)
        {
            unsigned long int e= sources[shader].source.find('\n', b);   // fin de ligne normalisee, cf IOFilesystem::readText()
            if(e != std::string::npos)
            {
                version= sources[shader].source.substr(0, e +1);
                sources[shader].source.erase(0, e +1);
                
                if(sources[shader].source.find("#version") != std::string::npos)
                    ERROR("found several #version directives. failed.\n");
            }
        }
        else
            ERROR("no #version directive found. failed.\n");
    }
    
    // prepare le texte complet : version, defines, source
    std::string text;
    text.append(version);                               // version
    
    for(unsigned int i= 0; i < includes.size(); i++)    // defines
        text.append(includes[i].definitions);
    text.append(sources[shader].definitions);           // defines
    
    for(unsigned int i= 0; i < includes.size(); i++)    // includes
        text.append(includes[i].source);
    text.append(sources[shader].source);                // sources
    
    //~ printf("shader %d: source:\n%s\n", shader, text.c_str());
    return text;
}


struct parameter
{
    std::string name;
    int index;
    GLenum type;
    int location;
    int location_index;
    int location_component;
    
    parameter( ) : name(), index(-1), type(0), location(-1), location_index(-1), location_component(-1) {}
    parameter( const char *_name, const int _index, const int _type, const int _location= -1, const int _location_index= -1, const int _location_component= -1 )
        : name(_name), index(_index), type(_type), location(_location), location_index(_location_index), location_component(_location_component) {}
    
    bool operator< ( const parameter& b ) const {  return (location < b.location); }
};


GLProgram *GLCompiler::make( )
{
    bool errors= false;
    bool shaders= false;
    for(unsigned int i= 0; i < GLProgram::SHADERTYPE_LAST; i++)
    {
        if(sources[i].source.empty()) continue;
        
        // construit le source
        shaders= true;
        sources[i].build= build_source(i);
        
        // cree le program
        if(program == GLProgram::null())
            program= (new GLProgram(program_label))->create();
        
        // cree le shader
        GLuint shader= program->shaders[i];
        if(shader == 0)
        {
            shader= glCreateShader(GLProgram::types[i]);
            glAttachShader(program->name, shader);
            program->shaders[i]= shader;
        }
        
        // compile le shader
        const char *text[]= { sources[i].build.c_str() };
        glShaderSource(shader, 1, text, NULL);
        glCompileShader(shader);
        
        GLint status= 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE)
        {
            errors= true;
            GLint length= 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            if(length > 0)
            {
                char *log= new char[length +1];
                glGetShaderInfoLog(shader, length, NULL, log);
                ERROR("error compiling %s shader:\n", GLProgram::labels[i]);
                printErrors(log, sources[i]);
                ERROR("shader defines:\n%s\n", sources[i].definitions.c_str());
                delete [] log;
            }
            else
                MESSAGE("error compiling %s shader: no log. failed.\n", GLProgram::labels[i]);
        }
        
        #if 0
        {
            GLint length= 0;
            glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);
            if(length > 0)
            {
                char log[4096];
                glGetShaderSource(shader, sizeof(log), NULL, log);
                DEBUGLOG("shader:\n%s\n", log);
            }
        }
        #endif
    }
    
    if(shaders == false || program == GLProgram::null())
    {
        ERROR("no shader. failed.\n");
        return program;
    }
    
    if(errors == true)
    {
        program->resources();
        program->errors= true;
        return program;
    }
    
    // link
    GLint status= 0;
    glLinkProgram(program->name);
    glGetProgramiv(program->name, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint length= 0;
        glGetProgramiv(program->name, GL_INFO_LOG_LENGTH, &length);
        if(length > 0)
        {
            char *log= new char[length +1];
            glGetProgramInfoLog(program->name, length, NULL, log);
            MESSAGE("error linking program:\n%s\nfailed.\n", log);
            delete [] log;
        }
        else
            MESSAGE("error linking program: no log. failed.\n");
        
        program->errors= true;
        return program;
    }
    
    //~ #ifdef GK_OPENGL4
    #if 0
    {
        // interface matching
        // introspection opengl >= 4.3
        // recompile chaque shader dans un program separe et recupere les varyings avec queryinterface(program_inputs / program_outputs)...
        
        std::vector<parameter> stage_inputs[GLProgram::SHADERTYPE_LAST];
        std::vector<parameter> stage_outputs[GLProgram::SHADERTYPE_LAST];
        
        for(unsigned int i= 0; i < GLProgram::SHADERTYPE_LAST; i++)
        {
            if(program->shaders[i] == 0)
                continue;
            
            // recupere le source du shader
            GLint stage_length= 0;
            glGetShaderiv(program->shaders[i], GL_SHADER_SOURCE_LENGTH, &stage_length);
            std::vector<GLchar> stage_source(stage_length +1);
            glGetShaderSource(program->shaders[i], stage_source.size(), NULL, &stage_source.front());

            // construit un program pipeline avec uniquement ce shader
            const GLchar *sources[]= { &stage_source.front() };
            GLuint stage= glCreateShader(GLProgram::types[i]);
            glShaderSource(stage, 1, sources, NULL);
            glCompileShader(stage);
            
            GLuint pipeline= glCreateProgram();
            glProgramParameteri(pipeline, GL_PROGRAM_SEPARABLE, GL_TRUE);
            glAttachShader(pipeline, stage);
            glLinkProgram(pipeline);
            glValidateProgram(pipeline);
            
            // recupere les varyings in / out
            GLint inputs;
            glGetProgramInterfaceiv(pipeline, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &inputs);
            for(int k= 0; k < inputs; k++)
            {
                GLenum properties[3]= { GL_TYPE, GL_LOCATION, GL_LOCATION_COMPONENT };
                GLint values[3]= { 0, -1, -1 };
                glGetProgramResourceiv(pipeline, GL_PROGRAM_INPUT, k, 3, properties, 3, NULL, values);
                
                GLchar name[4096]= { 0 };
                glGetProgramResourceName(pipeline, GL_PROGRAM_INPUT, k, sizeof(name), NULL, name);
                
                stage_inputs[i].push_back( parameter(name, k, values[0], values[1], -1, values[2]) );
            }
            std::stable_sort(stage_inputs[i].begin(), stage_inputs[i].end());
            
            GLint outputs;
            glGetProgramInterfaceiv(pipeline, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &outputs);
            for(int k= 0; k < outputs; k++)
            {
                GLenum properties[4]= { GL_TYPE, GL_LOCATION, GL_LOCATION_INDEX, GL_LOCATION_COMPONENT };
                GLint values[4]= { 0, -1, -1, -1 };
                glGetProgramResourceiv(pipeline, GL_PROGRAM_OUTPUT, k, 4, properties, 4, NULL, values);
                
                GLchar name[4096]= { 0 };
                glGetProgramResourceName(pipeline, GL_PROGRAM_OUTPUT, k, sizeof(name), NULL, name);
                
                stage_outputs[i].push_back( parameter(name, k, values[0], values[1], values[2], values[3]) );
            }
            std::stable_sort(stage_outputs[i].begin(), stage_outputs[i].end());
            
            // nettoyage
            glDeleteShader(stage);
            glDeleteProgram(pipeline);
        }
        
        // affichage : les sorties d'un shader doivent correspondre aux entrees du shader suivant dans le pipeline
        printf("program interfaces...\n");
        for(unsigned int i= 0; i < GLProgram::SHADERTYPE_LAST; i++)
        {
            if(stage_inputs[i].size())
            {
                printf("  %s shader inputs:\n", GLProgram::labels[i]);
                for(unsigned int k= 0; k < stage_inputs[i].size(); k++)
                {
                    const char *type= glsl::type_string(stage_inputs[i][k].type);
                    if(type)
                        printf("    %02d/-1: '%s', type '%s'\n", 
                            stage_inputs[i][k].location, stage_inputs[i][k].name.c_str(), type);
                    else
                        printf("    %02d/-1: '%s', type 0x%x\n", 
                            stage_inputs[i][k].location, stage_inputs[i][k].name.c_str(), stage_inputs[i][k].type);
                }
            }
            
            if(stage_outputs[i].size())
            {
                printf("  %s shader outputs:\n", GLProgram::labels[i]);
                for(unsigned int k= 0; k < stage_outputs[i].size(); k++)
                {
                    const char *type= glsl::type_string(stage_outputs[i][k].type);
                    if(type)
                        printf("    %02d/%02d: '%s', type '%s'\n", 
                            stage_outputs[i][k].location, stage_outputs[i][k].location_index, stage_outputs[i][k].name.c_str(), type);
                    else
                        printf("    %02d/%02d: '%s', type 0x%x\n", 
                            stage_outputs[i][k].location, stage_outputs[i][k].location_index, stage_outputs[i][k].name.c_str(), stage_outputs[i][k].type);
                }
            }
            
            //! \todo verification des interfaces
        }
    }
    #endif
    
    program->resources();
    program->errors= false;
    
    MESSAGE("done.\n");
    return program;
}

}       // namespace
