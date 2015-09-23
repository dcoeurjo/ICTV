
#ifndef _GLSL_UNIFORMS_H
#define _GLSL_UNIFORMS_H

#include <cstdio>
#include <vector>

#include "Vec.h"

namespace gk {

//! representation des types glsl pour simplifier la construction des uniform buffers et les affectations.
//! \ingroup OpenGL.
namespace glsl {        // std140 / 430

#ifdef _MSC_VER   // visual studio >= 2012 necessaire
# define ALIGN(...) __declspec(align(__VA_ARGS__))

#else   // gcc, clang, icc
# define ALIGN(...) __attribute__((aligned(__VA_ARGS__)))
#endif



//! scalar bool, int, uint, float
template < typename T >
struct ALIGN(4) gscalar
{
    T value;

    gscalar( const T& v= 0 ) : value(v) {}
    gscalar& operator= ( const T& v ) { value= v; return *this; }

    operator T( ) { return value; }
    operator T( ) const { return value; }

    bool operator== ( const T& b ) { return (value == b); }
    bool operator!= ( const T& b ) { return !((*this) == b); }
    //~ bool operator== ( const gscalar<T>& b ) const{ return (value == b.value); }
    //~ bool operator!= ( const gscalar<T>& b ) const { return !((*this) == b); }
};

//! vec2, ivec2, uvec2, bvec2
template < typename T >
struct ALIGN(8) gvec2
{
    gscalar<T> x;
    gscalar<T> y;

    gvec2( const T& v ) : x(v), y(v) {}
    gvec2( const T& _x= 0, const T& _y= 0 ) : x(_x), y(_y) {}
    gvec2( const TVec2<T>& v ) : x(v.x), y(v.y) {}

    operator const T *( ) const { return &x.value; }
    operator T *( ) { return &x.value; }
    bool operator== ( const gvec2<T>& b ) const { return (x == b.x) && (y == b.y); }
    bool operator!= ( const gvec2<T>& b ) const { return !((*this) == b); }
};

//! vec3, ivec3, uvec3, bvec3
template < typename T >
struct ALIGN(16) gvec3
{
    gscalar<T> x;
    gscalar<T> y;
    gscalar<T> z;

    gvec3( const T& v ) : x(v), y(v), z(v) {}
    gvec3( const gvec2<T>& v, const T& _z= 0 ) : x(v.x), y(v.y), z(_z) {}
    gvec3( const T& _x= 0, const T& _y= 0, const T& _z= 0 ) : x(_x), y(_y), z(_z) {}
    gvec3( const TVec3<T>& v ) : x(v.x), y(v.y), z(v.z) {}

    operator const T *( ) const { return &x.value; }
    operator T *( ) { return &x.value; }
    bool operator== ( const gvec3<T>& b ) const { return (x == b.x) && (y == b.y); }
    bool operator!= ( const gvec3<T>& b ) const { return !((*this) == b); }
};

//! vec4, ivec4, uvec4, bvec4
template < typename T >
struct ALIGN(16) gvec4
{
    gscalar<T> x;
    gscalar<T> y;
    gscalar<T> z;
    gscalar<T> w;

    gvec4( const T& v ) : x(v), y(v), z(v), w(v) {}
    gvec4( const gvec2<T>& v, const T& _z= 0, const T& _w= 0 ) : x(v.x), y(v.y), z(_z), w(_w) {}
    gvec4( const gvec3<T>& v, const T& _w= 0 ) : x(v.x), y(v.y), z(v.z), w(_w) {}
    gvec4( const T& _x= 0, const T& _y= 0, const T& _z= 0, const T& _w= 0 ) : x(_x), y(_y), z(_z), w(_w) {}
    gvec4( const TVec4<T>& v ) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    operator const T *( ) const { return &x.value; }
    operator T *( ) { return &x.value; }
    bool operator== ( const gvec4<T>& b ) const { return (x == b.x) && (y == b.y); }
    bool operator!= ( const gvec4<T>& b ) const { return !((*this) == b); }
};


//! row major matrix
template < typename T, unsigned int R >  // inutile ??
struct grmat
{
    T row[R];

    T& operator[]( const unsigned int id ) { return row[id]; }
    const T& operator[]( const unsigned int id ) const { return row[id]; }

    operator const T *( ) const { return &row[0].x.x; }
    operator T *( ) { return &row[0].x.x; }
};

template< typename T >
struct grmat<T, 2u>
{
    T row[2];

    grmat( const T& a= T(), const T& b= T() ) { row[0]= a; row[1]= b; }

    T& operator[]( const unsigned int id ) { return row[id]; }
    const T& operator[]( const unsigned int id ) const { return row[id]; }

    operator const T *( ) const { return &row[0].x.x; }
    operator T *( ) { return &row[0].x.x; }
};

template< typename T >
struct grmat<T, 3u>
{
    T row[3];

    grmat( const T& a= T(), const T& b= T(), const T& c= T() ) { row[0]= a; row[1]= b; row[2]= c; }

    T& operator[]( const unsigned int id ) { return row[id]; }
    const T& operator[]( const unsigned int id ) const { return row[id]; }

    operator const T *( ) const { return &row[0].x.x; }
    operator T *( ) { return &row[0].x.x; }
};

template< typename T >
struct grmat<T, 4u>
{
    T row[4];

    grmat( const T& a= T(), const T& b= T(), const T& c= T(), const T& d= T() ) { row[0]= a; row[1]= b; row[2]= c; row[3]= d; }

    T& operator[]( const unsigned int id ) { return row[id]; }
    const T& operator[]( const unsigned int id ) const { return row[id]; }

    operator const T *( ) const { return &row[0].x.x; }
    operator T *( ) { return &row[0].x.x; }
};

//! column major matrix
template < typename T, unsigned int C >
struct gcmat
{
    T column[C];

    T& operator[]( const unsigned int id ) { return column[id]; }
};

//! row major matrix mul
template < typename T, unsigned int R >
grmat<T,R> grmul( const grmat<T,R>& a, const grmat<T,R>& b )
{
    grmat<T,R> m;
    for(unsigned int r= 0; r < R; r++)
        for(unsigned int c= 0; c < R; c++)
            for(unsigned int k= 0; k < R; k++)
                m[r][c]+= a[r][k] * b[k][c];

    return m;
}

//! row major matrix transpose
template < typename T, unsigned int R >
grmat<T,R> grtranspose( const grmat<T,R>& a )
{
    grmat<T,R> m;
    for(unsigned int r= 0; r < R; r++)
        for(unsigned int c= 0; c < R; c++)
            m[r][c]= a[c][r];

    return m;
}


//! user types
typedef gscalar<bool> bscalar;
typedef gvec2<bool> bvec2;
typedef gvec3<bool> bvec3;
typedef gvec4<bool> bvec4;

typedef gscalar<unsigned int> uscalar;
typedef gvec2<unsigned int> uvec2;
typedef gvec3<unsigned int> uvec3;
typedef gvec4<unsigned int> uvec4;

typedef gscalar<int> iscalar;
typedef gvec2<int> ivec2;
typedef gvec3<int> ivec3;
typedef gvec4<int> ivec4;

typedef gscalar<float> scalar;
typedef gvec2<float> vec2;
typedef gvec3<float> vec3;
typedef gvec4<float> vec4;

typedef grmat< gvec4<float>, 4 > mat4;
typedef grmat< gvec4<float>, 4 > matrix;
typedef grmat< gvec3<float>, 3 > mat3;
typedef grmat< gvec2<float>, 2 > mat2;

}       // namespace glsl
}       // namespace

#endif
