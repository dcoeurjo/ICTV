
#ifndef _GK_ORBITER_H
#define _GK_ORBITER_H

#include "Geometry.h"
#include "Transform.h"


namespace gk {

//! camera type inspection d'objet.
/*! \code utilisation :
    int x, y;
    unsigned int button= SDL_GetRelativeMouseState(&x, &y);
    printf("mouse %d %d %u (%d %d %d)\n", x,y, button, button & SDL_BUTTON(1), button & SDL_BUTTON(2), button & SDL_BUTTON(3));

    if(button & SDL_BUTTON(1))
        m_camera.rotate(x, y);      // orbit
    else if(button & SDL_BUTTON(2))
        m_camera.move( (float) x / windowWidth(), (float) y / windowHeight() ); // pan
    else if(button & SDL_BUTTON(3))
        m_camera.move(x);           // dolly
    \endcode
*/
struct Orbiter
{
    Orbiter( ) : rotation(0, 0), position(0, 0, 10), size(10) {}
    Orbiter( const BBox& object ) { look(object); }
    ~Orbiter( ) {}
    
    //! re-oriente la camera en fonction de mouvements relatifs de la souris.
    void rotate( const float x, const float y ) { rotation.x+= y; rotation.y+= x; }
    //! deplace la camera en fonction de mouvements relatifs de la souris.
    void move( const float x, const float y ) { position.x-= position.z * x; position.y+= position.z * y;}
    //! approche/recule la camera en fonction de mouvements relatifs de la souris.
    void move( const float z ) { position.z-= position.z * 0.01f * z; position.z= std::max(0.001f, position.z); }
    
    //! repositionne la camera pour observer un objet.
    void look( const BBox& object ) { size= (object.pMax - object.pMin).Length(); rotation= Vec2(0, 0); position= Vec3(0, 0, size); }

    //! renvoie la transformation view.
    Transform view() { return gk::Translate( -Vector(position) ) * gk::RotateX(rotation.x) * gk::RotateY(rotation.y); }
    //! renvoie la transformation perspective de la camera, en fonction des dimensions de la fenetre.
    Transform projection( const float width, const float height ) { return Perspective(45.f, width / height, size*0.001f, size + position.z); }
    //! renvoie la transformation perspective de la camera. (suppose que la fenetre est toujours carree).
    Transform projection( ) { return Perspective(45.f, 1.f, size*0.001f, size + position.z); }
    
    Vec2 rotation;
    Vec3 position;
    float size;
};

}       // namespace

#endif
