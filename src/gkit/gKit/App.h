
#ifndef _GK_APP_H
#define _GK_APP_H

#include "GL/GLPlatform.h"
#include "SDLPlatform.h"


namespace gk {

//! parametres de creation du contexte openGL.
struct AppSettings
{
    int depth_size;     //!< precision du zbuffer.
    int double_buffer;  //!< buffers de dessin et d'affichage separes.
    int swap_control;   //!< synchronisation de l'affichage avec l'ecran.
    
    int multi_samples;  //!< sur-echantillonnage, mode msaa
    
    int major_version;  //!< version openGL.
    int minor_version;  //!< version openGL.
    unsigned int context_flags; //!< parametres de creation du contexte, debug par exemple.
    unsigned int profile_flags; //!< parametres de creation du contexte, core profile par exemple.
    
    unsigned int flags; //! \todo GK_OPENCL, GK_CUDA
    
    //! constructeur par defaut.
    AppSettings( )
        :
        depth_size(16),
        double_buffer(1),
        swap_control(1),
        multi_samples(0),
        major_version(0),
        minor_version(0),
        context_flags(0),
        profile_flags(0),
        flags(0)
    {
        flags= flags | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    }
    
    ~AppSettings( ) {}
    
    //! constructeur par defaut. parametres de la fenetre : fullscreen, resizeable, etc. cf SDL_CreateWindow.
    AppSettings( const unsigned int _flags )
        :
        depth_size(16),
        double_buffer(1),
        swap_control(1),
        multi_samples(0),
        major_version(0),
        minor_version(0),
        context_flags(0),
        profile_flags(0),
        flags(_flags)
    {
        flags= flags | SDL_WINDOW_OPENGL;
    }
    
    //! fixe les parametres de la fenetre. cf SDL_CreateWindow.
    void setFlags( const unsigned int _flags )
    {
        flags= _flags | SDL_WINDOW_OPENGL;
    }
    
    //! desactive des parametres de la fenetre. cf SDL_CreateWindow.
    void maskFlags( const unsigned int _flags )
    {
        flags= (flags & ~_flags) | SDL_WINDOW_OPENGL;
    }
    
    //! active le mode plein ecran, au lieu du mode fenetre definit par defaut.
    void setFullscreen( )
    {
        flags= (flags & ~SDL_WINDOW_RESIZABLE) | SDL_WINDOW_FULLSCREEN;
        // fullscreen et resizable sont mutuellement exclusifs
    }
    
    //! taux de sur-echantillonnage.
    void setSamples( const int samples )
    {
        multi_samples= samples;
    }
    
    //! precision du zbuffer.
    void setDepthBufferSize( const int size )
    {
        depth_size= size;
    }
    
    //! active (1) /desactive (0) la synchronisation de l'affichage.
    void setSwapControl( const int swap= 1 )
    {
        swap_control= swap;
    }
    
    //! fixe la version openGL du contexte, par exemple setGLVersion(3,3).
    void setGLVersion( const int major, const int minor )
    {
        major_version= major;
        minor_version= minor;
    }
    
    //! utilisation interne. fixe les parametres de creation du contexte, cf. SDL_GL_SetAttribute.
    void setGLContextFlags( const unsigned int flags )
    {
        context_flags= flags;
    }
    
    //! utilisation interne. fixe les parametres du type de contexte (compatibility, debug, etc.), cf. SDL_GL_SetAttribute.
    void setGLProfileFlags( const unsigned int flags )
    {
        profile_flags= flags;
    }
    
    //! demande la creation d'un contexte debug.
    void setGLDebugContext( )
    {
        setGLContextFlags(context_flags | SDL_GL_CONTEXT_DEBUG_FLAG);
    }
    
    //! demande la creation d'un contexte core profile.
    void setGLCoreProfile( )
    {
        setGLProfileFlags(profile_flags | SDL_GL_CONTEXT_PROFILE_CORE);
    }
   
    //! demande la creation d'un contexte openGL ES 1/2/3.
    //! \todo a tester
    void setGLESProfile( const int major, const int minor= 0 )
    {
        setGLProfileFlags(profile_flags | SDL_GL_CONTEXT_PROFILE_ES);
        major_version= major;
        minor_version= minor;
    }
    
    //! utilisation interne. 
    void apply( ) const;
};

#ifndef _MSC_VER
  #define GK_CALLBACK
#else
  #define GK_CALLBACK __stdcall
#endif

extern void GK_CALLBACK AppDebug( GLenum source, GLenum type, unsigned int id, GLenum severity, 
    GLsizei length, const char *message, const void *userParam );


//! application de base SDL + openGL. 
//! consultez le <a href="http://www710.univ-lyon1.fr/~jciehl/Public/SDL_PG/index.html">libSDL Programming Guide</a> 
class App
{
protected:
    SDL_Window *m_window;
    SDL_GLContext m_gl_context;

protected:
    unsigned char *m_key_state;

    int m_width, m_height;
    SDL_bool m_fullscreen;
    int m_stop;

    App( const App& );
    App& operator=( const App& );

    int createGLContext( const AppSettings& settings );

public:
    //! \name initialisation manuelle de l'application.
    // @{
    //! construction d'une application par defaut (non initialisee, pas de contexte opengl, pas de fenetre, appeller createWindow() pour initialiser opengl).
    App( );

    //! cree une fenetre de dimension w x h et termine l'initialisation.
    int createWindow( const int w, const int h, const AppSettings& flags= AppSettings() );

    //! \name initialisation complete de l'application.
    // @{
    //! construction d'une application.
    //! \param w largeur de la fenetre
    //! \param h largeur de la fenetre
    //! \param flags parametres du context openGL, cf AppSettings.
    App( const int w, const int h, const AppSettings& flags= AppSettings() );
    // @}

    //! destruction de l'application.
    virtual ~App( );

    //! redimensionne la fenetre.
    int resizeWindow( const int w, const int h );
    
    //! renvoie les dimensions de la fenetre
    int windowWidth( ) const
    {
        return m_width;
    }

    //! renvoie les dimensions de la fenetre
    int windowHeight( ) const
    {
        return m_height;
    }
    
    //! indique si la fenetre est fermee.
    bool isClosed( );

    //! demander a fermer la fenetre.
    void closeWindow( );
    
    //! demande a afficher l'image construite par opengl
    void present( );
    
    //! traitement des evenements systemes : clavier, souris, fenetre, etc.
    bool processEvents( );

    //! \name interception des evenements clavier / souris / redimensionnement.
    //! App fournit une implementation par defaut qui ne fait rien 
    //! (les evenements sont reellement traites par processEvents()).
    //! a deriver, pour integrer nvWidgets, par exemple.
    
    // @{
    //! traitement des redimensionnements de la fenetre de l'application.
    virtual void processWindowResize( SDL_WindowEvent& event )
    {
        return;
    }
    
    //! traitement des clicks souris.
    virtual void processMouseButtonEvent( SDL_MouseButtonEvent& event )
    {
        return;
    }
    
    //! traitement des deplacements de la souris.
    virtual void processMouseMotionEvent( SDL_MouseMotionEvent& event )
    {
        return;
    }
    
    //! traitement des evenements claviers.
    virtual void processKeyboardEvent( SDL_KeyboardEvent& event )
    {
        return;
    }
    
    //! traitement des evenements claviers.
    virtual void processTextEvent( const char *string )
    {
        return;
    }    
    
    //! traitement des evenements drag & drop.
    virtual void processDropEvent( const char *file )
    {
        return;
    }
    // @}
    
    //! renvoie l'etat d'une touche selon son code ascii ou son keycode SDLK_xxx. 
    //! la liste des codes est dans <a href="http://www710.univ-lyon1.fr/~jciehl/Public/SDL_PG/sdlkey.html">libSDL Programming Guide</a> 
    /*! exemple : key('a'), ou key(SDLK_UP)
    
        la fonction renvoie une reference, on peut l'utiliser pour desactiver manuellement 
        une touche lorsque l'action a ete traitee et que l'on ne souhaite pas la re-executer a chaque image.
    
        \code
if(key('a')) {
    printf("a\n"); 
    key('a')= 0; 
}
        \endcode
     */
    unsigned char& key( const SDL_Keycode key );
    
    //! \name encapsulation de la boucle principale
    //! deriver de App pour construire une nouvelle application.

    // @{
    //! appele apres l'initialisation complete de App et la creation du device.
    //! a redefinir en derivant App.
    //! doit renvoyer -1 en cas d'echec. 
    virtual int init( ) { return 0; }

    //! demarre la boucle principale.
    //! App fournit une version complete, il n'est pas necessaire de la redefinir.
    virtual int run( );

    //! appele avant la fermeture de l'application.
    //! a redefinir en derivant App.
    //! doit renvoyer -1 en cas d'echec. 
    virtual int quit( ) { return 0; }

    //! appele regulierement pour mettre a jour la scene (animations, deplacements, etc.).
    //! \param time millisecondes ecoulees depuis le lancement,
    //! \param delta millisecondes ecoulees depuis l'appel precedent.
    //! a redefinir en derivant App.
    //! doit renvoyer 0 pour sortir de la boucle principale, ou 1 pour continuer.
    virtual int update( const int time, const int delta ) { return 1; }
    
    //! appele regulierement pour afficher la scene.
    //! a redefinir en derivant App.
    //! doit renvoyer 0 pour sortir de la boucle principale, ou 1 pour continuer.
    virtual int draw( ) { return 1; }
    // @}
};

}

#endif
