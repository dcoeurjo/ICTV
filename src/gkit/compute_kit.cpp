
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "ProgramManager.h"

#include "ProgramTweaks.h"
#include "GL/GLProgramUniforms.h"

#include "Image.h"
#include "ImageManager.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLSampler.h"
#include "GL/GLFramebuffer.h"

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
    
    std::vector<Filename> m_texture_filenames;
    
    gk::GLProgram *m_program;
    
    std::vector<gk::GLTexture *> m_textures;
    gk::GLSampler *m_sampler;
    int m_filter;
    int m_wrap;
    
    gk::GLFramebuffer *m_zoom;
    gk::GLFramebuffer *m_framebuffer;
    bool m_freeze;
    float m_time;
    
    gk::GLCounter *m_perf;
    
    nv::SdlContext m_widgets;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( const char *filename, char **first, char **last )
        :
        gk::App(),
        m_filename(filename),
        m_texture_filenames(first, last),
        m_program(NULL),
        m_textures(),
        m_sampler(NULL),
        m_filter(2), m_wrap(0),
        m_freeze(false), m_time(0)
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(4,3);     // version 4.3, utilisation compute shader
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(1280, 640, settings) < 0)
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
        if(m_program == gk::GLProgram::null())
            return -1;
        
        loadSampler(m_filter, m_wrap);
        
        // charge les images passees en option sur la ligne de commande
        for(unsigned int i= 0; i < m_texture_filenames.size(); i++)
            loadTexture(i, m_texture_filenames[i]);
        
        // ajoute une place pour charger une image supplementaire, si necessaire
        if(m_texture_filenames.empty())
            m_texture_filenames.push_back( Filename("click and drop file...") );
        
        // cree les framebuffers pour afficher le resultat et zoomer dessus
        m_zoom= gk::createFramebuffer(GL_DRAW_FRAMEBUFFER, windowWidth(), windowHeight(), gk::GLFramebuffer::COLOR0_BIT);
        m_framebuffer= gk::createFramebuffer(GL_DRAW_FRAMEBUFFER, windowWidth(), windowHeight(), gk::GLFramebuffer::COLOR0_BIT);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        // mesure du temps de dessin
        m_perf= gk::createTimer();
        
        return 0;       // tout c'est bien passe, sinon renvoyer -1
    }
    
    int quit( ) 
    {
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
        
        const GLenum minfilters[]= 
        { 
            GL_NEAREST, 
            GL_LINEAR, 
            GL_LINEAR_MIPMAP_LINEAR, 
            GL_LINEAR_MIPMAP_LINEAR /* aniso 8 */, 
            GL_LINEAR_MIPMAP_LINEAR /* aniso 16 */
        };
        const GLenum magfilters[]= 
        { 
            GL_NEAREST, 
            GL_LINEAR, 
            GL_LINEAR,               
            GL_LINEAR /* aniso 8 */,               
            GL_LINEAR /* aniso 16 */
        };
        const GLenum wraps[]= 
        { 
            GL_REPEAT, 
            GL_CLAMP_TO_EDGE, 
            GL_CLAMP_TO_BORDER 
        };
        
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

        if(m_widgets.isOnFocus() == false)
        {
            if(key('s'))
            {
                key('s')= 0;
                
                static int count= 1;
                gk::writeFramebuffer( Format("screenshot%02d.png", count++) );
            }
            
            if(key('r'))
            {
                key('r')= 0;
                gk::reloadPrograms();
            }
        }
        
        // fixer la transformation viewport en fonction des dimensions de la fenetre
        glViewport(0, 0, windowWidth(), windowHeight());

        // effacer l'image
        if(m_program->errors)
            glClearColor(0.8, 0.4f, 0.0f, 1.0f);        // changer la couleur de fond si le shader n'est pas correctement compile
        else
            glClearColor(0.2, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // preparation
        glUseProgram(m_program->name);
        m_program->uniform("time")= m_time;
        
        // parametrer les textures en entree.
        for(unsigned int i= 0; i < m_textures.size(); i++)
        {
            m_program->sampler(Format("image%d", i))= int(i);      // int obligatoire pour initialiser un sampler !!
            glBindSampler(i, m_sampler->name);
            
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(m_textures[i]->target, m_textures[i]->name);
        }
        
        // parameter la sortie : associe la texture du framebuffer a la sortie 0 du compute shader (cf shader, layout(binding = 0))
        gk::GLTexture *output= m_framebuffer->texture(gk::GLFramebuffer::COLOR0);
        glBindImageTexture(0, output->name, 0, GL_FALSE, 0, GL_WRITE_ONLY, output->format.internal);
        
        // applique les valeurs des uniforms modifies par l'interface...
        gk::tweakProgram(m_program);
        
        // execute le traitement, sauf si freeze est coche
        if(m_freeze == false)
        {
            // mesure le temps d'execution du shader
            m_perf->start();
            
            // recupere la taille des groupes de threads.
            GLint group_size[3];
            glGetProgramiv(m_program->name, GL_COMPUTE_WORK_GROUP_SIZE, group_size);
            
            // calcule le nombre de groupes de threads a "lancer".
            //! \todo calculer proprement le nombre de groupes si size % group_size != 0
            
            // utilise la taille de l'image 0 (si disponible) pour executer les shaders
            if(m_textures.size() != 0 && m_textures[0] != NULL && m_textures[0] != gk::GLTexture::null())
                glDispatchCompute(m_textures[0]->width / group_size[0], m_textures[0]->height / group_size[1], 1);        
            
            m_perf->stop();
            
            // eviter de recalculer constament l'image resultat, si le calcul a pris beaucoup de temps  > 100ms ?
            if(m_perf->gpu64() / 1000u > 100000u)
            {
                printf("processing time %lums... auto freeze.\n", m_perf->gpu64()/1000000u);
                m_freeze= true;
            }
        }
        
        // copier le resultat dans la fenetre
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer->name);
        GLenum buffer= GL_COLOR_ATTACHMENT0;
        glReadBuffer(buffer);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, output->width, output->height,
            0, 0, m_framebuffer->width, m_framebuffer->height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);       
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        
        // framebuffer zoom, sur click bouton droit
        {
            int x, y;
            unsigned int buttons;
            buttons= SDL_GetMouseState(&x, &y);
            if(buttons & SDL_BUTTON(3))
            {
                y= windowHeight() - 1 - y;
                
                int size= 64;
                int scale= 4;
                
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_zoom->name);
                GLenum buffer= GL_COLOR_ATTACHMENT0;
                glDrawBuffer(buffer);
                
                glBlitFramebuffer(
                    x - size, y - size, x + size, y + size, 
                    0, 0, size * 2, size * 2, 
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
                
                glBindFramebuffer(GL_READ_FRAMEBUFFER, m_zoom->name);
                glReadBuffer(buffer);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                
                glBlitFramebuffer(
                    0, 0, size * 2, size * 2,
                    x - size * scale, y - size * scale, x + size * scale, y + size * scale, 
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
                
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            }        
        }
    
        // nettoyage, desactive les differents objets selectionnes
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        
        // widgets
        static nv::Rect load_panel;
        static bool load_unfold= 0;
        
        m_widgets.begin();
        
        // time
        m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
        m_widgets.doLabel(nv::Rect(), m_perf->summary("compute").c_str());
        
        // program state : ok / errors
        if(m_program->errors)
            m_widgets.doLabel(nv::Rect(), "compilation errors...");
        
        else
        {
            m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
            m_widgets.doLabel(nv::Rect(), "running...");
            m_widgets.doCheckButton(nv::Rect(), "freeze", &m_freeze);
            
            if(m_widgets.isOnFocus() == false && key(' '))
            {
                key(' ')= 0;
                m_freeze= !m_freeze;    // bascule le mode pause
            }
        }
        m_widgets.endGroup();
        
        gk::doTweaks(m_widgets, m_program);
        
        if(m_widgets.beginPanel(load_panel, "assets", &load_unfold))
        {
            // texture data
            for(unsigned int i= 0; i < m_texture_filenames.size(); i++)
            {
                m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
                m_widgets.doLabel(nv::Rect(), Format("load image %d", i));
                
                Filename file= m_texture_filenames[i];
                if(m_widgets.doLineEdit(nv::Rect(), file.path, sizeof(file.path)))
                {
                    if(file != m_texture_filenames[i]) 
                        loadTexture(i, file);
                }
                m_widgets.endGroup();
            }

            // sampler parameters
            m_widgets.beginGroup(nv::GroupFlags_GrowLeftFromTop);
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
    
    TP app(argv[1], &argv[2], &argv[argc]);
    app.run();
    
    return 0;
}

