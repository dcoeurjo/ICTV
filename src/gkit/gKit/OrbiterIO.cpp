
#include <cstdio>

#include "Logger.h"
#include "IOFileSystem.h"

#include "Transform.h"

#include "Orbiter.h"
#include "OrbiterIO.h"


namespace gk {

Orbiter readOrbiter( const std::string& filename )
{
    MESSAGE("loading camera '%s'...\n", filename.c_str());
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        ERROR("error opening '%s'... failed.\n", filename.c_str());
        return Orbiter();
    }
    
    Orbiter orbiter;
    float px= 0, py= 0, pz= 0, rx= 0, ry= 0, s= 0;
    if(fscanf(in, "position %f %f %f rotation %f %f size %f", &px, &py, &pz, &rx, &ry, &s) == 6)
    {
        orbiter.position= gk::Vec3(px, py, pz);
        orbiter.rotation= gk::Vec2(rx, ry);
        orbiter.size= s;
    }
    
    fclose(in);
    return orbiter;
}


int writeOrbiter( const Orbiter& orbiter, const std::string& filename )
{
    MESSAGE("writing camera '%s'...\n", filename.c_str());
    FILE *out= fopen(filename.c_str(), "wt");
    if(out == NULL)
    {
        ERROR("error creating '%s'... failed.\n", filename.c_str());
        return -1;
    }
    
    fprintf(out, "position %f %f %f rotation %f %f size %f\n", 
        orbiter.position.x, orbiter.position.y, orbiter.position.z,
        orbiter.rotation.x, orbiter.rotation.y,
        orbiter.size);
    
    fclose(out);
    return 0;
}

}       // namespace
