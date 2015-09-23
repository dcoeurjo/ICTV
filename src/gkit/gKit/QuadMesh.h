
#ifndef _QUAD_MESH_H
#define _QUAD_MESH_H

#include <vector>
#include <string>

#include "Vec.h"
#include "Quad.h"
#include "Mesh.h"
#include "MeshData.h"


namespace gk {

//! representation d'un ensemble de quads eventuellement associes a des matieres.
struct QuadMesh
{
    std::string filename;               //!< nom du fichier.
    
    std::vector<Vec3> positions;        //!< position des sommets.
    std::vector<Vec3> texcoords;        //!< coordonnees de textures.
    std::vector<Vec3> normals;          //!< normales des sommets.
    
    std::vector<Face> faces;            //!< adjacence 
    std::vector<AFace> afaces;          //!< adjacence 
    std::vector<int> materials;         //!< indice du groupe par quad / face.
    
    std::vector<MeshGroup> groups;      //!< groupes de faces associes a une matiere.
    
    BBox box;                           //!< englobant des sommets.
    
    //! constructeur.
    QuadMesh( )
        :
        filename(),
        positions(), texcoords(), normals(),
        faces(), afaces(), 
        materials(), groups()
    {}

    int buildNormals( );
    
    //! renvoie la matiere d'un quad du mesh.
    MeshMaterial quadMaterial( const unsigned int id )
    {
        if(id > materials.size() || materials[id] < 0)
            return MeshMaterial();      // renvoie une matiere par defaut
        
        return groups[materials[id]].material;
    }
    
    //! renvoie un quad du mesh.
    Quad quad( const unsigned int id ) const
    {
        if(id >= faces.size())
            return Quad();
        
        return Quad(
            Point(positions[faces[id].vertex[0].position]), 
            Point(positions[faces[id].vertex[1].position]),
            Point(positions[faces[id].vertex[2].position]),
            Point(positions[faces[id].vertex[3].position]), id);
    }
};

}       // namespace

#endif
