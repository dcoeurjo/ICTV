/* version restructuree mailto:jean-claude.iehl@liris.cnrs.fr */

/*
 * pbrt source code Copyright(c) 1998-2005 Matt Pharr and Greg Humphreys
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 * (See file License.txt for complete license)
 */

#include "Geometry.h"

namespace gk {

// BBox Method Definitions
//! ajoute un point a l'aabox et renvoie la nouvelle aabox.
BBox Union( const BBox &b, const Point &p )
{
    BBox ret = b;
    ret.pMin.x = std::min(b.pMin.x, p.x);
    ret.pMin.y = std::min(b.pMin.y, p.y);
    ret.pMin.z = std::min(b.pMin.z, p.z);
    ret.pMax.x = std::max(b.pMax.x, p.x);
    ret.pMax.y = std::max(b.pMax.y, p.y);
    ret.pMax.z = std::max(b.pMax.z, p.z);
    
    return ret;
}

//! ajoute une aabox dans une autre et renvoie la nouvelle aabox.
BBox Union( const BBox &b, const BBox &b2 )
{
    BBox ret;
    ret.pMin.x = std::min(b.pMin.x, b2.pMin.x);
    ret.pMin.y = std::min(b.pMin.y, b2.pMin.y);
    ret.pMin.z = std::min(b.pMin.z, b2.pMin.z);
    ret.pMax.x = std::max(b.pMax.x, b2.pMax.x);
    ret.pMax.y = std::max(b.pMax.y, b2.pMax.y);
    ret.pMax.z = std::max(b.pMax.z, b2.pMax.z);
    
    return ret;
}

BBox Intersection( const BBox& a, const BBox& b )
{
    BBox ret;
    ret.pMin.x= std::max(a.pMin.x, b.pMin.x);
    ret.pMin.y= std::max(a.pMin.y, b.pMin.y);
    ret.pMin.z= std::max(a.pMin.z, b.pMin.z);
    ret.pMax.x= std::min(a.pMax.x, b.pMax.x);
    ret.pMax.y= std::min(a.pMax.y, b.pMax.y);
    ret.pMax.z= std::min(a.pMax.z, b.pMax.z);        
    
    return ret;
}

void BBox::BoundingSphere( Point& c, float& r ) const
{
    c= .5f * (pMin + pMax);
    r= Inside(c) ? Distance(c, pMax) : 0.f;
}

} // namespace
