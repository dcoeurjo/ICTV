
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Geometry.h"
#include "Transform.h"

#include "ProgramManager.h"
#include "ProgramTweaks.h"

#include "Image.h"
#include "ImageArray.h"
#include "ImageManager.h"

#include "Orbiter.h"

#include "Mesh.h"
#include "MeshIO.h"

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

struct Filename
{
    char path[1024];
    
    Filename( ) { path[0]= 0; }
    Filename( const char *file ) { strcpy(path, file); }
    
    bool operator== ( const Filename& b ) { return strcmp(path, b.path) == 0; }
    bool operator!= ( const Filename& b ) { return strcmp(path, b.path) != 0; }
};

class TP : public gk::App
{
    std::string m_filename;
    
    Filename m_mesh_filename;
    std::vector<Filename> m_texture_filenames;
    
    gk::GLProgram *m_program;
    gk::GLBasicMesh *m_mesh;
    std::vector<gk::GLTexture *> m_textures;
    gk::GLSampler *m_sampler;
    int m_filter;
    int m_wrap;
    
    bool m_freeze;
    float m_time;
    
    gk::Point m_center;
    gk::Orbiter m_camera;
    
    gk::GLQuery *m_perf;
    
    nv::SdlContext m_widgets;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( const char *filename )
        :
        gk::App(),
        m_filename(filename),
        m_mesh_filename(),
        m_texture_filenames(),
        m_program(NULL),
        m_mesh(NULL),
        m_textures(),
        m_sampler(NULL),
        m_filter(2), m_wrap(0),
        m_freeze(false), m_time(0)
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(3,3);     // version 3.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(800, 800, settings) < 0)
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
    
    // a redefinir pour utiliser les widgets.
    void processTextEvent( const char *string )
    {
        m_widgets.processTextEvent(string);
    }
    
    // a redefinir pour utiliser les widgets.
    void processDropEvent( const char *file )
    {
        m_widgets.processDropEvent(file);
    }
    
    int init( )
    {
        gk::imagePath("images");
        gk::programPath("shaders");
        m_program= gk::createProgram(m_filename);
        
        m_mesh= new gk::GLBasicMesh(GL_TRIANGLE_STRIP, 4);
        glBindVertexArray(0);
        
        loadSampler(m_filter, m_wrap);
        
        m_mesh_filename= Filename("click and drop file...");
        m_texture_filenames.push_back( Filename("click and drop file...") );
        
        m_center= gk::Point();
        m_camera= gk::Orbiter();
        
        // mesure du temps de dessin
        m_perf= gk::createTimeQuery();
        
        return 0;       // tout c'est bien passe, sinon renvoyer -1
    }
    
    int quit( ) 
    {
        delete m_mesh;
        // le shader program et les shaders sont detruits automatiquement par gKit.
        return 0;
    }
    
    int loadMesh( const Filename& filename )
    {
        //~ printf("load '%s'\n", filename.path);
        gk::Mesh *mesh= gk::MeshIO::readOBJ(filename.path);
        if(mesh == NULL)
            return -1;
        m_mesh_filename= filename;
        
        delete m_mesh;
        m_mesh= new gk::GLBasicMesh(GL_TRIANGLES, mesh->indices.size());
        m_mesh->createBuffer(0, mesh->positions);
        m_mesh->createBuffer(1, mesh->texcoords);
        m_mesh->createBuffer(2, mesh->normals);
        m_mesh->createIndexBuffer(mesh->indices);
        glBindVertexArray(0);
        
        m_center= mesh->box.center();
        m_camera.look(mesh->box);
        delete mesh;
        
        return 0;
    }
    
    int loadTexture( unsigned int id, const Filename& filename )
    {
        //~ printf("load '%s'\n", filename.path);
        gk::Image *image= gk::readImage(filename.path);
        if(image == NULL)
            return -1;
        
        m_texture_filenames[id]= filename;
        
        // reserve un slot pour charger une autre texture
        if(id +1 >= m_texture_filenames.size())
            m_texture_filenames.push_back( Filename("click and drop file...") );
        if(id +1 >= m_textures.size())
            m_textures.push_back( gk::GLTexture::null() );
        
        //~ if(m_textures[id] != gk::GLTexture::null())
            //~ delete m_textures[id];      // il est temps d'ecrire TextureManager...
        
        m_textures[id]= gk::createTexture2D(id, image);
        glBindTexture(m_textures[id]->target, 0);
        return 0;
    }
    
    int loadSampler( const int filter, const int wrap )
    {
        if(filter < 0 || filter > 4)
            return -1;
        
        const GLenum minfilters[]= { GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR /* aniso 8 */, GL_LINEAR_MIPMAP_LINEAR /* aniso 16 */};
        const GLenum magfilters[]= { GL_NEAREST, GL_LINEAR, GL_LINEAR,               GL_LINEAR /* aniso 8 */,               GL_LINEAR /* aniso 16 */ };
        const GLenum wraps[]= { GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER };
        
        m_sampler= gk::createSampler(wraps[wrap], minfilters[filter], magfilters[filter]);
        if(filter >= 3)
            glSamplerParameterf(m_sampler->name, GL_TEXTURE_MAX_ANISOTROPY_EXT, (filter -2) * 8);
        
        return 0;
    }
    
    
    int update( const int time, const int delta ) 
    {
        if(m_freeze == false)
            m_time= time % 16;
        return 1; 
    }
    
    int draw( )
    {
        if(key(SDLK_ESCAPE))
            // fermer l'application si l'utilisateur appuie sur ESCAPE
            closeWindow();

        static int wireframe= 0;
        if(m_widgets.isOnFocus() == false)
        {
            if(key('s'))
            {
                key('s')= 0;
                gk::writeFramebuffer("screenshot.png");
            }
        
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
            
            //~ if(key('c'))
            //~ {
                //~ key('c')= 0;
                //~ gk::writeOrbiter(m_camera, gk::IOFileSystem::changeType(m_filename, ".gkcamera"));
            //~ }
            
            //~ if(key('v'))
            //~ {
                //~ key('v')= 0;
                //~ m_camera= gk::readOrbiter(gk::IOFileSystem::changeType(m_filename, ".gkcamera"));
            //~ }
            
            if(key('r'))
            {
                key('r')= 0;
                gk::reloadPrograms();
            }
            
            if(key('w'))
            {
                key('w')= 0;
                wireframe= (wireframe + 1) % 2;
            }
        }
        
        if(wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // transformations
        gk::Transform model= gk::Translate( -gk::Vector(m_center) );
        gk::Transform view= m_camera.view();
        gk::Transform projection= m_camera.projection(windowWidth(), windowHeight());
        
        gk::Transform mv= view * model;
        gk::Transform mvp= projection * mv;
        
        // fixer la transformation viewport en fonction des dimensions de la fenetre
        glViewport(0, 0, windowWidth(), windowHeight());

        // effacer l'image
        if(m_program->errors)
            glClearColor(0.8, 0.4f, 0.0f, 1.0f);
        else
            glClearColor(0.2, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // mesurer le temps d'execution
        m_perf->begin();
        GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
        
        // draw
        glUseProgram(m_program->name);
        m_program->uniform("mvpMatrix")= mvp.matrix();
        m_program->uniform("mvMatrix")= mv.matrix();
        m_program->uniform("normalMatrix")= mv.normalMatrix();
        m_program->uniform("time")= m_time;

        // applique les valeurs des uniforms modifies dans l'interface...
        gk::tweakProgram(m_program);
        
        for(int i= 0; i < (int) m_textures.size(); i++)
        {
            m_program->sampler(Format("texture%d", i))= i;      // int obligatoire pour initialiser un sampler !!
            glBindSampler(i, m_sampler->name);
            
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(m_textures[i]->target, m_textures[i]->name);
        }
        
        m_mesh->draw();
        
        // mesurer le temps d'execution
        GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);     // nano secondes
        m_perf->end();
        GLuint64 cpu_time= (stop - start) / 1000;               // conversion en micro secondes
        GLuint64 gpu_time= m_perf->result64() / 1000;        // conversion en micro secondes
        
        // nettoyage, desactive les differents objets selectionnes
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        
        static nv::Rect load_panel;
        static bool load_unfold= 0;
        
        m_widgets.begin();
        // time
        m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
            m_widgets.doLabel(nv::Rect(), Format("cpu time % 6ldus", cpu_time));
            m_widgets.doLabel(nv::Rect(), Format("gpu time % 3ldms % 3ldus", gpu_time / 1000, gpu_time % 1000));
            
            // program state : ok / errors
            if(m_program->errors)
                m_widgets.doLabel(nv::Rect(), "compilation errors...");
            else
            {
                m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
                m_widgets.doLabel(nv::Rect(), "running...");
                m_widgets.doCheckButton(nv::Rect(), "freeze", &m_freeze);
                if(key(' '))
                {
                    key(' ')= 0;
                    m_freeze= !m_freeze;
                }
                
                if(m_freeze)
                    m_widgets.doHorizontalSlider(nv::Rect(0, 0, 400, 0), 0, 15, &m_time);
                
                m_widgets.endGroup();
            }
            
            gk::doTweaks(&m_widgets, m_program);
            
            if(m_widgets.beginPanel(load_panel, "assets", &load_unfold))
            {
                // mesh data
                m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
                    m_widgets.doLabel(nv::Rect(), "load mesh");
                    
                    Filename file= m_mesh_filename;
                    if(m_widgets.doLineEdit(nv::Rect(), file.path, sizeof(file.path)))
                        if(file != m_mesh_filename) loadMesh(file);
                    
                    if(m_mesh == NULL || m_mesh->buffers.size() == 0 || m_mesh->buffers[0] == gk::GLBuffer::null())
                        m_widgets.doLabel(nv::Rect(), "no data...");
                    else
                    {
                        m_widgets.doLabel(nv::Rect(), "position");
                        if(m_mesh->buffers.size() > 1 && m_mesh->buffers[1] != gk::GLBuffer::null())
                            m_widgets.doLabel(nv::Rect(), "texcoords");
                        if(m_mesh->buffers.size() > 2 && m_mesh->buffers[2] != gk::GLBuffer::null())
                            m_widgets.doLabel(nv::Rect(), "normal");
                    }
                m_widgets.endGroup();
                
                // texture data
                for(unsigned int i= 0; i < m_texture_filenames.size(); i++)
                {
                    m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
                        m_widgets.doLabel(nv::Rect(), Format("load texture %d", i));
                        
                        Filename file= m_texture_filenames[i];
                        if(m_widgets.doLineEdit(nv::Rect(), file.path, sizeof(file.path)))
                            if(file != m_texture_filenames[i]) loadTexture(i, file);
                    m_widgets.endGroup();
                }

                // sampler parameters
                m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
                {
                    m_widgets.doLabel(nv::Rect(), "texture sampler");
                    static const char *samplers[]= { "nearest", "linear", "mipmap", "aniso 8", "aniso 16" };
                    int changes= 0;
                    if(m_widgets.doComboBox(nv::Rect(), 5, samplers, &m_filter))
                        changes++;
                    static const char *wraps[]= { "repeat", "edge", "border" };
                    if(m_widgets.doComboBox(nv::Rect(), 3, wraps, &m_wrap))
                        changes++;
                    if(changes > 0)
                        loadSampler(m_filter, m_wrap);
                }
                m_widgets.endGroup();
                
                m_widgets.endPanel();
            }
        m_widgets.endGroup();
        m_widgets.end();
        
        // afficher le buffer de dessin
        present();
        return 1;       // continuer, dessiner une autre fois l'image, renvoyer 0 pour arreter l'application
    }
};


int main( int argc, char **argv )
{
    if(argc < 2)
    {
        printf("usage: %s shader.glsl\n", argv[0]);
        return 1;
    }
    
    TP app(argv[1]);
    app.run();
    
    return 0;
}

