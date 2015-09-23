
#include "Vec.h"
#include "Geometry.h"
#include "Triangle.h"
#include "Image.h"
#include "ImageIO.h"

struct edge
{
    float a, b, c;
    
    edge( const gk::Point& pa, const gk::Point& pb )
    {
        a= -(pb.y - pa.y);
        b= pb.x - pa.x;
        c= -pa.x * a - pa.y * b;
    }
    
    float operator() ( const gk::Point& p ) { return a*p.x + b*p.y + c; }
};

int main( )
{
    gk::Image *image= gk::createImage(512, 512);
    
    gk::Triangle triangle( gk::Point(100, 100), gk::Point(300, 150), gk::Point(150, 300) );
    gk::BBox box( triangle.bbox() );
    
    edge ab(triangle.a, triangle.b);
    edge bc(triangle.b, triangle.c);
    edge ca(triangle.c, triangle.a);
    
    for(int y= box.pMin.y; y <= box.pMax.y; y++)
    for(int x= box.pMin.x; x <= box.pMax.x; x++)
    {
        const gk::Point p(x, y);
        bool inside= (ab(p) > 0.f) && (bc(p) > 0.f) && (ca(p) > 0.f);
        if(inside)
            image->setPixel(x, y, gk::VecColor(1, 0, 0));
    }
    
    
    gk::ImageIO::writeImage("out.png", image);
    delete image;
    
    return 0;
}
