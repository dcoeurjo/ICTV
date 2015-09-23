
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Geometry.h"
#include "Transform.h"


namespace gk {

//! representation d'un triangle.    
struct Triangle
{
    Point a, b, c;
    unsigned int id;    
    
    Triangle( ) : a(), b(), c(), id(-1) {}
    Triangle( const Point& _a, const Point& _b, const Point& _c, const unsigned int _id= -1 ) : a(_a),  b(_b), c(_c), id(_id) {}
    
    //! calcule l'aire du triangle.
    float area( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        if(ab.Length() == 0.0f || ac.Length() == 0.0f)
            return 0.0f;
        return .5f * Cross(ab, ac).Length();
    }
    
    //! renvoie la normale du triangle.
    Normal normal( ) const
    {
        Vector ab(a, b);
        Vector ac(a, c);
        
        if(ab.Length() == 0.0f || ac.Length() == 0.0f)
            return Normal();
        return Normal( Normalize(Cross(ab, ac)) );
    }
    
    //! changement de repere du vecteur 'v' global, renvoie le vecteur dans le repere local (bitangent, tangent, normal).
    Vector local( const Vector& v ) const
    {
        // construit le changement de repere
        Vector t= Normalize( Vector(a, b) );
        Vector n= Normalize( Cross(t, Vector(a, c)) );
        Vector b= Cross(t, n);
        
        // repere de l'objet
        return Vector( Dot(v, b), Dot(v, t), Dot(v, n) );
    }

    //! changement de repere du vecteur 'v' local, renvoie le vecteur dans le repere global.
    Vector world( const Vector& v ) const
    {
        // construit le changement de repere
        Vector t= Normalize( Vector(a, b) );
        Vector n= Normalize( Cross(t, Vector(a, c)) );
        Vector b= Cross(t, n);
        
        // repere global
        return v.x * b + v.y * t + v.z * n;
    }
    
    //! calcule la boite englobante du triangle.
    BBox bbox( ) const
    {
        BBox bbox(a, b);
        bbox.Union(c);
        return bbox;
    }
    
    //! renvoie un triangle transforme par 't'.
    Triangle transform( const Transform& t ) const
    {
        return Triangle( t(a), t(b), t(c) );
    }
    
    //! intersection avec un rayon.
    //! renvoie faux s'il n'y a pas d'intersection valide, une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 htmax] du rayon. \n
    //! renvoie vrai + les coordonnees barycentriques (ru, rv) du point d'intersection + sa position le long du rayon (rt). \n
    //! convention barycentrique : t(u, v)= (1 - u - v) * a + u * b + v * c \n
    //! cf point(u, v) pour "retrouver" le point associe aux coordonnees (u,v, 1-u-v), PNTriangle::normal(u, v) la normale, et PTNTriangle::texcoord(u, v) les coordonnees de textures.

    /*! le parametre  htmax permet de trouver tres facilement l'intersection la plus proche de l'origine du rayon.
    \code
        float t= ray.tmax;      // ou t= HUGE_VAL; la plus grande distance le long du rayon.
        // rechercher le triangle le plus proche de l'origine du rayon
        for(int i= 0; i < n; i++)
        {
            float rt;
            float ru, rv;
            if(triangle[i].Intersect(ray, t, rt, ru, rv))
                t= rt;
        }
    \endcode
    */
    
    bool Intersect( const Ray &ray, const float htmax, 
        float &rt, float &ru, float&rv ) const
    {
        /* begin calculating determinant - also used to calculate U parameter */
        Vector ac(a, c);
        const Vector pvec= Cross(ray.d, ac);
        
        /* if determinant is near zero, ray lies in plane of triangle */
        Vector ab(a, b);
        const float det= Dot(ab, pvec);
        if (det > -EPSILON && det < EPSILON)
            return false;
        
        const float inv_det= 1.0f / det;
        
        /* calculate distance from vert0 to ray origin */
        const Vector tvec(a, ray.o);
        
        /* calculate U parameter and test bounds */
        const float u= Dot(tvec, pvec) * inv_det;
        if(u < 0.0f || u > 1.0f)
            return false;
        
        /* prepare to test V parameter */
        const Vector qvec= Cross(tvec, ab);
        
        /* calculate V parameter and test bounds */
        const float v= Dot(ray.d, qvec) * inv_det;
        if(v < 0.0f || u + v > 1.0f)
            return false;
        
        /* calculate t, ray intersects triangle */
        rt= Dot(ac, qvec) * inv_det;
        ru= u;
        rv= v;
        
        // ne renvoie vrai que si l'intersection est valide (comprise entre tmin et tmax du rayon)
        return (rt < htmax && rt > RAY_EPSILON);
    }    
    
    //! renvoie un point a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Point point( const float u, const float v ) const
    {
        const float w= 1.f - u - v;
        return a * w + b * u + c * v;
    }

    //! choisit un point aleatoirement a la surface du triangle et renvoie la probabilite de l'avoir choisi.
    //! \param u1, u2 valeurs aleatoires entre [0 .. 1] utilisées pour le tirage aleatoire.
    float sampleUniform( const float u1, const float u2, Point& p ) const
    {
        float u, v;
        float pdf= sampleUniformUV(u1, u2, u, v);
        
        p= point(u, v);
        return pdf;
    }
    
    //! choisit une position aleatoirement a la surface du triangle et les coordonnees barycentriques (u,v, 1-u-v) et la probabilite de l'avoir choisie.
    //! \param u1, u2 valeurs aleatoires entre [0 .. 1] utilisées pour le tirage aleatoire.
    //! point(u, v) permet de "retrouver" le point associe, PNTriangle::normal(u, v) la normale, et PTNTriangle::texcoord(u, v) les coordonnees de textures.
    float sampleUniformUV( const float u1, const float u2, float &u, float& v ) const
    {
        float s= sqrtf(u1);
        float t= u2;
        u= 1.f - s;
        v= (1.f - t) * s;
        return 1.f / area();
    }
    
    //! renvoie la probabilite de choisir le point p aleatoirement.
    //! remarque: le point doit appartenir au triangle.
    float pdfUniform( const Point& p ) const
    {
        return 1.f / area();
    }
};


//! representation d'un triangle position + normale.
struct PNTriangle : public Triangle
{
    Normal na, nb, nc;
    
    //! constructeur par defaut.
    PNTriangle( ) : Triangle() {}
    
    //! construit un triangle connaissant ses 3 sommets et leur normales.
    PNTriangle( 
        const Point& _a, const Normal& _na, 
        const Point& _b, const Normal& _nb, 
        const Point& _c, const Normal& _nc,
        const int _id )
        :
        Triangle(_a, _b, _c, _id), na(_na), nb(_nb), nc(_nc) {}
    
    //! construit un triangle connaissant le triangle de base et les normales des sommets.
    PNTriangle( const Triangle& abc, const Normal& _na, const Normal& _nb, const Normal& _nc )
        :
        Triangle(abc), na(_na), nb(_nb), nc(_nc) {}
    
    //! destructeur.
    ~PNTriangle( ) {}
    
    //! calcule la normale a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention n(u, v)= (1 - u - v) * na + u * nb + v * nc.
    //! cf Triangle::sampleUniformUV() pour choisir uniformement une position sur le triangle.
    Normal normal( const float u, const float v ) const
    {
        const float w= 1.f - u - v;
        return Normalize(na * w + nb * u + nc * v);
    }
    
    //! renvoie un pntriangle transforme par 't'.
    PNTriangle transform( const Transform& t ) const
    {
        return PNTriangle( t(a), t(na), t(b), t(nb), t(c), t(nc), id );
    }
    
    //! renvoie le triangle 'geometrique' support du PN triangle.
    Triangle triangle( ) const { return Triangle(a, b, c, id); }
};


//! representation d'un triangle position + coordonnees de texture + normale.
struct PTNTriangle : public PNTriangle
{
    Point ta, tb, tc;
    
    //! constructeur par defaut.
    PTNTriangle( )  : PNTriangle() {}
    
    //! construit un triangle connaissant ses 3 sommets, leurs normales et leurs coordonnees de textures.
    PTNTriangle( 
        const Point& _a, const Point& _ta, const Normal& _na, 
        const Point& _b, const Point& _tb, const Normal& _nb, 
        const Point& _c, const Point& _tc, const Normal& _nc,
        const int _id )
        :
        PNTriangle(_a, _na, _b, _nb, _c, _nc, _id), ta(_ta), tb(_tb), tc(_tc) {}
    
    //! construit un triangle connaissant le triangle de base, les normales et les coordonnees de textures des sommets.
    PTNTriangle( const Triangle& _abc,
        const Point& _ta, const Normal& _na, 
        const Point& _tb, const Normal& _nb, 
        const Point& _tc, const Normal& _nc )
        :
        PNTriangle(_abc, _na, _nb, _nc), ta(_ta), tb(_tb), tc(_tc) {}
    
    //! construit un triangle connaissant un pn triangle et les coordonnees de textures des sommets.
    PTNTriangle( const PNTriangle& _abc, const Point& _ta, const Point& _tb, const Point& _tc )
        :
        PNTriangle(_abc), ta(_ta), tb(_tb), tc(_tc) {}
    
    //! destructeur.
    ~PTNTriangle( ) {}
    
    //! renvoie un pntriangle transforme par 't'.
    PTNTriangle transform( const Transform& t ) const
    {
        return PTNTriangle( 
            t(a), ta, t(na),
            t(b), tb, t(nb),
            t(c), tc, t(nc), id );
    }
    
    //! renvoie le triangle geometrique.
    Triangle triangle( ) const { return Triangle(a, b, c, id); }
    
    //! renvoie le pn triangle.
    PNTriangle pntriangle( ) const { return PNTriangle(a, na, b, nb, c, nc, id); }
    
    //! calcule les coordonnees de texture d'un point a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention t(u, v)= (1 - u - v) * ta + u * tb + v * tc.
    //! cf Triangle::sampleUniformUV() pour choisir uniformement une position sur le triangle.
    Point texcoord( const float u, const float v ) const
    {
        const float w= 1.f - u - v;
        return ta * w + tb * u + tc * v;
    }
};

}       // namespace

#endif
