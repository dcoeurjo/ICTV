// gk::Widgets utilise plusieurs portions de code issues de nvWidgets
// http://code.google.com/p/nvidia-widgets/

// nvWidgets.h - User Interface library
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#ifndef NV_WIDGETS_H
#define NV_WIDGETS_H

#include <cstdlib>
#include <algorithm>

#include <sys/types.h>


namespace nv {

struct Point {
    Point() : x(0), y(0) {}
    Point(int ix, int iy) : x(ix), y(iy) {}
    Point(const Point & p) : x(p.x), y(p.y) {}

    const Point& operator= (const Point & p) { x= p.x; y= p.y; return *this; }

    int x, y;
};

struct Rect {
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(const Point & p) : x(p.x), y(p.y), w(0), h(0) {}
    Rect(int ix, int iy, int iw = 0, int ih = 0) : x(ix), y(iy), w(iw), h(ih) {}
    Rect(const Rect & r) : x(r.x), y(r.y), w(r.w), h(r.h) {}

    const Rect& operator= (const Rect & r) { x= r.x; y= r.y; w= r.w; h= r.h; return *this; }

    int x, y;
    int w, h;

    static const Rect null;
};

struct RGB8 {
    RGB8() : r(0), g(0), b(0), a(255) {}
    RGB8( const unsigned char _r, const unsigned char _g, const unsigned char _b ) : r(_r), g(_g), b(_b), a(255) {}
    RGB8( const float _r, const float _g, const float _b ) : r(clamp(_r)), g(clamp(_g)), b(clamp(_b)), a(255) {}
    
    RGB8( const RGB8& _color ) : r(_color.r), g(_color.g), b(_color.b), a(_color.a) {}
    RGB8& operator= ( const RGB8& _color ) { r= _color.r; g= _color.g; b= _color.b; a= _color.a; return *this; }

    float clamp ( const float value ) { if(value < 0.f) return 0.f; else return std::min(255.f, value * 255.f); }
    
    unsigned char r, g, b, a;
};

enum ButtonFlags {
    ButtonFlags_Off = 0x0,
    ButtonFlags_On = 0x1,
    ButtonFlags_Begin = 0x2,
    ButtonFlags_End = 0x4,
    ButtonFlags_Shift = 0x8,
    ButtonFlags_Alt = 0x10,
    ButtonFlags_Ctrl = 0x20,
};

struct ButtonState {
    int state;
    time_t time;
    Point cursor;
};

// An enum to identify the mouse buttons
enum MouseButton {
    MouseButton_Left,
    MouseButton_Middle,
    MouseButton_Right,
};

// An enum to identify the special key buttons not translated with ASCII codes
enum Key {
    Key_Enter= 128,
    Key_Backspace,
    Key_Delete,
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,

    Key_Left,
    Key_Up,
    Key_Right,
    Key_Down,
    Key_PageUp,
    Key_PageDown,
    Key_Home,
    Key_End,
    Key_Insert,
    Key_Tab
};

// The various flags to modify the behavior of the groups
enum GroupFlags {
    // Layout behavior flags
    GroupFlags_LayoutNone = 0x01,
    GroupFlags_LayoutVertical = 0x02,
    GroupFlags_LayoutHorizontal = 0x04,
    GroupFlags_LayoutMask = 0x07,
    GroupFlags_LayoutXMask = 0xffff ^ GroupFlags_LayoutMask,

    // Alignment flags for the widgets inserted in the group
    GroupFlags_AlignLeft = 0x10,
    GroupFlags_AlignRight = 0x20,
    GroupFlags_AlignTop = 0x40,
    GroupFlags_AlignBottom = 0x80,
    GroupFlags_AlignMask = 0xf0,
    GroupFlags_AlignXMask = 0xffff ^ GroupFlags_AlignMask,

    // Start flags defining the starting origin of the group
    GroupFlags_StartLeft = 0x100,
    GroupFlags_StartRight = 0x200,
    GroupFlags_StartTop = 0x400,
    GroupFlags_StartBottom = 0x800,
    GroupFlags_StartMask = 0xf00,
    GroupFlags_StartXMask = 0xffff ^ GroupFlags_StartMask,

    // Optional flags
    GroupFlags_LayoutForce = 0x8000,
    GroupFlags_LayoutDefault = 0x4000,
    GroupFlags_LayoutNoMargin = 0x2000,
    GroupFlags_LayoutNoSpace = 0x1000,

    // Predefined configurations
    GroupFlags_GrowRightFromBottom   = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartBottom | GroupFlags_AlignBottom ,
    GroupFlags_GrowRightFromTop      = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartTop    | GroupFlags_AlignTop ,
    GroupFlags_GrowLeftFromBottom    = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartBottom | GroupFlags_AlignBottom,
    GroupFlags_GrowLeftFromTop       = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartTop    | GroupFlags_AlignTop,
    GroupFlags_GrowUpFromLeft        = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
    GroupFlags_GrowUpFromRight       = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartRight  | GroupFlags_AlignRight ,
    GroupFlags_GrowDownFromLeft      = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
    GroupFlags_GrowDownFromRight     = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartRight  | GroupFlags_AlignRight ,

    GroupFlags_LayoutDefaultFallback = GroupFlags_GrowDownFromLeft,
};

struct Group {
    Rect bounds;  // anchor point + width and height of the region
    int flags;    // group behavior
    int margin;   // border
    int space;    // interior
};


enum Color {
    cBase = 0,
    cBool = 4,
    cOutline = 8,
    cFont = 12,
    cFontBack = 16,
    cTranslucent = 20,
    cNbColors = 24,
};


//! glyph metrics + texture bounds
struct UIGlyph
{
    //! character metrics, cf freetype doc, sdl_ttf doc
    int xmin, xmax;
    int ymin, ymax;
    int advance;

    //! texture bounds
    float tex_xmin, tex_xmax;
    float tex_ymin, tex_ymax;
    int tex_w, tex_h;
};

//! texture font data.
struct UIFontTexture
{
    void *data;         //!< rgba8 texel data[w*h*4]
    int width;          //!< texture width
    int height;         //!< texture height

    UIFontTexture( ) : data(NULL), width(0), height(0) {}
};

//! system independent font.
class UIFont
{
protected:
    UIGlyph m_glyphs[128];
    UIFontTexture m_texture;
    int m_line_skip;

public:
    UIFont( ) : m_texture(), m_line_skip(0) {}
    virtual ~UIFont( ) {}

    virtual int init( )= 0;     // ??

    const UIGlyph& getGlyph( const int glyph ) const;
    const UIFontTexture *getFontTexture( ) const;

    int getFontHeight( ) const;
    int getTextLineWidth( const char *text ) const;
    int getTextLineWidthAt( const char *text, int charNb ) const;
    int getTextSize( const char *text, int& nbLines ) const;
    Rect getTextRect( const char *text ) const;
    Rect getTextRect( const int n, const char *options[] ) const;
    int getPickedCharNb( const char *text, const nv::Point& at ) const;
};


class UIPainter {
    // non copyable
    UIPainter( const UIPainter& );
    UIPainter& operator=( const UIPainter& );

public:
    UIPainter( ) {}
    virtual ~UIPainter( ) {}

    virtual int init( UIFont *font )= 0;
    virtual void reshape( const Rect &window )= 0;
    virtual void begin( )= 0;
    virtual void end()= 0;

    // These methods should be called between begin/end
    void drawFrame(const Rect& r, int margin, int style);

    Rect getLabelRect(const Rect& r, const Rect& text, Rect& rt ) const;
    void drawLabel(const Rect& r, const char *text, const Rect& rt, bool isHover, int style);

    Rect getTimebarRect( const Rect& r, const Rect& text, Rect& rt, const float start, const float stop, Rect& rb );
    void drawTimebar( const Rect& r, const char *text, const Rect& rt, const float start, const float stop, const Rect& rb );

    Rect getGraphRect( const Rect& r, const int *values, const int n, Rect& rg ) const;
    void drawGraph( const Rect& r, const int *values, const int n, const Rect& rg );

    Rect getMatrixRect( const Rect& r, const int n, Rect& rc ) const;
    void drawMatrix( const Rect& r, const RGB8 *values, const int n, int *selected, int *hovered );
    
    Rect getButtonRect(const Rect& r, const Rect& text, Rect& rt) const;
    void drawButton(const Rect& r, const char *text, const Rect& rt, bool isDown, bool isHover, bool isFocus, int style);

    Rect getCheckRect(const Rect& r, const Rect& text, Rect& rt, Rect& rc) const;
    void drawCheckButton(const Rect& r, const char *text, const Rect& rt, const Rect& rr, bool isChecked, bool isHover, bool isFocus, int style);

    Rect getRadioRect(const Rect& r, const Rect& text, Rect& rt, Rect& rr) const;
    void drawRadioButton(const Rect& r, const char *text, const Rect& rt, const Rect& rr, bool isOn, bool isHover, bool isFocus, int style);

    Rect getTweakRect( const Rect& r, const Rect& text, Rect& rt, Rect& rm, Rect& rp, Rect& rs );
    void drawTweak( const Rect& r, const char *text, const Rect& rt, const Rect& rm, const Rect& rp, const Rect& rs );

    Rect getHorizontalSliderRect(const Rect& r, Rect& rs, float v, Rect& rc) const;
    void drawHorizontalSlider(const Rect& r, Rect& rs, float v, Rect& rc, bool isHover, int style);

    Rect getItemRect(const Rect& r, const Rect& text, Rect& rt) const;
    void drawListItem(const Rect& r, const char *text, const Rect& rt, bool isSelected, bool isHover, int style);

    Rect getListRect(const Rect& r, int numOptions, const Rect& options, Rect& ri, Rect& rt) const;
    void drawListBox(const Rect& r, int numOptions, const char *options[], const Rect& ri, const Rect& rt, int selected, int hovered, int style);

    Rect getComboRect(const Rect& r, int numOptions, const Rect& options, int selected, Rect& rt, Rect& ra) const;
    Rect getComboOptionsRect(const Rect& rCombo, int numOptions, const Rect& options, Rect& ri, Rect& rit) const;

    void drawComboBox(const Rect& rect, int numOptions, const char * options[], const Rect& rt, const Rect& ra, int selected, bool isHover, bool isFocus, int style);
    void drawComboOptions(const Rect& rect, int numOptions, const char * options[], const Rect& ri, const Rect& rit, int selected, int hovered, bool isHover, bool isFocus, int style);

    Rect getLineEditRect(const Rect& r, const Rect& text, Rect& rt) const;
    void drawLineEdit(const Rect& r, const char *text, const Rect& rt, int caretPos, bool isSelected, bool isHover, int style);

    Rect getPanelRect(const Rect& r, const Rect& text, Rect& rt, Rect& ra) const;
    void drawPanel(const Rect& rect, const char *text, const Rect& rt, const Rect& ra, bool isUnfold, bool isHover, bool isFocus, int style);

    // Eval widget dimensions
    int getWidgetMargin() const { return 3; }
    int getWidgetSpace() const { return 2; }
    int getAutoWidth() const { return 100; }
    int getAutoHeight() const { return m_font->getFontHeight() + 2; }

    int getCanvasMargin() const { return 5; }
    int getCanvasSpace() const { return 5; }

    // composite
    void drawText( const Rect& r , const char * text, int nbLines = 0, int caretPos = -1, bool isHover = false, bool isOn = false, bool isFocus = false );
    void drawFrame( const Rect& rect, const Point& corner, bool isHover = false, bool isOn = false, bool isFocus = false );
    void drawBoolFrame( const Rect& rect, const Point& corner, bool isHover = false, bool isOn = false, bool isFocus = false );

    // Draw primitive shapes using implementation provided by a subclass
    virtual void drawString( const Rect& rect, const char *text, int colorId )= 0;
    virtual void drawRect( const Rect& rect, int fillColorId, int borderColorId )= 0;
    virtual void drawRGBRect( const Rect& rect, const RGB8 fillColor, const RGB8 borderColor )= 0;
    virtual void drawRoundedRect( const Rect& rect, const Point& corner, int fillColorId, int borderColorId )= 0;
    virtual void drawRoundedRectOutline( const Rect& rect, const Point& corner, int borderColorId )= 0;
    virtual void drawCircle( const Rect& rect, int fillColorId, int borderColorId )= 0;
    virtual void drawMinus( const Rect& rect, int width, int fillColorId, int borderColorId )= 0;
    virtual void drawPlus( const Rect& rect, int width, int fillColorId, int borderColorId )= 0;
    virtual void drawDownArrow( const Rect& rect, int width, int fillColorId, int borderColorId )= 0;
    virtual void drawUpArrow( const Rect& rect, int width, int fillColorId, int borderColorId )= 0;

protected:
    UIFont *m_font;
};


class UIContext {
    // non copyable
    UIContext( const UIContext& );
    UIContext& operator= ( const UIContext& );

public:
    UIContext( );
    UIContext( UIFont *font, UIPainter *painter );

    virtual ~UIContext( ) {}

    // one time init
    virtual int init( )= 0;

    // UI method for processing window size events
    void reshape(int w, int h);
    void reshape(int x, int y, int w, int h);

    // UI method for processing mouse events
    void mouse(int button, int state, int modifier, int x, int y);

    // UI method for processing mouse motion events
    void mouseMotion(int x, int y);

    // UI method for processing key events
    void keyboard(unsigned int k, int x, int y);

    // UI method for processing text events
    void text(const char *string, int x, int y);

    // UI method for processing drop file events
    void file( const char *file, int x, int y );

    // Check if the UI is currently on Focus
    bool isOnFocus() const { return m_uiOnFocus; }

    // UI method for entering UI processing mode
    // This function must be used to begin the UI processing
    virtual void begin();

    // UI method for leaving UI processing mode
    // This function must be used to end the UI processing
    virtual void end();

    // UI element processing
    // The following methods provide the interface for rendering and querying
    // UI objects. These methods must be called between begin/end.

    // UI method for drawing a static text label
    // The label display a non interactive text.
    // The text can have multiple lines
    //
    // rect - optionally provides a location and size for the label
    // text - Text to display for the label (can have several lines)
    void doLabel(const Rect & rect, const char * text, int style = 0);

    //
    void doGraph( const Rect& rect, const int *values, const int n );

    // UI method to draw a time bar from start to stop (> 0 and < 1).
    void doTimebar( const Rect& rect, const char *label, const float start, const float stop );

    // 
    bool doMatrix( const Rect& rect, const RGB8 *colors, const int numColors, int *selected= NULL, int *hovered= NULL );
    
    // UI method for rendering and processing a push button
    //
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // state -  whether the button is depressed
    //          if state is NULL, the buttoin behave like a touch button
    //          else, the button behave like a toggle button
    // style - optional style flag to modify the look
    //
    // @return  True if the button'state changed
    bool doButton(const Rect & rect, const char * text, bool * state = NULL, int style = 0);

    // UI method for rendering and processing a check button
    // Check button behaves similarly as a toggle button
    // used to display and edit a bool property.
    //
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // state -  whether the check button is checked or not
    //          if state is NULL, the buttoin behave like if a touch button unchecked
    // style - optional style flag to modify the look
    //
    // @return  True if the check button'state changed
    bool doCheckButton(const Rect & rect, const char * text, bool * state, int style = 0);

    // UI method for rendering and processing a radio button
    // Radio buttons are typically used in groups to diplay and edit
    // the possible reference values taken by an int value.
    //
    // One radio button is representing a possible reference value taken by the current value.
    // It is displaying a boolean state true if the current value is equal to the reference value.
    //
    // reference - The reference int value represented by this radio button.
    // rect - optionally provides a location and size for the button
    // text - text to display on the button
    // value -  The value parameter compared to the reference value parameter.
    //          if value is NULL, the radio button is off
    // style - optional style flag to modify the look
    //
    // @return  True if the radio button's value changed
    bool doRadioButton(int reference, const Rect & r, const char * text, int * value, int style = 0);

    // UI method for rendering and processing a horizontal slider
    // Horizontal slider is used to edit and display a scalar value in the specified range [min, max].
    //
    // rect - optionally provides a location and size for the widget
    // min - min bound of the varying range of the value
    // max - max bound of the varying range of the value
    // value -  the value edited byt the widget
    //          if value is NULL, the value is set to min
    // style - optional style flag to modify the look
    //
    // @return  True if the slider's value changed
    bool doHorizontalSlider(const Rect & rect, float min, float max, float * value, int style = 0);

    bool doListItem(int index, const Rect & rect, const char * text, int * selected, int style = 0);

    // UI method for rendering a list of options.
    bool doListBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);

    // UI method for displaying the selected option and unrolling a list to choose another option.
    bool doComboBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);

    // UI method for displaying / editing a text line.
    bool doLineEdit(const Rect & rect, char * text, int maxTextLength, int * nbCharsReturned= NULL, int style = 0);

    void beginGroup(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null);
    void endGroup();

    void beginFrame(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null, int style = 0);
    void endFrame();

    bool beginPanel(Rect & rect, const char * text, bool * isUnfold, int groupFlags = GroupFlags_LayoutDefault, int style = 0);
    void endPanel();

    int getGroupWidth() { return m_groupStack[m_groupIndex].bounds.w; }
    int getGroupHeight() { return m_groupStack[m_groupIndex].bounds.h; }

    int getCursorX() { return m_currentCursor.x;}
    int getCursorY() { return m_currentCursor.y;}

    const ButtonState& getMouseState( int button ) { return m_mouseButton[button]; }

protected:
    UIPainter *getPainter() { return m_painter; }
    UIFont *getFont() { return m_font; }

protected:
    void setCursor(int x, int y);

    bool overlap(const Rect & rect, const Point & p);
    bool hasFocus(const Rect & rect);
    bool isHover(const Rect & rect);

    Rect placeRect(const Rect & r);

protected:
    UIPainter *m_painter;
    UIFont *m_font;

    // layout
    Group m_groupStack[64];
    int m_groupIndex;

    Rect m_window;
    Point m_currentCursor;

    // events
    ButtonState m_mouseButton[3];
    unsigned int m_keyBuffer[1024];
    int m_nbKeys;

    char *m_fileBuffer;

    // focus
    int m_focusCaretPos;
    Point m_focusPoint;
    bool m_twoStepFocus;
    bool m_uiOnFocus;
};

}       // namespace

#endif
