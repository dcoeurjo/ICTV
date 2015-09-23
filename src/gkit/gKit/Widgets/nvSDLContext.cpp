
#include "Widgets/nvSDLContext.h"
#include "Widgets/nvSDLFont.h"
#include "Widgets/nvGLCorePainter.h"


namespace nv {

SdlContext::SdlContext()
    :
    UIContext( new SdlFont, new GLCorePainter ),
    m_mouseX(0), m_mouseY(0), m_mouseButton(0)
{
    SDL_StartTextInput();
}

SdlContext::~SdlContext()
{
    SDL_StopTextInput();
    delete m_font;
    delete m_painter;
}
    
int SdlContext::init( )
{
    if(m_font->init() < 0)
        return -1;
    if(m_painter->init(m_font) < 0)
        return -1;
    
    return 0;
}

void SdlContext::processMouseMotionEvent( SDL_MouseMotionEvent& event )
{
    if(event.state != 0)
        UIContext::mouseMotion(event.x, event.y);    
}

void SdlContext::processMouseButtonEvent( SDL_MouseButtonEvent& event )
{
    int uiButton= -1;
    if(event.button == SDL_BUTTON_LEFT)
        uiButton= MouseButton_Left;
    else if(event.button == SDL_BUTTON_MIDDLE)
        uiButton= MouseButton_Middle;
    else if(event.button == SDL_BUTTON_RIGHT)
        uiButton= MouseButton_Right;
    else
        // another button event (wheel ?)
        return;
    
    int uiState= -1;
    if(event.type == SDL_MOUSEBUTTONDOWN)
        uiState= 1;
    else if(event.type == SDL_MOUSEBUTTONUP)
        uiState= 0;
    else
        // should never happen
        return;
    
    m_mouseX= event.x;
    m_mouseY= event.y;
    m_mouseButton= event.button;

    int uiMod= 0;
    SDL_Keymod mod= SDL_GetModState();
    
    if ( mod & KMOD_ALT) uiMod |= ButtonFlags_Alt;
    if ( mod & KMOD_SHIFT) uiMod|= ButtonFlags_Shift;
    if ( mod & KMOD_CTRL) uiMod |= ButtonFlags_Ctrl;

    UIContext::mouse(uiButton, uiState, uiMod, event.x, event.y);
}

void SdlContext::processDropEvent( const char *file )
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    UIContext::file(file, x, y);
}

void SdlContext::processTextEvent( const char *string )
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    //~ printf("widgets: text %s\n", string);
    UIContext::text(string, x, y);
}

void SdlContext::processKeyboardEvent( SDL_KeyboardEvent& event )
{
    if(event.type != SDL_KEYDOWN)
        return;
    
    unsigned int key= translateKey(event.keysym.sym);
    if(key == 0)
        return;
    
    int x, y;
    SDL_GetMouseState(&x, &y);
    
    //~ printf("widgets: scan 0x%x '%c' key %c %d, key name %s\n", event.keysym.scancode, event.keysym.scancode, event.keysym.sym, translateKey(event.keysym.sym), SDL_GetKeyName(event.keysym.sym));
    UIContext::keyboard(key, x, y);
}

unsigned int SdlContext::translateKey( const SDL_Keycode k ) {
    switch (k) {
    case SDLK_F1 :
        return Key_F1;
    case SDLK_F2 :
        return Key_F2;
    case SDLK_F3 :
        return Key_F3;
    case SDLK_F4 :
        return Key_F4;
    case SDLK_F5 :
        return Key_F5;
    case SDLK_F6 :
        return Key_F6;
    case SDLK_F7 :
        return Key_F7;
    case SDLK_F8 :
        return Key_F8;
    case SDLK_F9 :
        return Key_F9;
    case SDLK_F10 :
        return Key_F10;
    case SDLK_F11 :
        return Key_F11;
    case SDLK_F12 :
        return Key_F12;
    case SDLK_LEFT :
        return Key_Left;
    case SDLK_UP :
        return Key_Up;
    case SDLK_RIGHT :
        return Key_Right;
    case SDLK_DOWN :
        return Key_Down;
    case SDLK_PAGEUP :
        return Key_PageUp;
    case SDLK_PAGEDOWN :
        return Key_PageDown;
    case SDLK_HOME :
        return Key_Home;
    case SDLK_END :
        return Key_End;
    case SDLK_INSERT :
        return Key_Insert;
    case SDLK_DELETE:
        return Key_Delete;
    case SDLK_TAB:
        return Key_Tab;
    case SDLK_BACKSPACE:
        return Key_Backspace;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        return Key_Enter;
    default:
        return 0;
        //~ return k;
    }
}

}       // namespace 

