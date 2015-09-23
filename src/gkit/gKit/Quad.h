
#ifndef _QUAD_H
#define _QUAD_H

#include "Geometry.h"


namespace gk {

struct Quad
{
    Point a, b, c, d;
    unsigned int id;
    
    Quad( ) : a(), b(), c(), d(), id(-1) {}
    Quad( const Point& _a, const Point& _b, const Point& _c, const Point& _d, const unsigned int _id= -1 ) : a(_a), b(_b), c(_c), d(_d), id(_id) {}
    
    Point point( const float u, const float v ) const
    {
        // calcule la position du sommet dans le repere local du quad abcd,  
        Point q=  (1.f-u) * (1.f-v) * a
                +      u  * (1.f-v) * b
                + (1.f-u) *      v  * d
                +      u  *      v  * c;
        
        return q;
    }
};

}       // namespace

#endif
