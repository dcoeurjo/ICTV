
#include "Geometry.h"
#include "Transform.h"

#include "Image.h"
#include "ImageIO.h"

struct Sphere
{
    gk::Point center;
    float radius;
    
    Sphere( const gk::Point& c, const float r) : center(c), radius(r) {}
    
    gk::Point eval( const float theta, const float phi ) const
    {
        return center + gk::Point(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta)) * radius;
    }
};

struct Patch
{
    float thetaMin, thetaMax;
    float phiMin, phiMax;
    
    Patch( ) : thetaMin(0.f), thetaMax(0.f), phiMin(0.f), phiMax(0.f) {}
    
    Patch( const float tmin, const float tmax, const float pmin, const float pmax) : thetaMin(tmin), thetaMax(tmax), phiMin(pmin), phiMax(pmax) {}
};


struct Reyes
{
    gk::Transform mvp;
    gk::Transform viewport;
    gk::Image *image;
    gk::Image *zbuffer;
    gk::Image *n;
    int mp;

    Reyes( gk::Image *_image ) 
        :
        image(_image)
    {
        assert(image != NULL);
        zbuffer= (new gk::Image())->create(image->width, image->height, 1, gk::Image::FLOAT);
        n= (new gk::Image())->create(image->width, image->height, 1, gk::Image::FLOAT);
    }
    
    bool culled( const gk::Point& a, const gk::Point& b, const gk::Point& c, const gk::Point& d )
    {
        // projette les 4 points
        gk::HPoint ha; mvp(a, ha); 
        gk::HPoint hb; mvp(b, hb);
        gk::HPoint hc; mvp(c, hc);
        gk::HPoint hd; mvp(d, hd);
        return (ha.isCulled() && hb.isCulled() && hc.isCulled() && hd.isCulled());
    }
    
    bool stop( const gk::Point& a, const gk::Point& b, const gk::Point& c, const gk::Point& d )
    {
        // projette les 4 points
        gk::HPoint ha; mvp(a, ha); 
        if(ha.isCulled()) return false;
        gk::Point pa= viewport(ha.project());
        
        gk::HPoint hb; mvp(b, hb);
        if(hb.isCulled()) return false;
        gk::Point pb= viewport(hb.project());
        
        gk::HPoint hc; mvp(c, hc);
        if(hc.isCulled()) return false;
        gk::Point pc= viewport(hc.project());
        
        gk::HPoint hd; mvp(d, hd);
        if(hd.isCulled()) return false;
        gk::Point pd= viewport(hd.project());
        
        // verifie qu'ils se projettent sur le meme pixel que a (par exemple)
        if(gk::Vector(pa, pb).Length() > 1.f)
            return false;
        if(gk::Vector(pa, pc).Length() > 1.f)
            return false;
        if(gk::Vector(pa, pd).Length() > 1.f)
            return false;
        
        return true;
    }
    
    void draw( const gk::Point& a )
    {
        gk::HPoint ha; mvp(a, ha); 
        if(ha.isCulled()) return;
        gk::Point pa= viewport(ha.project());
        
        // colorie le pixel en rouge
        image->setPixel(pa.x, pa.y, gk::VecColor(pa.z, 0, 0));
        n->setPixel(pa.x, pa.y, gk::Color(n->pixel(pa.x, pa.y)) + gk::Color(1.f) );
        zbuffer->setPixel(pa.x, pa.y, gk::VecColor(pa.z));
    }
    
    void subdivide( const Sphere& sphere, const Patch& patch, const int depth= 0 )
    {
        gk::Point a, b, c, d;
        a= sphere.eval(patch.thetaMin, patch.phiMin);
        b= sphere.eval(patch.thetaMax, patch.phiMin);
        c= sphere.eval(patch.thetaMax, patch.phiMax);
        d= sphere.eval(patch.thetaMin, patch.phiMax);
        
        if(culled(a, b, c, d))
            return;
        
        mp++;
        if(stop(a, b, c, d) || depth > 10)
        {
            draw(a);
        }
        else
        {
            // subdivision du patch
            float theta= (patch.thetaMin + patch.thetaMax) *0.5f;
            float phi= (patch.phiMin + patch.phiMax) *0.5f;
            
            subdivide(sphere, Patch(patch.thetaMin, theta, patch.phiMin, phi), depth+1);
            subdivide(sphere, Patch(theta, patch.thetaMax, patch.phiMin, phi), depth+1);
            subdivide(sphere, Patch(theta, patch.thetaMax, phi, patch.phiMax), depth+1);
            subdivide(sphere, Patch(patch.thetaMin, theta, phi, patch.phiMax), depth+1);
        }
    }
};


int main( )
{
    gk::Image *image= gk::createImage(512, 512);
    
    gk::Transform model;
    gk::Transform view;
    gk::Transform projection;
    gk::Transform viewport= gk::Viewport(image->width -1, image->height -1);
    
    Reyes reyes(image);
    reyes.mvp= projection * view * model;
    reyes.viewport= viewport;
    reyes.mp= 0;

    Sphere sphere(gk::Point(0,0,0), 0.5f);
    reyes.subdivide(sphere, Patch(0.5f*M_PI, M_PI, 0.f, M_PI));
    reyes.subdivide(sphere, Patch(0.f, 0.5f*M_PI, 0.f, M_PI));
    reyes.subdivide(sphere, Patch(0.5f*M_PI, M_PI, M_PI, 2.f*M_PI));
    reyes.subdivide(sphere, Patch(0.f, 0.5f*M_PI, M_PI, 2.f*M_PI));
    
    printf("mp %d\n", reyes.mp);
    
    gk::ImageIO::writeImage("render.bmp", image);
    gk::ImageIO::writeImage("rendermp.hdr", reyes.n);
    gk::ImageIO::writeImage("renderz.hdr", reyes.zbuffer);
    delete image;
    return 0;
}
