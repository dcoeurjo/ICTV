
#include <map>

#include "App.h"
#include "Widgets/nvSDLContext.h"


#include "Geometry.h"
#include "Transform.h"

#include "ProgramManager.h"

#include "Image.h"
#include "ImageArray.h"
#include "ImageManager.h"

#include "Mesh.h"
#include "MeshIO.h"

#include "Orbiter.h"
#include "OrbiterIO.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLSampler.h"
#include "GL/GLBasicMesh.h"


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
    
    operator std::string( )
    {
        return std::string(text);
    }
};


class TP : public gk::App
{
    std::string m_filename;
    
    gk::GLProgram *m_program;
    gk::GLProgram *m_debug_program;
    gk::GLBasicMesh *m_mesh;
    std::vector<gk::MeshGroup> m_groups;
    std::vector<gk::GLTexture *> m_textures;
    
    int m_points;
    gk::Point m_center;
    gk::Orbiter m_camera;
    
    gk::GLQuery *m_time;
    
    nv::SdlContext m_widgets;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( const char *filename )
        :
        gk::App(),
        m_filename(filename),
        m_program(NULL),
        m_debug_program(NULL),
        m_mesh(NULL),
        m_groups(),
        m_textures()
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(3,3);     // version 3.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(1280, 640, settings) < 0)
            closeWindow();
        
        char *path= SDL_GetBasePath();
        if(path)
        {
            gk::IOFileSystem::manager().basePath(path);
            SDL_free(path);
        }
        
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
        // charger l'objet
        gk::Mesh *mesh= gk::MeshIO::readOBJ(m_filename);
        if(mesh == NULL)
            return -1;  // erreur de lecture
        
        //~ if(mesh->normals.size() != mesh->positions.size())
            //~ gk::MeshIO::buildNormals(mesh);     // construire les normales si necessaire.
        
        // positionne la camera pour voir l'objet
        m_center= mesh->box.center();
        m_camera.look(mesh->box);
        
        // shader program
        gk::programPath("shaders");
        // standard : utilise la texture difffuse
        m_program= gk::createProgram("texture.glsl");
        if(m_program == gk::GLProgram::null())
            ERROR("program not found\n");
        
        // debug
        m_debug_program= gk::createProgram("dFnormal.glsl");
        if(m_debug_program == gk::GLProgram::null())
            ERROR("program not found\n");
        
        // construit les buffers + vertex array
        m_points= mesh->positions.size();
        m_mesh= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        if(mesh == NULL)
            return -1;
        
       // indexation directe des attributs, vao independant du shader. cf layout(location= x) dans le shader
        m_mesh->createBuffer(0, mesh->positions);
        m_mesh->createBuffer(1, mesh->texcoords);
        m_mesh->createBuffer(2, mesh->normals);
        m_mesh->createIndexBuffer(mesh->indices);       // un index buffer est cree, glDrawElements pour dessiner au lieu de glDrawArrays.
        
        // charger les images associees aux matieres
        std::map<gk::Image *, gk::GLTexture *> textures;
        gk::imagePath("images");
        for(unsigned int i= 0; i < mesh->groups.size(); i++)
        {
            const std::string& texture= mesh->groups[i].material.diffuse_texture;
            
            gk::Image *image= NULL;
            if(texture.empty() == false)
                image= gk::readImage(texture);
            if(image == NULL)
                image= gk::defaultImage();      // utilise une texture debug, si la texture n'est pas disponible
            
            //~ m_textures.push_back( gk::createTexture2D(gk::GLTexture::UNIT0, image) );
            // ne cree qu'une seule texture par image chargee
            std::pair<std::map<gk::Image *, gk::GLTexture *>::iterator, bool> found= textures.insert( std::make_pair(image, gk::GLTexture::null()) );
            if(found.second)
                found.first->second= gk::createTexture2D(gk::GLTexture::UNIT0, image);
            
            m_textures.push_back(found.first->second);
        }
        
        // conserver les groupes de faces
        m_groups= mesh->groups;
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete mesh;
        
        // mesure du temps de dessin
        m_time= gk::createTimeQuery();
        
        return 0;       // tout c'est bien passe, sinon renvoyer -1
    }
    
    int quit( ) 
    {
        delete m_mesh;
        // le shader program et les shaders sont detruits automatiquement par gKit.
        return 0;
    }
    
    int draw( )
    {
        if(key(SDLK_ESCAPE))
            // fermer l'application si l'utilisateur appuie sur ESCAPE
            closeWindow();

        // controle de la camera a la souris
        int x, y;
        unsigned int button= SDL_GetRelativeMouseState(&x, &y);
        //~ printf("mouse %d %d %u (%d %d %d)\n", x,y, button, button & SDL_BUTTON(1), button & SDL_BUTTON(2), button & SDL_BUTTON(3));
        
        if(button & SDL_BUTTON(1))
            m_camera.rotate(x, y);      // orbit
        else if(button & SDL_BUTTON(2))
            m_camera.move( float(x) / float(windowWidth()), float(y) / float(windowHeight()) ); // pan
        else if(button & SDL_BUTTON(3))
            m_camera.move(x);           // dolly
        
        if(key('c'))
        {
            key('c')= 0;
            gk::writeOrbiter(m_camera, gk::IOFileSystem::changeType(m_filename, ".gkcamera"));
        }
        
        if(key('v'))
        {
            key('v')= 0;
            m_camera= gk::readOrbiter(gk::IOFileSystem::changeType(m_filename, ".gkcamera"));
        }
        
        if(key('r'))
        {
            key('r')= 0;
            gk::reloadPrograms();
        }
        
        static int wireframe= 0;
        if(key('w'))
        {
            key('w')= 0;
            wireframe= (wireframe + 1) % 2;
        }
        
        if(wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glDisable(GL_CULL_FACE);
        
        
        // transformations
        gk::Transform model= gk::Translate( -gk::Vector(m_center) );
        gk::Transform view= m_camera.view();
        gk::Transform projection= m_camera.projection(windowWidth(), windowHeight());
        
        // composition des transformations
        gk::Transform mv= view * model;
        gk::Transform mvp= projection * mv;
        
        // fixer la transformation viewport en fonction des dimensions de la fenetre
        glViewport(0, 0, windowWidth(), windowHeight());
        // effacer l'image
        glClearColor(0.2, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // selectionner le shader
        static int solid= 0;
        if(key('x'))
        {
            key('x')= 0;
            solid= (solid +1) % 2;
        }
        
        // selectionner le type d'affichage, par groupe ou pas.
        static int group= 1;
        if(key('g'))
        {
            key('g')= 0;
            group= (group +1) % 2;
        }
        
        // mesurer le temps d'execution
        m_time->begin();
        GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
        
        gk::GLProgram *program= gk::GLProgram::null();
        if(solid == 1)
            program= m_program;
        else
            program= m_debug_program;
        
        glUseProgram(program->name);
        
        // parametrer le shader : matrices
        program->uniform("mvpMatrix")= mvp.matrix();
        program->uniform("normalMatrix")= mv.normalMatrix();
        program->uniform("diffuse_color")= gk::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        
        int batches= 0;
        if(program == m_debug_program && wireframe == 1)
        {
            program->uniform("diffuse_color")= gk::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            
            glPointSize(3.f);
            glBindVertexArray(m_mesh->vao->name);
            glDrawArrays(GL_POINTS, 0, m_points);
        }
        else
        {
            if(group > 0)
            {
                // afficher les groupes de faces du mesh
                for(unsigned int i= 0; i < m_groups.size(); i++)
                {
                    int unit= 0;    // couleur diffuse sur l'unite 0
                    program->sampler("diffuse_texture")= unit;
                    glBindSampler(unit, gk::defaultSampler()->name);
                    
                    glActiveTexture(GL_TEXTURE0 + unit);
                    glBindTexture(m_textures[i]->target, m_textures[i]->name);
                    
                    program->uniform("diffuse_color")= gk::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    //~ program->uniform("diffuse_color")= m_groups[i].material.diffuse_color;
                    
                    // dessiner
                    m_mesh->drawGroup(m_groups[i].begin, m_groups[i].end);
                    batches++;
                }
            }
            else
            {
                    // afficher toutes les faces
                    int unit= 0;    // couleur diffuse sur l'unite 0
                    program->sampler("diffuse_texture")= unit;
                    glBindSampler(unit, gk::defaultSampler()->name);
                    
                    glActiveTexture(GL_TEXTURE0 + unit);
                    glBindTexture(m_textures[0]->target, m_textures[0]->name);
                    
                    program->uniform("diffuse_color")= gk::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
                
                    m_mesh->draw();
                    batches++;
            }
        }
    
        // mesurer le temps d'execution
        GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);     // nano secondes
        m_time->end();
        GLuint64 cpu_time= (stop - start) / 1000;               // conversion en micro secondes
        GLuint64 gpu_time= m_time->result64() / 1000;        // conversion en micro secondes
        
        // nettoyage, desactive les differents objets selectionnes
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        
        m_widgets.begin();
        m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            m_widgets.doLabel(nv::Rect(), Format("cpu time % 6ldus, % 03d batches", cpu_time, batches));
            m_widgets.doLabel(nv::Rect(), Format("gpu time % 3ldms % 3ldus", gpu_time / 1000, gpu_time % 1000));
        m_widgets.endGroup();
        m_widgets.end();
        
        // afficher le buffer de dessin
        present();

        if(key('s'))
        {
            key('s')= 0;
            
            static int counter= 1;
            gk::writeFramebuffer( Format("screenshot%02d.png", counter++) );
        }
        
        return 1;       // continuer, dessiner une autre fois l'image, renvoyer 0 pour arreter l'application
    }
};


int main( int argc, char **argv )
{
    const char *filename= "bigguy.obj";
    if(argc > 1)
        filename= argv[1];
    
    TP app(filename);
    app.run();
    
    return 0;
}

