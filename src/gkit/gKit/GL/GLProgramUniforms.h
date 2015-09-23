
#ifndef _PROGRAM_UNIFORMS_H
#define _PROGRAM_UNIFORMS_H

#include "GL/GLProgram.h"
#include "GL/ProgramName.h"


namespace gk {

//! representation de la valeur d'un uniform. classe de base a deriver en fonction du type de l'uniform.
struct UniformValue
{
    ProgramName uniform;
    std::string value;
    std::string edit_value;
    int index;  // arrays
    bool edit;
    
    UniformValue( const ProgramName& _uniform, const int _index= 0 ) : uniform(_uniform), value(), edit_value(), index(_index), edit(false) {}
    virtual ~UniformValue( ) {}
    
    //! affecte une valeur a l'uniform.
    virtual UniformValue& operator= ( const std::string& _value )= 0;
    
    //! renvoie la valeur actuelle de l'uniform.
    virtual operator std::string( ) { return value; }
    
    //! recupere la valeur de l'uniform.
    virtual void update( )= 0;
    
    //! utilisation interne, creation d'une classe derivee en fonction du type de l'uniform.
    static UniformValue *create( const ProgramName& uniform, const int index= 0 );
    static UniformValue *create( UniformValue *value, const ProgramName& uniform, const int index= 0 );
    
    virtual bool isConst( ) { return false; }
};


//! representation d'un uniform non modifiable / constant. permet de conserver une valeur editee entre 2 versions d'un shader.
struct UniformConst : public UniformValue
{
    UniformConst( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformConst& operator= ( const std::string& _value ) { return *this; }
    void update( ) { return; }
    bool isConst( ) { return true; }
    
    static UniformValue *create( UniformValue *u );
};

//! representation d'un uniform de type int.
struct UniformInt : public UniformValue
{
    UniformInt( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformInt& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type float.
struct UniformFloat : public UniformValue
{
    UniformFloat( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformFloat& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec2.
struct UniformVec2 : public UniformValue
{
    UniformVec2( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformVec2& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec3.
struct UniformVec3 : public UniformValue
{
    UniformVec3( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformVec3& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type vec4.
struct UniformVec4 : public UniformValue
{
    UniformVec4( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformVec4& operator= ( const std::string& _value );
    void update( );
};

//! representation d'un uniform de type mat4.
struct UniformMat4 : public UniformValue
{
    UniformMat4( const ProgramName& _uniform, const int _index ) : UniformValue(_uniform, _index) {}
    UniformMat4& operator= ( const std::string& _value );
    void update( );
};


//! representation de l'ensemble d'uniforms d'un shader program.
struct GLProgramUniforms
{
    std::vector<std::string> names;
    std::vector<UniformValue *> values;
    
    GLProgram *program;
    unsigned int program_changes;
    
    int find( const std::string& name, const int index= 0 );
    
    GLProgramUniforms(  ) : names(), values(), program(GLProgram::null()), program_changes(0) {}
    GLProgramUniforms( GLProgram *_program );
    ~GLProgramUniforms( );
    
    void update( );
    void assign( );
    
    std::string summary( );
};

}       // namespace
#endif
