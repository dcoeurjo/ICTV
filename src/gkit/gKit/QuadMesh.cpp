
#include "Logger.h"
#include "QuadMesh.h"


namespace gk {

int QuadMesh::buildNormals( )
{
    MESSAGE("building normals...\n");
    
    unsigned int count= positions.size();
    normals.assign(count, Vec3());
    
    for(unsigned int i= 0; i < faces.size(); i++)
    {
        assert(faces[i].n == 4);
        int ia= faces[i].vertex[0].position;
        int ib= faces[i].vertex[1].position;
        int ic= faces[i].vertex[2].position;
        int id= faces[i].vertex[3].position;
        Point a(positions[ia]);
        Point b(positions[ib]);
        Point c(positions[ic]);
        Point d(positions[id]);
        
        normals[ia]= Normal(normals[ia]) + Normalize(Normal(Cross( Vector(a, b), Vector(a, d) )));
        normals[ib]= Normal(normals[ia]) + Normalize(Normal(Cross( Vector(b, c), Vector(b, a) )));
        normals[ic]= Normal(normals[ia]) + Normalize(Normal(Cross( Vector(c, d), Vector(c, b) )));
        normals[id]= Normal(normals[ia]) + Normalize(Normal(Cross( Vector(d, a), Vector(d, c) )));
    }
    
    for(unsigned int i= 0; i < count; i++)
        normals[i]= Normalize(Normal(normals[i]));
    
    return 0;
}

}       // namespace
