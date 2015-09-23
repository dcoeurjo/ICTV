
#ifndef _PROGRAM_NAME_H
#define _PROGRAM_NAME_H

#include "GL/GLPlatform.h"
#include "GL/GLSLUniforms.h"
#include "Vec.h"


namespace gk {

namespace glsl {
    
bool is_sampler( const GLenum type );
bool is_image( const GLenum type );
bool is_integer( const GLenum type );
bool is_vec2( const GLenum type );
bool is_vec3( const GLenum type );
bool is_vec4( const GLenum type );
bool is_matrix( const GLenum type );
const char *type_string( const GLenum value );

}       // namespace 

class GLProgram;

//! utilisation interne. identifiant generique d'une variable d'un GLProgram.
//! \ingroup OpenGL.
class ProgramName
{
    ProgramName& operator= ( const ProgramName& );
    
public:
    enum {
        UNIFORM= 0,
        INTEGER,
        SAMPLER,
        IMAGE,
        VEC2,
        VEC3, 
        VEC4,
        MAT2,
        MAT3,
        MAT4,
        ARRAY
    };
    
    const GLProgram *program;
    GLint location;
    GLint index;
    int size;
    GLenum type;
    unsigned int flags;
    unsigned int changes;

    ProgramName( )
        :
        program(NULL), location(-1), index(-1),
        size(0), type(0),
        flags(0u), changes(0u)
    {}

    ProgramName( const ProgramName& name );
    ProgramName( const GLProgram *_program, const GLint _location, const GLint _index, 
        const int _size= 0, const GLenum _type= 0, const unsigned int _flags= UNIFORM );
        
    ~ProgramName( ) {}
    
    /*! renvoie un accesseur sur le ieme element, si l'uniform est un tableau
        \todo pas tres pratique, la syntaxe est pourrie, la declaration :
        \code 
        uniform float v[3][3]= { ... }; 
        \endcode
        genere les uniforms :
        \code 
        float v[0][0], size 3
        float v[1][0], size 3
        float v[2][0], size 3
        \endcode
        et pour les modifier il faut utiliser :
        \code
        program->uniform("v[0][0]").at(0)= 1;   // v[0][0]
        program->uniform("v[0][0]").at(1)= 1;   // v[0][1]
        program->uniform("v[0][0]").at(2)= 1;   // v[0][2]
        program->uniform("v[1][0]").at(0)= 1;   // v[1][0]
        program->uniform("v[1][0]").at(1)= 1;   // v[1][1]
        ...
        \endcode
    */
    ProgramName at( const int id );
    
    ProgramName& operator= ( const bool x );
    ProgramName& operator= ( const int x );
    ProgramName& operator= ( const unsigned int x );
    ProgramName& operator= ( const float x );
    
    template < typename T > ProgramName& operator= ( const TVec2<T>& x );
    template < typename T > ProgramName& operator= ( const TVec3<T>& x );
    template < typename T > ProgramName& operator= ( const TVec4<T>& x );
    ProgramName& operator= ( const VecColor& x );
    
    ProgramName& operator= ( const Mat4& v );
    
    bool isValid( ) const;
    
    operator GLint ( ) const { return location; }
    
    bool isInteger( ) const { return flags & (1 << INTEGER); }
    
    bool isVec2( ) const { return flags & (1 << VEC2); }
    bool isVec3( ) const { return flags & (1 << VEC3); }
    bool isVec4( ) const { return flags & (1 << VEC4); }
    
    bool isMat4( ) const { return flags & (1 << MAT4); }
    
    bool isSampler( ) const { return flags & (1 << SAMPLER); }
    
    bool isImage( ) const { return flags & (1 << IMAGE); }
    
    bool isArray( ) const { return (size > 1);}
    
};

template <> ProgramName& ProgramName::operator= ( const TVec2<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec2<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<unsigned int>& v );
template <> ProgramName& ProgramName::operator= ( const TVec2<float>& v );
template <> ProgramName& ProgramName::operator= ( const TVec3<float>& v );
template <> ProgramName& ProgramName::operator= ( const TVec4<float>& v );

//! identifiant d'un uniform.
typedef ProgramName ProgramUniform;
//! identifiant d'un attribut.
typedef ProgramName ProgramAttribute;
//! identifiant d'un transform feedback varying.
typedef ProgramName ProgramFeedback;
//! identifiant d'un sampler.
typedef ProgramName ProgramSampler;
//! identifiant d'une image.
typedef ProgramName ProgramImage;

//! identifiant d'un uniform block / shader storage buffer.
typedef ProgramName ProgramBuffer;


//! modifier la valeur d'un sampler du shader program actif : index de l'unite de texture.
int setSamplerUniform( const ProgramSampler& sampler, const int unit );

}

#endif
