
#include <time.h>
#include <stdint.h>

#include "Vec.h"
#include "Geometry.h"
#include "Image.h"
#include "ImageIO.h"


//! utilitaire, mesure le temps ecoule entre les appels start() et stop() en micro secondes.
struct Timer
{
    timespec base;
    
    Timer( ) { start(); }       //!< demarre automatiquement le timer.
    ~Timer( ) {}
    
    void start( )       //!< redemarre le timer.
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &base);
    }
    
    unsigned int stop( )        //!< renvoie le temps ecoule depuis start() en micro secondes.
    {
        timespec b;
        clock_gettime(CLOCK_MONOTONIC_RAW, &b);
        
        uint64_t delay = uint64_t(b.tv_sec - base.tv_sec) * uint64_t(1000000) 
            + uint64_t(b.tv_nsec) / uint64_t(1000) - uint64_t(base.tv_nsec) / uint64_t(1000);
        return delay;
    }
};


//! renvoie 0 si les coordonnees sont en dehors de l'image.
gk::Color border( const gk::Image * const in, const int x, const int y )
{
    if(x < 0 || x >= in->width) return gk::Color();
    if(y < 0 || y >= in->height) return gk::Color();
    return gk::Color(in->pixel(x, y));
}


//! filtre l'image.
gk::Image *process( const gk::Image * const image, const gk::Image * const filter )
{
    // cree l'image resultat de la meme taille que l'imag en entree
    gk::Image *out= gk::createImage(image->width, image->height);
    
    // parcourir tous les pixels de l'image
    #pragma omp parallel for schedule(dynamic, 1)
    for(int y= 0; y < image->height; y++)
    for(int x= 0; x < image->width; x++)
    {
        // convolution du voisinage du pixel par le filtre
        gk::Color r;
        gk::Color norm;
        for(int yy= 0; yy < filter->height; yy++)
        for(int xx= 0; xx < filter->width; xx++)
        {
            // recupere un pixel 
            gk::Color pixel= border(image, x + xx - filter->width/2, y + yy - filter->height/2);
            // recupere la valeur du filtre
            gk::Color f= border(filter, xx, yy);
            
            // calculer
            r+= pixel * f;
            norm+= f;
        }
        
        // ecrire le resultat
        out->setPixel(x, y, r / norm);
    }
    
    return out;
}


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
    
    // filtrer et mesurer le temps
    Timer timer;
    gk::Image *out= process(image, filter);
    
    printf("process %ums\n", timer.stop() / 1000);
    
    // nettoyer
    delete image;
    delete filter;
    
    // enregistrer le resultat
    gk::ImageIO::writeImage("out.png", out);
    delete out;
    
    return 0;
}

