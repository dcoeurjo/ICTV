
#include "App.h"

#include "ProgramManager.h"


class TP : public gk::App
{
    gk::GLProgram *m_program;
    
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
    }
    
    ~TP( ) {}
    
    int init( )
    {
        // cree un shader program
        m_program= gk::createProgram("tp1sphere.glsl");
        if(m_program == gk::GLProgram::null())
            return -1;
        
        // tout c'est bien passe.
        return 0;
    }
    
    int quit( )
    {
        // rien a faire les ressources opengl sont detruites automatiquement par gk::GLManager.
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
        
        // redimensionne l'image en fonction de la fenetre de l'application
        glViewport(0, 0, windowHeight(), windowHeight());
        // efface l'image
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // selectionne le shader program a utiliser
        glUseProgram(m_program->name);
        
        // dessine quelquechose
        
        // un peu de nettoyage
        glUseProgram(0);
        
        // visualise l'image que l'on vient de dessiner.
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

