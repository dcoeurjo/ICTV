
#include "MeshData.h"
#include "IOFileSystem.h"
#include "MeshIO.h"

#include "Logger.h"


namespace gk {


void triangulate( std::vector<Face>& faces )
{
    MESSAGE("triangulating...\n");
    
    unsigned int n= faces.size();
    
    std::vector<Face> triangles;
    triangles.reserve(n*2);
    for(unsigned int i= 0; i < n; i++)
    {
        triangles.push_back(faces[i]);
        if(triangles.back().n > 3)
        {
            triangles.back().n= 3;
            
            Face triangle1;
            triangle1.push_back(faces[i].vertex[0]);
            triangle1.push_back(faces[i].vertex[2]);
            triangle1.push_back(faces[i].vertex[3]);
            triangles.push_back(triangle1);
        }
    }
    
    faces.swap(triangles);
}


void faceCount( const std::vector<Face>& faces, unsigned int &triangles, unsigned int &quads, unsigned int &others )
{
    triangles= 0;
    quads= 0;
    others= 0;
    
    for(unsigned int i= 0; i < faces.size(); i++)
    {
        if(faces[i].n == 3)     triangles++;
        else if(faces[i].n == 4)     quads++;
        else others++;
    }
}


//! representation privee d'une arete, 
//! utilisee pour construire l'adjacence des faces, cf. buildFaces()
struct Edge
{
    int a, b;   //!< indice des sommets de l'arete.
    int face;   //!< indice de la face.
    int id;     //!< indice de l'arete dans la face.
    
    Edge( ) : a(-1), b(-1), face(-1), id(-1) {}
    Edge( const int _a, const int _b, const int _face, const int _id ) : a(_a), b(_b), face(_face), id(_id) {}
};

int buildFaces( MeshData& mesh )
{
    MESSAGE("building faces...\n");
    
    /*  constuire la liste d'aretes incidentes a chaque sommet
        parcourir la liste d'incidences et construire l'adjacence des faces en recherchant les aretes opposees
    */
    
    std::vector<int> head(mesh.positions.size() +1, 0);
    
    // passe 1: compte le nombre d'aretes incidentes a chaque sommet
    unsigned int faces= mesh.faces.size();
    for(unsigned int i= 0; i < faces; i++)
    {
        unsigned int n= mesh.faces[i].n;
        for(unsigned int k= 0; k < n; k++)
        {
            int ia= mesh.faces[i].vertex[k].position;
            head[ia]++;
        }
    }
    
    // passe 2: cree la liste d'adjacence...
    int start= 0;
    int next= 0;
    for(unsigned int i= 0; i < head.size(); i++)
    {
        next= start + head[i];
        head[i]= start; // ... debut de la liste d'aretes de chaque sommet
        start= next;
    }
    
    std::vector<Edge> list(next, Edge());
    std::vector<int> tail(head);        // ... fin de la liste d'aretes de chaque sommet
    
    // passe 3: remplit la liste d'aretes
    for(unsigned int i= 0; i < faces; i++)
    {
        unsigned int n= mesh.faces[i].n;
        for(unsigned int k= 0; k < n; k++)
        {
            int ia= mesh.faces[i].vertex[k].position;
            int ib= mesh.faces[i].vertex[(k+1)%n].position;
            
            list[tail[ia]++]= Edge(ia, ib, i, k); 
            assert(tail[ia] <= head[ia+1]);
        }
    }
    
    // passe 4: construit l'adjacence des faces
    mesh.afaces.resize(mesh.faces.size());
    for(unsigned int i= 0; i < faces; i++)
    {
        unsigned int n= mesh.faces[i].n;
        mesh.afaces[i].n= mesh.faces[i].n;
        for(unsigned int k= 0; k < n; k++)
        {
            int ia= mesh.faces[i].vertex[k].position;
            int ib= mesh.faces[i].vertex[(k+1)%n].position;
            
            // arete (ia, ib) : recherche la face associee a l'arete (ib, ia) dans les aretes adjacentes au sommet b
            for(int edge= head[ib]; edge < tail[ib]; edge++)
            {
            #if 0
                if(list[edge].b != ia)
                    continue;
                assert(list[edge].a == ib);
                assert(list[edge].face != (int) i);
            #endif
                
                if(list[edge].face == (int) i || list[edge].b != ia || list[edge].a != ib)
                    continue;
                mesh.afaces[i].faces[k]= list[edge].face;
                mesh.afaces[i].edges[k]= list[edge].id;
                break;
            }
        }
        
        //~ printf("face %d: opposite %d %d %d %d, edges %d %d %d %d\n", i,
            //~ mesh.afaces[i].faces[0], mesh.afaces[i].faces[1], mesh.afaces[i].faces[2], mesh.afaces[i].faces[3],
            //~ mesh.afaces[i].edges[0], mesh.afaces[i].edges[1], mesh.afaces[i].edges[2], mesh.afaces[i].edges[3]);
    }
    
    return mesh.afaces.empty() ? 0 : -1;
}


//!  comparaison de 2 positions. utilisation par buildIndex().
struct vec3_less 
{
    bool operator( ) ( const Vec3& a, const Vec3& b ) const
    {
        if(a.x != b.x) return (a.x < b.x);
        if(a.y != b.y) return (a.y < b.y);
        return (a.z < b.z);
    }
};

int buildIndex( const std::vector<Vec3>& points, MeshData& mesh )
{
    mesh.positions.reserve(points.size());

    // indexe les positions
    std::vector<int> indices;
    indices.reserve(points.size());
    
    std::map<Vec3, int, vec3_less> remap;
    for(unsigned int i= 0; i < points.size(); i++)
    {
        // renumerote les positions et elimine les doublons.
        std::pair< std::map<Vec3, int, vec3_less>::iterator, bool > found= 
            remap.insert( std::make_pair(points[i], remap.size()) );
        
        indices.push_back(found.first->second);
        if(found.second)
            // insere le nouveau point.
            mesh.positions.push_back(found.first->first);
    }
    
    // construit les triangles
    unsigned int n= indices.size() / 3;
    mesh.faces.reserve(n);
    for(unsigned int i= 0; i < n; i++)
    {
        Face triangle;
        triangle.push_back( Vertex(indices[3u*i     ], -1, -1, -1) );
        triangle.push_back( Vertex(indices[3u*i + 1u], -1, -1, -1) );
        triangle.push_back( Vertex(indices[3u*i + 2u], -1, -1, -1) );
        
        mesh.faces.push_back(triangle);
    }
    
    return 0;
}


static
int attribute( const int index, const int count )
{
    int a= -1;
    if(index < 0)
        a= count + index;       // indexation relative aux derniers sommets
    else
        a= index - 1;           // indexations classique
    
    if(a < 0 || a >= count)
    {
        static int warnings= 0;
        if(warnings == 0)
        {       // beurk
            warnings++;
            printf("invalid index %d, max %d\n", index, count);
        }
        
        a= -1;
    }
    
    return a;
}


int readOBJ( const std::string& filename, MeshData& mesh )
{
    // ouvre le fichier
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        printf("error reading '%s'.\n", filename.c_str());
        return -1;
    }
    
    printf("loading '%s'...\n", filename.c_str());
    
    int material_id= -1;
    int material_base= 0;
    
    char line[1024];
    char tmp[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line, sizeof(line), in) == NULL)
        {
            error= false;               // fin du fichier, pas d'erreur detectee
            break;
        }
        
        line[sizeof(line) -1]= 0;       // force la fin de la ligne
        if(line[0] == 'v')
        {
            if(line[1] == ' ')          // position
            {
                float x, y, z;
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                mesh.positions.push_back( Vec3(x, y, z) );
                mesh.box.Union( Point(x, y, z) );
            }
            else if(line[1] == 'n')     // normal
            {
                float x, y, z;
                if(sscanf(line, "vn %f %f %f", &x, &y, &z) != 3)
                    break;
                mesh.normals.push_back( Vec3(x, y, z) );
            }
            else if(line[1] == 't')     // texcoord
            {
                float u, v, w;
                int status= sscanf(line, "vt %f %f %f", &u, &v, &w);
                if(status == 2)
                    w= 0.f;             // vt u v, pas de composante w
                if(status >= 2)
                    mesh.texcoords.push_back( Vec3(u, v, w) );
                else
                    break;
            }
        }
        
        else if(line[0] == 'm')
        {
            // charger un ensemble de matieres
            if(sscanf(line, "mtllib %[^\r\n]", tmp) == 1)       // lit toute la ligne, recupere les noms de fichiers avec des espaces.
            {
                material_base= mesh.materials.size();
                readMTL(IOFileSystem::pathname(filename) + tmp, mesh.materials);
            }
        }
        
        else if(line[0] == 'u')
        {
            if(sscanf(line, "usemtl %[^\r\n]", tmp) == 1) // lit toute la ligne, recupere les noms avec des espaces.
            {
                // ajouter les faces suivantes au groupe associe a la matiere
                material_id= -1;
                for(unsigned int i= material_base; i < mesh.materials.size(); i++)
                    if(mesh.materials[i].name == tmp)
                    {
                        material_id= i;
                        break;
                    }
            }
        }
        
        else if(line[0] == 'f')         // triangle/quad
        {
            Face face;
            int ia, ita, ina;
            int n;
            
            int count= 0;
            char *next= line +1;
            for(;;)
            {
                n= 0;
                if(sscanf(next, " %d/%d/%d %n", &ia, &ita, &ina, &n) == 3)      // v/t/n format
                    face.push_back(
                        Vertex(attribute(ia, mesh.positions.size()), 
                            attribute(ita, mesh.texcoords.size()), 
                            attribute(ina, mesh.normals.size()), 
                            material_id));
                
                else if(sscanf(next, " %d//%d %n", &ia, &ina, &n) == 2)         // v//n format
                    face.push_back(
                        Vertex(attribute(ia, mesh.positions.size()), 
                            -1, 
                            attribute(ina, mesh.normals.size()), 
                            material_id));
                
                else if(sscanf(next, " %d/%d %n", &ia, &ita, &n) == 2)          // v/t  format
                    face.push_back(
                        Vertex(attribute(ia, mesh.positions.size()), 
                            attribute(ita, mesh.texcoords.size()), 
                            -1, 
                            material_id));
                
                else if(sscanf(next, " %d %n", &ia, &n) == 1)                   // v format
                    face.push_back(
                        Vertex(attribute(ia, mesh.positions.size()), 
                            -1, 
                            -1, 
                            material_id));
                
                else
                    break;
                
                // verifie la coherence du modele
                if(material_id < 0)
                    mesh.has_material= false;
                if(face.vertex[count].normal == -1)
                    mesh.has_normal= false;
                if(face.vertex[count].texcoord == -1)
                    mesh.has_texcoord= false;
                
                count++;
                next= next + n;
                if(n == 0 || count >= 4)
                    break;
            }
            
            mesh.faces.push_back( face );
        }
    }
    
    fclose(in);
    
    // cree une matiere par defaut, si necessaire
    if(mesh.faces.empty() == false && mesh.has_material == false)
    {
        mesh.materials.push_back( MeshMaterial() );  
        
        // affecte la matiere par defaut
        for(unsigned int i= 0; i < mesh.faces.size(); i++)
            for(unsigned int k= 0; k < mesh.faces[i].n; k++)
                if(mesh.faces[i].vertex[k].material < 0)
                    mesh.faces[i].vertex[k].material= mesh.materials.size() -1; 
    }

    return (error ? -1 : 0);
}


int readMTL( const std::string& filename, std::vector<MeshMaterial>& materials )
{
    FILE *in= fopen(filename.c_str(), "rt");
    if(in == NULL)
    {
        printf("error reading '%s'.\n", filename.c_str());
        return -1;
    }
    
    printf("loading '%s'...\n", filename.c_str());
    
    char line[1024];
    char tmp[1024];
    MeshMaterial *material= NULL;
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line, sizeof(line), in) == NULL)
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        line[sizeof(line) -1]= 0;        // force la fin de la ligne, a cas ou
        if(line[0] == 'n')
        {
            if(sscanf(line, "newmtl %[^\r\n]", tmp) == 1)
            {
                materials.push_back( MeshMaterial(tmp) );
                material= &materials.back();
            }
        }
        if(material == NULL)
            continue;
        
        if(line[0] == 'K')
        {
            float r, g, b;
            if(sscanf(line, "Ka %f %f %f", &r, &g, &b) == 3) 
            {
                Vector v(r, g, b);
                float k= v.Length();
                if(k > 0) v= Normalize(v); 
                else k= 1.f;
                material->ambient_color= VecColor(v.x, v.y, v.z, 1);
                material->ka= k;
            }
            else if(sscanf(line, "Kd %f %f %f", &r, &g, &b) == 3) 
            {
                Vector v(r, g, b);
                float k= v.Length();
                if(k > 0) v= Normalize(v); 
                else k= 1.f;
                material->diffuse_color= VecColor(v.x, v.y, v.z, 1);
                material->kd= k;
            }
            else if(sscanf(line, "Ks %f %f %f", &r, &g, &b) == 3) 
            {
                Vector v(r, g, b);
                float k= v.Length();
                if(k > 0) v= Normalize(v); 
                else k= 1.f;
                material->specular_color= VecColor(v.x, v.y, v.z, 1);
                material->ks= k;
            }
        }
        
        if(line[0] == 'k')
        {
            float k;
            if(sscanf(line, "ka %f", &k) == 1)
                material->ka= k;
            else if(sscanf(line, "kd %f", &k) == 1)
                material->kd= k;
            else if(sscanf(line, "ks %f", &k) == 1)
                material->ks= k;
        }
        
        else if(line[0] == 'm')
        {
            if(sscanf(line, "map_Ka %[^\r\n]", tmp) == 1)
                material->ambient_texture= IOFileSystem::pathname( filename ) + tmp;
            else if(sscanf(line, "map_Kd %[^\r\n]", tmp) == 1)
                material->diffuse_texture= IOFileSystem::pathname( filename ) + tmp;
            else if(sscanf(line, "map_Ks %[^\r\n]", tmp) == 1)
                material->specular_texture= IOFileSystem::pathname( filename ) + tmp;
            
            //~ printf("using texture path '%s', file '%s'.\n", IOFileSystem::pathname( filename ).c_str(), tmp);
        }
        
        else if(line[0] == 'N')
        {
            float n;
            if(sscanf(line, "Ns %f", &n) == 1)          // Ns, puissance / concentration du reflet, modele blinn phong
                material->ns= n;
            else if(sscanf(line, "Ni %f", &n) == 1)     // Ni, indice de refraction pour les miroirs et les objets transparents
                material->ni= n;
        }
        
        else if(line[0] == 'L')     // Le
        {
            float r, g, b;
            if(sscanf(line, "Le %f %f %f", &r, &g, &b) == 3) 
                material->emission= VecColor(r, g, b);
        }
    }
    
    fclose(in);
    if(error)
    {
        printf("loading '%s'... failed, parsing line :\n%s\n", filename.c_str(), line);
        return -1;
    }
    
    return 0;
}


}       // namespace

