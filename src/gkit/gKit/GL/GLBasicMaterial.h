
#ifndef _BASIC_MATERIAL_H
#define _BASIC_MATERIAL_H

#include "Vec.h"
#include "Transform.h"




namespace gk {

class GLProgram;
class GLTexture;
class GLSampler;
class MeshMaterial;
    
//! genere un shader program associe a une description de matiere, cf MeshMaterial.
/*! utilise un modele d'eclairement simple :
    ambient / PI + kd * diffuse + ks * specular
    
    ambient, diffus et specular sont des constantes eventuellement modulees par une texture :
    diffuse= diffuse_color * diffuse_texture * diffuse_emission * cos 
    specular= specular_color * specular_texture * specular_emission * cos * pow
    ambient= ambient_color * ambiant_texture * ambiant_emission * abs(cos)
    
    pow utilise le modele de blinn, pow(ns, dot(n, h)) * (terme de normalisation)
    
    cos est dot(normal, light_direction) limite aux valeurs positives
    ou la valeur absolue en mode both sides.
    
    cf material_viewer et hair_viewer pour un exemple complet.
    cf envmap_viewer 
 */
class GLBasicMaterial
{
    // non copyable
    GLBasicMaterial( const GLBasicMaterial& );
    GLBasicMaterial& operator= ( const GLBasicMaterial& );
    
public:
    GLProgram *material_program;        //!< shader program.
    Transform model;                    //!< transformation model.
    Transform view;                     //!< transformation view.
    Transform projection;               //!< transformation perspective.

    GLTexture *ambient_texture;         //!< texture ambiante.
    GLTexture *diffuse_texture;         //!< texture diffuse.
    GLTexture *specular_texture;        //!< texture speculaire.
    GLTexture *envmap_texture;             //!< texture d'environnement.
    GLSampler *sampler;                 //!< mode de filtrage des textures.
    
    VecColor ambient_color;             //! couleur ambiante.
    VecColor diffuse_color;
    VecColor specular_color;
    
    float ka;                           //!< influence du comportement ambiant
    float kd;                           //!< influence du comportement diffus.
    float ks;                           //!< influence du comportement speculaire.
    float ns;                           //!< comportement speculaire, ks * cos**ns
    
    Vec3 light_position;
    VecColor light_ambient_color;
    VecColor light_diffuse_color;
    VecColor light_specular_color;
    
    bool both_sides;
    
    //! constructeur.
    GLBasicMaterial( );
    //! constructeur.
    GLBasicMaterial( const MeshMaterial& material );

    //! renvoie le shader program associe a la matiere.
    GLProgram *program( );
    
    //! renvoie l'indice de l'attribut 'position' utilise par le shader program. 
    //! utile pour construire les buffers et configurer un vertex array object / GLBasicMesh.
    static int positionLocation( );
    //! renvoie l'indice de l'attribut 'texcoords' utilise par le shader program. 
    //! utile pour construire les buffers et configurer un vertex array object / GLBasicMesh.
    static int texcoordsLocation( );
    //! renvoie l'indice de l'attribut 'normals'utilise par le shader program. 
    //! utile pour construire les buffers et configurer un vertex array object / GLBasicMesh.
    static int normalLocation( );

    //! utilise un terme ambiant.
    void useAmbient( const float ka, const VecColor& ambient, GLTexture *texture= GLTexture::null() );
    //! utilise un terme ambiant.
    void useAmbient( const float ka, const VecColor& ambient, const std::string& texture_filename );
    
    //! utilise un terme diffus.
    void useDiffuse( const float kd, const VecColor& diffuse, GLTexture *texture= GLTexture::null() );
    //! utilise un terme diffus.
    void useDiffuse( const float kd, const VecColor& diffuse, const std::string& texture_filename );
    
    //! utilise un terme speculaire.
    void useSpecular( const float ks, const VecColor& specular, const float ns, GLTexture *texture= GLTexture::null() );
    //! utilise un terme speculaire.
    void useSpecular( const float ks, const VecColor& specular, const float ns, const std::string& texture_filename );
    
    //! re-oriente la normale vers la source dans le calcul de l'energie reflechie par la matiere.
    void useBothsides( );
    
    //! utilise une texture cube / environnement comme source de lumiere.
    void useEnvmapLight( GLTexture *envmap );
    
    //! utilise une source de lumiere ponctuelle.
    void useLight( const Vec3& position, const VecColor& diffuse= VecColor(1.0f, 1.0f,1.0f), const VecColor& specular= VecColor() );
    
    //! utilise une energie ambiante.
    void useAmbientLight( const VecColor& ambient= VecColor(1.0f, 1.0f,1.0f) );
    
    //! deplace la source de lumiere. position est representee dans le repere du monde.
    void moveLight( const Vec3& position );
    
    //! transformation model a utiliser pour dessiner le prochain objet.
    void useModelTransform( const Transform& t );
    
    //! transformation view a utiliser pour dessiner le prochain objet.
    void useViewTransform( const Transform& t );
    
    //! transformation perspective a utiliser pour dessiner le prochain objet.
    void useProjectionTransform( const Transform& t );
    
    //! compile, active et parametre le shader associe a la matiere pour dessiner le prochain objet.
    int use( );
};

}       // namespace

#endif

