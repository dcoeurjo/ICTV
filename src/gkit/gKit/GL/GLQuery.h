
#ifndef _GL_QUERY_H
#define _GL_QUERY_H

#include "GL/GLPlatform.h"
#include "GLResource.h"


namespace gk {

struct GLCounterState
{
    GLuint64 cpu_start;
    GLuint64 cpu_stop;
    GLuint64 cpu_time;
    
    GLuint64 gpu_start;
    GLuint64 gpu_stop;
    GLuint64 gpu_time;
    
    GLCounterState( ) : cpu_start(0), cpu_stop(0), cpu_time(0), gpu_start(0), gpu_stop(0), gpu_time(0) {}
    
    std::string summary( const std::string& message= "" );
};

std::string OverlapGPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b );
std::string OverlapCPUSummary( const std::string &message, const GLCounterState& a, const GLCounterState& b );


//! representation d'une requete.
class GLQuery : public GLResource
{
    // non copyable
    GLQuery( const GLQuery& );
    GLQuery& operator= ( const GLQuery& );
    
public:
    GLuint64 value;     //!< resultat de la requete.
    GLint64 cpu_start;
    GLint64 cpu_stop;
    
    GLenum target;      //!< type de la requete.
    bool started;       //!< etat de la requete (demarree ou pas)
    
    //! constructeur par defaut.
    GLQuery( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLQuery( const char *_label ) : GLResource(_label) {}
    
    //! creation d'une requete de type target.
    GLQuery *create( const GLenum _target )
    {
        glGenQueries(1, &name);
        if(name == 0)
            return this;
        
        target= _target;
        started= false;
        value= 0;
        manage();
        return this;
    }
    
    //! destruction de la requete.
    void release( )
    {
        if(name != 0)
            glDeleteQueries(1, &name);
        name= 0;
    }
    
    //! destructeur de l'objet c++, l'objet opengl doit etre deja detruit par release( ).
    ~GLQuery( ) {}
    
    //! debut de la requete. 
    //! et obtention du resultat de la requete si elle a demarree. permet de limiter l'attente du resultat.
    void begin( )
    {
        if(started)
        {
            //~ GLint64 start; glGetInteger64v(GL_TIMESTAMP, &start);
            // retarde le plus possible l'obtention du resultat, pour limiter l'attente...
            glGetQueryObjectui64v(name, GL_QUERY_RESULT, &value);
            //~ GLint64 stop; glGetInteger64v(GL_TIMESTAMP, &stop);
            //~ GLint64 time= (stop - start) / 1000;
            //~ printf("query sync: %luus\n", time);
        }
        
        glGetInteger64v(GL_TIMESTAMP, &cpu_start);
        glBeginQuery(target, name);
        started= true;
    }
    
    //! fin de la requete.
    void end( )
    {
        glGetInteger64v(GL_TIMESTAMP, &cpu_start);
        glEndQuery(target);
    }
    
    //! renvoie vrai si le resultat de la requete est immediatement disponible.
    bool available( )
    {
        GLuint status= 0;
        glGetQueryObjectuiv(name, GL_QUERY_RESULT_AVAILABLE, &status);
        return (status == GL_TRUE);
    }
    
    //! renvoie le resultat de la requete.
    GLuint result( ) { return (GLuint) value; }
    
    //! renvoie le resultat de la requete.
    GLuint64 result64( ) { return value; }

    //! renvoie le resultat de la requete.
    GLuint64 sync( )
    {
        if(started == false)
            ERROR("query not started / counter not available\n");
        
        glGetQueryObjectui64v(name, GL_QUERY_RESULT, &value);
        return value;
    }
    
    //! renvoie un resume texte des compteurs.
    std::string summary( const std::string& message= "" );
    
    GLCounterState state( )
    {
        GLCounterState counter;
        
        counter.cpu_start= cpu_start;
        counter.cpu_stop= cpu_stop;
        counter.cpu_time= cpu_stop - cpu_start;
        counter.gpu_time= value;
        return counter;
    }
    
    //! renvoie un objet c++ associe a la requete 0.
    static 
    GLQuery *null( )
    {
        static GLQuery object("null query");
        return &object;
    }
};


//! representation d'un chrono opengl, mesure le temps d'execution cpu et gpu.
class GLCounter : public GLResource
{
    // non copyable
    GLCounter( const GLCounter& );
    GLCounter& operator= ( const GLCounter& );
    
public:
    GLQuery *counter0;          //!< stocke l'instant de depart / temps gpu.
    GLQuery *counter1;          //!< stocke l'instant de fin / temps gpu.
    
    GLint64 cpu_value0;         //!< stocke l'instant de depart / temps cpu.
    GLint64 cpu_value1;         //!< stocke l'instant de fin / temps cpu.
    
    GLuint64 value;             //!< resultat de la requete / temps gpu.
    GLuint64 cpu_value;         //!< resultat de la requete / temps cpu.
    
    bool started;               //!< etat du compteur (demarre ou pas)
    
    //! constructeur par defaut.
    GLCounter( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLCounter( const char *_label ) : GLResource(_label) {}
    
    //! constructeur nomme.
    GLCounter *create( ) 
    {
        counter0= (new GLQuery())->create(GL_TIME_ELAPSED);
        counter1= (new GLQuery())->create(GL_TIME_ELAPSED);
        cpu_value0= 0;
        cpu_value1= 0;
        value= 0;
        cpu_value= 0;
        started= false;
        return this;
    }
    
    //! destructeur nomme.
    void release( ) {}
    
    //! destructeur.
    ~GLCounter( ) {}
    
    //! demarre les compteurs.
    void start( )
    {
        if(started)
        {
            GLuint64 value0= 0;
            GLuint64 value1= 0;
            glGetQueryObjectui64v(counter0->name, GL_QUERY_RESULT, &value0);
            glGetQueryObjectui64v(counter1->name, GL_QUERY_RESULT, &value1);
            value= value1 - value0;
            cpu_value= cpu_value1 - cpu_value0;
        }
        
        glGetInteger64v(GL_TIMESTAMP, &cpu_value0);
        glQueryCounter(counter0->name, GL_TIMESTAMP);
        started= true;
    }
    
    //! arrete les compteurs.
    void stop( )
    {
        glGetInteger64v(GL_TIMESTAMP, &cpu_value1);
        glQueryCounter(counter1->name, GL_TIMESTAMP);
    }
    
    //! force l'attente des compteurs gpu. et renvoie le temps gpu.
    GLuint64 sync( )
    {
        GLuint64 value0= 0;
        GLuint64 value1= 0;
        glGetQueryObjectui64v(counter0->name, GL_QUERY_RESULT, &value0);
        glGetQueryObjectui64v(counter1->name, GL_QUERY_RESULT, &value1);
        value= value1 - value0;
        cpu_value= cpu_value1 - cpu_value0;
        return value;
    }
    
    //! renvoie le temps gpu.
    GLuint64 gpu64() { return value; }       // retourne 0 la premiere fois
    //! renvoie le temps cpu.
    GLuint64 cpu64() { return cpu_value; }        // retourne 0 la premiere fois
    
    //! renvoie un resume texte des compteurs.
    std::string summary( const std::string& message= "" );
    
    GLCounterState state( )
    {
        GLCounterState counter;
        
        counter.cpu_start= cpu_value0;
        counter.cpu_stop= cpu_value1;
        counter.cpu_time= cpu_value;
        
        glGetQueryObjectui64v(counter0->name, GL_QUERY_RESULT, &counter.gpu_start);
        glGetQueryObjectui64v(counter1->name, GL_QUERY_RESULT, &counter.gpu_stop);
        counter.gpu_time= value;
        
        return counter;
    }
    
    //! renvoie un objet null.
    static GLCounter *null( )
    {
        static GLCounter object("null query counter");
        return &object;
    }
};


class GLSync : public GLResource
{
    GLSync( const GLSync& );
    GLSync& operator= ( const GLSync& );
    
public:
    GLsync fence;
    GLenum condition;
    GLbitfield flags;
    
    //! constructeur par defaut.
    GLSync( ) : GLResource() {}
    //! constructeur d'un objet opengl nomme, cf. utilisation de khr_debug.
    GLSync( const char *_label ) : GLResource(_label) {}
    
    //! constructeur nomme.
    GLSync *create( const GLenum _condition= GL_SYNC_GPU_COMMANDS_COMPLETE, const GLbitfield _flags= 0 )
    {
        condition= _condition;
        flags= _flags;
        fence= glFenceSync(condition, flags);
        if(fence == 0)
            return this;
        
        // n'insere pas la barriere dans le gestionnaire de ressource opengl. elle sera detruite tout de suite.
        return this;
    }
    
    void release( ) 
    {
        
        if(fence != 0)
            glDeleteSync(fence);
        fence= 0;
    }
    
    void sync( )
    {
        if(fence != 0)
            glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
        release();
    }
    
    void clientSync( const GLuint64 timeout )
    {
        glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
        release();
    }
    
    static
    GLSync *null( )
    {
        static GLSync object("null sync");
        return &object;
    }
};


//! fonction utilitaire : creation simplifiee d'une requete GL_TIME_ELAPSED.
inline
GLQuery *createTimeQuery( )
{
    return (new GLQuery())->create(GL_TIME_ELAPSED);
}

//! fonction utilitaire : creation simplifee d'un timer cpu / gpu.
inline
GLCounter *createTimer( )
{
    return (new GLCounter())->create();
}

//! fonction utilitaire : creation simplifiee d'une requete GL_SAMPLES_PASSED.
inline
GLQuery *createOcclusionQuery( )
{
    return (new GLQuery())->create(GL_SAMPLES_PASSED);
}

//! fonction utilitaire : creation simplifiee d'une requete GL_PRIMITIVES_GENERATED.
inline
GLQuery *createPrimitiveQuery( )
{
    return (new GLQuery())->create(GL_PRIMITIVES_GENERATED);
}

//! fonction utilitaire : creation simplifiee d'une requete GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN.
inline
GLQuery *createFeedbackQuery( )
{
    return (new GLQuery())->create(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
}


}       // namespace 

#endif
