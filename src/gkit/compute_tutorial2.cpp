/*
exemple de compute shader : intersections rayons / triangles.

etape 1 :
le compute shader calcule l'intersection de chaque rayon associe a un pixel de l'image avec l'ensemble des triangles d'un maillage
entree : les triangles
sortie : pour chaque rayon / pixel, la couleur du triangle le plus proche de la camera

etape 2 :
affiche le resultat 
*/


#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Vec.h"
#include "Geometry.h"

#include "GL/GLSLUniforms.h"

#include "GL/GLProgram.h"
#include "GL/GLQuery.h"
#include "GL/GLBuffer.h"
#include "GL/GLTexture.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLBasicMesh.h"
#include "ProgramManager.h"

#include "Mesh.h"
#include "MeshIO.h"


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


struct Triangle
{
    gk::glsl::vec3 p;
    gk::glsl::vec3 edge1;
    gk::glsl::vec3 edge2;
    gk::glsl::vec3 color;
    
    Triangle( const gk::Vec3& a, const gk::Vec3& b, const gk::Vec3& c, const gk::Vec3& _color= gk::Vec3(1, 1, 1) )
        : p(a), edge1(gk::Point(b) - gk::Point(a)), edge2(gk::Point(c) - gk::Point(a)), color(_color) {}
};

class TP : public gk::App
{
    gk::GLProgram *program;
    gk::GLProgram *display;
    gk::GLBuffer *input;
    gk::GLTexture *output;
    gk::GLFramebuffer *framebuffer;
    gk::GLBasicMesh *mesh;
    gk::GLQuery *m_time;
    
    float rotate;
    float distance;
    
    nv::SdlContext m_widgets;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( )
        :
        gk::App()
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(4,3);     // version 4.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(512, 512, settings) < 0)
            closeWindow();
        
        m_widgets.init();
        m_widgets.reshape(windowWidth(), windowHeight());
    }
    
    ~TP( ) {}

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
        // cree un shader program
        gk::programPath("compute");
        program= gk::createProgram("triangle.glsl");
        if(program == gk::GLProgram::null())
            return -1;

        m_time= gk::createTimeQuery();
        
        std::string filename= "bigguy.obj";
        gk::Mesh *mesh= gk::MeshIO::readOBJ(filename);
        if(mesh == NULL)
            return -1;  // erreur de lecture
        
        // recupere les triangles de l'objet
        std::vector<Triangle> triangles;
        for(int i= 0; i < mesh->triangleCount(); i++)
        {
            const gk::Triangle& t= mesh->triangle(i);
            gk::Vec3 color(1.f - float(i % 100) / 99.f, float(i % 10) / 9.f, float(i % 1000) / 999.f);
            triangles.push_back( Triangle(t.a, t.b, t.c, color) );
        }
        
        delete mesh;
        
        input= gk::createBuffer(GL_SHADER_STORAGE_BUFFER, triangles);
        
        framebuffer= gk::createFramebuffer(GL_READ_FRAMEBUFFER, windowWidth(), windowHeight(), gk::GLFramebuffer::COLOR0_BIT, gk::TextureRGBA);
        output= framebuffer->texture(gk::GLFramebuffer::COLOR0);
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        //
        distance= 50;
        rotate= -40;
        
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
        
        if(key(SDLK_LEFT))
            rotate-= 1.f;
        if(key(SDLK_RIGHT))
            rotate+= 1.f;
        if(key(SDLK_UP))
            distance+= 1.f;
        if(key(SDLK_DOWN))
            distance-= 1.f;
        
        // redimensionne l'image en fonction de la fenetre de l'application
        glViewport(0, 0, windowHeight(), windowHeight());
        // efface l'image
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // mesurer le temps d'execution
        m_time->begin();
        GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
        
        // transformations
        gk::Transform model= gk::RotateY(30.f); 
        gk::Transform view= gk::Translate( gk::Vector(0.f, 0.f, -distance) ) * gk::RotateY(rotate);
        gk::Transform projection= gk::Perspective(50.f, 1.f, 1.f, 1000.f);
        gk::Transform viewport= gk::Viewport(windowWidth(), windowHeight());
        
        // composition des transformations
        gk::Transform mv= view * model;
        gk::Transform mvp= projection * mv;
        gk::Transform mvpv= viewport * mvp;

        glUseProgram(program->name);
        
        program->uniform("mvpMatrix")= mvp.matrix();
        program->uniform("mvpvInvMatrix")= mvpv.inverseMatrix();
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input->name);
        glBindImageTexture(0, output->name, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
        
        GLint group_size[3];
        glGetProgramiv(program->name, GL_COMPUTE_WORK_GROUP_SIZE, group_size);
        
        glDispatchCompute(output->width / group_size[0], output->height / group_size[1], 1);   // groupe 16x16 = 256 threads
        
        // mesurer le temps d'execution
        GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);     // nano secondes
        m_time->end();
        GLuint64 cpu_time= (stop - start) / 1000;               // conversion en micro secondes
        GLuint64 gpu_time= m_time->result64() / 1000;        // conversion en micro secondes
        
        // un peu de nettoyage
        glUseProgram(0);
        
        // copier le resultat sur le framebuffer 
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer->name);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0,0, framebuffer->width, framebuffer->height, 
            0,0, windowWidth(), windowHeight(),
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        // afficher le temps d'execution
        {
            m_widgets.begin();
            m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            
            m_widgets.doLabel(nv::Rect(), Format("cpu time % 6ldus", cpu_time));
            m_widgets.doLabel(nv::Rect(), Format("gpu time % 3ldms % 3ldus", gpu_time / 1000, gpu_time % 1000));
            
            m_widgets.endGroup();
            m_widgets.end();
        }
        
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

