
#ifndef _BITRIANGLE_H
#define _BITRIANGLE_H

namespace gk {
    
#include "Geometry.h"

//! representation d'un couple de triangles abc, acd adjacents sur l'arete ac
//! encodage "compact" index buffer : cf http://fgiesen.wordpress.com/2013/12/14/simple-lossless-index-buffer-compression/
//! tests minimaux sur les aretes : cf http://www.cs.utah.edu/~aek/research/triangle.pdf et http://realtimecollisiondetection.net/blog/?p=13

struct Bitriangle
{
    Point a, b, c, d;
    unsigned int id;    

    Bitriangle( ) : a(), b(), c(), d(), id(-1) {}
    Bitriangle( const Point& _a, const Point& _b, const Point& _c, const Point& _d, const unsigned int _id= -1 ) : a(_a),  b(_b), c(_c), d(_d), id(_id) {}
    
    bool Intersect( const Ray &ray, const float htmax, 
        float &rt, float &ru, float&rv ) const
    {
    abc:
        /* begin calculating determinant - also used to calculate U parameter */
        Vector ac(a, c);
        const Vector pvec= Cross(ray.d, ac);
        
        /* if determinant is near zero, ray lies in plane of triangle */
        Vector ab(a, b);
        const float det= Dot(ab, pvec);
        if (det > -EPSILON && det < EPSILON)
            //~ return false;
            goto acd;
        
        const float inv_det= 1.0f / det;
        
        /* calculate distance from vert0 to ray origin */
        const Vector tvec(a, ray.o);
        
        /* calculate U parameter and test bounds */
        const float u= Dot(tvec, pvec) * inv_det;
        //~ if(u < 0.0f || u > 1.0f)
            //~ return false;
        //! \todo reprendre les tests sur les 2 triangles pour detecter quel triangle il faut tester.
        if(u < 0.f)
            goto acd;
        
        /* prepare to test V parameter */
        const Vector qvec= Cross(tvec, ab);
        
        /* calculate V parameter and test bounds */
        const float v= Dot(ray.d, qvec) * inv_det;
        if(v < 0.0f || u + v > 1.0f)
            //~ return false;
            goto acd;
        
        /* calculate t, ray intersects triangle */
        rt= Dot(ac, qvec) * inv_det;
        ru= u;
        rv= v;
        
        // ne renvoie vrai que si l'intersection est valide (comprise entre tmin et tmax du rayon)
        //~ return (rt < htmax && rt > RAY_EPSILON);
        
    acd:
        /* begin calculating determinant - also used to calculate U parameter */
        //~ Vector ac(a, c);
        //~ const Vector pvec= Cross(ray.d, ac);
        
        /* if determinant is near zero, ray lies in plane of triangle */
        Vector cb(c, b);
        const float bdet= - Dot(cb, pvec);
        if (bdet > -EPSILON && bdet < EPSILON)
            //~ return false;
            return (rt < htmax && rt > RAY_EPSILON);
        
        const float inv_bdet= 1.0f / bdet;
        
        /* calculate distance from vert0 to ray origin */
        //~ const Vector tvec(a, ray.o);
        
        /* calculate U parameter and test bounds */
        const float bu= - Dot(tvec, pvec) * inv_bdet;
        if(bu < 0.0f || bu > 1.0f)
            //~ return false;
            return (rt < htmax && rt > RAY_EPSILON);
        
        /* prepare to test V parameter */
        const Vector bqvec= Cross(tvec, cb);
        
        /* calculate V parameter and test bounds */
        const float bv= Dot(ray.d, bqvec) * inv_bdet;
        if(bv < 0.0f || bu + bv > 1.0f)
            //~ return false;
            return (rt < htmax && rt > RAY_EPSILON);
        
        /* calculate t, ray intersects triangle */
        float rbt= Dot(ac, bqvec) * inv_bdet;
      
        /* return the closest intersection point */
        if(rt < rbt)
            return (rt < htmax && rt > RAY_EPSILON);
        
        rt= rbt;
        ru= bu;
        rv= bv;
        return (rt < htmax && rt > RAY_EPSILON);
    }
};

}       // namespace

#endif

