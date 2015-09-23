
#include <string>
#include <fstream>
#include <sstream>

#include "GL/GLPlatform.h"
#include "SDLPlatform.h"

#ifndef _MSC_VER
  #define GK_CALLBACK
#else
  #define GK_CALLBACK __stdcall
#endif

GLuint program;
GLint mvp_location;
GLint color_location;

GLuint vao;
GLuint vertex_buffer;
GLuint index_buffer;

//! charge un fichier texte.
std::string read( const char *filename )
{
    std::stringbuf source;
    std::ifstream in(filename);
    if(in.good() == false)
        printf("error reading '%s'\n", filename);
    
    in.get(source, 0);        // lire tout le fichier, le caractere '\0' ne peut pas se trouver dans le source de shader
    return source.str();
}

//! affiche les erreurs de compilation de shader.
int shader_errors( const GLuint shader )
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_TRUE)
        return 0;
    char errors[4096];
    glGetShaderInfoLog(shader, sizeof(errors), NULL, errors);
    printf("errors:\n%s\n", errors);
    return -1;
}

//! affiche les erreurs d'edition de liens du programme.
int program_errors( const GLuint program )
{
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_TRUE)
        return 0;
    char errors[4096];
    glGetProgramInfoLog(program, sizeof(errors), NULL, errors);
    printf("errors:\n%s\n", errors);
    return -1;
}

//! compile les shaders et construit le programme + les buffers + le vertex array
int init( )
{
    // gl core profile : compiler un shader program
    program= glCreateProgram();
    
    GLuint vertex_shader= glCreateShader(GL_VERTEX_SHADER);
    {
        std::string source= read("mini_vertex.glsl");
        const char *sources[]= { source.c_str() };
        
        glShaderSource(vertex_shader, 1, sources, NULL);
        glCompileShader(vertex_shader);
        if(shader_errors(vertex_shader) < 0)
        {
            printf("vertex source:\n%s\n", sources[0]);
            return -1;
        }
    }
    
    GLuint fragment_shader= glCreateShader(GL_FRAGMENT_SHADER);
    {
        std::string source= read("mini_fragment.glsl");
        const char *sources[]= { source.c_str() };
        
        glShaderSource(fragment_shader, 1, sources, NULL);
        glCompileShader(fragment_shader);
        if(shader_errors(fragment_shader) < 0)
        {
            printf("fragment source:\n%s\n", sources[0]);
            return -1;
        }
    }
    
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    if(program_errors(program) < 0)
        return -1;
    
    // plus besoin des shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    // recupere les identifiants des uniforms
    mvp_location= glGetUniformLocation(program, "mvpMatrix");
    color_location= glGetUniformLocation(program, "color");
    
    // buffers : decrire un cube indexe.
    float positions[][3] = { 
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},  // face arriere z= 0
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}  // face avant z= 1
    }; 
    
    unsigned int indices[] = {
        4, 5, 6,     // face avant, manque le 2ieme triangle
        1, 2, 6,  
        0, 1, 5,
        0, 3, 2,     // face arriere 
        0, 4, 7, 
        2, 3, 7,
    };
    
    // gl core profile : configurer un vertex array
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // vertex buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    
    // associer le contenu du buffer avec un attribut du vertex shader
    // l'indice 0 est impose dans le source du shader, cf layout(location= 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    // index buffer
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // nettoyage
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return 0;
}

int quit( )
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    return 0;
}

int draw( )
{
    // effacer l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // selectionner une configuration : contenu des buffers / attributs de shaders
    glBindVertexArray(vao);
    // selectionner un shader program
    glUseProgram(program);
    
    // initialiser les transformations
    float mvp[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    glUniformMatrix4fv(mvp_location, 1, GL_TRUE, mvp);
    
    // donner une couleur a l'objet
    float color[] = {1, 1, 0};
    glUniform3fv(color_location, 1, color);
    
    // draw
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
    
    return 0;
}

int run( SDL_Window *window )
{
    SDL_Event event;
    int width;
    int height;
    int stop= 0;
    
    while(stop == 0)
    {
        // gestion des evenements 
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        width= event.window.data1;
                        height= event.window.data2;
                        SDL_SetWindowSize(window, width, height);
                        
                        glViewport(0, 0, width, height);
                    }
                    break;
                    
                case SDL_QUIT:
                    stop= 1;
                    break;
            }
        }
        
        // dessiner
        draw();
        // afficher le dessin
        SDL_GL_SwapWindow(window);
    }

    return 0;
}


//! affiche les messages opengl dans un contexte debug core profile.
void GK_CALLBACK AppDebug( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam )
{
    if(severity == GL_DEBUG_SEVERITY_HIGH)
        printf("openGL error:\n%s\n", message);
    else if(severity == GL_DEBUG_SEVERITY_MEDIUM)
        printf("openGL warning:\n%s\n", message);
    else
        printf("openGL message:\n%s\n", message);
}

int main( )
{
    // init sdl
    if(SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE ) < 0)
    {
        printf("SDL_Init() failed:\n%s\n", SDL_GetError());
        return 1;
    }
    // enregistre le destructeur de sdl
    atexit(SDL_Quit);
    
    // creer la fenetre et le contexte openGL
    SDL_Window *m_window= SDL_CreateWindow("gKit", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        512, 512, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(m_window == NULL)
    {
        printf("error creating sdl2 window.\n");
        return 1;
    }
    SDL_SetWindowDisplayMode(m_window, NULL);

    // configure la creation du contexte opengl 3.3 core profile, debug 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    SDL_GLContext m_gl_context;
    m_gl_context= SDL_GL_CreateContext(m_window);
    if(m_gl_context == NULL)
    {
        printf("error creating openGL context.\n");
        return 1;
    }
    
    SDL_GL_SetSwapInterval(1);
    
    // initialise les extensions opengl
    glewExperimental= 1;
    GLenum err= glewInit();
    if(err != GLEW_OK)
    {
        printf("error loading extensions: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    // purge les erreurs opengl generees par glew !
    while(glGetError() != GL_NO_ERROR) {;}
    
    // configure l'affichage des messages d'erreurs opengl
    if(GLEW_ARB_debug_output)
    {
        glDebugMessageCallbackARB(AppDebug, NULL);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    }
    
    // go !!
    if(init() < 0)
    {
        printf("init failed.\n");
        return 1;
    }
    
    run(m_window);
    quit();
    
    if(m_gl_context != NULL)
        SDL_GL_DeleteContext(m_gl_context);
    if(m_window != NULL)
        SDL_DestroyWindow(m_window);
    
    return 0;
}

