
#ifndef NV_GLCORE_PAINTER_H
#define NV_GLCORE_PAINTER_H

#include <cassert>

#include "GL/GLPlatform.h"
#include "GL/GLSLUniforms.h"

#include "Widgets/nvWidgets.h"
#include "Widgets/nvGLCoreDraw.h"


namespace nv {
namespace GLCore {

struct widget_program
{
    GLuint program;     //!< required, shader program name
    GLint projection;   //!< required, projection matrix uniform location
    
    GLint fillColor;    //!< additional uniform locations
    GLint borderColor;
    GLint zones;
    
    widget_program( )
        :
        program(0),
        projection(-1),
        fillColor(-1),
        borderColor(-1),
        zones(-1)
    {}
};

struct widget_params : public params
{
    const widget_program *program;
    
    int borderId;
    int fillId;
    
    gk::glsl::vec4 border;
    gk::glsl::vec4 fill;
    gk::glsl::vec2 zones;
    
    widget_params( const widget_program *_program )
        :
        params(),
        program(_program),
        borderId(0),
        fillId(0),
        border(), fill(), zones()
    {}
    
    bool operator== ( const widget_params& b ) const
    {
        
        if(program != b.program) return false;
        //~ if(borderId != b.borderId) //~ return false;
        //~ if(fillId != b.fillId) //~ return false;
        if(border != b.border) return false;
        if(fill != b.fill) return false;
        if(zones != b.zones) return false;
        return true;
    }
    
    bool operator!= ( const widget_params& b ) const
    {
        return !((*this) == b);
    }

    void apply( )
    {
        assert(program != NULL);
        assert(program->fillColor > -1);
        assert(program->borderColor > -1);
        assert(program->zones > -1);
        glUniform4fv(program->fillColor, 1, fill);
        glUniform4fv(program->borderColor, 1, border);
        glUniform2fv(program->zones, 1, zones);
    }
};


struct string_program
{
    GLuint program;     //!< required, shader program name
    GLint projection;   //!< required, projection matrix uniform location
    
    GLint font;         //!< additional uniform locations
    GLint color;
    
    string_program( )
        :
        program(0),
        projection(-1),
        font(-1),
        color(-1)
    {}
};

struct string_params : public params
{
    const string_program *program;
    
    int colorId;
    gk::glsl::vec4 color;
    int font;
    
    string_params( const string_program *_program )
        :
        params(),
        program(_program),
        colorId(0),
        color(1.f, 1.f, 1.f, 1.f),
        font(0)
    {}
    
    bool operator== ( const string_params& b ) const
    {
        if(program != b.program) return false;
        //~ if(colorId != b.colorId) return false;
        if(color != b.color) return false;
        if(font != b.font) return false;
        return true;
    }
    
    bool operator!= ( const string_params& b ) const
    {
        return !((*this) == b);
    }
    
    void apply( ) const
    {
        assert(program != NULL);
        assert(program->color != -1);
        assert(program->font != -1);
        glUniform4fv(program->color, 1, (const GLfloat *) color);
        glUniform1i(program->font, font);   //!< sampler, texture unit 0
    }
};

#if 0
struct graph_program
{
    GLuint program;     //!< required, shader program name
    GLint projection;   //!< required, projection matrix uniform location
    
    GLint color;        //!< additional uniform locations
    GLuint data;        //!< texture data
};

struct graph_param : public params
{
    const graph_program *program;
    
    int colorId;
    gk::glsl::vec4 color;
};
#endif


}       // namespace GLCore


class GLCorePainter : public UIPainter
{
public:
    GLCorePainter( );
    ~GLCorePainter( );

    int init( UIFont *font );

    void reshape( const Rect &window );
    void begin( );
    void end();
    
    void drawString( const Rect& rect, const char *text, int colorId );
    void drawRect( const Rect& rect, int fillColorId, int borderColorId );
    void drawRGBRect( const Rect& rect, const RGB8 fillColor, const RGB8 borderColor );
    void drawRoundedRect( const Rect& rect, const Point& corner, int fillColorId, int borderColorId );
    void drawRoundedRectOutline( const Rect& rect, const Point& corner, int borderColorId );
    void drawCircle( const Rect& rect, int fillColorId, int borderColorId );
    void drawMinus( const Rect& rect, int width, int fillColorId, int borderColorId );
    void drawPlus( const Rect& rect, int width, int fillColorId, int borderColorId );
    void drawDownArrow( const Rect& rect, int width, int fillColorId, int borderColorId );
    void drawUpArrow( const Rect& rect, int width, int fillColorId, int borderColorId );

    void drawDataCurve( const Rect& rect, /* scale */ const int *data, const int n, int colorId );

protected:
    void draw_glyph( const int x, const int y, const UIGlyph& g );

    GLuint m_font_texture;

    nv::GLCore::widget_program m_widget_program;
    nv::GLCore::Draw<nv::GLCore::widget_params> m_widgets;

    nv::GLCore::string_program m_string_program;
    nv::GLCore::Draw<nv::GLCore::string_params> m_strings;
};

}       // namespace
#endif
