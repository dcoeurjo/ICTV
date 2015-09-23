
#include "App.h"

#include "Vec.h"
#include "Geometry.h"

#include "ProgramManager.h"
#include "Image.h"
#include "ImageIO.h"
#include "ImageManager.h"

#include "GL/GLTexture.h"
#include "GL/GLQuery.h"


class TP : public gk::App
{
    gk::Image *m_image;
    gk::Image *m_filter;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( gk::Image *image, gk::Image *filter )
        :
        gk::App(),
        m_image(image), m_filter(filter)
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(4,3);             // version 4.3, utilisation compute shader
        settings.setGLCoreProfile();            // core profile
        settings.setGLDebugContext();           // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(512, 512, settings) < 0)
            closeWindow();
    }
    
    int init()
    {
        // charger le shader
        gk::GLProgram *program= gk::createProgram("filter.glsl");
        //~ gk::GLProgram *program= gk::createProgram("filter5.glsl");
        if(program == gk::GLProgram::null())
            return -1;  // erreur a la creation du shader.
        
        // creer les textures d'entree et de sortie
        gk::GLTexture *image= gk::createTexture2D(gk::GLTexture::UNIT0, m_image);
        gk::GLTexture *filter= gk::createTexture2D(gk::GLTexture::UNIT0, m_filter);
        gk::GLTexture *output= gk::createTexture2D(gk::GLTexture::UNIT0, image->width, image->height);
        
        // selectionner le shader program a utiliser
        glUseProgram(program->name);
        
        // test
        //~ program->uniform("weights[0][0]").at(10)= 2.f;      // superbe !!
        //~ program->uniform("weights[1][0]").at(1)= 2.f;
        
        // entree du shader : une texture 
        glActiveTexture(GL_TEXTURE0);                   // selectionner une entree
        glBindTexture(image->target, image->name);      // selectionner la texture
        program->sampler("image0")= int(0);             // parametrer le shader: int obligatoire pour initialiser un sampler !!
        
        // entree du shader : un filtre (une texture aussi)
        glActiveTexture(GL_TEXTURE1);                   // selectionner une autre entree
        glBindTexture(filter->target, filter->name);    // selectionner la texture
        program->sampler("image1")= int(1);             // int obligatoire pour initialiser un sampler !!
        
        // sortie du shader : une image (cad une texture utilisee en sortie d'un shader) 
        glBindImageTexture(0, output->name, 0, GL_FALSE, 0, GL_WRITE_ONLY, output->format.internal);
        
        // recuperer le nombre de threads par groupe declare par le shader
        GLint group_size[3]; glGetProgramiv(program->name, GL_COMPUTE_WORK_GROUP_SIZE, group_size);
        
        // mesure le temps d'execution du shader
        gk::GLCounter *perf= gk::createTimer();
        
    #if 1
        perf->start();
        {
            // calcule le nombre de groupes de threads a "lancer". 
            // todo : calculer correctement l'arrondi... lorsque size % group_size != 0
            glDispatchCompute(image->width / group_size[0], image->height / group_size[1], 1);
        }
        perf->stop();
        
        // recuperer le contenu de la texture / image resultat
        gk::Image *resultat= output->image(gk::GLTexture::UNIT0);
        
        // ecrire l'image resultat
        gk::writeImage("out.png", resultat);
        
        // afficher le temps d'execution
        perf->sync();
        printf("%s\n", perf->summary("filter").c_str());
        
    #else
        // evolution du temps de calcul en fonction de la taille de l'image
        for(;;)
        {
            perf->start();
            static int frame= 0;
            frame++;
            
            int gx= std::min(image->width / group_size[0], frame);
            int gy= std::min(image->height / group_size[1], frame);
            glDispatchCompute(gx, gy, 1);
            perf->stop();
        
            // afficher le temps d'execution
            perf->sync();
            printf("%d %d %f\n", gx * group_size[0], gy * group_size[1], (float) perf->gpu64() / float(1000000));
            
            if(gx == image->width / group_size[0] && gy == image->height / group_size[1])
                break;
        }
    #endif
        
        return 0;
        
        // todo nettoyage...
    }
    
    int draw( )
    {
        // on affiche rien, quitter tout de suite.
        return 0;
    }
    
    int quit( )
    {
        // nettoyage, dans cet exemple c'est deja fait a la fin de init()...
        return 0;
    }
};


int main( int argc, char **argv )
{
    if(argc < 2)
    {
        printf("usage: %s image.[png|bmp|tga|jpg|exr|hdr]\n", argv[0]);
        return 1;
    }
    
    // charger l'image a filtrer
    gk::Image *image= gk::ImageIO::readImage(argv[1]);
    if(image == NULL)
        return 1;
    
    // charger le filtre
    gk::Image *filter= gk::ImageIO::readImage("filter.png");
    if(filter == NULL)
        return 1;
    
    // creer un contexte opengl, 
    TP app(image, filter);
    app.run();
    
    delete image;
    delete filter;
    return 0;
}
