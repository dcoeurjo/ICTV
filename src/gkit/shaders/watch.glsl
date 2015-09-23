
#define GLDEBUGGER
    
#ifdef GLDEBUGGER
    struct watch
    {
        int n;
        int type;       // type de la variable
        int id;         // identifiant de la variable watch1, watch2, etc.
        int pad;        // aligne sur un vec4 de toutes facons
        vec4 value;
    };
    
    layout( binding= 0 ) buffer watchBuffer
    {
        watch watches[];
        // watches[0].n compteur atomique pour inserer les valeurs dans le buffer
        // watches[1..7].n compteur atomique pour chaque shader
        // watches[8..] debut du buffer
    };
    
    uniform int watch_type;
    uniform vec4 watch_invocation;
    
    // watch generique watch_id / type / value / shader / condition
    #define WATCHCC(w, t, v, cc) do { \
        if(cc) { \
            if(atomicAdd(watches[SHADER].n, 1) < 16) { \
                int _wi= atomicAdd(watches[0].n, 1); \
                _wi = _wi + 8; \
                watches[_wi].type= t; \
                watches[_wi].id= w; \
                watches[_wi].pad= __LINE__; \
                watches[_wi].value= v; \
            } \
        } \
    } while(false)

    // assert generique assert_id / shader / contexte / condition
    #define ASSERT(a, t, v, cc) WATCHCC(-(a), t, v, !(cc))
    
#else
    #define WATCHCC(w, t, v, cc)
    #define ASSERT(a, t, v, cc)
    
#endif
    
    #define   WATCH1(variable)  WATCH(1, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH1(variable)  WATCH(1, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH1(variable)  WATCH(1, 3, vec4(variable.xyz, 0))
    #define V4WATCH1(variable)  WATCH(1, 4, vec4(variable))
    
    #define   WATCH2(variable)  WATCH(2, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH2(variable)  WATCH(2, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH2(variable)  WATCH(2, 3, vec4(variable.xyz, 0))
    #define V4WATCH2(variable)  WATCH(2, 4, vec4(variable))
    
    #define   WATCH3(variable)  WATCH(3, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH3(variable)  WATCH(3, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH3(variable)  WATCH(3, 3, vec4(variable.xyz, 0))
    #define V4WATCH3(variable)  WATCH(3, 4, vec4(variable))

    #define   WATCH4(variable)  WATCH(4, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH4(variable)  WATCH(4, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH4(variable)  WATCH(4, 3, vec4(variable.xyz, 0))
    #define V4WATCH4(variable)  WATCH(4, 4, vec4(variable))
    
    #define   WATCH5(variable)  WATCH(5, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH5(variable)  WATCH(5, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH5(variable)  WATCH(5, 3, vec4(variable.xyz, 0))
    #define V4WATCH5(variable)  WATCH(5, 4, vec4(variable))

    #define   WATCH6(variable)  WATCH(6, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH6(variable)  WATCH(6, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH6(variable)  WATCH(6, 3, vec4(variable.xyz, 0))
    #define V4WATCH6(variable)  WATCH(6, 4, vec4(variable))
    
    #define   WATCH7(variable)  WATCH(7, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH7(variable)  WATCH(7, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH7(variable)  WATCH(7, 3, vec4(variable.xyz, 0))
    #define V4WATCH7(variable)  WATCH(7, 4, vec4(variable))

    #define   WATCH8(variable)  WATCH(8, 1, vec4(float(variable), 0, 0, 0))
    #define V2WATCH8(variable)  WATCH(8, 2, vec4(variable.xy, 0, 0))
    #define V3WATCH8(variable)  WATCH(8, 3, vec4(variable.xyz, 0))
    #define V4WATCH8(variable)  WATCH(8, 4, vec4(variable))
    
    
    #ifdef VERTEX_SHADER
        #define SHADER 1
        #define VERTEX vec4(gl_VertexID, gl_InstanceID, /*gl_DrawID*/ 0, 0)
        //~ #define WATCH(w, t, v) WATCHCC(w, t, v, (watch_type == SHADER) && (watch_invocation == VERTEX))
        #define WATCH(w, t, v) WATCHCC(w, t, v, (watch_invocation == VERTEX))
        
        #define ASSERT1(cc) ASSERT(1, SHADER, VERTEX, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, VERTEX, cc)
    #endif
    
    #ifdef FRAGMENT_SHADER
        #define SHADER 2
        #define FRAGMENT vec4(gl_FragCoord.xy, gl_SampleID, gl_PrimitiveID)
        //~ #define WATCH(w, t, v) WATCHCC(w, t, v, (watch_type == SHADER) && (watch_invocation == FRAGMENT))
        #define WATCH(w, t, v) WATCHCC(w, t, v, (watch_invocation == FRAGMENT))
        
        #define ASSERT1(cc) ASSERT(1, SHADER, FRAGMENT, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, FRAGMENT, cc)
    #endif
    
    #ifdef GEOMETRY_SHADER
        #define SHADER 3
        #define GEOMETRY vec4(gl_PrimitiveIDIn, gl_InvocationID, gl_PrimitiveID, 0)
        #define WATCH(w, t, v) WATCHCC(w, t, v, watch_invocation == GEOMETRY)
        
        #define ASSERT1(cc) ASSERT(1, SHADER, GEOMETRY, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, GEOMETRY, cc)
    #endif
    
    #ifdef CONTROL_SHADER
        #define SHADER 4
        #define CONTROL vec4(gl_PrimitiveID, gl_InvocationID, 0, 0)
        #define WATCH(w, t, v) WATCHCC(w, t, v, watch_invocation == CONTROL)
        
        #define ASSERT1(cc) ASSERT(1, SHADER, CONTROL, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, CONTROL, cc)
    #endif
    
    #ifdef EVALUATION_SHADER
        #define SHADER 5
        #define EVALUATION (vec4(gl_PrimitiveID, gl_InvocationID, 0, 0))
        #define WATCH(w, t, v) WATCHCC(w, t, v, watch_invocation == EVALUATION)
        
        #define ASSERT1(cc) ASSERT(1, SHADER, EVALUATION, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, EVALUATION, cc)
    #endif
    
    #ifdef COMPUTE_SHADER
        #define SHADER 6
        #define COMPUTE vec4(gl_GLobalInvocationID, gl_LocalInvocationIndex)
        #define WATCH(w, t, v) WATCHCC(w, t, v, watch_invocation == COMPUTE)
        
        #define ASSERT1(cc) ASSERT(1, SHADER, COMPUTE, cc)
        #define ASSERT2(cc) ASSERT(2, SHADER, COMPUTE, cc)
    #endif

