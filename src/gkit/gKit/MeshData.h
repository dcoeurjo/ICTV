
#ifndef _MESH_DATA_H
#define _MESH_DATA_H

#include <vector>
#include <map>

#include "Vec.h"
#include "Geometry.h"
#include "Mesh.h"


namespace gk {

//! utilisation interne. representation d'un sommet / vertex avec un index par attribut.
struct Vertex
{
    int position;
    int texcoord;
    int normal;
    int material;       //!< utilisation interne... a deplacer dans OBJ.
    
    Vertex( ) : position(-1), texcoord(-1), normal(-1), material(-1) {}
    Vertex( const int _p, const int _t, const int _n, const int _mat ) : position(_p), texcoord(_t), normal(_n), material(_mat) {}
    
    bool operator< ( const Vertex& b ) const
    {
        if(material != b.material)
            return (material < b.material);
        
        // ordre lexicographique pour comparer le triplet d'indices
        if(position != b.position)
            return (position < b.position);
        if(texcoord != b.texcoord)
            return (texcoord < b.texcoord);
        return (normal < b.normal);
    }
};

//! representation d'une face. un ensemble de sommets. (max 4).
struct Face
{
    Face( ) : n(0) {}
    
    void push_back( const Vertex& _vertex ) { assert(n < 4); vertex[n]= _vertex; n++; }
    
    Vertex vertex[4];          //!< sommets de la face.
    unsigned int n;             //!< nombre de sommets de la face.
};

//! representation de l'adjacence d'une face.
struct AFace
{
    int faces[4];               //!< face opposee a chaque arete.
    signed char edges[4];       //!< orientation de l'arete opposee.
    int n;                      //! nombre de sommets / d'aretes de la face.
    
    AFace( ) : n(0)
    {
        faces[0]= -1; faces[1]= -1; faces[2]= -1; faces[3]= -1; 
        edges[0]= -1; edges[1]= -1; edges[2]= -1; edges[3]= -1;
    }
};


//! representation des donnees d'un maillage.
struct MeshData
{
    std::vector<Vec3> positions;
    std::vector<Vec3> texcoords;
    std::vector<Vec3> normals;
    
    std::vector<Face> faces;
    std::vector<AFace> afaces;
    std::vector<MeshMaterial> materials;
    
    BBox box;
    
    bool has_normal;
    bool has_texcoord;
    bool has_material;
    
    MeshData( ) : positions(), texcoords(), normals(), faces(), afaces(), materials(), box(), has_normal(true), has_texcoord(true), has_material(true) {}
};


//! construit les faces adjacentes d'un maillage.
int buildFaces( MeshData& mesh );

//! identifie les sommets dupliques et construit des triangles indexes.
int buildIndex( const std::vector<Vec3>& points, MeshData& mesh );

//! triangule un ensemble de faces.
void triangulate( std::vector<Face>& faces );

//! comptabilise les triangles/quads presents dans un ensemble de faces.
void faceCount( const std::vector<Face>& faces, unsigned int &triangles, unsigned int &quads, unsigned int &others );

//! analyse un fichier .obj et construit un ensemble de faces.
int readOBJ( const std::string& filename, MeshData& mesh );

//! analyse un fichier .mtl et construit un ensemble de description de matieres.
int readMTL( const std::string& filename, std::vector<MeshMaterial>& materials );

}       // namespace 

#endif
