
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Mesh.h"
#include "MeshIO.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLBuffer.h"
#include "GL/GLVertexArray.h"
#include "ProgramManager.h"

//! classe utilitaire : permet de construire une chaine de caracteres formatee. cf sprintf.
struct Format
{
    char text[1024];
    
    Format( const char *_format, ... )
    {
        text[0]= 0;     // chaine vide
        
        // recupere la liste d'arguments supplementaires
        va_list args;
        va_start(args, _format);
        vsnprintf(text, sizeof(text), _format, args);
        va_end(args);
    }
    
    ~Format( ) {}
    
    // conversion implicite de l'objet en chaine de caracteres stantard
    operator const char *( )
    {
        return text;
    }
};


//! squelette d'application gKit.
class TP : public gk::App
{
    nv::SdlContext m_widgets;
    
    gk::GLProgram *m_program;
    gk::GLVertexArray *m_vao;
    
    gk::GLBuffer *m_vertex_buffer;
    gk::GLBuffer *m_index_buffer;
    int m_indices_size;
    
    gk::GLCounter *m_time;
    
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
        
        m_widgets.init();
        m_widgets.reshape(windowWidth(), windowHeight());
    }
    
    ~TP( ) {}
    
    int init( )
    {
        // compilation simplifiee d'un shader program
        gk::programPath("shaders");
        m_program= gk::createProgram("core.glsl");
        if(m_program == gk::GLProgram::null())
            return -1;
        
        // charge un mesh
        gk::Mesh *mesh= gk::MeshIO::readOBJ("bbox.obj");
        if(mesh == NULL)
            return -1;
        
        // cree le vertex array objet, description des attributs / associations aux variables du shader
        m_vao= gk::createVertexArray();
        
        // cree le buffer de position
        m_vertex_buffer= gk::createBuffer(GL_ARRAY_BUFFER, mesh->positions);
        // associe le contenu du buffer a la variable 'position' du shader
        glVertexAttribPointer(m_program->attribute("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        // active l'utilisation du buffer 
        glEnableVertexAttribArray(m_program->attribute("position"));
        
        // cree le buffer d'indices et l'associe au vertex array
        m_index_buffer= gk::createBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indices);
        // conserve le nombre d'indices (necessaire pour utiliser glDrawElements)
        m_indices_size= mesh->indices.size();
        
        // mesh n'est plus necessaire, les donnees sont transferees dans les buffers sur la carte graphique
        delete mesh;
        
        // nettoyage de l'etat opengl
        glBindVertexArray(0);   // desactive le vertex array
        glBindBuffer(GL_ARRAY_BUFFER, 0);       // desactive le buffer de positions
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);       // desactive le buffer d'indices
        
        // mesure du temps de dessin
        m_time= gk::createTimer();
        
        // ok, tout c'est bien passe
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
        
        if(key('c'))
        {
            key('c')= 0;
            // enregistre l'image opengl
            gk::writeFramebuffer("screenshot.png");
        }
        
        //
        glViewport(0, 0, windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // mesurer le temps d'execution
        m_time->start();
        
        // dessiner quelquechose
        glUseProgram(m_program->name);
        
        // parametrer le shader
        m_program->uniform("mvpMatrix")= gk::Transform().matrix();      // transformation model view projection
        m_program->uniform("color")= gk::VecColor(1, 1, 0);     // couleur des fragments
        
        // selectionner un ensemble de buffers et d'attributs de sommets
        glBindVertexArray(m_vao->name);
        // dessiner un maillage indexe
        glDrawElements(GL_TRIANGLES, m_indices_size, GL_UNSIGNED_INT, 0);
        
        // nettoyage
        glUseProgram(0);
        glBindVertexArray(0);
        
        // mesurer le temps d'execution
        m_time->stop();
        
        // afficher le temps d'execution
        {
            m_widgets.begin();
            m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            
            m_widgets.doLabel(nv::Rect(), m_time->summary("draw").c_str());
            
            m_widgets.endGroup();
            m_widgets.end();
        }
        
        // afficher le dessin
        present();
        // continuer
        return 1;
    }
};


int main( int argc, char **argv )
{
    TP app;
    app.run();
    
    return 0;
}

