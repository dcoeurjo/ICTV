
#ifndef NV_GLCORE_DRAW_H
#define NV_GLCORE_DRAW_H

#include <vector>
#include <cstdio>

#include "GL/GLPlatform.h"
#include "GL/GLSLUniforms.h"


namespace nv {
namespace GLCore {

//! vertex data: 2d position, 3d texcoords.
struct vertex
{
    float x, y;
    float s, t, p;
    
    vertex( ) 
        : x(0.f), y(0.f), s(0.f), t(0.f), p(0.f) {}
    vertex( const float _x, const float _y, const float _s, const float _t, const float _p= 0.f )
        : x(_x), y(_y), s(_s), t(_t), p(_p) {}
};


//! required state for drawing something with Draw<T>
struct params
{
    // gluint program
    // glint projection
    unsigned int start;
    unsigned int end;
    
    params( )
        :
        start(0),
        end(0)
    {}
};


//! basic batch with immediate mode vertex submission (glBegin / glEnd).
template < class P >
class Draw
{
protected:
    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_index_buffer;
    gk::glsl::matrix m_projection;
    
    vertex m_vertex;

    struct buffer
    {
        std::vector<vertex> data;
        std::vector<unsigned int> index;
        unsigned int data_offset;
        unsigned int index_offset;
        
        buffer( ) : data(), index(), data_offset(0), index_offset(0) {}
    };
    
    std::vector<buffer> m_buffers;
    std::vector<P> m_draws;
    unsigned int m_buffer_id;
    unsigned int m_restart_index;
    
public:
    Draw( )
        :
        m_vao(0), m_vertex_buffer(0), m_index_buffer(0),
        m_projection(),
        m_vertex(), 
        m_buffers(), m_draws(), 
        m_buffer_id(-1u)
    {}
    
    void init( )
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        glGenBuffers(1, &m_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, 8192, NULL, GL_STREAM_DRAW);
        
        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8192, NULL, GL_STREAM_DRAW);
        
        // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (const GLvoid *) 0);
        glEnableVertexAttribArray(0);
        // texcoord
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (const GLvoid *) sizeof(float [2]));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        m_restart_index= 0xffffffff;
    }
    
    ~Draw( )
    {
        glDeleteBuffers(1, &m_vertex_buffer);
        glDeleteBuffers(1, &m_index_buffer);
        glDeleteVertexArrays(1, &m_vao);
    }

    void begin( const P& params )
    {
        m_vertex= vertex();
        
        // sort states, improve batching but "kills" z order rendering
        unsigned int i= 0;
        for(; i < m_draws.size(); i++)
            if(m_draws[i] == params)
                break;
        
        if(i == m_draws.size())
        {
            // state not found, create a new batch
            m_draws.push_back( params );
            m_buffers.push_back(buffer());
            m_draws[i].start= 0;
        }
        
        m_buffer_id= i;
        assert(m_draws.size() == m_buffers.size());
        assert(i < m_draws.size());
    }
    
    void end( )
    {
        if(m_buffer_id == -1u)
            return;
        m_buffers[m_buffer_id].index.push_back(m_restart_index);    // restart index 
        m_draws[m_buffer_id].end= m_buffers[m_buffer_id].index.size();
        
        m_buffer_id= -1u;       // draw between begin() / end() calls
    }

    void clear( )
    {
        m_vertex= vertex();
        m_buffers.clear();
        m_draws.clear();
        m_buffer_id= -1u;
    }
    
    void reshape( const Rect& window )
    {
        const float left= window.x;
        const float right= window.x + window.w;
        const float bottom= window.y;
        const float top= window.y + window.h;
        const float znear= -1.f;
        const float zfar= 1.f;
        
        //! \todo utiliser gk::Mat4 de Vec.h
        m_projection= gk::glsl::matrix(
            gk::glsl::vec4(2.f / (right - left)     , 0.f                 , 0.f                  , -(right + left) / (right - left)),
            gk::glsl::vec4(0.f                      , 2.f / (top - bottom), 0.f                  , -(top + bottom) / (top - bottom)),
            gk::glsl::vec4(0.f                      , 0.f                 , -2.f / (zfar - znear), -(zfar + znear) / (zfar - znear)),
            gk::glsl::vec4(0.f, 0.f, 0.f, 1.f));
    }
    
    void push_texcoord( const float s, const float t, const float p= 0.f )
    {
        m_vertex.s= s;
        m_vertex.t= t;
        m_vertex.p= p;
    }
    
    void push_vertex( const float x, const float y )
    {
        m_vertex.x= x;
        m_vertex.y= y;
        
        if(m_buffer_id == -1u)
            return;
        m_buffers[m_buffer_id].index.push_back( m_buffers[m_buffer_id].data.size() );
        m_buffers[m_buffer_id].data.push_back( m_vertex );
    }
    
    void push_vertex( const float x, const float y, const float s, const float t, const float p= 0.f )
    {
        push_texcoord(s, t, p);
        push_vertex(x, y);
    }
    
    void restart( )
    {
        if(m_buffer_id == -1u)
            return;
        m_buffers[m_buffer_id].index.push_back(m_restart_index);    // restart index 
    }
    
    void draw( )
    {
        if(m_buffer_id != -1u)
            return;     // must call end() before draw()
        
        // organise buffer data
        unsigned int index_size= 0;
        unsigned int data_size= 0;
        for(unsigned int i= 0; i < m_buffers.size(); i++)
        {
            m_buffers[i].data_offset= data_size;
            m_buffers[i].index_offset= index_size;
            
            index_size+= m_buffers[i].index.size();
            data_size+= m_buffers[i].data.size();
        }
        
        // update buffers
        glBindVertexArray(0);
        
        // resize vertex buffer when necessary
        /*! \todo implement efficient buffer streaming :
            cf. https://developer.nvidia.com/content/how-modern-opengl-can-radically-reduce-driver-overhead-0
            and 
            https://github.com/nvMcJohn/apitest/blob/pdoane_newtests/streaming_vb_gl.cpp
        */
        GLint buffer_size;
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
        if((size_t) buffer_size < data_size * sizeof(vertex))
            glBufferData(GL_ARRAY_BUFFER, data_size * sizeof(vertex), NULL, GL_STREAM_DRAW);
        
        // update vertex buffer
        for(unsigned int i= 0; i < m_buffers.size(); i++)
            glBufferSubData(GL_ARRAY_BUFFER, 
                m_buffers[i].data_offset * sizeof(vertex), m_buffers[i].data.size() * sizeof(vertex), 
                (const GLvoid *) &m_buffers[i].data.front());
        
        // resize index buffer when necessary
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
        if((size_t) buffer_size < index_size * sizeof(unsigned int))
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned int), NULL, GL_STREAM_DRAW);
        
        // update index buffer
        for(unsigned int i= 0; i < m_buffers.size(); i++)
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 
                m_buffers[i].index_offset* sizeof(unsigned int), m_buffers[i].index.size() * sizeof(unsigned int), 
                (const GLvoid *) &m_buffers[i].index.front());

        // draw states
        glBindVertexArray(m_vao);

        glPrimitiveRestartIndex(m_restart_index);
        glEnable(GL_PRIMITIVE_RESTART); 
        
        const void *program= NULL;
        //~ printf("widgets batchs %lu\n", m_draws.size());
        for(unsigned int i= 0; i < m_draws.size(); i++)
        {
            // bind shader
            if(m_draws[i].program != program)
            {
                glUseProgram(m_draws[i].program->program);
                // update projection matrix
                assert(m_draws[i].program->projection > -1);
                glUniformMatrix4fv(m_draws[i].program->projection, 1, GL_TRUE, (const GLfloat *) &m_projection);
                program= m_draws[i].program;
            }
            
            // set uniforms
            m_draws[i].apply();
            
            // draw strips
            glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, 
                m_draws[i].end - m_draws[i].start, 
                GL_UNSIGNED_INT, (GLvoid *) ((m_buffers[i].index_offset + m_draws[i].start) * sizeof(unsigned int)),
                m_buffers[i].data_offset);
        }
        
	glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDisable(GL_PRIMITIVE_RESTART);
    }
};

}       // namespace GLCore
}       // namespace gk
#endif
