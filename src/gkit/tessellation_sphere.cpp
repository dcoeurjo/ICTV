
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Transform.h"

#include "GL/GLBuffer.h"
#include "GL/GLVertexArray.h"
#include "ProgramManager.h"
#include "Triangle.h"


class Tesselation4 : public gk::App
{
    gk::GLProgram *program;

    gk::GLBuffer *quad;
    gk::GLVertexArray *vao;
    
    nv::SdlContext ui;
    float inner_factor_u;
    float inner_factor_v;
    float edge0_factor;
    float edge1_factor;
    float edge2_factor;
    float edge3_factor;
    
public:
    // creation de la fenetre avec les reglages par defaut.
    Tesselation4( )
        :
        gk::App()
    {
        gk::AppSettings settings;
        
        settings.setSamples(4);
        settings.setGLVersion(4, 1);    // sur mac creer un contexte 3.2, bug sdl 2 :         settings.setGLVersion(3,2);

        settings.setGLCoreProfile();
        settings.setGLDebugContext();
        
        if(createWindow(1024, 768, settings) < 0)
            closeWindow();
        
        ui.init();
        ui.reshape(windowWidth(), windowHeight());
    }
    
    ~Tesselation4( ) {}
    
    // a redefinir pour utiliser les widgets.
    void processWindowResize( SDL_WindowEvent& event )
    {
        ui.reshape(event.data1, event.data2);
    }
    
    // a redefinir pour utiliser les widgets.
    void processMouseButtonEvent( SDL_MouseButtonEvent& event )
    {
        ui.processMouseButtonEvent(event);
    }
    
    // a redefinir pour utiliser les widgets.
    void processMouseMotionEvent( SDL_MouseMotionEvent& event )
    {
        ui.processMouseMotionEvent(event);
    }
    
    // a redefinir pour utiliser les widgets.
    void processKeyboardEvent( SDL_KeyboardEvent& event )
    {
        ui.processKeyboardEvent(event);
    }
    
    int init( )
    {
        // creer l'utilitaire de creation de shader a partir d'un fichier .gkfx
        program= gk::createProgram("tessellation_sphere.glsl");
        if(program == gk::GLProgram::null())
            return -1;
        
        vao= gk::createVertexArray();
        
        std::vector<gk::Vec4> points;
        points.push_back( gk::Vec4( .0f, .0f, .5f, .5f) );      // x, y, z, radius
        quad= gk::createBuffer(GL_ARRAY_BUFFER, points);
        
        inner_factor_u= 6.f;
        inner_factor_v= 6.f;
        edge0_factor= 6.f;
        edge1_factor= 6.f;
        edge2_factor= 6.f;
        edge3_factor= 6.f;
        
        glLineWidth(2.f);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_MULTISAMPLE);
        
        return 0;       // tout c'est bien passe, sinon renvoyer -1
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
        
        glViewport(0, 0, windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // dessiner les aretes sans remplir les triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //~ glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // activer le shader
        glUseProgram(program->name);
        
        // parametrer le shader
        glBindVertexArray(vao->name);
        
        GLint location= program->attribute("position").location;
        glBindBuffer(GL_ARRAY_BUFFER, quad->name);
        glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, 0);   // x, y, z, radius
        glEnableVertexAttribArray(location);
        
        program->uniform("inner_factor")= gk::Vec2(inner_factor_u, inner_factor_v);
        program->uniform("edge_factor")= gk::Vec4(edge0_factor, edge1_factor, edge2_factor, edge3_factor);
        
        gk::Transform mvp;  // identite
        program->uniform("mvpMatrix")= mvp.matrix();
        
        // dessiner un point 4d: centre + rayon de la sphere
        glPatchParameteri(GL_PATCH_VERTICES, 1);
        glDrawArrays(GL_PATCHES, 0, 1);
        
        // nettoyage
        glBindVertexArray(0);
        glUseProgram(0);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // interface
        char tmp[1024]= { 0 };
        ui.begin();
            ui.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            
            sprintf(tmp, "inner level u %.1f", inner_factor_u);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &inner_factor_u);
            sprintf(tmp, "inner level v %.1f", inner_factor_u);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &inner_factor_v);
            
            sprintf(tmp, "edge0 level %.1f", edge0_factor);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &edge0_factor);
            
            sprintf(tmp, "edge1 level %.1f", edge1_factor);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &edge1_factor);
            
            sprintf(tmp, "edge2 level %.1f", edge2_factor);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &edge2_factor);

            sprintf(tmp, "edge3 level %.1f", edge3_factor);
            ui.doLabel(nv::Rect(), tmp);
            ui.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 1.f, 32.f, &edge3_factor);

            ui.endGroup();
        ui.end();

	float speed= .1f;
        if(key(SDLK_LEFT))
        {
            //~ key(SDLK_LEFT)= 0;
            // bouge toutes les valeurs
            inner_factor_u= std::max(1.f, (inner_factor_u -speed));
            inner_factor_v= std::max(1.f, (inner_factor_v -speed));
            edge0_factor= std::max(1.f, (edge0_factor -speed));
            edge1_factor= std::max(1.f, (edge1_factor -speed));
            edge2_factor= std::max(1.f, (edge2_factor -speed));
            edge3_factor= std::max(1.f, (edge3_factor -speed));
        }
        if(key(SDLK_RIGHT))
        {
            //~ key(SDLK_RIGHT)= 0; 
            // bouge toutes les valeurs
            inner_factor_u= std::min(32.f, (inner_factor_u +speed));
            inner_factor_v= std::min(32.f, (inner_factor_v +speed));
            edge0_factor= std::min(32.f, (edge0_factor +speed));
            edge1_factor= std::min(32.f, (edge1_factor +speed));
            edge2_factor= std::min(32.f, (edge2_factor +speed));
            edge3_factor= std::min(32.f, (edge3_factor +speed));
        }
        
        // afficher le buffer de dessin
        present();
        return 1;       // continuer
    }
};


int main( int argc, char **argv )
{
    Tesselation4 app;
    app.run();
    
    return 0;
}

