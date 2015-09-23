
#ifndef _CAMERAIO_H
#define _CAMERAIO_H


namespace gk {

struct Orbiter;

Orbiter readOrbiter( const std::string& filename );
int writeOrbiter( const Orbiter& orbiter, const std::string& filename );

}

#endif
