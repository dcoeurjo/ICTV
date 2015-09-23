/* version restructuree mailto:jean-claude.iehl@liris.cnrs.fr */

/*
 * pbrt source code Copyright(c) 1998-2005 Matt Pharr and Greg Humphreys
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 * (See file License.txt for complete license)
 */

#include <algorithm>
#include <iostream>
#include <cstring>

#include "Transform.h"

namespace gk {

// Matrix4x4 Methods Definitions
Matrix4x4::Matrix4x4( const float mat[4][4] ) : Mat4(mat) {}

Matrix4x4::Matrix4x4(
    float t00, float t01, float t02, float t03,
    float t10, float t11, float t12, float t13,
    float t20, float t21, float t22, float t23,
    float t30, float t31, float t32, float t33)
    :
    Mat4(
    t00, t01, t02, t03,
    t10, t11, t12, t13,
    t20, t21, t22, t23,
    t30, t31, t32, t33)
{}

Matrix4x4 Matrix4x4::transpose() const 
{
    return Matrix4x4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]);
}

Matrix4x4 Matrix4x4::getInverse() const 
{
    int indxc[4], indxr[4];
    int ipiv[4] = { 0, 0, 0, 0 };
    
    float minv[4][4];
    memcpy(minv, m, sizeof(float[16]));
    
    for (int i = 0; i < 4; i++) {
        int irow = -1, icol = -1;
        float big = 0.f;
        
        // Choose pivot
        for (int j = 0; j < 4; j++) {
            if (ipiv[j] != 1) {
                for (int k = 0; k < 4; k++) {
                    if (ipiv[k] == 0) {
                        if (fabsf(minv[j][k]) >= big) {
                            big = float(fabsf(minv[j][k]));
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1)
                        printf("Singular matrix in Matrix4x4::getInverse()\n");
                }
            }
        }
        
        ++ipiv[icol];
        // Swap rows _irow_ and _icol_ for pivot
        if (irow != icol) {
            for (int k = 0; k < 4; ++k)
                std::swap(minv[irow][k], minv[icol][k]);
        }
        
        indxr[i] = irow;
        indxc[i] = icol;
        if (minv[icol][icol] == 0.)
            printf("Singular matrix in Matrix4x4::getInverse()\n");
        
        // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
        float pivinv = 1.f / minv[icol][icol];
        minv[icol][icol] = 1.f;
        for (int j = 0; j < 4; j++)
            minv[icol][j] *= pivinv;
        
        // Subtract this row from others to zero out their columns
        for (int j = 0; j < 4; j++) {
            if (j != icol) {
                float save = minv[j][icol];
                minv[j][icol] = 0;
                for (int k = 0; k < 4; k++)
                    minv[j][k] -= minv[icol][k]*save;
            }
        }
    }
    
    // Swap columns to reflect permutation
    for (int j = 3; j >= 0; j--) {
        if (indxr[j] != indxc[j]) {
            for (int k = 0; k < 4; k++)
                std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
        }
    }
    
    return Matrix4x4(minv);
}


// Transform Method Definitions

//! renvoie la transformation associee a une translatation du vecteur delta.
Transform Translate( const Vector &delta )
{
    Matrix4x4 m(
        1, 0, 0, delta.x,
        0, 1, 0, delta.y,
        0, 0, 1, delta.z,
        0, 0, 0,       1 );
    
    Matrix4x4 minv( 
        1, 0, 0, -delta.x,
        0, 1, 0, -delta.y,
        0, 0, 1, -delta.z,
        0, 0, 0,        1 );
    
    return Transform( m, minv );
}

//! renvoie la transformation associee au changement d'echelle (x, y, z).
Transform Scale( float x, float y, float z )
{
    Matrix4x4 m(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1 );
    
    Matrix4x4 minv(
        1.f / x,       0,       0, 0,
              0, 1.f / y,       0, 0,
              0,       0, 1.f / z, 0,
              0,       0,       0, 1 );
    
    return Transform( m, minv );
}

//! renvoie la transformation associee au changement d'echelle (v, v, v).
Transform Scale( float v )
{
    Matrix4x4 m(
        v, 0, 0, 0,
        0, v, 0, 0,
        0, 0, v, 0,
        0, 0, 0, 1 );
    
    Matrix4x4 minv(
        1.f / v,       0,       0, 0,
              0, 1.f / v,       0, 0,
              0,       0, 1.f / v, 0,
              0,       0,       0, 1 );
    
    return Transform( m, minv );
}


//! renvoie la transformation associee a une rotation autour de l'axe X, angle est en degres.
Transform RotateX( float angle )
{
    const float sin_t = sinf( Radians( angle ) );
    const float cos_t = cosf( Radians( angle ) );
    
    Matrix4x4 m(
        1,     0,      0, 0,
        0, cos_t, -sin_t, 0,
        0, sin_t,  cos_t, 0,
        0,     0,      0, 1 );

    return Transform( m, m.transpose() );
}

//! renvoie la transformation associee a une rotation autour de l'axe Y, angle est en degres.
Transform RotateY( float angle )
{
    const float sin_t = sinf( Radians( angle ) );
    const float cos_t = cosf( Radians( angle ) );
    
    Matrix4x4 m(
         cos_t,   0, sin_t, 0,
             0,   1,     0, 0,
        -sin_t,   0, cos_t, 0,
             0,   0,     0, 1 );
    
    return Transform( m, m.transpose() );
}

//! renvoie la transformation associee a une rotation autour de l'axe Z, angle est en degres.
Transform RotateZ( float angle )
{
    const float sin_t = sinf( Radians( angle ) );
    const float cos_t = cosf( Radians( angle ) );
    
    Matrix4x4 m( 
        cos_t, -sin_t, 0, 0,
        sin_t,  cos_t, 0, 0,
            0,      0, 1, 0,
            0,      0, 0, 1 );
    
    return Transform( m, m.transpose() );
}

//! renvoie la transformation associee a une rotation autour d'un vecteur, angle est en degres.
Transform Rotate( float angle, const Vector &axis )
{
    const Vector a = Normalize( axis );
    const float s = sinf( Radians( angle ) );
    const float c = cosf( Radians( angle ) );
    
    float m[4][4];
    
    m[0][0] = a.x * a.x + ( 1.f - a.x * a.x ) * c;
    m[0][1] = a.x * a.y * ( 1.f - c ) - a.z * s;
    m[0][2] = a.x * a.z * ( 1.f - c ) + a.y * s;
    m[0][3] = 0;
    
    m[1][0] = a.x * a.y * ( 1.f - c ) + a.z * s;
    m[1][1] = a.y * a.y + ( 1.f - a.y * a.y ) * c;
    m[1][2] = a.y * a.z * ( 1.f - c ) - a.x * s;
    m[1][3] = 0;
    
    m[2][0] = a.x * a.z * ( 1.f - c ) - a.y * s;
    m[2][1] = a.y * a.z * ( 1.f - c ) + a.x * s;
    m[2][2] = a.z * a.z + ( 1.f - a.z * a.z ) * c;
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
    Matrix4x4 mat( m );
    return Transform( mat, mat.transpose() );
}

//! renvoie la transformation camera (View).
Transform LookAt( const Point &pos, const Point &look, const Vector &up )
{
    float m[4][4];

#if 0
    // import from pbrt-v2 
    // Initialize first three columns of viewing matrix
    Vector dir = Normalize(look - pos);
    Vector left = Cross(Normalize(up), dir);
    Vector newUp = Cross(dir, left);
    m[0][0] = left.x;
    m[1][0] = left.y;
    m[2][0] = left.z;
    m[3][0] = 0.f;
    m[0][1] = newUp.x;
    m[1][1] = newUp.y;
    m[2][1] = newUp.z;
    m[3][1] = 0.f;
    m[0][2] = dir.x;
    m[1][2] = dir.y;
    m[2][2] = dir.z;
    m[3][2] = 0.f;
#endif

#if 1
    // pbrt-v1 
    // Initialize first three columns of viewing matrix
    Vector dir = Normalize( Vector(pos, look) );
    Vector right = Normalize( Cross(dir, Normalize(up)) );
    Vector newUp = Normalize( Cross(right, dir) );
    m[0][0] = right.x;
    m[1][0] = right.y;
    m[2][0] = right.z;
    m[3][0] = 0.f;
    m[0][1] = newUp.x;
    m[1][1] = newUp.y;
    m[2][1] = newUp.z;
    m[3][1] = 0.f;
    m[0][2] = -dir.x;   // opengl convention, look down the negative z axis
    m[1][2] = -dir.y;
    m[2][2] = -dir.z;
    m[3][2] = 0.f;
#endif

    // Initialize fourth column of viewing matrix
    m[0][3] = pos.x;
    m[1][3] = pos.y;
    m[2][3] = pos.z;
    m[3][3] = 1.f;

    Matrix4x4 camToWorld(m);
    return Transform( camToWorld.getInverse(), camToWorld );
}

#if 0 // inline
BBox Transform::operator()( const BBox &b ) const
{
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

Transform Transform::operator*( const Transform &t2 ) const
{
    Matrix4x4 m1 = Matrix4x4::mul( m, t2.m );
    Matrix4x4 m2 = Matrix4x4::mul( t2.mInv, mInv );
    
    return Transform( m1, m2 );
}

bool Transform::swapsHandedness() const
{
    const float det = 
        (( m.m[0][0] *
           ( m.m[1][1] * m.m[2][2] -
             m.m[1][2] * m.m[2][1] ) ) -
         ( m.m[0][1] *
           ( m.m[1][0] * m.m[2][2] -
             m.m[1][2] * m.m[2][0] ) ) +
         ( m.m[0][2] *
           ( m.m[1][0] * m.m[2][1] -
             m.m[1][1] * m.m[2][0] ) )
        );
    
    return (det < 0.f);
}

//! renvoie la transformation associee a une camera orthographique (Projection)
Transform Orthographic( float znear, float zfar )
{
    // pbrt version
    return Scale( 1.f, 1.f, 1.f / ( zfar - znear ) ) * Translate( Vector( 0.f, 0.f, -znear ) );
}

//! renvoie la transformation associee a une camera orthographique (Projection)
Transform Orthographic( const float left, const float right, 
    const float bottom, const float top, 
    const float znear, const float zfar )
{
    // opengl version
    Matrix4x4 ortho(
        2.f / (right - left)     , 0.f                 , 0.f                  , -(right + left) / (right - left),
        0.f                      , 2.f / (top - bottom), 0.f                  , -(top + bottom) / (top - bottom),
        0.f                      , 0.f                 , -2.f / (zfar - znear), -(zfar + znear) / (zfar - znear),
        0.f, 0.f, 0.f, 1.f
    );
    
    return Transform(ortho);
}


//! renvoie la transformation associee a une camera perspective (Projection).
Transform Perspective( float fov, float aspect, float znear, float zfar )
{
#if 0
    // Perform projective divide, pbrt version
    float inv_denom = 1.f / ( zfar - znear );
    Matrix4x4 persp( 
        1, 0,           0,              0,
        0, 1,           0,              0,
        0, 0, zfar*inv_denom, -zfar*znear*inv_denom,
        0, 0,           1,              0 
    );
    
    // Scale to canonical viewing volume
    float invTanAng = 1.f / tanf( Radians( fov ) / 2.f );
    return Scale( invTanAng, invTanAng, 1 ) * Transform( persp );

#else
    // perspective, openGL version
    const float inv_tan = 1.f / tanf( Radians( fov ) / 2.f );
    const float inv_denom = 1.f / ( znear - zfar );
    Matrix4x4 persp( 
        inv_tan/aspect,       0,                    0,                      0,
                     0, inv_tan,                    0,                      0,
                     0,       0, (zfar+znear)*inv_denom, 2.f*zfar*znear*inv_denom,
                     0,       0,                   -1,                      0
    );

    return Transform(persp);
#endif
}

//! renvoie la transformation entre le repere projectif de la camera et le repere fenetre (Viewport).
Transform Viewport( float width, float height )
{
    const float w= width / 2.f;
    const float h= height / 2.f;
    
    Matrix4x4 viewport(
        w, 0,   0,   w,
        0, h,   0,   h,
        0, 0, .5f, .5f,
        0, 0,   0,   1
    );
    
    return Transform(viewport);
}

} // namespace
