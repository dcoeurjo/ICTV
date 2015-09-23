/* version restructuree mailto:jean-claude.iehl@liris.cnrs.fr */

/*
 * pbrt source code Copyright(c) 1998-2005 Matt Pharr and Greg Humphreys
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 * (See file License.txt for complete license)
 */

#ifndef PBRT_TRANSFORM_H
#define PBRT_TRANSFORM_H

#include <iostream>
#include <cstdio>
#include <cassert>

#include "Geometry.h"

namespace gk {

//! \defgroup Transform representation matrices homogenes, transformations, compositions de transformations, etc.
//@{
    
// Matrix4x4 Declarations
//! representation d'une matrice homogene 4x4.
struct Matrix4x4 : public Mat4
{
    // Matrix4x4 Public Methods
    //! construit une matrice identite, par defaut.
    Matrix4x4( ) : Mat4() {}
    //! construit une matrice a partir d'un tableau 2d de reels [ligne][colonne].
    Matrix4x4( const float mat[4][4] );
    
    //! construit une matrice a partir des 16 elements.
    Matrix4x4( 
       float t00, float t01, float t02, float t03,
       float t10, float t11, float t12, float t13,
       float t20, float t21, float t22, float t23,
       float t30, float t31, float t32, float t33 );
    
    //! renvoie la matrice transposee.
    Matrix4x4 transpose( ) const;
    
    //! affiche la matrice.
    void print( ) const
    {
        #define M44(m, r, c) m[r][c]
        
        printf("% -.8f  % -.8f  % -.8f  % -.8f\n", 
            M44(m, 0, 0), M44(m, 0, 1), M44(m, 0, 2), M44(m, 0, 3));
        printf("% -.8f  % -.8f  % -.8f  % -.8f\n", 
            M44(m, 1, 0), M44(m, 1, 1), M44(m, 1, 2), M44(m, 1, 3));
        printf("% -.8f  % -.8f  % -.8f  % -.8f\n", 
            M44(m, 2, 0), M44(m, 2, 1), M44(m, 2, 2), M44(m, 2, 3));
        printf("% -.8f  % -.8f  % -.8f  % -.8f\n", 
            M44(m, 3, 0), M44(m, 3, 1), M44(m, 3, 2), M44(m, 3, 3));
        printf("\n");
        
        #undef M44
    }

    //! produit de 2 matrices : renvoie m1 * m2.
    static 
    Matrix4x4 mul( const Matrix4x4 &m1, const Matrix4x4 &m2 )
    {
        float r[4][4];
        
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r[i][j] = 
                    m1.m[i][0] * m2.m[0][j] +
                    m1.m[i][1] * m2.m[1][j] +
                    m1.m[i][2] * m2.m[2][j] +
                    m1.m[i][3] * m2.m[3][j];
        
        return Matrix4x4(r);
    }
    
    //! renvoie l'inverse de la matrice.
    Matrix4x4 getInverse( ) const;
};


// Transform Declarations
//! representation d'une transformation == un changement de repere, du repere '1' vers le repere '2'. 
class Transform
{
public:
    // Transform Public Methods
    //! constructeur par defaut, transformation identite.
    Transform( ) {}
    
    //! construction a partir d'une matrice representee par un tableau 2d de reels.
    Transform( float mat[4][4] )
    {
        m= Matrix4x4(mat);
        mInv = m.getInverse();
    }
    
    //! construction a partir d'une matrice.
    Transform( const Matrix4x4& mat )
    {
        m = mat;
        mInv = m.getInverse();
    }
    
    //! construction a partir d'une matrice et de son inverse.
    Transform( const Matrix4x4& mat, const Matrix4x4& minv )
    {
        m = mat;
        mInv = minv;
    }
    
    //! affiche la matrice representant la transformation.
    void print() const
    {
        m.print();
    }
    
    //! reinitialise la transformation
    void identity( )
    {
        *this= Transform();
    }
    
    //! renvoie la transformation sous forme de matrice.
    const Matrix4x4& matrix( ) const
    {
        return m;
    }
    
    Matrix4x4 transposeMatrix( ) const
    {
        return m.transpose();
    }
    //! renvoie la transformation inverse sous forme de matrice.
    const Matrix4x4& inverseMatrix( ) const
    {
        return mInv;
    }
    
    //! renvoie la matrice de transformation des normales associee a la transformation directe = inverse transpose.
    Matrix4x4 normalMatrix( ) const
    {
        return mInv.transpose();
    }
    
    //! renvoie la transformation inverse.
    Transform inverse( ) const
    {
        return Transform( mInv, m );
    }
    
    //! \name transformations de points, vecteurs, normales, rayons, aabox. passage du repere '1' au repere '2'.
    // @{
    inline Point operator()( const Point &p ) const;
    inline void operator()( const Point &p, Point &pt ) const;

    //! renvoie le point homogene transforme.
    inline void operator()( const Point &p, HPoint &pt ) const;
    inline HPoint operator()( const HPoint &p ) const;
    inline void operator()( const HPoint &p, HPoint &pt ) const;
    
    inline Vector operator()( const Vector &v ) const;
    inline void operator()( const Vector &v, Vector &vt ) const;
    inline Normal operator()( const Normal & ) const;
    inline void operator()( const Normal &, Normal &nt ) const;
    inline Ray operator()( const Ray &r ) const;
    inline void operator()( const Ray &r, Ray &rt ) const;
    BBox operator()( const BBox &b ) const;
    // @}

    //! \name transformations inverses de points, vecteurs, normales, rayons, aabox. passage du repere '2' vers le repere '1'.
    // @{
    inline Point inverse( const Point &p ) const;
    inline void inverse( const Point &p, Point &pt ) const;

    //! renvoie le point homogene transforme.
    inline HPoint inverse( const HPoint &p ) const;
    inline void inverse( const Point &p, HPoint &pt ) const;
    
    inline Vector inverse( const Vector &v ) const;
    inline void inverse( const Vector &v, Vector &vt ) const;
    inline Normal inverse( const Normal & ) const;
    inline void inverse( const Normal &, Normal &nt ) const;
    inline Ray inverse( const Ray &r ) const;
    inline void inverse( const Ray &r, Ray &rt ) const;
    BBox inverse( const BBox &b ) const;
    // @}
    
    //! composition de 2 transformations.
    Transform operator*( const Transform &t2 ) const;
    
    bool swapsHandedness() const;

protected:
    // Transform Private Data
    //! les matrices directe et inverse de changement de repere.
    Matrix4x4 m, mInv;
};

Transform Viewport( float width, float height );
Transform Perspective( float fov, float aspect, float znear, float zfar );
Transform Orthographic( float znear, float zfar );
Transform Orthographic( const float left, const float right, const float bottom, const float top, const float znear, const float zfar );
Transform LookAt( const Point &pos, const Point &look, const Vector &up );
Transform Rotate( float angle, const Vector &axis );
Transform RotateX( float angle );
Transform RotateY( float angle );
Transform RotateZ( float angle );
Transform Scale( float x, float y, float z );
Transform Scale( float value );
Transform Translate( const Vector &delta );


// Transform Inline Functions
inline 
Point Transform::operator()( const Point &p ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    const float xt = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
    const float yt = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
    const float zt = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
    const float wt = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
    
    assert( wt != 0 );
    if( wt == 1.f ) 
        return Point( xt, yt, zt );
    else
        return Point( xt, yt, zt ) / wt;
}


inline 
void Transform::operator()( const Point &p, Point &pt ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    pt.x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
    pt.y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
    pt.z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
    
    const float wt = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
    assert( wt != 0 );
    if( wt != 1.f ) 
        pt /= wt;
}

inline 
void Transform::operator()( const Point &p, HPoint &pt ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    pt.x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3];
    pt.y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3];
    pt.z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3];
    pt.w = m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3];
}

inline 
void Transform::operator()( const HPoint &p, HPoint &pt ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    const float w = p.w;

    pt.x= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3] * w;
    pt.y= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3] * w;
    pt.z= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3] * w; 
    pt.w= m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3] * w;
}

inline 
HPoint Transform::operator()( const HPoint &p ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    const float w = p.w;

    HPoint pt;
    pt.x= m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3] * w;
    pt.y= m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3] * w;
    pt.z= m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3] * w; 
    pt.w= m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3] * w;
    
    return pt;
}

inline 
Vector Transform::operator()( const Vector &v ) const
{
    const float x = v.x;
    const float y = v.y;
    const float z = v.z;
    
    return Vector( 
        m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z,
        m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z,
        m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z);
}

inline 
void Transform::operator()( const Vector &v, Vector &vt ) const
{
    const float x = v.x;
    const float y = v.y;
    const float z = v.z;
    
    vt.x = m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z;
    vt.y = m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z;
    vt.z = m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z;
}

inline 
Normal Transform::operator()( const Normal &n ) const
{
    const float x = n.x;
    const float y = n.y;
    const float z = n.z;
    
    return Normal( 
        mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z,
        mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z,
        mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z);
}

inline 
void Transform::operator()( const Normal &n, Normal& nt ) const
{
    const float x = n.x;
    const float y = n.y;
    const float z = n.z;
    
    nt.x = mInv.m[0][0] * x + mInv.m[1][0] * y + mInv.m[2][0] * z;
    nt.y = mInv.m[0][1] * x + mInv.m[1][1] * y + mInv.m[2][1] * z;
    nt.z = mInv.m[0][2] * x + mInv.m[1][2] * y + mInv.m[2][2] * z;
}

#if 1
inline
BBox Transform::operator()( const BBox &b ) const
{
    // transformation rigide ...
    const Transform &M = *this;
    
    BBox ret( M( Point( b.pMin.x, b.pMin.y, b.pMin.z ) ) );
    ret = Union( ret, M( Point( b.pMax.x, b.pMin.y, b.pMin.z ) ) );
    ret = Union( ret, M( Point( b.pMin.x, b.pMax.y, b.pMin.z ) ) );
    ret = Union( ret, M( Point( b.pMin.x, b.pMin.y, b.pMax.z ) ) );
    ret = Union( ret, M( Point( b.pMin.x, b.pMax.y, b.pMax.z ) ) );
    ret = Union( ret, M( Point( b.pMax.x, b.pMax.y, b.pMin.z ) ) );
    ret = Union( ret, M( Point( b.pMax.x, b.pMin.y, b.pMax.z ) ) );
    ret = Union( ret, M( Point( b.pMax.x, b.pMax.y, b.pMax.z ) ) );
    
    return ret;
}
#endif

inline 
Ray Transform::operator()( const Ray &ray ) const
{
    //~ return Ray( (*this)(ray.o), (*this)(ray.d), ray.tmin, ray.tmax );
    return Ray( (*this)(ray.o), (*this)(ray.d), RAY_EPSILON, ray.tmax );
}

inline 
void Transform::operator()( const Ray &ray, Ray &rt ) const
{
    //~ rt= Ray( (*this)(ray.o), (*this)(ray.d), ray.tmin, ray.tmax );
    rt= Ray( (*this)(ray.o), (*this)(ray.d), RAY_EPSILON, ray.tmax );
}


// inverse Transform Inline Functions
inline 
Point Transform::inverse( const Point &p ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    const float xt = mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z + mInv.m[0][3];
    const float yt = mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z + mInv.m[1][3];
    const float zt = mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z + mInv.m[2][3];
    const float wt = mInv.m[3][0] * x + mInv.m[3][1] * y + mInv.m[3][2] * z + mInv.m[3][3];
    
    assert( wt != 0 );
    if( wt == 1.f ) 
        return Point( xt, yt, zt );
    else
        return Point( xt, yt, zt ) / wt;
}


inline 
void Transform::inverse( const Point &p, Point &pt ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    pt.x = mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z + mInv.m[0][3];
    pt.y = mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z + mInv.m[1][3];
    pt.z = mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z + mInv.m[2][3];
    
    const float wt = mInv.m[3][0] * x + mInv.m[3][1] * y + mInv.m[3][2] * z + mInv.m[3][3];
    assert( wt != 0 );
    if( wt != 1.f ) 
        pt /= wt;
}

inline 
HPoint Transform::inverse( const HPoint &p ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    const float w = p.w;
    
    return HPoint(
        mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z + w * mInv.m[0][3],
        mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z + w * mInv.m[1][3],
        mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z + w * mInv.m[2][3],
        mInv.m[3][0] * x + mInv.m[3][1] * y + mInv.m[3][2] * z + w * mInv.m[3][3]);
}

inline 
void Transform::inverse( const Point &p, HPoint &pt ) const
{
    const float x = p.x;
    const float y = p.y;
    const float z = p.z;
    
    pt.x = mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z + mInv.m[0][3];
    pt.y = mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z + mInv.m[1][3];
    pt.z = mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z + mInv.m[2][3];
    pt.w = mInv.m[3][0] * x + mInv.m[3][1] * y + mInv.m[3][2] * z + mInv.m[3][3];
}

inline 
Vector Transform::inverse( const Vector &v ) const
{
    const float x = v.x;
    const float y = v.y;
    const float z = v.z;
    
    return Vector( 
        mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z,
        mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z,
        mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z);
}

inline 
void Transform::inverse( const Vector &v, Vector &vt ) const
{
    const float x = v.x;
    const float y = v.y;
    const float z = v.z;
    
    vt.x = mInv.m[0][0] * x + mInv.m[0][1] * y + mInv.m[0][2] * z;
    vt.y = mInv.m[1][0] * x + mInv.m[1][1] * y + mInv.m[1][2] * z;
    vt.z = mInv.m[2][0] * x + mInv.m[2][1] * y + mInv.m[2][2] * z;
}

inline 
Normal Transform::inverse( const Normal &n ) const
{
    const float x = n.x;
    const float y = n.y;
    const float z = n.z;
    
    return Normal( 
        m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z,
        m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z,
        m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z);
}

inline 
void Transform::inverse( const Normal &n, Normal& nt ) const
{
    const float x = n.x;
    const float y = n.y;
    const float z = n.z;
    
    nt.x = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z;
    nt.y = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z;
    nt.z = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z;
}

#if 1
inline
BBox Transform::inverse( const BBox &b ) const
{
    // transformation rigide ...
    const Transform &M = *this;
    
    BBox ret( M.inverse( Point( b.pMin.x, b.pMin.y, b.pMin.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMax.x, b.pMin.y, b.pMin.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMin.x, b.pMax.y, b.pMin.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMin.x, b.pMin.y, b.pMax.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMin.x, b.pMax.y, b.pMax.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMax.x, b.pMax.y, b.pMin.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMax.x, b.pMin.y, b.pMax.z ) ) );
    ret = Union( ret, M.inverse( Point( b.pMax.x, b.pMax.y, b.pMax.z ) ) );
    
    return ret;
}
#endif

inline 
Ray Transform::inverse( const Ray &ray ) const
{
    //~ return Ray( (*this).inverse(ray.o), (*this).inverse(ray.d), ray.tmin, ray.tmax );
    return Ray( (*this).inverse(ray.o), (*this).inverse(ray.d), RAY_EPSILON, ray.tmax );
}

inline 
void Transform::inverse( const Ray &ray, Ray &rt ) const
{
    //~ rt= Ray( (*this).inverse(ray.o), (*this).inverse(ray.d), ray.tmin, ray.tmax );
    rt= Ray( (*this).inverse(ray.o), (*this).inverse(ray.d), RAY_EPSILON, ray.tmax );
}
// @}

} // namespace

#endif // PBRT_TRANSFORM_H
