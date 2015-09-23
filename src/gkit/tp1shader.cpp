
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "ProgramManager.h"
#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"


class TP : public gk::App
{
    nv::SdlContext m_widgets;
    
    gk::GLProgram *m_program;
    gk::GLVertexArray *m_vao;
    gk::GLQuery *m_time;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( )
        :
        gk::App()
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(3,3);     // version 3.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(512, 512, settings) < 0)
            closeWindow();
        
        // initilalise les widgets
        m_widgets.init();
        m_widgets.reshape(windowWidth(), windowHeight());
    }
    
    ~TP( ) {}
    
    int init( )
    {
        // compilation simplifiee
        m_program= gk::createProgram("tp1sphere.glsl");
        //~ m_program= gk::loadProgram("tp1sphere.glsl")->defineFragment("TOTO").make();
        if(m_program == gk::GLProgram::null())
            return -1;
        
        m_vao= gk::createVertexArray();
        m_time= gk::createTimeQuery();
        
        return 0;
    }
    
    int quit( )
    {
        return 0;
    }

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
        
        //
        glViewport(0, 0, windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // mesurer le temps d'execution
        GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
        m_time->begin();
        
        glUseProgram(m_program->name);
        glBindVertexArray(m_vao->name);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glUseProgram(0);
        glBindVertexArray(0);
        
        // mesurer le temps d'execution
        m_time->end();
        GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);     // nano secondes
        GLuint64 cpu_time= (stop - start) / 1000;               // conversion en micro secondes
        GLuint64 gpu_time= m_time->result64() / 1000;        // conversion en micro secondes
        
        // afficher le temps d'execution
        {
            char tmp[1024];
            
            m_widgets.begin();
            m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);

            sprintf(tmp, "cpu time % 6ldus", cpu_time);
            m_widgets.doLabel(nv::Rect(), tmp);
            
            sprintf(tmp, "gpu time % 3ldms % 3ldus", gpu_time / 1000, gpu_time % 1000);
            m_widgets.doLabel(nv::Rect(), tmp);
            
            m_widgets.endGroup();
            m_widgets.end();
        }
        
        present();
        return 1;
    }
};


int main( int argc, char **argv )
{
    TP app;
    app.run();
    
    return 0;
}

