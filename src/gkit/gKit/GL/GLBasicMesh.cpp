
#include "Logger.h"

#include "GL/GLBasicMesh.h"


namespace gk {

GLBasicMesh& GLBasicMesh::createBuffer( const int index, const int item_size, const GLenum item_type, 
    const unsigned int length, const void *data, const GLenum usage )
{
    if(index < 0 || length == 0)
        return *this;
    
    if(index >= (int) buffers.size())
        buffers.resize(index +1, GLBuffer::null());
    buffers[index]= gk::createBuffer(GL_ARRAY_BUFFER, length, data, usage);
    
#ifndef NDEBUG
    {
        GLint current;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if((GLuint) current != vao->name)
            ERROR("invalid vertex array %d, basic mesh %d\n", current, vao->name);
    }
#endif
    
    glVertexAttribPointer(index, item_size, item_type, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(index);
    return *this;
}

GLBasicMesh& GLBasicMesh::createIndexBuffer( const GLenum item_type, 
    const unsigned int length, const void *data, const GLenum usage  )
{
#ifndef NDEBUG
    {
        GLint current;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if((GLuint) current != vao->name)
            ERROR("invalid vertex array %d, basic mesh %d\n", current, vao->name);
    }
#endif
    
    index_type= item_type;
    switch(index_type)
    {
        case GL_UNSIGNED_BYTE:  
            index_sizeof= 1;
            break;
        case GL_UNSIGNED_SHORT:
            index_sizeof= 2;
            break;
        case GL_UNSIGNED_INT:
            index_sizeof= 4;
            break;
        default:
            ERROR("invalid index buffer format, basic mesh %d\n", vao->name);
            index_sizeof= 0;
    }
    
    index_buffer= gk::createBuffer(GL_ELEMENT_ARRAY_BUFFER, length, data, usage);
    return *this;
}


GLBasicMesh& GLBasicMesh::bindBuffer( const int index, const int item_size, const GLenum item_type, GLBuffer *buffer )
{
    if(index < 0 || buffer == NULL || buffer->length == 0)
        return *this;
    
    if(index >= (int) buffers.size())
        buffers.resize(index +1, GLBuffer::null());
    buffers[index]= buffer;
    
#ifndef NDEBUG
    {
        GLint current;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
        if((GLuint) current != vao->name)
            ERROR("invalid vertex array %d, basic mesh %d\n", current, vao->name);
    }
#endif
    
    glVertexAttribPointer(index, item_size, item_type, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(index);
    return *this;
}

int GLBasicMesh::drawGroup( const unsigned int begin, const unsigned int end )
{
    if(primitive == -1u || index_sizeof == 0u)
        return -1;
    if(end <= begin)
        return -1;      // rien a dessiner
    
    glBindVertexArray(vao->name);
    if(index_type == 0u)
        glDrawArrays(primitive, begin, end - begin);
    else
        glDrawElements(primitive, end - begin, index_type, (GLvoid *) (unsigned long int) (index_sizeof * begin));
    
    return 0;
}

int GLBasicMesh::draw( )
{
    if(primitive == -1u)
        return -1;
    
    glBindVertexArray(vao->name);
    if(index_type == 0u)
        glDrawArrays(primitive, 0, count);
    else
        glDrawElements(primitive, count, index_type, 0);
    
    return 0;
}

int GLBasicMesh::draw( const int base )
{
    if(primitive == -1u)
        return -1;
    
    glBindVertexArray(vao->name);
    if(index_type == 0u)
        glDrawArrays(primitive, base, count);
    else
        glDrawElementsBaseVertex(primitive, count, index_type, 0, base);

    return 0;
}

int GLBasicMesh::drawInstanced( const int n )
{
    if(primitive == -1u)
        return -1;
    
    glBindVertexArray(vao->name);
    if(index_type == 0u)
        glDrawArraysInstanced(primitive, 0, count, n);
    else
        glDrawElementsInstanced(primitive, count, index_type, 0, n);

    return 0;
}

}       // namespace
