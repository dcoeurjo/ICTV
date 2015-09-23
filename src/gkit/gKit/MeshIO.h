
#ifndef _MINI_OBJ_H
#define _MINI_OBJ_H

#include <vector>


namespace gk {

struct Mesh;
struct QuadMesh;
struct MeshMaterial;

namespace MeshIO {
    
//! charge un fichier .OBJ et construit un index buffer lineaire + les groupes de faces associes a chaque matiere.
    //! \todo renvoyer Mesh::null() en cas d'echec
Mesh *readOBJ( const std::string& filename );
    
//! charge un fichier .OBJ compose de quads.
QuadMesh *readQuadOBJ( const std::string& filename );

//! charge un fichier .MTL et ajoute l'ensemble de matieres lues a materials.
int readMTL( const std::string& filename, std::vector<MeshMaterial>& materials );
    
//! recalcule les normales moyennes des sommets.
int buildNormals( Mesh *mesh );

}       // namespace

}       // namespace

#endif
