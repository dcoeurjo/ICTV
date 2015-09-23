
#include <cassert>
#include <cstdio>

#include "GL/GLPlatform.h"
#ifndef _MSC_VER
  #define GK_CALLBACK
#else
  #define GK_CALLBACK __stdcall
#endif

#include "App.h"
#include "Logger.h"


namespace gk {

//! affiche les messages opengl dans un contexte debug core profile.
void GK_CALLBACK AppDebug( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam )
{
    if(severity == GL_DEBUG_SEVERITY_HIGH)
        ERROR("openGL error:\n%s\n", message);
    else if(severity == GL_DEBUG_SEVERITY_MEDIUM)
        WARNING("openGL warning:\n%s\n", message);
    else
        WARNING("openGL message:\n%s\n", message);
}


App::App( )
    :
    m_window(NULL), m_gl_context(NULL),
    m_key_state(NULL),
    m_width(0), m_height(0),
    m_fullscreen(SDL_FALSE),
    m_stop(1)
{}

App::App( const int w, const int h, const AppSettings& flags )
    :
    m_window(NULL), m_gl_context(NULL),
    m_key_state(NULL),
    m_width(0), m_height(0),
    m_fullscreen(SDL_FALSE),
    m_stop(0)
{
    if(createWindow(w, h, flags) < 0)
        closeWindow();
}

App::~App( )
{
    if(m_gl_context != NULL)
        SDL_GL_DeleteContext(m_gl_context);
    if(m_window != NULL)
        SDL_DestroyWindow(m_window);

    delete [] m_key_state;

    SDL_Quit();
}

void AppSettings::apply( ) const
{
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depth_size);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, double_buffer);
    if(multi_samples > 0)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multi_samples);
    }

    int major= major_version;
    int minor= minor_version;
    if((context_flags != 0 || profile_flags != 0) && major == 0)
    {
        // il faut imposer une version opengl pour obtenir un contexte debug / core
    #ifdef GK_OPENGL3
        major= 3; minor= 3;
    #endif
    #ifdef GK_OPENGL4
        major= 4; minor= 3;
    #endif
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);

    if(context_flags != 0)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
    if(profile_flags != 0)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile_flags);
}



int App::resizeWindow( const int w, const int h )
{
    // redimensionnement la fenetre
    SDL_SetWindowSize(m_window, w, h);

    // conserve la nouvelle taille
    SDL_GetWindowSize(m_window, &m_width, &m_height);
    return 0;
}

int App::createGLContext( const AppSettings& settings )
{
    //~ settings.apply();       // avant la creation de la fenetre !!

    m_gl_context= SDL_GL_CreateContext(m_window);
    if(m_gl_context == NULL)
    {
        printf("error creating openGL %d.%d context... failed.\n", settings.major_version, settings.minor_version);
        return -1;
    }

    SDL_GL_SetSwapInterval(settings.swap_control);      // apres la creation du contexte ...

    {
        printf("openGL version: '%s'\nGLSL version: '%s'\n",
            glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));

        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
            printf("  debug context.\n");

        GLint mask;
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
        if(mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
            printf("  compatibility profile.\n");
        if(mask & GL_CONTEXT_CORE_PROFILE_BIT)
            printf("  core profile.\n");
    }

    {
        int swap= 0;
        swap= SDL_GL_GetSwapInterval();
        printf("swap control: %s\n", swap ? "on" : "OFF");
    }

    {
        int direct_rendering= 0;
        SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &direct_rendering);
        printf("direct rendering: %s\n", direct_rendering ? "on" : "OFF");
    }

    {
        int multisample= 0;
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &multisample);
        if(multisample == 0)
            printf("multisample: OFF\n");

        else
        {
            int samples= 0;
            SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samples);
            printf("multisample: %d samples\n", samples);
        }
    }

    {
        GLint n;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &n);
        printf("texture units: %d\n", n);
    }

    {
        GLint n;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
        printf("vertex attributes: %d\n", n);
    }

    // initialise les extensions openGL
    glewExperimental= 1;        //!! force le chargement de toutes les fonctions exportees par le driver
    GLenum err= glewInit();
    if(err != GLEW_OK)
    {
        printf("%s\n", glewGetErrorString(err));
        closeWindow();
        return -1;
    }

    while(glGetError() != GL_NO_ERROR)
        {;}

    if(settings.context_flags & SDL_GL_CONTEXT_DEBUG_FLAG)
    {
        if(GLEW_ARB_debug_output)
        {
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
            glDebugMessageCallbackARB(AppDebug, NULL);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        }
    }

    if(settings.major_version != 0)
    {
        const char *version_string= (const char *) glGetString(GL_VERSION);
        int major, minor;
        if(sscanf(version_string, "%d.%d", &major, &minor) != 2)
        {
            printf("openGL version: parse error %s\n", version_string);
            major= settings.major_version;
            minor= settings.minor_version;
        }

        int flags_version= settings.major_version * 100 + settings.minor_version * 10;
        int version= major * 100 + minor * 10;
        if(flags_version > version)
        {
            printf("requested openGL version %d, not supported: %d\n", flags_version, version);
            closeWindow();
            return -1;
        }

    #ifdef VERBOSE_DEBUG
        printf("matching openGL version %d.%d, supported: %s\n",
            settings.major_version, settings.minor_version, version_string);
    #endif
    }

    // fixe l'etat openGL par defaut
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.f);

    glDisable(GL_DITHER);
    if(settings.multi_samples == 0)
        glDisable(GL_MULTISAMPLE);
    else
        glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return 0;
}


int App::createWindow( const int w, const int h, const AppSettings& settings )
{
    // initialise SDL
    if(SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE ) < 0)
    {
        printf("SDL_Init() failed:\n%s\n", SDL_GetError());
        closeWindow();
        return -1;
    }

    // enregistre le destructeur de sdl
    //~ atexit(SDL_Quit);

    settings.apply();

    // creer la fenetre et le contexte openGL
    m_window= SDL_CreateWindow("gKit",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, settings.flags);
    if(m_window == NULL)
    {
        closeWindow();
        return -1;
    }

    SDL_SetWindowDisplayMode(m_window, NULL);

    // conserve la nouvelle taille
    m_stop= 0;
    m_width= w;
    m_height= h;
    m_fullscreen= (settings.flags & SDL_WINDOW_FULLSCREEN) ? SDL_TRUE : SDL_FALSE;

    if(createGLContext(settings) < 0)
    {
        closeWindow();
        return -1;
    }

    // copie l'etat du clavier
    int keys;
    const unsigned char *state= SDL_GetKeyboardState(&keys);
    m_key_state= new unsigned char[keys];
    for(int i= 0; i < keys; i++)
        m_key_state[i]= state[i];

    // active le drag&drop de fichiers
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    return 0;
}

bool App::isClosed( )
{
    return (m_stop == 1);
}

void App::closeWindow( )
{
    m_stop= 1;
}

void App::present( )
{
    SDL_GL_SwapWindow(m_window);
}

unsigned char& App::key( const SDL_Keycode key )
{
    return m_key_state[SDL_GetScancodeFromKey(key)];
}

bool App::processEvents( )
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    m_width= event.window.data1;
                    m_height= event.window.data2;
                    SDL_SetWindowSize(m_window, m_width, m_height);

                    // prevenir l'application
                    processWindowResize(event.window);
                }
                break;

            case SDL_QUIT:
                m_stop= 1;
                break;

            case SDL_KEYUP:
                // modifier l'etat du clavier
                m_key_state[event.key.keysym.scancode]= 0;

                // prevenir l'application
                processKeyboardEvent(event.key);
                break;

            case SDL_KEYDOWN:
                // modifier l'etat du clavier
                m_key_state[event.key.keysym.scancode]= 1;

                // prevenir l'application
                //~ printf("keydown 0x%x\n", event.key.keysym.scancode);
                if(event.key.keysym.scancode >= SDL_SCANCODE_RETURN)
                    processKeyboardEvent(event.key);    // uniquement pour les touches speciales, sinon attendre les evenements textinput
                break;

            //~ case SDL_TEXTEDITING:
            //~ {
                //~ SDL_KeyboardEvent key;
                //~ key.type= SDL_KEYDOWN;
                //~ key.timestamp= event.edit.timestamp;
                //~ key.windowID= event.edit.windowID;
                //~ key.state= SDL_PRESSED;
                //~ key.keysym.scancode= SDL_Scancode(0);
                //~ printf("textedit %s\n", event.edit.text);
                //~ key.keysym.sym= event.edit.text[event.edit.length -1];
                //~ processKeyboardEvent(key);
                //~ break;
            //~ }

            case SDL_TEXTINPUT:
            {
                //~ SDL_KeyboardEvent key;
                //~ key.type= SDL_KEYDOWN;
                //~ key.timestamp= event.text.timestamp;
                //~ key.windowID= event.text.windowID;
                //~ key.state= SDL_PRESSED;
                //~ key.keysym.scancode= SDL_Scancode(0);
                //~ printf("textinput %s\n", event.edit.text);
                //~ key.keysym.sym= event.text.text[0];
                //~ processKeyboardEvent(key);

                processTextEvent(event.text.text);
                break;
            }

            case SDL_MOUSEMOTION:
                // prevenir l'application
                processMouseMotionEvent(event.motion);
                break;

            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                // prevenir l'application
                processMouseButtonEvent(event.button);
                break;

            case SDL_DROPFILE:
                //~ printf("event drop file '%s'\n", event.drop.file);
                processDropEvent(event.drop.file);
                SDL_free(event.drop.file);
                break;
        }
    }

    return (m_stop == 0);
}


int App::run( )
{
    if(isClosed())
    {
        printf("App::run( ): no window.\n");
        return -1;
    }

    // termine l'initialisation des classes derivees, chargement de donnees, etc.
    if(init() < 0)
    {
    #ifdef VERBOSE
        printf("App::init( ): failed.\n");
    #endif
        return -1;
    }

    Uint64 start= SDL_GetTicks();
    Uint64 last_frame= start;
    while(!isClosed())
    {
        // traitement des evenements : clavier, souris, fenetre, etc.
        processEvents();

        // mise a jour de la scene
        Uint64 ticks= SDL_GetTicks();
        Uint64 frame= ticks - start;
        Uint64 delta= ticks - last_frame;

        if(update(frame, delta) == 0)
            break;

        // affiche la scene
        if(draw() == 0)
            break;

        last_frame= frame;
    }

    // destruction des ressources chargees par les classes derivees.
    //! \todo detruire les ressources opengl : GLManager::manager().release(); avant la desctruction du contexte
    if(quit() < 0)
        return -1;

    return 0;
}

}
