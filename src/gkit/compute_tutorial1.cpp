/*
exemple de compute shader : transformation des sommets d'un maillage

etape 1 :
le compute shader applique une transformation model view projection a chaque sommet d'un maillage.
entree: les positions des sommets
sortie: les positions transformees

etape 2 :
affichage du maillage transforme.

remarque : ca n'a aucun interet pratique, c'est un exemple.
*/


#include "App.h"

#include "GL/GLBuffer.h"
#include "GL/GLBasicMesh.h"

#include "ProgramManager.h"

#include "Mesh.h"
#include "MeshIO.h"

class TP : public gk::App
{
    gk::GLProgram *program;
    gk::GLProgram *display;
    gk::GLBuffer *input;
    gk::GLBuffer *output;
    gk::GLBasicMesh *mesh;
    
    int input_size;
    
    float rotate;
    float distance;
    
public:
    // creation du contexte openGL et d'une fenetre
    TP( )
        :
        gk::App()
    {
        // specifie le type de contexte openGL a creer :
        gk::AppSettings settings;
        settings.setGLVersion(4,3);     // version 4.3
        settings.setGLCoreProfile();      // core profile
        settings.setGLDebugContext();     // version debug pour obtenir les messages d'erreur en cas de probleme
        
        // cree le contexte et une fenetre
        if(createWindow(512, 512, settings) < 0)
            closeWindow();
    }
    
    ~TP( ) {}
    
    int init( )
    {
        {
            GLint x, y, z;
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &x);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &y);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &z);
            printf("compute group count %d %d %d\n", x, y, z);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &x);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &y);
            glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &z);
            printf("compute group size %d %d %d\n", x, y, z);
            
            GLint size;
            glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &size);
            printf("compute shared memory %dK\n", size / 1024);
        }
        
        // cree un shader program
        gk::programPath("compute");
        program= gk::createProgram("compute_vertex.glsl");
        if(program == gk::GLProgram::null())
            return -1;

        display= gk::createProgram("display.glsl");
        if(display == gk::GLProgram::null())
            return -1;
        
        std::string filename= "bigguy.obj";
        gk::Mesh *bigguy= gk::MeshIO::readOBJ(filename);
        if(bigguy == NULL)
            return -1;  // erreur de lecture
        
        // les buffers doivent etre remplis en respectant certaines contraintes. 
        // par exemple, les vec3 sont alignes comme des vec4...
        // modifier le stockage des positions
        std::vector<gk::Vec4> positions;
        for(unsigned int i= 0; i < bigguy->positions.size(); i++)
        {
            gk::Vec3 p= bigguy->positions[i];
            positions.push_back( gk::Vec4(p.x, p.y, p.z, 1.0f) );
        }
        
        // cree le buffer d'entree : les positions a transformer
        input= gk::createBuffer(GL_SHADER_STORAGE_BUFFER, positions);
        input_size= (int) bigguy->positions.size();

        // configure un maillage 
        mesh= new gk::GLBasicMesh(GL_TRIANGLES, bigguy->indices.size());
        if(mesh == NULL)
            return -1;
        mesh->createBuffer(0, positions);       // cree un nouveau buffer structure comme input
        mesh->createBuffer(1, bigguy->texcoords);
        mesh->createBuffer(2, bigguy->normals);
        mesh->createIndexBuffer(bigguy->indices);
        
        output= mesh->buffers[0];       // recupere le buffer cree pour stocker les sommets transformes.
        delete bigguy;  // plus besoin de l'objet.
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        
        //
        distance= 50;
        rotate= -40;
        
        // tout c'est bien passe.
        return 0;
    }
    
    int quit( )
    {
        // rien a faire les ressources opengl sont detruites automatiquement par gk::GLManager.
        return 0;
    }
    
    int draw( )
    {
        if(key(SDLK_ESCAPE))
            // fermer l'application si l'utilisateur appuie sur ESCAPE
            closeWindow();
        
        if(key(SDLK_LEFT))
            rotate-= 1.f;
        if(key(SDLK_RIGHT))
            rotate+= 1.f;
        if(key(SDLK_UP))
            distance+= 1.f;
        if(key(SDLK_DOWN))
            distance-= 1.f;
        
        if(key('r'))
        {
            key('r')= 0;
            gk::reloadPrograms();
        }
        
        static int wireframe= 1;
        if(key('w'))
        {
            key('w')= 0;
            wireframe = (wireframe + 1) % 2;
        }
        
        if(wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // transformations
        gk::Transform model= gk::RotateY(30.f); 
        gk::Transform view= gk::Translate( gk::Vector(0.f, 0.f, -distance) ) * gk::RotateY(rotate);
        gk::Transform projection= gk::Perspective(50.f, 1.f, 1.f, 1000.f);
        
        // composition des transformations
        gk::Transform mv= view * model;
        gk::Transform mvp= projection * mv;
        
        // redimensionne l'image en fonction de la fenetre de l'application
        glViewport(0, 0, windowHeight(), windowHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // transforme les sommets avec le compute shader
        glUseProgram(program->name);
        program->uniform("mvpMatrix")= mvp.matrix();
        
        // selectionne les buffers d'entree et de sortie du compute shader
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input->name);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, output->name);
        program->uniform("positionSize")= input_size;

        // execute le compute shader : par groupes de n threads, cf le source du shader n== 32
        // ou utiliser GLint size[3]; glGetProgramiv(GL_COMPUTE_WORK_GROUP_SIZE, size); size[0] == 32, size[1] == 1, size[2] == 1
        glDispatchCompute(input_size / 32 +1, 1, 1);
        
        // attend que toutes les donnees soient disponibles
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        
        // affiche le maillage transforme
        glUseProgram(display->name);
        mesh->draw();
        
        // un peu de nettoyage
        glUseProgram(0);
        
        // visualise l'image que l'on vient de dessiner.
        present();
        return 1;
    }
};


int main( int argc, char **argv )
{
    TP app;
    app.run();
    
    return 0;
}

