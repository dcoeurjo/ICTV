
#ifndef _MESH_H
#define _MESH_H

#include <vector>
#include <string>

#include "Vec.h"
#include "Triangle.h"


namespace gk {

//! description d'une matiere.
//! R= kd * diffuse_color * diffuse_texture + ks * specular_color * specular_texture.
struct MeshMaterial
{
    std::string name;                   //!< nom de la matiere.
    
    std::string ambient_texture;        //!< nom de l'image a charger.
    std::string diffuse_texture;        //!< nom de l'image a charger.
    std::string specular_texture;       //!< nom de l'image a charger.
    
    VecColor ambient_color;             //!< couleur diffuse.
    VecColor diffuse_color;             //!< couleur diffuse.
    VecColor specular_color;            //!< couleur speculaire.
    
    VecColor emission;                  //!< energie emise par une source de lumiere.
    
    float ka;                           //!< influence du comportement ambiant.
    float kd;                           //!< influence du comportement diffus.
    float ks;                           //!< influence du comportement speculaire.
    float ns;                           //!< comportement speculaire, ks * cos**ns
    float ni;                           //!< indice de refraction, objets transparents
    
    //! constructeur par defaut.
    MeshMaterial( )
        :
        name("default"),
        ambient_texture(), diffuse_texture(), specular_texture(),
        ambient_color(0.f, 0.f, 0.f, 1.f), diffuse_color(.8f, .8f, .8f, 1.f), specular_color(.0f, .0f, .0f, 1.f), emission(.0f, .0f, .0f, 1.f),
        ka(.0f), kd(1.f), ks(.0f)
    {}
    
    //! construction d'une matiere nommee.
    MeshMaterial( const char *_name )
        :
        name(_name),
        ambient_texture(), diffuse_texture(), specular_texture(),
        ambient_color(), diffuse_color(), specular_color(), emission(),
        ka(.0f), kd(.0f), ks(.0f)
    {}
};


//! representation d'un groupe de faces associe a une matiere.
struct MeshGroup
{
    MeshMaterial material;
    unsigned int begin;
    unsigned int end;
    
    MeshGroup( ) : material(), begin(0), end(0) {}
    MeshGroup( const MeshMaterial&  _material, const unsigned int _begin, const unsigned int _end= 0 )
        : material(_material), begin(_begin), end(std::max(_begin, _end)) {}
};


//! representation d'un ensemble de triangles eventuellement associes a des matieres.
struct Mesh
{
    std::string filename;               //!< nom du fichier.
    
    std::vector<Vec3> positions;        //!< position des sommets.
    std::vector<Vec3> texcoords;        //!< coordonnees de textures.
    std::vector<Vec3> normals;          //!< normales des sommets.
    
    std::vector<unsigned int> indices;   //!< indexation des sommets de chaque triangle, numTriangles = indices.size() / 3.
    
    std::vector<MeshGroup> groups;      //!< groupes de faces associes a une matiere.
    std::vector<int> materials;         //!< groupes / matieres des triangles.
    
    BBox box;                           //!< englobant des sommets.
    
    //! constructeur.
    Mesh( )
        :
        filename(),
        positions(), texcoords(), normals(),
        indices(), groups(), materials()
    {}
    
    //! renvoie le nombre de triangles du maillage.
    int triangleCount( ) const
    {
        return (int) indices.size() / 3; 
    }
    
    //! renvoie la matiere associee au triangle id.
    MeshMaterial triangleMaterial( const unsigned id ) const
    {
        if(id > materials.size() || materials[id] < 0)
            return MeshMaterial();      // renvoie une matiere par defaut
        
        return groups[materials[id]].material;
    }
    
    //! renvoie un triangle.
    Triangle triangle( const unsigned int id ) const
    {
        assert(3u*id < indices.size());
        
        Point a, b, c;
        unsigned int ia= indices[3u*id];
        unsigned int ib= indices[3u*id +1u];
        unsigned int ic= indices[3u*id +2u];
        if(ia < positions.size()) a= Point(positions[ia]);
        if(ib < positions.size()) b= Point(positions[ib]);
        if(ic < positions.size()) c= Point(positions[ic]);
        
        return Triangle( a, b, c, id );
    }
    
    //! renvoie un pn triangle. (position + normale par sommet).
    PNTriangle pntriangle( const unsigned int id ) const
    { 
        assert(3u * id < indices.size());
        
        Normal a, b, c;
        unsigned int ia= indices[3u*id];
        unsigned int ib= indices[3u*id +1u];
        unsigned int ic= indices[3u*id +2u];
        if(ia < normals.size()) a= Normal(normals[ia]);
        if(ib < normals.size()) b= Normal(normals[ib]);
        if(ic < normals.size()) c= Normal(normals[ic]);
        
        Triangle abc= triangle(id);
        return PNTriangle(abc, a, b, c);
    }
    
    //! renvoie un ptn triangle. (position + texcoord + normale par sommet).
    PTNTriangle ptntriangle( const unsigned int id ) const
    { 
        assert(3u * id < indices.size());
        
        Point a, b, c;
        unsigned int ia= indices[3u*id];
        unsigned int ib= indices[3u*id +1u];
        unsigned int ic= indices[3u*id +2u];
        if(ia < texcoords.size()) a= Point(texcoords[ia]);
        if(ib < texcoords.size()) b= Point(texcoords[ib]);
        if(ic < texcoords.size()) c= Point(texcoords[ic]);
        
        PNTriangle abc= pntriangle(id);
        return PTNTriangle(abc, a, b, c);
    }
};


//! ajoute un sommet dans un objet Mesh. renvoie son indice.
inline
unsigned int pushVertex( Mesh *mesh, const Vec3 position )
{
    assert(mesh != NULL);
    mesh->positions.push_back(position);
    return mesh->positions.size() -1;
}

//! ajoute une normale dans un objet Mesh. renvoie son indice.
inline
unsigned int pushNormal( Mesh *mesh, const Vec3 normal )
{
    assert(mesh != NULL);
    mesh->normals.push_back(normal);
    return mesh->normals.size() -1;
}

//! ajoute une coordonnee de texture dans un objet Mesh. renvoie son indice.
inline
unsigned int pushTexcoord( Mesh *mesh, const Vec3 texcoord )
{
    assert(mesh != NULL);
    mesh->texcoords.push_back(texcoord);
    return mesh->texcoords.size() -1;
}

//! ajoute une coordonnee de texture dans un objet Mesh. renvoie son indice.
inline
unsigned int pushTexcoord( Mesh *mesh, const Vec2 texcoord ) 
{
    return pushTexcoord(mesh, Vec3(texcoord.x, texcoord.y, 0.f)); 
}

//! ajoute un sommet et ses attributs dans un objet Mesh. renvoie son indice.
inline
unsigned int pushPTNVertex( Mesh *mesh, const Vec3 position, const Vec3 texcoord, const Vec3 normal )
{
    assert(mesh != NULL);
    mesh->positions.push_back(position);
    mesh->texcoords.push_back(texcoord);
    mesh->normals.push_back(normal);
    
    assert("invalid mesh" && mesh->positions.size() == mesh->normals.size() && mesh->positions.size() == mesh->texcoords.size());
    return mesh->positions.size() -1;
}

//! ajoute les indices des sommets d'un triangle abc dans un objet Mesh.
inline
unsigned int pushTriangle( Mesh *mesh, const unsigned int a, const unsigned int b, const unsigned int c )
{
    assert(mesh != NULL);
    mesh->indices.push_back(a);
    mesh->indices.push_back(b);
    mesh->indices.push_back(c);
    return mesh->indices.size() -1;
}

//! remplace la matiere d'un objet Mesh. mesh doit etre entierement initialise / contenir le nombre final de triangles. 
inline
void setMaterial( Mesh *mesh, const MeshMaterial& material )
{
    assert(mesh != NULL);
    assert(mesh->indices.empty() == false);
    mesh->groups.clear();
    mesh->groups.push_back( MeshGroup(material, 0, mesh->indices.size()) );
    
    mesh->materials.clear();
    unsigned int n= mesh->indices.size() / 3u;
    for(unsigned int i= 0; i < n; i++)
        mesh->materials.push_back(0);
}


/* utilisation de l'interface Mesh

    gk::Mesh *mesh= new gk::Mesh;
    pushTriangle( mesh, 
        pushVertex( mesh, gk::Vec3(0, 0, 0) );
        pushVertex( mesh, gk::Vec3(1, 0, 0) );
        pushVertex( mesh, gk::Vec3(1, 1, 0) ));
    
    pushTriangle( mesh, ... );
    ...

    MeshMateral material;
    material.name= "red";       // optionnel
    material.diffuse_color= gk::VecColor(1, 0, 0);
    material.diffuse_texture= "pattern.png";    // optionnel
    setMaterial(mesh, material );

    remarque: setMaterial doit etre appele apres la creation (complete) des triangles.
*/

}       // namespace

#endif
