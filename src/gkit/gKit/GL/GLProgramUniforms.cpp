
#include "GL/GLProgramUniforms.h"

namespace gk {
    

UniformValue *UniformValue::create( const ProgramName& uniform, const int index )
{
    if(uniform.isInteger()) return new UniformInt(uniform, index);
    if(uniform.isSampler()) return new UniformInt(uniform, index);
    if(uniform.isVec2()) return new UniformVec2(uniform, index);
    if(uniform.isVec3()) return new UniformVec3(uniform, index);
    if(uniform.isVec4()) return new UniformVec4(uniform, index);
    if(uniform.isMat4()) return new UniformMat4(uniform, index);
    
    return new UniformFloat(uniform, index);
}

UniformValue *UniformValue::create( UniformValue *u, const ProgramName& uniform, const int index )
{
    UniformValue *v= create(uniform, index);
    v->value= u->value;
    v->edit_value= u->edit_value;
    v->edit= u->edit;
    
    if(v->uniform.isValid() == false)
        WARNING("uniformValue::create('%s'): invalid uniform...\n", v->uniform.program->uniformName(v->uniform));
    
    delete u;
    return v;
}

UniformValue *UniformConst::create( UniformValue *u )
{
    UniformValue *v= new UniformConst(u->uniform, u->index);
    v->value= u->value;
    v->edit_value= u->edit_value;
    v->edit= u->edit;
    
    delete u;
    return v;
}


UniformInt& UniformInt::operator= ( const std::string& _value )
{ 
    int x= 0;
    if(sscanf(_value.c_str(), "%d", &x) != 1)
        ERROR("UniformInt('%s') : wrong value '%s', using %d\n", uniform.program->uniformName(uniform), _value.c_str(), x);
    
    if(uniform.size > 1) uniform.at(index)= x;
    else uniform= x;
    return *this; 
}

void UniformInt::update( )
{
    int values[4];
    glGetUniformiv(uniform.program->name, uniform.location, values);
    
    char tmp[1024];
    sprintf(tmp, "%d", values[0]);
    //~ printf("update int '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformFloat& UniformFloat::operator= ( const std::string& _value )
{ 
    float x= 0;
    if(sscanf(_value.c_str(), "%f", &x) != 1)
        ERROR("UniformFloat('%s') : wrong value '%s', using %f\n", uniform.program->uniformName(uniform), _value.c_str(), x);
    
    if(uniform.size > 1) uniform.at(index)= x;
    else uniform= x;
    return *this; 
}

void UniformFloat::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location + index, values);
    
    char tmp[1024];
    sprintf(tmp, "%f", values[0]);
    //~ printf("update float '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec2& UniformVec2::operator= ( const std::string& _value )
{ 
    Vec2 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f", &v.x, &v.y) != 2)
        ERROR("UniformVec2('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y);
    
    if(uniform.size > 1) uniform.at(index)= v;
    else uniform= v;
    return *this; 
}

void UniformVec2::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location + index, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f", values[0], values[1]);
    //~ printf("update vec2 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec3& UniformVec3::operator= ( const std::string& _value )
{ 
    Vec3 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f%*[ f,] %f", &v.x, &v.y, &v.z) != 3)
        ERROR("UniformVec3('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y, v.z);
    
    if(uniform.size > 1) uniform.at(index)= v;
    else uniform= v;
    return *this; 
}

void UniformVec3::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location + index, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f %f", values[0], values[1], values[2]);
    //~ printf("update vec3 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformVec4& UniformVec4::operator= ( const std::string& _value )
{ 
    Vec4 v;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f%*[ f,] %f%*[ f,] %f", &v.x, &v.y, &v.z, &v.w) != 4)
        ERROR("UniformVec4('%s') : wrong value '%s', using %f %f %f %f\n", uniform.program->uniformName(uniform), _value.c_str(), v.x, v.y, v.z, v.w);
    
    if(uniform.size > 1) uniform.at(index)= v;
    else uniform= v;
    return *this; 
}

void UniformVec4::update( )
{
    float values[4];
    glGetUniformfv(uniform.program->name, uniform.location + index, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f %f %f", values[0], values[1], values[2], values[3]);
    //~ printf("update vec4 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


UniformMat4& UniformMat4::operator= ( const std::string& _value )
{ 
    Mat4 m;
    if(sscanf(_value.c_str(), "%f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f%*[ f,] %f", 
            &m.m[0][0], &m.m[0][1], &m.m[0][2], &m.m[0][3],
            &m.m[1][0], &m.m[1][1], &m.m[1][2], &m.m[1][3],
            &m.m[2][0], &m.m[2][1], &m.m[2][2], &m.m[2][3],
            &m.m[3][0], &m.m[3][1], &m.m[3][2], &m.m[3][3] ) != 16)
        ERROR("UniformMat4('%s') : wrong value '%s', using %f %f %f %f...\n", uniform.program->uniformName(uniform), _value.c_str(), m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]);
    
    if(uniform.size > 1) uniform.at(index)= m;
    else uniform= m;
    return *this; 
}

void UniformMat4::update( )
{
    float values[16];
    glGetUniformfv(uniform.program->name, uniform.location + index, values);
    
    char tmp[1024];
    sprintf(tmp, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
        values[0], values[1], values[2], values[3],
        values[4], values[5], values[6], values[7],
        values[8], values[9], values[10], values[11],
        values[12], values[13], values[14], values[15]);
    //~ printf("update mat4 '%s': %s\n", uniform.program->uniformName(uniform), tmp);
    value= tmp;
}


int GLProgramUniforms::find( const std::string& name, const int index )
{
    for(unsigned int i= 0; i < names.size(); i++)
        if(names[i] == name && values[i]->index == index)
            return i;
    
    return -1;
}

GLProgramUniforms::GLProgramUniforms( GLProgram *_program ) : names(), values(), program(GLProgram::null()), program_changes(0)
{
    if(_program == NULL || _program == GLProgram::null())
        return;
    
    program= _program;
    program_changes= program->changes;
    
    unsigned int n= program->uniformCount();
    for(unsigned int i= 0; i < n; i++)
    {
        ProgramUniform uniform= program->uniform(i);
        
        for(int id= 0; id < uniform.size; id++)
        {
            names.push_back( program->uniformName(uniform) );
            values.push_back( UniformValue::create(uniform, id) );
        }
    }
    
    //~ printf("%lu uniforms\n", values.size());
}

GLProgramUniforms::~GLProgramUniforms( )
{
    for(unsigned int i= 0; i < values.size(); i++)
        delete values[i];
}

void GLProgramUniforms::update( )
{
    if(program_changes != program->changes)
    {
        // le programme a change, enumere les nouveaux uniforms
        unsigned int n= program->uniformCount();
        for(unsigned int i= 0; i < n; i++)
        {
            ProgramUniform uniform= program->uniform(i);
            std::string name= program->uniformName(uniform);
            
            for(int id= 0; id < uniform.size; id++)
            {
                int u= find(name, id);
                if(u < 0)
                {
                    // nouvel uniform
                    names.push_back( name );
                    values.push_back( UniformValue::create(uniform, id) );
                    //~ printf("new   '%s'\n", name.c_str());
                }
                else
                {
                    // remplacer l'uniform 
                    values[u]= UniformValue::create(values[u], uniform, id);
                    //~ printf("found '%s'\n", name.c_str());
                }
            }
        }
        
        for(unsigned int i= 0; i < values.size(); i++)
            if(program->uniform(names[i].c_str()).isValid() == false)
                // remplacer l'uniform par une constante
                values[i]= UniformConst::create(values[i]);
    }
    
    for(unsigned int i= 0; i < values.size(); i++)
        if(values[i]->isConst() == false)
            values[i]->update();
}

void GLProgramUniforms::assign( )
{
    for(unsigned int i= 0; i < values.size(); i++)
        if(values[i]->edit)
            *values[i]= values[i]->edit_value;
}

std::string GLProgramUniforms::summary( )
{
    std::string tmp;
    for(unsigned int i= 0; i < values.size(); i++)
    {
        tmp.append(names[i]).append("= ");
        if(values[i]->isConst())
            tmp.append("(not used)");
        else
        {
            tmp.append(values[i]->value);
            if(values[i]->edit)
                tmp.append(" (edit ").append(values[i]->edit_value).append(")");
        }
        tmp.append("\n");
    }

    return tmp;
}

}       // namespace
