
#ifndef _MESH_MANAGER_H
#define _MESH_MANAGER_H

namespace gk {
    
class Mesh;

//! \todo
class MeshManager
{
protected:
    std::vector<Mesh *> m_meshes;
    std::vector<std::string> m_paths;

    MeshManager( ) {}
    ~MeshManager( ) {}
    
public:
    int searchPath( const std::string& pathname );
    
    std::string search( const std::string& filename );
    
    Mesh *read( const std::string& filename );
    
    Mesh *defaultMesh( );
    
    Mesh *cubeMesh( );
    
    Mesh *find( const std::string& filename );
    
    Mesh *insert( Mesh *mesh );
    
    static
    MeshManager& manager( )
    {
        static MeshManager object;
        return object;
    }
};


//! utilitaire: acces simplifie au singleton ImageManager.
inline 
int meshPath( const std::string& path )
{
    return MeshManager::manager().searchPath(path);
}


}       // namespace

#endif
