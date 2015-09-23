

#include "GL/GLProgram.h"
#include "GL/ProgramName.h"

#include "Logger.h"


namespace gk {  

namespace glsl {
    
bool is_sampler( const GLenum type )
{
    switch(type)
    {
        case  GL_SAMPLER_1D:
        case  GL_SAMPLER_2D:
        case  GL_SAMPLER_3D:
        case  GL_SAMPLER_CUBE:
        case  GL_SAMPLER_1D_SHADOW:
        case  GL_SAMPLER_2D_SHADOW:
        
    #ifdef GL_VERSION_3_0
        case  GL_SAMPLER_1D_ARRAY:
        case  GL_SAMPLER_2D_ARRAY:
        case  GL_SAMPLER_1D_ARRAY_SHADOW:
        case  GL_SAMPLER_2D_ARRAY_SHADOW:
        case  GL_SAMPLER_CUBE_SHADOW:
        case  GL_INT_SAMPLER_1D:
        case  GL_INT_SAMPLER_2D:
        case  GL_INT_SAMPLER_3D:
        case  GL_INT_SAMPLER_CUBE:
        case  GL_INT_SAMPLER_1D_ARRAY:
        case  GL_INT_SAMPLER_2D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_1D:
        case  GL_UNSIGNED_INT_SAMPLER_2D:
        case  GL_UNSIGNED_INT_SAMPLER_3D:
        case  GL_UNSIGNED_INT_SAMPLER_CUBE:
        case  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    #endif
    
    #ifdef GL_VERSION_3_1
        case  GL_SAMPLER_2D_RECT:
        case  GL_SAMPLER_2D_RECT_SHADOW:
        case GL_SAMPLER_BUFFER:
        case GL_INT_SAMPLER_2D_RECT:
        case GL_INT_SAMPLER_BUFFER:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    #endif
    
    #ifdef GL_VERSION_4_0
        case GL_SAMPLER_CUBE_MAP_ARRAY:
        case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
        case GL_INT_SAMPLER_CUBE_MAP_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY:
    #endif
    
    #ifdef GL_ARB_texture_multisample
        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    #endif
            return true;

        default:
            return false;
    }
}

bool is_image( const GLenum type )
{
    switch(type)
    {
        #ifdef GL_ARB_shader_image_load_store
        case  GL_IMAGE_1D:
        case  GL_IMAGE_2D:
        case  GL_IMAGE_3D:
        case  GL_IMAGE_2D_RECT:
        case  GL_IMAGE_CUBE:
        case  GL_IMAGE_BUFFER:
        case  GL_IMAGE_1D_ARRAY:
        case  GL_IMAGE_2D_ARRAY:
        case  GL_IMAGE_CUBE_MAP_ARRAY:
        case  GL_IMAGE_2D_MULTISAMPLE:
        case  GL_IMAGE_2D_MULTISAMPLE_ARRAY:
        case  GL_INT_IMAGE_1D:
        case  GL_INT_IMAGE_2D:
        case  GL_INT_IMAGE_3D:
        case  GL_INT_IMAGE_2D_RECT:
        case  GL_INT_IMAGE_CUBE:
        case  GL_INT_IMAGE_BUFFER:
        case  GL_INT_IMAGE_1D_ARRAY:
        case  GL_INT_IMAGE_2D_ARRAY:
        case  GL_INT_IMAGE_CUBE_MAP_ARRAY:
        case  GL_INT_IMAGE_2D_MULTISAMPLE:
        case  GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_1D:
        case  GL_UNSIGNED_INT_IMAGE_2D:
        case  GL_UNSIGNED_INT_IMAGE_3D:
        case  GL_UNSIGNED_INT_IMAGE_2D_RECT:
        case  GL_UNSIGNED_INT_IMAGE_CUBE:
        case  GL_UNSIGNED_INT_IMAGE_BUFFER:
        case  GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:
        case  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
        case  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
            return true;
        #endif
        
        default: 
            return false;
    }
}


bool is_integer( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL:
        case GL_BOOL_VEC2:
        case GL_BOOL_VEC3:
        case GL_BOOL_VEC4:
        case GL_INT:
        case GL_INT_VEC2:
        case GL_INT_VEC3:
        case GL_INT_VEC4:
        case GL_UNSIGNED_INT:
        case GL_UNSIGNED_INT_VEC2:
        case GL_UNSIGNED_INT_VEC3: 
        case GL_UNSIGNED_INT_VEC4:
            return true;
        
        default:
            return false;
    }
}

bool is_vec2( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL_VEC2:
        case GL_INT_VEC2:
        case GL_UNSIGNED_INT_VEC2:
        case GL_FLOAT_VEC2:
            return true;
        
        default:
            return false;
    }
}

bool is_vec3( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL_VEC3:
        case GL_INT_VEC3:
        case GL_UNSIGNED_INT_VEC3:
        case GL_FLOAT_VEC3:
            return true;
        
        default:
            return false;
    }
}

bool is_vec4( const GLenum type )
{
    switch(type)
    {
        case GL_BOOL_VEC4:
        case GL_INT_VEC4:
        case GL_UNSIGNED_INT_VEC4:
        case GL_FLOAT_VEC4:
            return true;
        
        default:
            return false;
    }
}

bool is_matrix( const GLenum type )
{
    switch(type)
    {
        case GL_FLOAT_MAT2:
        case GL_FLOAT_MAT2x3:
        case GL_FLOAT_MAT2x4:
        case GL_FLOAT_MAT3:
        case GL_FLOAT_MAT3x2:
        case GL_FLOAT_MAT3x4:
        case GL_FLOAT_MAT4:
        case GL_FLOAT_MAT4x2:
        case GL_FLOAT_MAT4x3:
            return true;
        
        default:
            return false;
    }    
}


const char *type_string( const GLenum value )
{
#define type(v, s) case v: return s;
    
    switch(value)
    {
        type(GL_FLOAT, "float")
        type(GL_FLOAT_VEC2, "vec2")
        type(GL_FLOAT_VEC3, "vec3")
        type(GL_FLOAT_VEC4, "vec4")
        
        type(GL_UNSIGNED_INT, "uint")
        type(GL_UNSIGNED_INT_VEC2, "uvec2")
        type(GL_UNSIGNED_INT_VEC3, "uvec3")
        type(GL_UNSIGNED_INT_VEC4, "uvec4")
        
        type(GL_INT, "int")
        type(GL_INT_VEC2, "ivec2")
        type(GL_INT_VEC3, "ivec3")
        type(GL_INT_VEC4, "ivec4")
        
        type(GL_BOOL, "bool")
        type(GL_BOOL_VEC2, "bvec2")
        type(GL_BOOL_VEC3, "bvec3")
        type(GL_BOOL_VEC4, "bvec4")
        
        type(GL_FLOAT_MAT2, "mat2")
        type(GL_FLOAT_MAT3, "mat3")
        type(GL_FLOAT_MAT4, "mat4")
        
        default: return NULL;
    }
    
#undef type
}

}       // namespace
    

ProgramName::ProgramName( const GLProgram *_program, const GLint _location, const GLint _index, 
    const int _size, const GLenum _type, const unsigned int _flags ) 
    :
    program(_program), location(_location), index(_index), 
    size(_size), type(_type), 
    flags(_flags), changes((_program != NULL) ? _program->changes : 0u)
{}

ProgramName::ProgramName( const ProgramName& name ) 
    :
    program(name.program), location(name.location), index(name.index),
    size(name.size), type(name.type),
    flags(name.flags), changes((name.program != NULL) ? name.program->changes : 0u)
{}

bool ProgramName::isValid( ) const 
{ 
    return (program != NULL && program->errors == false && changes == program->changes && location >= 0); 
}


static 
bool uniform_check( ProgramName& uniform, int size, GLenum type )
{
    if(uniform.isValid() == false)
        return false;

#ifndef NDEBUG
    if(uniform.size != size)
    {
        ERROR("setUniform('%s'): size %d != %d\n", uniform.program->uniformName(uniform), uniform.size, size);
        return false;
    }
    
    if(uniform.isSampler())
    {
        if(type != GL_INT)
        {
            ERROR("setSampler('%s'): not an int parameter\n", uniform.program->samplerName(uniform));
            return false;
        }
    }
    else
    {
        if(uniform.type != type)
        {
            ERROR("setUniform('%s'): type 0x%x != 0x%x\n", uniform.program->uniformName(uniform), uniform.type, type);
            return false;
        }
    }
#endif

#ifndef GK_OPENGL4
#ifndef NDEBUG
    {
        GLint current= 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        if(current == 0 || (GLuint) current != uniform.program->name)
        {
            ERROR("setUniform('%s'): program %d is not in use (current program %d).\n", 
                uniform.program->uniformName(uniform), 
                uniform.program->name, current);
            return false;
        }
    }
#endif
#endif
    
    return true;
}


ProgramName ProgramName::at( const int id )
{
    if(id < 0 || id >= size)
    {
        ERROR("setUniform('%s') [%d] > array bounds %d.\n", program->uniformName(*this), id, size);
        return *this;
    }

    return ProgramName(program, location + id, index, 1, type, flags);
}


ProgramName& ProgramName::operator= ( const bool x )
{
    if(uniform_check(*this, 1, GL_BOOL) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1i(location, x);
#else
    glProgramUniform1i(program->name, location, x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const int x )
{
    if(uniform_check(*this, 1, GL_INT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1i(location, x);
#else
    glProgramUniform1i(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2iv(location, size, &v.x);
#else
    glProgramUniform2iv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3iv(location, size, &v.x);
#else
    glProgramUniform3iv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<int>& v )
{
    if(uniform_check(*this, 1, GL_INT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4iv(location, size, &v.x);
#else
    glProgramUniform4iv(program->name, location, size, &v.x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const unsigned int x )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1ui(location, x);
#else
    glProgramUniform1ui(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2uiv(location, size, &v.x);
#else
    glProgramUniform2uiv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3uiv(location, size, &v.x);
#else
    glProgramUniform3uiv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<unsigned int>& v )
{
    if(uniform_check(*this, 1, GL_UNSIGNED_INT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4uiv(location, size, &v.x);
#else
    glProgramUniform4uiv(program->name, location, size, &v.x);
#endif
    return *this;
}


ProgramName& ProgramName::operator= ( const float x )
{
    if(uniform_check(*this, 1, GL_FLOAT) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform1f(location, x);
#else
    glProgramUniform1f(program->name, location, x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec2<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC2) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform2fv(location, size, &v.x);
#else
    glProgramUniform2fv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec3<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC3) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform3fv(location, size, &v.x);
#else
    glProgramUniform3fv(program->name, location, size, &v.x);
#endif
    return *this;
}

template <> ProgramName& ProgramName::operator= ( const TVec4<float>& v )
{
    if(uniform_check(*this, 1, GL_FLOAT_VEC4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniform4fv(location, size, &v.x);
#else
    glProgramUniform4fv(program->name, location, size, &v.x);
#endif
    return *this;
}

ProgramName& ProgramName::operator= ( const VecColor& c )
{
    return (*this)= Vec4(c); 
}

ProgramName& ProgramName::operator= ( const Mat4& m )
{
    if(uniform_check(*this, 1, GL_FLOAT_MAT4) == false)
        return *this;
    
#ifndef GK_OPENGL4
    glUniformMatrix4fv(location, size, GL_TRUE, &m.m[0][0]);
#else
    glProgramUniformMatrix4fv(program->name, location, size, GL_TRUE, &m.m[0][0]);
#endif
    return *this;
}

}
