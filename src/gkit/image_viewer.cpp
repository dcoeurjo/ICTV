
#include "App.h"
#include "Widgets/nvSDLContext.h"

#include "Transform.h"

#include "ProgramManager.h"
#include "GL/GLBuffer.h"
#include "GL/GLSampler.h"
#include "GL/GLTexture.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLBasicMesh.h"

#include "Image.h"
#include "ImageArray.h"
#include "ImageIO.h"
#include "ImageManager.h"


class TP : public gk::App
{
    const char **m_filenames;
    int m_count;
    
    gk::GLProgram *m_program;
    gk::GLBasicMesh *m_quad;
    
    gk::GLProgram *m_color_program;
    gk::GLBasicMesh *m_plot;
    
    gk::ImageArray *m_images;
    gk::Image *m_image;
    int m_image_index;
    
    gk::GLTexture *m_texture;
    gk::GLTexture *m_colors;
    gk::GLSampler *m_sampler;
    
    gk::GLFramebuffer *m_zoom;
    
    float m_compression;
    float m_saturation_max;
    float m_saturation_delta;
    
    float m_saturation;
    float m_saturation_fine;
    int m_mode;
    bool m_show_histogram;
    
    bool m_red;
    bool m_green;
    bool m_blue;
    bool m_alpha;
    
    float m_ymin;
    float m_ymax;
    
    float m_bins[256];
    int m_bins_n;
    float m_bins_min;
    float m_bins_max;
    
    nv::SdlContext m_widgets;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( const char **_filenames, const int n )
        :
        gk::App(),
        m_filenames(_filenames), m_count(n)
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(3,3);     // version 3.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(512, 512, settings) < 0)
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
    
    int init( )
    {
        // compilation simplifiee
        m_program= gk::createProgram("tonemap.glsl");
        if(m_program == gk::GLProgram::null())
            return -1;
        
        m_color_program= gk::createProgram("core.glsl");
        if(m_color_program == gk::GLProgram::null())
            return -1;
        
        m_quad= new gk::GLBasicMesh(GL_TRIANGLE_STRIP, 4);
        
        // image sur texture 0
        m_images= new gk::ImageArray();
        for(int i= 0; i < m_count; i++)
        {
            std::string file= gk::searchImage(m_filenames[i]);
            if(gk::IOFileSystem::isFilename(file) == false)
                continue;
            
            m_images->push_back( gk::readImage(file) );
        }
        
        if(m_images->images.size() == 0)
            return -1;
        if(m_images->images.size() == 1)
            m_texture= gk::createTexture2D(gk::GLTexture::UNIT0, m_images->images[0], gk::TextureRGB16F);     // 3 float par pixel
        else
            m_texture= gk::createTexture2DArray(gk::GLTexture::UNIT0, m_images, gk::TextureRGB16F);     // 3 float par pixel

        // modifier le titre et la taille de la fenetre
        m_image= m_images->images[0];
        m_image_index= 0;
        resizeWindow(m_image->width, m_image->height);
        SDL_SetWindowTitle(m_window, m_filenames[0]);

        
        //~ m_image= gk::readImage(m_filename.c_str());
        //~ if(m_image == NULL)
            //~ return -1;
        //~ m_texture= gk::createTexture2D(gk::GLTexture::UNIT0, m_image, gk::TextureRGB16F);     // 3 float par pixel
        
        //~ // modifier le titre et la taille de la fenetre
        //~ resizeWindow(m_image->width, m_image->height);
        //~ SDL_SetWindowTitle(m_window, m_filename.c_str());
        
        // fausses couleurs sur texture 1
        //~ gk::Image *colors= gk::ImageIO::readImage("images/false_colors.png");
        gk::Image *colors= gk::readImage("images/false_colors.png");
        if(colors == NULL)
            return -1;
        m_colors= gk::createTexture2D(gk::GLTexture::UNIT1, colors);
        //~ delete colors;
        
        m_sampler= gk::createLinearSampler(GL_CLAMP_TO_BORDER);
        
        m_zoom= gk::createFramebuffer(GL_DRAW_FRAMEBUFFER, windowWidth(), windowHeight(), gk::GLFramebuffer::COLOR0_BIT);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        // estimation des parametres initiaux
        m_show_histogram= true;
        
        // . valeurs par defaut
        m_compression= 2.f;
        m_saturation= 100.f;
        m_saturation_max= 100.f;
        m_saturation_delta= 10.f;
        m_saturation_fine= 0.f;
        m_mode= 0.f;
        m_red= true;
        m_green= true;
        m_blue= true;
        m_alpha= true;
        
        m_bins_n= 256;
        for(int i= 0; i< m_bins_n; i++)
            m_bins[i]= 0.f;
        
        // . trouver les extremes
        float ymin= HUGE_VAL;
        float ymax= -HUGE_VAL;
        float sum= 0.f;
        for(int y= 0; y < m_image->height; y++)
            for(int x= 0; x < m_image->width; x++)
            {
                const gk::VecColor &pixel= m_image->pixel(x, y);
                const float y= .3f * pixel.r + .59f * pixel.g + .11f * pixel.b;
                
                if(y < ymin)
                    ymin= y;
                if(y > ymax)
                    ymax= y;
                
                sum= sum + y;
            }
        
        m_ymin= ymin;
        m_ymax= ymax;
            
        printf("min %f < %f < max %f\n", 
            ymin, sum / (m_image->width * m_image->height), ymax);
        
        // . construit l'histogramme
        const float bin_scale= (float) m_bins_n / (ymax - ymin);
        for(int y= 0; y < m_image->height; y++)
            for(int x= 0; x < m_image->width; x++)
            {
                const gk::VecColor &pixel= m_image->pixel(x, y);
                const float y= .3f * pixel.r + .59f * pixel.g + .11f * pixel.b;
                
                int k= (y - ymin) * bin_scale;
                if(k < 0)
                    k= 0;
                if(k >= m_bins_n)
                    k= m_bins_n -1;
                
                m_bins[k]+= 1.f;
            }
        
        m_bins_min= m_image->width * m_image->height;
        m_bins_max= 0;
        float bins_sum= 0.f;
        const float bin_normalize= 1.f / (m_image->width * m_image->height);
        for(int i= 0; i < m_bins_n; i++)
        {
            m_bins[i]= m_bins[i] * bin_normalize;
            
            if(m_bins[i] < m_bins_min)
                m_bins_min= m_bins[i];
            if(m_bins[i] > m_bins_max)
                m_bins_max= m_bins[i];

            bins_sum+= m_bins[i];
        }
        
        // affichage de l'histogramme
    #if 0
        const float w= windowWidth();
        const float h= windowHeight();
        
        std::vector<gk::Vec3> plot;
        for(int i= 0; i < m_bins_n; i++)
        {
            plot.push_back( gk::Vec3(10.f + (float) i / (float) m_bins_n * (float) (w / 2), 10.f, 0.f) );
            plot.push_back( gk::Vec3(10.f + (float) i / (float) m_bins_n * (float) (w / 2), 10.f + m_bins[i] / m_bins_max * (float) h / 2.f, 0.f) );
        }
        
        m_plot= new gk::GLBasicMesh(GL_LINES, plot.size());
        m_plot->createBuffer(m_color_program->attribute("position"), plot);
    #endif
        
        // . reglage de la dynamique de l'histogramme
        m_bins_max= bins_sum / m_bins_n;
        
        // . reglage des plages des sliders
        m_saturation= ymax;
        m_saturation_max= ymax;
        m_saturation_delta= ymax / 10.f;
        m_saturation_fine= 0.f;
        m_mode= 0.f;
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
    
    // a redefinir pour utiliser les widgets.
    void processTextEvent( const char *string )
    {
        m_widgets.processTextEvent(string);
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
        
        // 
        bool resize= false;
        if(key('n'))
        {
            key('n')= 0;
            resize= true;       // redimensionne la fenetre aux dimensions de l'image
        }
        
        // selectionne une nouvelle image, si possible
        if(key(SDLK_LEFT))
        {
            key(SDLK_LEFT)= 0;
            m_image_index= (m_image_index + m_count -1) % m_count;
            m_image= m_images->images[m_image_index];
            SDL_SetWindowTitle(m_window, m_filenames[m_image_index]);
        }
        if(key(SDLK_RIGHT))
        {
            key(SDLK_RIGHT)= 0;
            m_image_index= (m_image_index + m_count +1) % m_count;
            m_image= m_images->images[m_image_index];
            SDL_SetWindowTitle(m_window, m_filenames[m_image_index]);
        }
        
        // redimensionne la fenetre, en conservant les proportions de l'image
        if(m_image != NULL)
        {
            int w= windowWidth();
            int h= windowHeight();
            if(resize)
            {
                resize= false;
                w= m_image->width;
                h= m_image->height;
            }
            
            float ratio= float(m_image->width) / float(m_image->height);
            w= h * ratio;
            resizeWindow(w, h);
        }
        
        glViewport(0, 0, windowWidth(), windowHeight());
        m_widgets.reshape(windowWidth(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // dessiner l'image compressee
        glUseProgram(m_program->name);

        glActiveTexture(GL_TEXTURE0);
        //~ glBindTexture(GL_TEXTURE_2D, m_texture->name);
        glBindTexture(m_texture->target, m_texture->name);
        m_program->uniform("image_index")= m_image_index;
        m_program->sampler("image")= 0;
        m_program->sampler("images")= 0;
        glBindSampler(0, m_sampler->name);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_colors->name);
        m_program->sampler("colors")= 1;
        glBindSampler(1, m_sampler->name);

        m_program->uniform("compression")= m_compression;
        m_program->uniform("saturation")= m_saturation + m_saturation_fine;
        m_program->uniform("heat")= (m_mode != 0) ? 1.f : 0.f;
        
        m_program->uniform("channels")= gk::Vec4(m_red ? 1.f : 0.f, m_green ? 1.f : 0.f, m_blue ? 1.f : 0.f, m_alpha ? 1.f : 0.f);
        
        m_quad->draw();
        
        // framebuffer zoom
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
        
    #if 0
        // dessiner l'histogramme
        if(m_show_histogram)
        {
            glUseProgram(m_color_program->name);
            
            m_color_program->uniform("mvpMatrix")= gk::Orthographic(0, windowWidth(), 0, windowHeight(), -1.f, 1.f).matrix();
            m_color_program->uniform("color")= gk::VecColor(1, 0, 0, 1);
            
            glLineWidth(2.f);
            m_plot->draw();
        }
    #endif
        
        // nettoyage
        glUseProgram(0);
        glBindVertexArray(0);
        
        static bool do_screenshot= false;
        if(do_screenshot || key('c'))
        {
            key('c')= 0;
            do_screenshot= false;
            
            // enregistre l'image opengl
            gk::writeFramebuffer(gk::IOFileSystem::changeType(m_filenames[m_image_index], "_tone.png"));
        }
        
        if(key('e'))    // enregistre l'image en exr
        {
            key('e')= 0;
            gk::ImageIO::writeImage(gk::IOFileSystem::changeType(m_filenames[m_image_index], ".exr"), m_image);
        }
        
        if(key('h'))    // enregistre l'image en hdr / rgbe
        {
            key('h')= 0;
            gk::ImageIO::writeImage(gk::IOFileSystem::changeType(m_filenames[m_image_index], ".hdr"), m_image);
        }
        
        
        // widgets
        m_widgets.begin();
            m_widgets.beginGroup(nv::GroupFlags_GrowDownFromLeft);
                m_widgets.beginGroup(nv::GroupFlags_GrowRightFromTop);
                    m_widgets.doLabel(nv::Rect(), "saturation");
                    {
                        char tmp[1024];
                        sprintf(tmp, "%.6f", m_saturation + m_saturation_fine);
                        
                        int edit= 0;
                        if(m_widgets.doLineEdit(nv::Rect(), tmp, sizeof(tmp), &edit))
                        {
                            float value= m_saturation + m_saturation_fine;
                            if(sscanf(tmp, "%f", &value) == 1)
                            {
                                m_saturation= value;
                                m_saturation_fine= 0.f;
                            }
                        }
                    }
                    
                    if(m_widgets.doHorizontalSlider(nv::Rect(0,0, 200, 0), 0.f, m_saturation_max * 4.f + 10.f, &m_saturation))
                        m_saturation_fine= 0.f;
                    
                    m_widgets.doHorizontalSlider(nv::Rect(), 0.f, m_saturation_delta, &m_saturation_fine);
                    //~ m_widgets.doCheckButton(nv::Rect(), "histogram", &m_show_histogram);
                m_widgets.endGroup();
                    
                m_widgets.beginGroup(nv::GroupFlags_GrowRightFromTop);
                    m_widgets.doRadioButton(1, nv::Rect(), "heat", &m_mode);
                    m_widgets.doRadioButton(0, nv::Rect(), "compression", &m_mode);
                    
                    if(m_mode == 0)
                    {
                        char tmp[1024];
                        sprintf(tmp, "%.2f", m_compression);
                        
                        int edit= 0;
                        if(m_widgets.doLineEdit(nv::Rect(), tmp, sizeof(tmp), &edit))
                        {
                            float value= m_compression;
                            if(sscanf(tmp, "%f", &value) == 1)
                                m_compression= value;
                        }
                        
                        m_widgets.doHorizontalSlider(nv::Rect(0,0, 200, 0), 0.f, 10.f, &m_compression);
                    }
                m_widgets.endGroup();

                m_widgets.beginGroup(nv::GroupFlags_GrowRightFromTop);
                    if(m_widgets.doButton(nv::Rect(), "save"))
                        do_screenshot= true;
                    m_widgets.doCheckButton(nv::Rect(), "R", &m_red);
                    m_widgets.doCheckButton(nv::Rect(), "G", &m_green);
                    m_widgets.doCheckButton(nv::Rect(), "B", &m_blue);
                    m_widgets.doCheckButton(nv::Rect(), "A", &m_alpha);
                m_widgets.endGroup();
            m_widgets.endGroup();
        m_widgets.end();
        
        present();
        return 1;
    }
};

int main( int argc, const char **argv )
{
    if(argc < 2)
    {
        printf("usage: %s .bmp .png .jpg .tga .exr .hdr\n\n", argv[0]);
        return 1;
    }
    
    TP app(argv +1, argc -1);
    app.run();
    
    return 0;
}

