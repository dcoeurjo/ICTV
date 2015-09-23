/* version restructuree mailto:jean-claude.iehl@liris.cnrs.fr */

/*
 * pbrt source code Copyright(c) 1998-2005 Matt Pharr and Greg Humphreys
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 * (See file License.txt for complete license)
 */

#ifndef PBRT_GEOMETRY_H
#define PBRT_GEOMETRY_H

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>

#include "Vec.h"


//! namespace pour regrouper les types et les fonctions.
namespace gk {

#define RAY_EPSILON 0.0001f
#define EPSILON 0.00001f

    
// Geometry Declarations
class Point;
class Normal;
class Vector;

//! \defgroup Geometry representation de points, vecteurs, normales, couleurs, etc.
//@{
    

//! vecteur en dimension 3.
//! represente une direction, cf. Point et Normal pour representer un point et une normale.
class Vector : public Vec3
{
public:
    // Vector Public Methods
    //! constructeur.
    Vector( const float _x = 0.f, const float _y = 0.f, const float _z = 0.f ) :  Vec3(_x, _y, _z) {}
    explicit Vector( const Vec3& v ) : Vec3(v) {}
    
    //! construit un vecteur a partir des coordonnees d'un point.
    explicit Vector( const Point &p );
    
    //! construit un vecteur a partir des coordonnees d'une normale.
    explicit Vector( const Normal &n );
    
    //! construit un vecteur a partir de 3 floats en memoire.
    explicit Vector( const float * p ) : Vec3(p) {}
    
    //! construit le vecteur pq, origine p, direction q - p.
    Vector( const Point& p, const Point& q );
    
    Vector& operator=( const Vec3& v ) { x= v.x; y= v.y; z= v.z; return *this;}

    //! affiche un vecteur.
    void print( ) const { printf("% -.8f % -.8f % -.8f\n", x, y, z); }
    
    //! addition de 2 vecteurs, w= u + v, renvoie w.
    Vector operator+( const Vector &v ) const { return Vector( x + v.x, y + v.y, z + v.z ); }
    
    //! addition de 2 vecteurs, u= u + v.
    Vector& operator+=( const Vector &v ) { x += v.x; y += v.y; z += v.z; return *this; }
    
    //! soustraction de 2 vecteurs, w= u - v, renvoie w.
    Vector operator-( const Vector &v ) const { return Vector( x - v.x, y - v.y, z - v.z ); }
    
    //! soustraction de 2 vecteurs, u= u - v.
    Vector& operator-=( const Vector &v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    
    //! produit par un vector, w= k * u, renvoie w.
    Vector operator*( const Vector& v ) const { return Vector( v.x*x, v.y*y, v.z*z ); }
    
    //! produit par un reel, w= k * u, renvoie w.
    Vector operator*( const float f ) const { return Vector( f*x, f*y, f*z ); }
    
    //! produit par un reel, u= k * u.
    Vector &operator*=( const float f ) { x *= f; y *= f; z *= f; return *this; }
    
    //! division par un reel, w= u / k, renvoie w.
    Vector operator/( const float f ) const { assert( f != 0 ); float inv = 1.f / f; return Vector( x * inv, y * inv, z * inv ); }
    
    //! division par un reel, u= u / k.
    Vector &operator/=( const float f ) { assert( f != 0 ); float inv = 1.f / f; x *= inv; y *= inv; z *= inv; return *this; }
    
    //! negation d'un vecteur, w= -u, renvoie w.
    Vector operator-( ) const { return Vector( -x, -y, -z ); }
    
    //! renvoie le carre de la longueur du vecteur.
    float LengthSquared() const  { return x*x + y*y + z*z; }
    
    //! renvoie la longueur du vecteur.
    float Length() const { return sqrtf( LengthSquared() ); }
};

float Clamp( const float value , const float low, const float high );


//! represente une quantite d'energie, un tuple <rgba>.
//! a reprendre, verifier les operations sur alpha... cf algebre de porter & duff.
class Color : public VecColor
{
public:
    // Color Public Methods
    //! constructeur.
    Color( ) : VecColor(0.f, 0.f, 0.f, 1.f) {}
    explicit Color( const VecColor& color ) : VecColor(color) {}
    
    Color( const float _r, const float _g, const float _b, const float _a= 1.f ) :  VecColor( _r, _g, _b, _a ) {}
    
    //! constructeur.
    Color( const float _v, const float _a= 1.f ) : VecColor( _v, _v, _v, _a ) {}
    
    Color& operator= ( const Vec4& c ) { r= c.x; g= c.y; b= c.z; a= c.w; return *this; }
    Color& operator= ( const VecColor& c ) {  r= c.r; g= c.g; b= c.b; a= c.a; return *this; }
    
    //! affiche une couleur.
    void print( ) const { printf("% -.4f % -.4f % -.4f % -.6f\n", r, g, b, a); }
    
    //! addition de 2 couleurs, w= u + v, renvoie w.
    Color operator+( const Color &v ) const { return Color( r + v.r, g + v.g, b + v.b, a + v.a ); }
    
    //! addition de 2 vecteurs, u= u + v.
    Color& operator+=( const Color &v ) { r += v.r; g += v.g; b += v.b; a+= v.a; return *this; }
    
    Color operator-( const Color &v ) const { return Color( r - v.r, g - v.g, b - v.b, a - v.a ); }
    
    Color& operator-=( const Color &v ) { r -= v.r; g -= v.g; b -= v.b; a -= v.a; return *this; }
    
    Color operator*( const Color& v ) const { return Color( r * v.r, g * v.g, b * v.b, a * v.a ); }
    
    Color operator/( const Color& v ) const { return Color( r / v.r, g / v.g, b / v.b, a / v.a ); }
    
    Color& operator*=( const Color& v) { r *= v.r; g *= v.g; b *= v.b; a *= v.a; return *this; }
    
    //! produit par un reel, w= k * u, renvoie w.
    Color operator*( const float f ) const { return Color( f*r, f*g, f*b, f*a ); }
    
    //! produit par un reel, u= k * u.
    Color &operator*=( const float f ) { r *= f; g *= f; b *= f; a *= f; return *this; }
    
    //! division par un reel.
    Color operator/( const float f ) const { assert( f != 0 ); float inv = 1.f / f; return Color( r * inv, g * inv, b * inv, a * inv ); }
    
    //! division par un reel.
    Color &operator/=( const float f ) { assert( f != 0 ); float inv = 1.f / f; r *= inv; g *= inv; b *= inv; a *= inv; return *this; }
    
    //! negation d'un vecteur, w= -u, renvoie w.
    Color operator-( ) const { return Color( -r, -g, -b, -a ); }
    
    //! renvoie la quantite d'energie associe
    float power( ) const { return (r + g + b) / 3.f; }
    
    //! renvoie vrai la quantite d'energie est nulle
    bool isBlack( ) const { return (r + g + b) == 0.f; }
};

//! declare une couleur, un tripet <rgb> comme une couleur.
typedef Color Energy;


//! representation d'un point de dimension 3. memes operations que sur Vector.
//! memes operations que sur un Vector. 
class Point : public Vec3
{
public:
    // Point Methods
    Point( ) : Vec3() {}
    explicit Point( const Vec3& v ) : Vec3(v) {}
    
    Point( const float v ) : Vec3(v, v, v) {}
    
    Point( const float _x, const float _y, const float _z= 0.f ) : Vec3(_x, _y, _z) {}
    
    explicit Point( const Vector &v ) :  Vec3( v.x, v.y, v.z )  {}
   
    explicit Point ( const float *p ) : Vec3(p) {}
    
    Point& operator=( const Vec3& v ) { x= v.x; y= v.y; z= v.z; return *this;}
    
    //! affiche un point.
    void print( ) const { printf("%.10f %.10f %.10f\n", x, y, z); }
    
    //! addition d'un point et d'un vecteur, q= p + v, renvoie le point q.
    Point operator+( const Vector &v ) const { return Point( x + v.x, y + v.y, z + v.z ); }
    
    //! addition d'un point et d'un vecteur, p= p + v.
    Point &operator+=( const Vector &v ) { x += v.x; y += v.y; z += v.z; return *this; }
    
    //! soustraction de 2 points, v= p - q, renvoie le vecteur v.
    Vector operator-( const Point &q ) const { return Vector( x - q.x, y - q.y, z - q.z ); }
    
    //! soustraction d'un point et d'un vecteur, q= p - v, renvoie le point q.
    Point operator-( const Vector &v ) const { return Point( x - v.x, y - v.y, z - v.z ); }
    
    //! soutraction d'un point et d'un vecteur, p= p - v.
    Point &operator-=( const Vector &v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    
    //! addition de 2 points, ca n'existe pas, mais c'est pratique ! p= p + q.
    Point &operator+=( const Point &q ) { x += q.x; y += q.y; z += q.z; return *this; }
    
    Point operator+( const Point &q ) const { return Point( x + q.x, y + q.y, z + q.z ); }
    
    Point operator*( const float f ) const { return Point( f*x, f*y, f*z ); }
    
    Point &operator*=( const float f ) { x *= f; y *= f; z *= f; return *this; }
    
    Point operator/ ( const float f ) const { float inv = 1.f / f; return Point( inv*x, inv*y, inv*z ); }
    
    Point &operator/=( const float f ) { float inv = 1.f / f; x *= inv; y *= inv; z *= inv; return *this; }
};

//! representation d'un point homogene de dimension 4, transformation d'un Point par une projection (cf Perspective() et Transform).
class HPoint : public Vec4
{
public:
    // Point Methods
    //! constructeur.
    HPoint( const float _x = 0.f, const float _y = 0.f, const float _z = 0.f, const float _w= 1.f ) : Vec4( _x, _y, _z, _w ) {}
    
    //! construit un point homogene a partir d'un point 3d.
    HPoint( const Point& p ) : Vec4(p.x, p.y, p.z, 1.f) {}
    
    HPoint& operator=( const Vec4& p ) { x= p.x; y= p.y; z= p.z; w= p.w; return *this; }
    
    //! renvoie le point 3d associe au point homogene.
    Point project( ) const { assert( w != 0.f ); return Point(x / w, y / w, z / w); }
    
    //! determine si le point est visible apres avoir subit une projection, cf Perspective() et Transform.
    bool isVisible( ) const{ return (-w < x && x < w && -w < y && y < w && -w < z && z < w); }

    //! determine si le point est rejette apres avoir subit une projection, cf isVisible().
    bool isCulled( ) const { return !isVisible(); }
};

//! representation d'une normale de dimension 3, cf. Vector pour la description des operations.
class Normal : public Vec3
{
public:
    // Normal Methods
    Normal( const float _x = 0.f, const float _y = 0.f, const float _z = 0.f ) :  Vec3(_x, _y, _z) {}
    
    explicit Normal( const Vec3& v ) : Vec3(v) {}
    explicit Normal( const float *p ) : Vec3(p) {}
    
    explicit Normal( const Vector &v ) :  Vec3( v.x, v.y,v.z )  {}

    Normal& operator=( const Vec3& v ) { x= v.x; y= v.y; z= v.z; return *this; }
    
    Normal operator-( ) const { return Normal( -x, -y, -z ); }
    
    Normal operator+ ( const Normal &v ) const { return Normal( x + v.x, y + v.y, z + v.z ); }
    
    Normal& operator+=( const Normal &v ) { x += v.x; y += v.y; z += v.z; return *this; }
    
    Normal operator- ( const Normal &v ) const { return Normal( x - v.x, y - v.y, z - v.z ); }
    
    Normal& operator-=( const Normal &v ) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    
    Normal operator*( const float f ) const { return Normal( f*x, f*y, f*z ); }
    
    Normal &operator*=( const float f ) { x *= f; y *= f; z *= f; return *this; }
    
    Normal operator/ ( const float f ) const { float inv = 1.f / f; return Normal( x * inv, y * inv, z * inv ); }
    
    Normal &operator/=( float f ) { float inv = 1.f / f; x *= inv; y *= inv; z *= inv; return *this; }
    
    float LengthSquared() const { return x*x + y*y + z*z; }
    
    float Length() const { return sqrtf( LengthSquared() ); }
};

typedef Normal Tangent; //!< une tangente se transforme de la meme maniere qu'une normale.
typedef Normal Bitangent; //!< une bitangente se transforme de la meme maniere qu'une normale.


//! representation minimale d'un rayon.
struct BasicRay
{
    // Ray Public Methods
    BasicRay( const unsigned int _id= -1 )
        : 
        tmax( HUGE_VAL ),
        id(_id)
    {}

    //! constructeur (origine, direction). direction est un vecteur unitaire.
    BasicRay( const Point &origin, const Vector &direction,
        const unsigned int _id= -1 )
        : 
        o( origin ), 
        tmax( HUGE_VAL ),
        d( direction ), 
        id(_id)
    {}
    
    //! constructeur (origine, destination).
    BasicRay( const Point &origin, const Point& destination,
        const unsigned int _id= -1 )
        :
        o( origin ),
        tmax( 1.f - RAY_EPSILON ),
        d( origin, destination ), 
        id(_id)
    {}
    
    //! construit le point a l'abscisse 't'.
    Point operator( )( const float t ) const
    {
        return o + d * t; 
    }
    
    // Ray Public Data
    Point o;    //!< origine.
    float tmax;   //!< intervalle valide le long du rayon.
    Vector d;   //!< direction.
    unsigned int id;    //!< identifiant du rayon
};


//! representation 'pre-calculee' d'un rayon.
struct Ray : public BasicRay
{
    // Ray Public Methods
    Ray()
        : 
        BasicRay()
    {}

    //! constructeur (origine, direction). direction est un vecteur unitaire.
    Ray( const Point &origin, const Vector &direction,
        const float start = RAY_EPSILON, const float end = HUGE_VAL,
        unsigned int _id= -1)
        : 
        //~ BasicRay(origin, direction, start, end, id)
        BasicRay(origin, direction, id)
    {
        inv_d= Vector(1.f / d.x, 1.f / d.y, 1.f / d.z);
        sign_d[0]= (inv_d[0] < 0.f) ? 1 : 0;
        sign_d[1]= (inv_d[1] < 0.f) ? 1 : 0;
        sign_d[2]= (inv_d[2] < 0.f) ? 1 : 0;
        sign_d[3]= 0;
    }
    
    //! constructeur (origine, destination).
    Ray( const Point &origin, const Point& destination,
        const float start = RAY_EPSILON, const float end = 1.f - RAY_EPSILON,
        unsigned int _id= -1 )
        :
        //~ BasicRay(origin, destination, start, end, id )
        BasicRay(origin, destination, id )
    {
        inv_d= Vector(1.f / d.x, 1.f / d.y, 1.f / d.z);
        sign_d[0]= (inv_d[0] < 0.f) ? 1 : 0;
        sign_d[1]= (inv_d[1] < 0.f) ? 1 : 0;
        sign_d[2]= (inv_d[2] < 0.f) ? 1 : 0;
        sign_d[3]= 0;
    }
    
    //! renvoie vrai si la direction du rayon est < 0 pour l'axe 'axis'.
    bool isBackward( const int axis ) const
    {
        return (sign_d[axis]);
    }
    
    Vector inv_d;       //!< 1 / direction.
    unsigned char sign_d[4];    //!< vrai (==1) si direction[i] < 0.
};


//! representation d'un point d'intersection rayon / objet.
struct Hit
{
    Hit( )
        :
        tmin(RAY_EPSILON),
        t(HUGE_VAL),
        object_id(-1), 
        node_id(-1),
        child_id(-1),
        user_data(0.f)
    {}
    
    Hit( const Ray& ray )
        :
        //~ tmin(ray.tmin),
        tmin(RAY_EPSILON),
        t(ray.tmax),
        object_id(-1),
        node_id(-1),
        child_id(-1),
        user_data(0.f)
    {}
    
    ~Hit( ) {}
    
    // Hit public data
    Point p;            //!< point d'intersection
    Normal n;           //!< normale
    Tangent t1, t2;     //!< repere tangent
    float tmin;         //!< abscisse min le long du rayon
    float t;            //!< abscisse le long du rayon
    float u, v;         //!< coordonnees de textures / parametres de surface
    int object_id;      //!< identifiant de l'objet/triangle intersecte
    int node_id;        //!< identifiant du noeud / pere du noeud intersecte
    int child_id;       //! identifiant du fils intersecte
    float user_data;    //!< donnee collectee pendant le suivi de rayon
};


//! representation d'une boite englobante alignee sur les axes (aabox).
class BBox
{
public:
    // BBox Public Methods
    //! constructeur par defaut.
    BBox()
    {
        clear();
    }
    
    //! re-initialise les extremites.
    void clear( )
    {
        pMin = Point( (float) HUGE_VAL,  (float) HUGE_VAL,  (float) HUGE_VAL );
        pMax = Point( (float) -HUGE_VAL, (float) -HUGE_VAL, (float) -HUGE_VAL );
    }
    
    //! construction de l'aabbox d'un point, cf. Union( aabox, q ) pour ajouter d'autres points.
    BBox( const Point &p ) 
        : 
        pMin( p ), pMax( p ) 
    { }
    
    //! construction d'une aabox connaissant 2 points.
    BBox( const Point &p1, const Point &p2 )
    {
        pMin = Point( 
            std::min( p1.x, p2.x ),
            std::min( p1.y, p2.y ),
            std::min( p1.z, p2.z ) );
        
        pMax = Point( 
            std::max( p1.x, p2.x ),
            std::max( p1.y, p2.y ),
            std::max( p1.z, p2.z ) );
    }
    
    void print( ) const
    {
        printf("[ %.10f %.10f %.10f ] x [ %.10f %.10f %.10f ] ", 
            pMin.x, pMin.y, pMin.z, 
            pMax.x, pMax.y, pMax.z);
        
        Vector d(pMin, pMax);
        printf("extents (%.10f %.10f %.10f)\n",
            d.x, d.y, d.z);
    }
    
    //! ajoute un point dans l'aabox.
    friend BBox Union( const BBox &b, const Point &p );
    
    //! ajoute une autre aabox.
    friend BBox Union( const BBox &b, const BBox &b2 );
    
    //! ajoute une autre aabox
    void Union( const BBox& bbox )
    {
    #if 0
        pMin.x= std::min(pMin.x, bbox.pMin.x);
        pMin.y= std::min(pMin.y, bbox.pMin.y);
        pMin.z= std::min(pMin.z, bbox.pMin.z);
        pMax.x= std::max(pMax.x, bbox.pMax.x);
        pMax.y= std::max(pMax.y, bbox.pMax.y);
        pMax.z= std::max(pMax.z, bbox.pMax.z);        
    #else
        // plus rapide ... 
        if(bbox.pMin.x < pMin.x)
            pMin.x= bbox.pMin.x;
        if(bbox.pMin.y < pMin.y)
            pMin.y= bbox.pMin.y;
        if(bbox.pMin.z < pMin.z)
            pMin.z= bbox.pMin.z;
        
        if(bbox.pMax.x > pMax.x)
            pMax.x= bbox.pMax.x;
        if(bbox.pMax.y > pMax.y)
            pMax.y= bbox.pMax.y;
        if(bbox.pMax.z > pMax.z)
            pMax.z= bbox.pMax.z;
    #endif
    }
    
    //! ajoute un point a la bbox.
    void Union( const Point& p )
    {
    #if 0
        pMin.x= std::min(pMin.x, p.x);
        pMin.y= std::min(pMin.y, p.y);
        pMin.z= std::min(pMin.z, p.z);
        pMax.x= std::max(pMax.x, p.x);
        pMax.y= std::max(pMax.y, p.y);
        pMax.z= std::max(pMax.z, p.z);
    #else
        // plus rapide ... 
        if(p.x < pMin.x)
            pMin.x= p.x;
        if(p.y < pMin.y)
            pMin.y= p.y;
        if(p.z < pMin.z)
            pMin.z= p.z;
        
        if(p.x > pMax.x)
            pMax.x= p.x;
        if(p.y > pMax.y)
            pMax.y= p.y;
        if(p.z > pMax.z)
            pMax.z= p.z;
    #endif
    }
    
    //! verifie l'intersection de 2 aabbox.
    bool Overlaps( const BBox &b ) const
    {
        bool x = ( pMax.x >= b.pMin.x ) && ( pMin.x <= b.pMax.x );
        bool y = ( pMax.y >= b.pMin.y ) && ( pMin.y <= b.pMax.y );
        bool z = ( pMax.z >= b.pMin.z ) && ( pMin.z <= b.pMax.z );
        return ( x && y && z );
    }
    
    void Intersection( const BBox& b )
    {
        pMin.x= std::max(pMin.x, b.pMin.x);
        pMin.y= std::max(pMin.y, b.pMin.y);
        pMin.z= std::max(pMin.z, b.pMin.z);
        pMax.x= std::min(pMax.x, b.pMax.x);
        pMax.y= std::min(pMax.y, b.pMax.y);
        pMax.z= std::min(pMax.z, b.pMax.z);        
    }
    
    friend BBox Intersection( const BBox& a, const BBox& b );
    
    bool isEmpty( ) const
    {
        bool x = ( pMax.x < pMin.x );
        bool y = ( pMax.y < pMin.y );
        bool z = ( pMax.z < pMin.z );
        return ( x && y && z );
    }
    
    //! verifie l'inclusion d'un point dans l'aabox.
    bool Inside( const Point &pt ) const
    {
        return ( pt.x >= pMin.x && pt.x <= pMax.x 
            && pt.y >= pMin.y && pt.y <= pMax.y 
            && pt.z >= pMin.z && pt.z <= pMax.z );
    }
    
    //! dilate l'aabox de 'delta' dans toutes les directions.
    void Expand( float delta )
    {
        pMin -= Vector( delta, delta, delta );
        pMax += Vector( delta, delta, delta );
    }
    
    //! renvoie le centre de la boite.
    const Point center( ) const
    {
        return (pMin + pMax) * .5f;
    }
    
    //! renvoie le centre de la boite, sur un axe particulier.
    float center( const int axis ) const
    {
        return (pMin[axis] + pMax[axis]) * .5f;
    }
    
    //! renvoie le volume de l'aabox.
    float Volume( ) const
    {
        const Vector d(pMin, pMax);
        return d.x * d.y * d.z;
    }
    
    //! renvoie l'aire de l'aabox
    float SurfaceArea( ) const
    {
        //~ if(isEmpty()) return 0.f;
        
        const Vector d(pMin, pMax);
        const float area= 2.f * d.x * d.y + 2.f * d.x * d.z + 2.f * d.y * d.z;
        return area;
    }
    
    //! renvoie l'indice de l'axe le plus long de l'aabox, 0 pour X, 1 pour Y et 2 pour Z.
    int MaximumExtent( ) const
    {
        const Vector d(pMin, pMax);
        if ( d.x > d.y && d.x > d.z )
            return 0;
        if ( d.y > d.z )
            return 1;
        else
            return 2;
    }
    
    //! renvoie une sphere englobante de l'aabox.
    void BoundingSphere( Point &c, float &r ) const;
    
    //! intersection avec rayon.
    //! renvoie faux si une intersection existe mais n'est pas dans l'intervalle [0 htmax]. \n
    //! renvoie vrai + la position du point d'entree (rtmin) et celle du point de sortie (rtmax). \n
    /*! "An Efficient and Robust Ray-Box Intersection Algorithm"
        cf http://cag.csail.mit.edu/%7Eamy/papers/box-jgt.pdf
    */
    
    /*! le parametre htmax permet de trouver tres facilement l'intersection la plus proche de l'origine du rayon.
    \code
        float t= ray.tmax;      // ou t= HUGE_VAL; la plus grande distance le long du rayon.
        // rechercher la bbox la plus proche de l'origine du rayon
        for(int i= 0; i < n; i++)
        {
            float tmin;
            float tmax;
            if(bbox[i].Intersect(ray, t, tmin, tmax))
                // t= tmin; ne suffit pas si l'intervalle [tmin tmax] est en partie negatif, tmin < 0
                t= std::max(0.f, tmin);
        }
    \endcode
    */
    bool Intersect( const Ray &ray, const float htmax, 
        float &rtmin, float &rtmax ) const
    {
        float tmin= ((*this)[ray.sign_d[0]].x - ray.o.x) * ray.inv_d.x;
        float tmax= ((*this)[1u - ray.sign_d[0]].x - ray.o.x) * ray.inv_d.x;
        
        // y slab
        const float tymin= ((*this)[ray.sign_d[1]].y - ray.o.y) * ray.inv_d.y;
        const float tymax= ((*this)[1u - ray.sign_d[1]].y - ray.o.y) * ray.inv_d.y;

        if((tmin > tymax) || (tymin > tmax))
            return false;
        if(tymin > tmin)
            tmin= tymin;
        if(tymax < tmax)
            tmax= tymax;
        
        // z slab
        const float tzmin= ((*this)[ray.sign_d[2]].z - ray.o.z) * ray.inv_d.z;
        const float tzmax= ((*this)[1u - ray.sign_d[2]].z - ray.o.z) * ray.inv_d.z;

        if((tmin > tzmax) || (tzmin > tmax))
            return false;
        if(tzmin > tmin)
            tmin= tzmin;
        if(tzmax < tmax)
            tmax= tzmax;
        
        // ne renvoie vrai que si l'intersection est valide
        rtmin= tmin;
        rtmax= tmax;
        //~ return ((tmin < htmax) && (tmax > ray.tmin));
        return ((tmin < htmax) && (tmax > RAY_EPSILON));
    }

    bool Occluded( const Ray& ray ) const
    {
        float rtmin;
        float rtmax;
        return Intersect(ray, ray.tmax, rtmin, rtmax);
    }
    
    bool Occluded( const Ray& ray, const float tmax ) const
    {
        float rtmin;
        float rtmax;
        return Intersect(ray, tmax, rtmin, rtmax);
    }
    
    //! renvoie egalement la normale de l'intersection.
    bool Intersect( const Ray &ray, const float htmax, 
        float &rtmin, float &rtmax, Normal& rn ) const
    {
        Normal n(-1.f, 0.f, 0.f);
        float tmin= ((*this)[ray.sign_d[0]].x - ray.o.x) * ray.inv_d.x;
        float tmax= ((*this)[1 - ray.sign_d[0]].x - ray.o.x) * ray.inv_d.x;
        
        // y slab
        const float tymin= ((*this)[ray.sign_d[1]].y - ray.o.y) * ray.inv_d.y;
        const float tymax= ((*this)[1 - ray.sign_d[1]].y - ray.o.y) * ray.inv_d.y;

        if((tmin > tymax) || (tymin > tmax))
            return false;
        if(tymin > tmin)
        {
            tmin= tymin;
            n= Normal(0.f, -1.f, 0.f);
        }
        if(tymax < tmax)
            tmax= tymax;
        
        // z slab
        const float tzmin= ((*this)[ray.sign_d[2]].z - ray.o.z) * ray.inv_d.z;
        const float tzmax= ((*this)[1 - ray.sign_d[2]].z - ray.o.z) * ray.inv_d.z;

        if((tmin > tzmax) || (tzmin > tmax))
            return false;
        if(tzmin > tmin)
        {
            tmin= tzmin;
            n= Normal(0.f, 0.f, -1.f);
        }
        if(tzmax < tmax)
            tmax= tzmax;
        
        // ne renvoie vrai que si l'intersection est valide
        rtmin= tmin;
        rtmax= tmax;
        rn= n;
        //~ return ((tmin < htmax) && (tmax > ray.tmin));
        return ((tmin < htmax) && (tmax > RAY_EPSILON));
    }

    //! intersection avec un rayon.
    bool Intersect( const BasicRay &ray, const float htmax, 
        float &rtmin, float &rtmax ) const
    {
        unsigned int sign_d[4];
        Vector inv_d= Vector(1.f / ray.d.x, 1.f / ray.d.y, 1.f / ray.d.z);
        
        sign_d[0]= (inv_d[0] < 0.f) ? 1u : 0u;
        float tmin= ((*this)[sign_d[0]].x - ray.o.x) * inv_d.x;
        float tmax= ((*this)[1u - sign_d[0]].x - ray.o.x) * inv_d.x;
        
        // y slab
        sign_d[1]= (inv_d[1] < 0.f) ? 1u : 0u;
        const float tymin= ((*this)[sign_d[1]].y - ray.o.y) * inv_d.y;
        const float tymax= ((*this)[1u - sign_d[1]].y - ray.o.y) * inv_d.y;

        if((tmin > tymax) || (tymin > tmax))
            return false;
        if(tymin > tmin)
            tmin= tymin;
        if(tymax < tmax)
            tmax= tymax;
        
        // z slab
        sign_d[2]= (inv_d[2] < 0.f) ? 1u : 0u;
        const float tzmin= ((*this)[sign_d[2]].z - ray.o.z) * inv_d.z;
        const float tzmax= ((*this)[1u - sign_d[2]].z - ray.o.z) * inv_d.z;

        if((tmin > tzmax) || (tzmin > tmax))
            return false;
        if(tzmin > tmin)
            tmin= tzmin;
        if(tzmax < tmax)
            tmax= tzmax;
        
        // ne renvoie vrai que si l'intersection est valide
        rtmin= tmin;
        rtmax= tmax;
        //~ return ((tmin < htmax) && (tmax > ray.tmin));
        return ((tmin < htmax) && (tmax > RAY_EPSILON));
    }

    bool Occluded( const BasicRay& ray ) const
    {
        float rtmin;
        float rtmax;
        return Intersect(ray, ray.tmax, rtmin, rtmax);
    }
    
    bool Occluded( const BasicRay& ray, const float tmax ) const
    {
        float rtmin;
        float rtmax;
        return Intersect(ray, tmax, rtmin, rtmax);
    }    
    
    //! acces indexe aux extremes de l'aabox.
    Point& operator[] ( const int id )
    {
        assert(id == 0 || id == 1);
        return (&pMin)[id];
    }

    //! acces indexe aux extremes de l'aabox.
    const Point& operator[] ( const int id ) const
    {
        assert(id == 0 || id == 1);
        return (&pMin)[id];
    }
    
    // BBox Public Data
    //! points extremes de l'aabox.
    Point pMin, pMax;
};


// Geometry Inline Functions
inline 
Vector::Vector( const Point &p )
    : 
    //~ x( p.x ), y( p.y ), z( p.z )
    Vec3( p.x, p.y, p.z )
{}

inline
Vector::Vector( const Point& p, const Point& q )
    :
    //~ x( q.x - p.x ), y( q.y - p.y ), z( q.z - p.z )
    Vec3( q.x - p.x, q.y - p.y, q.z - p.z )
{}

//! scalaire * vecteur.
inline 
Vector operator*( float f, const Vector &v )
{
    return v*f;
}

//! valeur absolue des composantes d'un vecteur
inline Vector Abs( const Vector& v )
{
    return Vector(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}

//! valeur absolue des composantes d'un point
inline Point Abs( const Point& v )
{
    return Point(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}

//! valeur absolue des composantes d'une couleur/energy
inline Color Abs( const Color& v )
{
    return Color(fabsf(v.r), fabsf(v.g), fabsf(v.b), fabsf(v.a));
}


//! produit scalaire de 2 vecteurs.
 inline 
float Dot( const Vector &v1, const Vector &v2 )
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//! valeur absolue du produit scalaire de 2 vecteurs.
 inline 
float AbsDot( const Vector &v1, const Vector &v2 )
{
    return fabsf( Dot( v1, v2 ) );
}

//! max(0, dot) du produit scalaire de 2 vecteurs.
 inline 
float ZeroDot( const Vector &v1, const Vector &v2 )
{
    return std::max( 0.f, Dot( v1, v2 ) );
}

//! produit vectoriel de 2 vecteurs.
 inline 
Vector Cross( const Vector &v1, const Vector &v2 )
{
    return Vector(
        ( v1.y * v2.z ) - ( v1.z * v2.y ),
        ( v1.z * v2.x ) - ( v1.x * v2.z ),
        ( v1.x * v2.y ) - ( v1.y * v2.x ) );
}

//! produit vectoriel d'un vecteur et d'une normale.
 inline 
Vector Cross( const Vector &v1, const Normal &v2 )
{
    return Vector(
        ( v1.y * v2.z ) - ( v1.z * v2.y ),
        ( v1.z * v2.x ) - ( v1.x * v2.z ),
        ( v1.x * v2.y ) - ( v1.y * v2.x ) );
}

//! produit vectoriel d'une normale et d'un vecteur.
 inline 
Vector Cross( const Normal &v1, const Vector &v2 )
{
    return Vector(
        ( v1.y * v2.z ) - ( v1.z * v2.y ),
        ( v1.z * v2.x ) - ( v1.x * v2.z ),
        ( v1.x * v2.y ) - ( v1.y * v2.x ) );
}

//! renvoie un vecteur de longueur 1 de meme direction que v.
inline 
Vector Normalize( const Vector &v )
{
    return v / v.Length();
}

//! construit un repere orthogonal dont la normale est aligne sur un vecteur v1, v2 et v3 sont les 2 tangentes.
inline 
void CoordinateSystem( const Vector &v1, Vector *v2, Vector *v3 )
{
    if ( fabsf( v1.x ) > fabsf( v1.y ) )
    {
        float invLen = 1.f / sqrtf( v1.x * v1.x + v1.z * v1.z );
        *v2 = Vector( -v1.z * invLen, 0.f, v1.x * invLen );
    }
    else
    {
        float invLen = 1.f / sqrtf( v1.y * v1.y + v1.z * v1.z );
        *v2 = Vector( 0.f, v1.z * invLen, -v1.y * invLen );
    }
    
    *v3 = Cross( v1, *v2 );
    //~ *v3 = Cross( *v2, v1 );
}

//! renvoie la distance entre 2 points.
inline 
float Distance( const Point &p1, const Point &p2 )
{
    return ( p1 - p2 ).Length();
}

//! renvoie le carre de la distance entre 2 points.
inline 
float DistanceSquared( const Point &p1, const Point &p2 )
{
    return ( p1 - p2 ).LengthSquared();
}

//! scalaire * point.
inline 
Point operator*( float f, const Point &p )
{
    return p*f;
}

//! scalaire * normale.
inline 
Normal operator*( float f, const Normal &n )
{
    return Normal( f*n.x, f*n.y, f*n.z );
}

//! renvoie une normale de meme direction, mais de longeur 1.
inline 
Normal Normalize( const Normal &n )
{
    return n / n.Length();
}

inline 
Vector::Vector( const Normal &n )
    :
    //~ x( n.x ), y( n.y ), z( n.z )
    Vec3( n.x, n.y, n.z )
{}

//! produit scalaire d'une normale et d'un vecteur.
inline 
float Dot( const Normal &n1, const Vector &v2 )
{
    return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
}

//! produit scalaire d'un vecteur et d'une normale.
inline 
float Dot( const Vector &v1, const Normal &n2 )
{
    return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}

//! produit scalaire de 2 normales.
inline 
float Dot( const Normal &n1, const Normal &n2 )
{
    return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

//! valeur absolue du produit scalaire d'une normale et d'un vecteur.
inline 
float AbsDot( const Normal &n1, const Vector &v2 )
{
    return fabsf( n1.x * v2.x + n1.y * v2.y + n1.z * v2.z );
}

//! valeur absolue du produit scalaire d'un vecteur et d'une normale.
inline 
float AbsDot( const Vector &v1, const Normal &n2 )
{
    return fabsf( v1.x * n2.x + v1.y * n2.y + v1.z * n2.z );
}

//! valeur absolue du produit scalaire de 2 normales.
inline 
float AbsDot( const Normal &n1, const Normal &n2 )
{
    return fabsf( n1.x * n2.x + n1.y * n2.y + n1.z * n2.z );
}

//! max(0, dot) du produit scalaire de 2 vecteurs.
inline 
float ZeroDot( const Normal &v1, const Vector &v2 )
{
    return std::max( 0.f, Dot( v1, v2 ) );
}

//! max(0, dot) du produit scalaire de 2 vecteurs.
inline 
float ZeroDot( const Vector &v1, const Normal &v2 )
{
    return std::max( 0.f, Dot( v1, v2 ) );
}

//! max(0, dot) du produit scalaire de 2 vecteurs.
inline 
float ZeroDot( const Normal &v1, const Normal &v2 )
{
    return std::max( 0.f, Dot( v1, v2 ) );
}


//! interpolation lineaire entre 2 reels, x= (1 - t) v1 + t v2 
inline 
float Lerp( const float t, const float v1, const float v2 ) 
{
    return (1.f - t) * v1 + t * v2;
}

//! limite une valeur entre un min et un max.
inline 
float Clamp( const float value, const float low, const float high)
{
    if(value < low) 
        return low;
    else if (value > high)
        return high;
    else 
        return value;
}

//! limite une valeur entre un min et un max.
inline 
int Clamp( const int value , const int low, const int high )
{
    if(value < low) 
        return low;
    else if (value > high) 
        return high;
    else 
        return value;
}

//! conversion degres vers radians.
 inline 
float Radians( const float deg )
{
	return ( (float) M_PI / 180.f) * deg;
}

//! conversion radians vers degres.
 inline 
float Degrees( const float rad )
{
    return (180.f / (float) M_PI) * rad;
}

//! renvoie un vecteur dont la direction est representee en coordonness polaires.
 inline 
Vector SphericalDirection( float sintheta, float costheta, float phi )
{
    return Vector( 
        sintheta * cosf( phi ), 
        sintheta * sinf( phi ), 
        costheta );
}

//! renvoie les composantes d'un vecteur dont la direction est representee en coordonnees polaires, dans la base x, y, z.
 inline 
Vector SphericalDirection( float sintheta, float costheta, float phi,
    const Vector &x, const Vector &y, const Vector &z )
{
    return sintheta * cosf( phi ) * x 
        + sintheta * sinf( phi ) * y 
        + costheta * z;
}

//! renvoie l'angle theta d'un vecteur avec la normale, l'axe Z, (utilisation dans un repere local).
 inline 
float SphericalTheta( const Vector &v )
{
    return acosf( Clamp( v.z, -1.f, 1.f ) );
}

//! renvoie l'angle phi d'un vecteur avec un vecteur tangent, l'axe X, (utilisation dans un repere local).
 inline 
float SphericalPhi( const Vector &v )
{
    float p = atan2f( v.y, v.x );
    return ( p < 0.f ) ? p + 2.f*M_PI : p;
}
//@}

} // namespace

#endif // PBRT_GEOMETRY_H
