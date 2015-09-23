
#include "Vec.h"
#include "Transform.h"
#include "Mesh.h"
#include "Image.h"
#include "ImageManager.h"
#include "ProgramManager.h"
#include "GL/GLTexture.h"
#include "GL/GLSampler.h"

#include "GL/GLBasicMaterial.h"

namespace gk {

GLBasicMaterial::GLBasicMaterial( ) 
    : 
    material_program(GLProgram::null()), 
    ambient_texture(GLTexture::null()), diffuse_texture(GLTexture::null()), specular_texture(GLTexture::null()), envmap_texture(GLTexture::null()),
    ambient_color(), diffuse_color(0.8f, 0.8f, 0.8f), specular_color(), 
    ka(0.0f), kd(1.0f), ks(0.0f), ns(0.0f),
    light_position(), light_ambient_color(), light_diffuse_color(1.0f, 1.0f, 1.0f), light_specular_color(1.0f, 1.0f, 1.0f), 
    both_sides(false)
{
    //~ sampler= defaultSampler();
    sampler= createLinearSampler(GL_CLAMP_TO_BORDER);
}


GLBasicMaterial::GLBasicMaterial( const MeshMaterial& material ) 
    : 
    material_program(GLProgram::null()), 
    ambient_texture(GLTexture::null()), diffuse_texture(GLTexture::null()), specular_texture(GLTexture::null()), envmap_texture(GLTexture::null()),
    ambient_color(), diffuse_color(0.8f, 0.8f, 0.8f), specular_color(), 
    ka(0.0f), kd(1.0f), ks(0.0f), ns(0.0f),
    light_position(), light_ambient_color(), light_diffuse_color(1.0f, 1.0f, 1.0f), light_specular_color(1.0f, 1.0f, 1.0f), 
    both_sides(false)
{
    useDiffuse(material.kd, material.diffuse_color, material.diffuse_texture);
    useSpecular(material.ks, material.specular_color, material.ns, material.specular_texture);
    useAmbient(material.ka, material.ambient_color, material.ambient_texture);
    
    //~ sampler= defaultSampler();
    sampler= createLinearSampler(GL_CLAMP_TO_BORDER);
}


void GLBasicMaterial::useDiffuse( const float k, const VecColor& color, GLTexture *texture )
{
    diffuse_texture= texture;
    diffuse_color= color;
    kd= k;
}

void GLBasicMaterial::useDiffuse( const float k, const VecColor& color, const std::string& texture_filename )
{
    GLTexture *texture= GLTexture::null();
    if(texture_filename.empty() == false)
    {
        Image *image= readImage(texture_filename);
        if(image == NULL) image= defaultImage();
        texture= createTexture2D(GLTexture::UNIT0, image);
    }
    
    useDiffuse(k, color, texture);
}


void GLBasicMaterial::useSpecular( const float k, const VecColor& color, const float s, GLTexture *texture )
{
    specular_texture= texture;
    specular_color= color;
    ks= k;
    ns= s;
}

void GLBasicMaterial::useSpecular( const float k, const VecColor& color, const float s, const std::string& texture_filename )
{
    GLTexture *texture= GLTexture::null();
    if(texture_filename.empty() == false)
    {
        Image *image= readImage(texture_filename);
        if(image == NULL) image= defaultImage();
        texture= createTexture2D(GLTexture::UNIT0, image);
    }
    
    useSpecular(k, color, s, texture);
}


void GLBasicMaterial::useAmbient( const float k, const VecColor& color, GLTexture *texture )
{
    ambient_texture= texture;
    ambient_color= color;
    ka= k;
}

void GLBasicMaterial::useAmbient( const float ka, const VecColor& ambient, const std::string& texture_filename )
{
    GLTexture *texture= GLTexture::null();
    if(texture_filename.empty() == false)
    {
        Image *image= readImage(texture_filename);
        if(image == NULL) image= defaultImage();
        texture= createTexture2D(GLTexture::UNIT0, image);
    }
    
    useAmbient(ka, ambient, texture);
}


void GLBasicMaterial::useEnvmapLight( GLTexture *texture )
{
    envmap_texture= texture;
}

GLProgram *GLBasicMaterial::program( )
{
    if(material_program != GLProgram::null()) 
        return material_program;
    
    // generer le shader program
    unsigned int version= 0;
    if(diffuse_texture != GLTexture::null()) version|= 1;
    if(specular_texture != GLTexture::null()) version|= 2;
    if(ambient_texture != GLTexture::null()) version|= 4;
    if(envmap_texture != GLTexture::null()) version|= 8;
    if(both_sides == true) version|= 16;
    
    // recherche une version compatible du shader
    GLProgram *program= findProgram("basic_material.glsl", version);
    if(program == GLProgram::null())
    {
        GLCompiler &compiler= loadProgram("basic_material.glsl");
        if(version & 1) compiler.defineFragment("USE_DIFFUSE_TEXTURE");
        if(version & 2) compiler.defineFragment("USE_SPECULAR_TEXTURE");
        if(version & 4) compiler.defineFragment("USE_AMBIENT_TEXTURE");
        if(version & 8) compiler.defineFragment("USE_ENVMAP_LIGHT");
        if(version & 16) compiler.defineFragment("USE_BOTH_SIDES");
        
        program= compiler.makeVersion(version);
    }
    
    material_program= program;
    return material_program;
}

int GLBasicMaterial::positionLocation( ) { return 0; }

int GLBasicMaterial::texcoordsLocation( ) { return 1; }

int GLBasicMaterial::normalLocation( ) { return 2; }

void GLBasicMaterial::useBothsides( ) { both_sides= true; }

void GLBasicMaterial::useLight( const Vec3& position, const VecColor& diffuse, const VecColor& specular )
{
    light_position= position;
    light_ambient_color= VecColor();
    light_diffuse_color= diffuse;
    light_specular_color= specular;
}

void GLBasicMaterial::useAmbientLight( const VecColor& ambient )
{
    light_ambient_color= ambient;
    //! \todo verifier que les autres parametres ka, ambient_color sont non nuls...
}

void GLBasicMaterial::moveLight( const Vec3& position ) { light_position= position; }

void GLBasicMaterial::useModelTransform( const Transform& t ) { model= t; }

void GLBasicMaterial::useViewTransform( const Transform& t ) { view= t; }

void GLBasicMaterial::useProjectionTransform( const Transform& t ) { projection= t; }

int GLBasicMaterial::use( )
{
    if(material_program == GLProgram::null())
        program();
    
    // termine le calcul des transformations
    Transform mv= view * model;
    Transform mvp= projection * mv;
    Transform normal= mv.normalMatrix();
    
    glUseProgram(material_program->name);
    
    material_program->uniform("mvpMatrix")= mvp.matrix();
    material_program->uniform("mvMatrix")= mv.matrix();
    material_program->uniform("normalMatrix")= normal.matrix();
    
    material_program->uniform("ambient_color")= ambient_color;
    material_program->uniform("diffuse_color")= diffuse_color;
    material_program->uniform("specular_color")= specular_color;
    material_program->uniform("ka")= ka;
    material_program->uniform("kd")= kd;
    material_program->uniform("ks")= ks;
    material_program->uniform("ns")= ns;
    
    material_program->uniform("light_position")= mv(Point(light_position)); // place la source dans view
    material_program->uniform("light_ambient")= light_ambient_color;
    material_program->uniform("light_diffuse")= light_diffuse_color;
    material_program->uniform("light_specular")= light_specular_color;
    
    // bind textures + samplers
    if(diffuse_texture != GLTexture::null())
    {
        const int unit= 0;
        material_program->sampler("diffuse_texture")= unit;
        glBindSampler(unit, sampler->name);
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(diffuse_texture->target, diffuse_texture->name);
    }
    
    if(specular_texture != GLTexture::null())
    {
        const int unit= 1;
        material_program->sampler("specular_texture")= unit;
        glBindSampler(unit, sampler->name);
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(specular_texture->target, specular_texture->name);
    }        
    
    if(ambient_texture != GLTexture::null())
    {
        const int unit= 2;
        material_program->sampler("ambient_texture")= unit;
        glBindSampler(unit, sampler->name);
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(ambient_texture->target, ambient_texture->name);
    }
    
    if(envmap_texture != GLTexture::null())
    {
        const int unit= 3;
        material_program->sampler("envmap_texture")= unit;
        glBindSampler(unit, sampler->name);
        
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(envmap_texture->target, envmap_texture->name);
    }
    
    return 0;
}
}       // namespace
