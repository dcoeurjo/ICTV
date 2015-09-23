

#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Geometry.h"
#include "Transform.h"

#include "Mesh.h"
#include "MeshIO.h"

#include "GL/GLTexture.h"
#include "GL/GLQuery.h"
#include "GL/GLBasicMesh.h"
#include "ProgramManager.h"

struct DrawElementsIndirect
{
    unsigned int count;
    unsigned int instanceCount;
    unsigned int baseIndex;
    unsigned int baseVertex;
    unsigned int baseInstance;
    
    DrawElementsIndirect( ) : count(0), instanceCount(0), baseIndex(0), baseVertex(0), baseInstance(0) {}
    DrawElementsIndirect(const unsigned int _count, const unsigned int _instances= 1, const unsigned int _index= 0, const unsigned int _vertex= 0, const unsigned int _instance= 0 ) 
        : count(_count), instanceCount(_instances), baseIndex(_index), baseVertex(_vertex), baseInstance(_instance) {}
};

class Batch : public gk::App
{
    nv::SdlContext m_widgets;

    gk::GLProgram *m_program[2];
    gk::GLBasicMesh *m_mesh[4];
    gk::GLVertexArray *m_vao;
    gk::GLBuffer *m_indirect;
    DrawElementsIndirect *m_indirect_storage;
    
    gk::GLQuery *m_time;
    
    std::vector<int> m_cpu_graph;
    //~ std::vector<DrawElementsIndirect> m_elements_indirect;
    
    float m_scale;
    int m_draw_count;
    bool m_draw_degenerate;
    
    bool m_multi_draw;
    bool m_static_vao;
    bool m_change_mesh;
    bool m_change_shader;
    bool m_change_texture;
    
public:
    // creation du contexte openGL et d'une fenetre
    Batch( )
        :
        gk::App()
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(3,3);     // version 3.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(1024, 1024, settings) < 0)
            return;
    
        m_widgets.init();
        m_widgets.reshape(windowWidth(), windowHeight());
    }
    
    ~Batch( ) {}

    // a redefinir pour utiliser les widgets.
    void processWindowResize( SDL_WindowEvent& event )
    {
        m_widgets.reshape(event.data1, event.data2);
    }
    
    // a redefinir pour utiliser les widgets.
    void processMouseButtonEvent( SDL_MouseButtonEvent& event )
    {
        m_widgets.processMouseButtonEvent(event);
    }
    
    // a redefinir pour utiliser les widgets.
    void processMouseMotionEvent( SDL_MouseMotionEvent& event )
    {
        m_widgets.processMouseMotionEvent(event);
    }
    
    // a redefinir pour utiliser les widgets.
    void processKeyboardEvent( SDL_KeyboardEvent& event )
    {
        m_widgets.processKeyboardEvent(event);
    }
    
    int init( )
    {
        // compile 2 shaders
        gk::programPath("shaders");
        m_program[0]= gk::createProgram("batch.glsl");
        if(m_program[0] == gk::GLProgram::null())
            return -1;
    
        m_program[1]= gk::createProgram("batch.glsl");
        if(m_program[1] == gk::GLProgram::null())
            return -1;
        
        // charge un mesh
        gk::Mesh *mesh= gk::MeshIO::readOBJ("bigguy.obj");
        if(mesh == NULL)
            return -1;
    
        // cree 2 objets
        m_mesh[0]= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        m_mesh[0]->createBuffer(0, mesh->positions);
        m_mesh[0]->createBuffer(1, mesh->texcoords);
        m_mesh[0]->createBuffer(2, mesh->normals);
        m_mesh[0]->createIndexBuffer(mesh->indices);
        
        m_mesh[1]= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        m_mesh[1]->createBuffer(0, mesh->positions);
        m_mesh[1]->createBuffer(1, mesh->texcoords);
        m_mesh[1]->createBuffer(2, mesh->normals);
        m_mesh[1]->createIndexBuffer(mesh->indices);
        
        // cree 2 objets degeneres, meme nombre de triangles
        for(unsigned int i= 1; i < mesh->indices.size(); i++)
            mesh->indices[i]= mesh->indices[0]; // tous les triangles referencent le meme sommet
        
        m_mesh[2]= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        m_mesh[2]->createBuffer(0, mesh->positions);
        m_mesh[2]->createBuffer(1, mesh->texcoords);
        m_mesh[2]->createBuffer(2, mesh->normals);
        m_mesh[2]->createIndexBuffer(mesh->indices);
        
        m_mesh[3]= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        m_mesh[3]->createBuffer(0, mesh->positions);
        m_mesh[3]->createBuffer(1, mesh->texcoords);
        m_mesh[3]->createBuffer(2, mesh->normals);
        m_mesh[3]->createIndexBuffer(mesh->indices);
        
        // vao
        m_vao= gk::createVertexArray();
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        // multi draw indirect storage buffer
        m_indirect= gk::createBufferStorage(GL_DRAW_INDIRECT_BUFFER, 
            10000u*sizeof(DrawElementsIndirect), NULL, 
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_DYNAMIC_STORAGE_BIT | GL_CLIENT_STORAGE_BIT);
        // persistant map : eviter mapRange() / unmap()
        m_indirect_storage= (DrawElementsIndirect *) m_indirect->mapRange(GL_DRAW_INDIRECT_BUFFER, 
            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        if(m_indirect_storage == NULL)
            return -1;
        
        m_time= gk::createTimeQuery();
        
        //
        m_cpu_graph.push_back(0);
        
        m_scale = 1.0f;
        
        m_draw_count= 1;
        m_draw_degenerate= false;
        
        m_change_shader= false;
        m_change_mesh= false;
        m_change_texture= false;
        
        m_static_vao= true;
        m_multi_draw= false;
        
        return 0;
    }
    
    int quit( )
    {
        return 0;
    }
    
    
    int draw( )
    {
        if(key(SDLK_ESCAPE))
            // fermer l'application si l'utilisateur appuie sur ESCAPE
            closeWindow();
        
        if(key('r'))
        {
            key('r')= 0;
            // recharge et recompile les shaders
            gk::reloadPrograms();
        }
        
        if(key('s'))
        {
            key('s')= 0;
            // enregistre l'image opengl
            gk::writeFramebuffer("screenshot.png");
        }
        
        glViewport(0, 0, windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        gk::Transform model= gk::Translate( gk::Vector(0.f, 0.f, -50.f) ) * gk::Scale(m_scale);
        gk::Transform view;
        gk::Transform perspective= gk::Perspective(50.f, 1.f, 1.f, 1000.f);
        gk::Transform mv= view * model;
        gk::Transform mvp= perspective * mv;
        
        m_time->begin();
        GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
        //~ GLint64 start= 0;
        
        int program_id= 0;
        int mesh_id= 0;
        int texture_id= 0;
        int current_program= -1;
        int current_mesh= -1;
        int current_texture= -1;        
        int current_vao= -1;
        
        if(m_multi_draw)
        {
            glUseProgram(m_program[program_id]->name);
        
            m_program[program_id]->uniform("mvpMatrix")= mvp.matrix();
            m_program[program_id]->uniform("normalMatrix")= mv.normalMatrix();
            m_program[program_id]->uniform("color")= gk::Vec4(1.f, 1.f, 1.f);            
            
            GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
            DrawElementsIndirect draw(m_mesh[mesh_id]->count);
            
            // construit les draws directement dans le buffer indrect
            assert(m_draw_count <= 10000);
            for(int i= 0; i < m_draw_count; i++)
                m_indirect_storage[i]= draw;
            
            // attendre que les donnees soient transferees
            glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
            
            GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);
            int cpu_time= (stop - start);
            //~ printf("multi draw cpu time % 6dus  ", cpu_time / 1000);
            
            if(m_draw_degenerate)
                mesh_id= (mesh_id + 2) % 4;
            glBindVertexArray(m_mesh[mesh_id]->vao->name);
            
            // draw !
            glMultiDrawElementsIndirect(m_mesh[mesh_id]->primitive, m_mesh[mesh_id]->index_type, 
                0, m_draw_count, sizeof(DrawElementsIndirect));
        }
        else
        {
            for(int draw= 0; draw < m_draw_count; draw++)
            {
                // modifie l'etat, si necessaire
                if(m_change_shader)
                    program_id= draw % 2;
                if(m_change_mesh)
                    mesh_id= draw % 2;
                if(m_change_texture)
                    texture_id= draw % 2;
                
                if(current_program != program_id)
                {
                    current_program= program_id;
                    glUseProgram(m_program[current_program]->name);
                
                    m_program[current_program]->uniform("mvpMatrix")= mvp.matrix();
                    m_program[current_program]->uniform("normalMatrix")= mv.normalMatrix();
                    m_program[current_program]->uniform("color")= gk::Vec4(1.f, 1.f, 1.f);
                }
                
                if(current_mesh != mesh_id)
                {
                    current_mesh= mesh_id;
                    if(m_draw_degenerate)
                    {
                        mesh_id= (mesh_id + 2) % 4;
                        current_mesh= mesh_id;
                    }
                    
                    if(m_static_vao)
                    {
                        glBindVertexArray(m_mesh[current_mesh]->vao->name);
                        current_vao= m_mesh[current_mesh]->vao->name;
                    }
                    
                    else
                    {
                        glBindVertexArray(m_vao->name);
                        current_vao= m_vao->name;
                        
                        // recycle le meme vao pour chaque draw
                        glBindBuffer(GL_ARRAY_BUFFER, m_mesh[current_mesh]->buffers[0]->name);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
                        glEnableVertexAttribArray(0);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, m_mesh[current_mesh]->buffers[1]->name);
                        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
                        glEnableVertexAttribArray(1);
                        
                        glBindBuffer(GL_ARRAY_BUFFER, m_mesh[current_mesh]->buffers[2]->name);
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
                        glEnableVertexAttribArray(2);
                        
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh[current_mesh]->index_buffer->name);
                    }
                }
                
                if(current_texture != texture_id)
                {
                    current_texture= texture_id;
                    // todo
                }
                
                // draw
                if(m_mesh[current_mesh]->index_type == 0u)
                    glDrawArrays(m_mesh[current_mesh]->primitive, 0, m_mesh[current_mesh]->count);
                else
                    glDrawElements(m_mesh[current_mesh]->primitive, m_mesh[current_mesh]->count, m_mesh[current_mesh]->index_type, 0);
            }
        }
        
        glUseProgram(0);
        glBindVertexArray(0);
        
        m_time->end();
        int gpu_time= m_time->result64();
        //~ int gpu_time= 0;

        //~ GLint64 stop= 0;
        GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);
        int cpu_time= (stop - start);
        
        if(m_cpu_graph.size() >= 100)
            m_cpu_graph.erase(m_cpu_graph.begin());     // argh !!
        m_cpu_graph.push_back(cpu_time);
        
        int cpu_average= 0;
        for(unsigned int i= 0; i < m_cpu_graph.size(); i++)
            cpu_average+= m_cpu_graph[i];
        cpu_average/= (int) m_cpu_graph.size();
        
        char tmp[1024];
        m_widgets.begin();
        m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            
            m_widgets.doButton(nv::Rect(), "change shader", &m_change_shader);
            m_widgets.doButton(nv::Rect(), "change buffer", &m_change_mesh);
            //~ m_widgets.doButton(nv::Rect(), "change texture", &m_change_texture);
            m_widgets.doButton(nv::Rect(), "draw degenerate triangles", &m_draw_degenerate);
            m_widgets.doButton(nv::Rect(), "use static vao", &m_static_vao);
            //~ m_widgets.doLabel(nv::Rect(), "use bindless buffers");
            m_widgets.doButton(nv::Rect(), "use multi draw indirect", &m_multi_draw);
        
            sprintf(tmp, "draw count %d", m_draw_count);
            m_widgets.doLabel(nv::Rect(), tmp);
            
            float count= m_draw_count;
            if(m_widgets.doHorizontalSlider(nv::Rect(0, 0, 1000, 0), 1.f, 10000.f, &count))
                m_draw_count= count;

            sprintf(tmp, "scale %f", m_scale);
            m_widgets.doLabel(nv::Rect(), tmp);
            m_widgets.doHorizontalSlider(nv::Rect(0, 0, 1000, 0), 0.1f, 10.f, &m_scale);
            
            sprintf(tmp, "cpu time % 6dus [% 6dus:%d]", cpu_time / 1000, cpu_average / 1000, (int) m_cpu_graph.size());
            m_widgets.doLabel(nv::Rect(), tmp);
            
            sprintf(tmp, "gpu time % 3dms", gpu_time / 1000000);
            m_widgets.doLabel(nv::Rect(), tmp);
            
            //~ m_widgets.doGraph(nv::Rect(), &m_cpu_graph.front(), m_cpu_graph.size());
            
        m_widgets.endGroup();
        m_widgets.end();
        //~ glFinish(); 
    
        present();
        return 1;
    }
};


int main( int argc, char **argv )
{
    Batch app;
    app.run();
    
    return 0;
}

