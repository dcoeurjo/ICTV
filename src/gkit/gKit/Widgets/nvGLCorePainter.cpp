
#include <cmath>

#include "Widgets/nvGLCorePainter.h"
#include "ProgramManager.h"


namespace nv {

#define norm255( i ) ( (float) ( i ) / 255.0f )

const static gk::glsl::vec4 s_colors[cNbColors]=
{
    // cBase
    gk::glsl::vec4( norm255(89), norm255(89), norm255(89), 0.7f ),
    gk::glsl::vec4( norm255(166), norm255(166), norm255(166), 0.8f ),
    gk::glsl::vec4( norm255(212), norm255(228), norm255(60), 0.5f ),
    gk::glsl::vec4( norm255(227), norm255(237), norm255(127), 0.5f ),

    // cBool
    gk::glsl::vec4( norm255(99), norm255(37), norm255(35), 1.0f ),
    gk::glsl::vec4( norm255(149), norm255(55), norm255(53), 1.0f ),
    gk::glsl::vec4( norm255(212), norm255(228), norm255(60), 1.0f ),
    gk::glsl::vec4( norm255(227), norm255(237), norm255(127), 1.0f ),

    // cOutline
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),

    // cFont
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),
    gk::glsl::vec4( norm255(255), norm255(255), norm255(255), 1.0f ),

    // cFontBack
    gk::glsl::vec4( norm255(79), norm255(129), norm255(189), 1.0 ),
    gk::glsl::vec4( norm255(79), norm255(129), norm255(189), 1.0 ),
    gk::glsl::vec4( norm255(128), norm255(100), norm255(162), 1.0 ),
    gk::glsl::vec4( norm255(128), norm255(100), norm255(162), 1.0 ),

    // cTranslucent
    gk::glsl::vec4( norm255(0), norm255(0), norm255(0), 0.0 ),
    gk::glsl::vec4( norm255(0), norm255(0), norm255(0), 0.0 ),
    gk::glsl::vec4( norm255(0), norm255(0), norm255(0), 0.0 ),
    gk::glsl::vec4( norm255(0), norm255(0), norm255(0), 0.0 ),
};


GLCorePainter::GLCorePainter( )
    :
    m_font_texture(0),
	m_widget_program(), m_widgets(), 
	m_string_program(), m_strings()
{}

GLCorePainter::~GLCorePainter( )
{}

int GLCorePainter::init( UIFont *font )
{
    m_font= font;
    
    gk::ProgramManager::manager().searchPath("shaders");

    // init widget program
    GLuint program= 0;
    program= gk::ProgramManager::manager().createProgram("widget.glsl")->name;
    if(program == 0)
        return -1;
    
    m_widget_program.program= program;
    m_widget_program.projection= glGetUniformLocation(program, "projection");
    m_widget_program.fillColor= glGetUniformLocation(program, "fillColor");
    m_widget_program.borderColor= glGetUniformLocation(program, "borderColor");
    m_widget_program.zones= glGetUniformLocation(program, "zones");
    
    m_widgets.init();

    // init font texture
    glGenTextures(1, &m_font_texture);
    if(m_font_texture == 0)
        return -1;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_font_texture);

    //~ glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 
        0, GL_RGBA, m_font->getFontTexture()->width, m_font->getFontTexture()->height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, m_font->getFontTexture()->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // init string program
    program= gk::ProgramManager::manager().createProgram("string.glsl")->name;
    if(program == 0)
        return -1;
    
    m_string_program.program= program;
    m_string_program.projection= glGetUniformLocation(program, "projection");
    m_string_program.color= glGetUniformLocation(program, "color");
    m_string_program.font= glGetUniformLocation(program, "font");
    
    m_strings.init();
    return 0;
}

void GLCorePainter::reshape( const Rect &window )
{
    m_strings.reshape(window);
    m_widgets.reshape(window);
}

void GLCorePainter::begin( )
{
}

void GLCorePainter::end( )
{
	//! \todo retrieve current state
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_font_texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //~ printf("shapes: ");
    m_widgets.draw();
    //~ printf("texts: ");
    m_strings.draw();
    
    m_widgets.clear();
    m_strings.clear();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}


void GLCorePainter::draw_glyph( const int x, const int y, const UIGlyph& glyph )
{
    const float xx= x;
    const float yy= y;
    
    m_strings.push_texcoord(glyph.tex_xmin, glyph.tex_ymin);
    m_strings.push_vertex(xx, yy);
    
    m_strings.push_texcoord(glyph.tex_xmax, glyph.tex_ymin);
    m_strings.push_vertex(xx + glyph.tex_w, yy);
    
    m_strings.push_texcoord(glyph.tex_xmin, glyph.tex_ymax);
    m_strings.push_vertex(xx, yy + glyph.tex_h);
    
    m_strings.push_texcoord(glyph.tex_xmax, glyph.tex_ymax);
    m_strings.push_vertex(xx + glyph.tex_w, yy + glyph.tex_h);
    
    m_strings.restart();
}


void GLCorePainter::drawString( const Rect& rect, const char *text, int colorId )
{
    GLCore::string_params params(&m_string_program);
    params.colorId= colorId;
    params.color= s_colors[colorId];
    params.font= 0;     // texture unit
    
    m_strings.begin(params);
    
    int w= 0;
    int n= 1;
    for(int i= 0; text[i] != 0; i++)
    {
        if(text[i] == '\n')
        {
            n+= 1;
            if(rect.h - m_font->getFontHeight() * n < 0) break;
            
            w= 0;
            continue;
        }
        
        const UIGlyph& glyph= m_font->getGlyph(text[i]);
        draw_glyph(rect.x + w, rect.y + rect.h - m_font->getFontHeight() * n, glyph);
        
        w+= glyph.advance;
    }
    
    m_strings.end();
}


void GLCorePainter::drawRect( const Rect & rect, int fillColorId, int borderColorId ) {
    float x0 = rect.x;
    float x1 = rect.x + rect.w;

    float y0 = rect.y;
    float y1 = rect.y + rect.h;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(0, 0);

    m_widgets.begin(params);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x0, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x1, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x0, y1);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x1, y1);
    m_widgets.end();
}


void GLCorePainter::drawRGBRect( const Rect & rect, const RGB8 fillColor, const RGB8 borderColor ) {
    float x0 = rect.x;
    float x1 = rect.x + rect.w;

    float y0 = rect.y;
    float y1 = rect.y + rect.h;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= -1;
    params.border= gk::glsl::vec4(borderColor.r / 255.f, borderColor.g / 255.f, borderColor.b / 255.f, borderColor.a / 255.f);
    params.fillId= -1;
    params.fill= gk::glsl::vec4(fillColor.r / 255.f, fillColor.g / 255.f, fillColor.b / 255.f, fillColor.a / 255.f);
    params.zones= gk::glsl::vec2(0, 0);

    m_widgets.begin(params);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x0, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x1, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x0, y1);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex(x1, y1);
    m_widgets.end();
}

void GLCorePainter::drawRoundedRect( const Rect& rect, const Point& corner, int fillColorId, int borderColorId ) {
    float xb = corner.x;
    float yb = corner.y;

    float x0 = rect.x;
    float x1 = rect.x + corner.x;
    float x2 = rect.x + rect.w - corner.x;
    float x3 = rect.x + rect.w;

    float y0 = rect.y;
    float y1 = rect.y + corner.y;
    float y2 = rect.y + rect.h - corner.y;
    float y3 = rect.y + rect.h;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(corner.x - 1, corner.x - 2);
    
    m_widgets.begin(params);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x0, y0);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y0);

        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x0, y1);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y1);

        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x0, y2);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y2);

        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x0, y3);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y3);

    m_widgets.restart();
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y0);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x3, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y1);
        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x3, y1);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y2);
        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x3, y2);

        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y3);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x3, y3);
        
    m_widgets.restart();
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y0);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y1);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y1);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y2);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y2);

        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y3);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y3);
    m_widgets.end();
}

void GLCorePainter::drawRoundedRectOutline( const Rect& rect, const Point& corner, int borderColorId ) {
    float xb = corner.x;
    float yb = corner.y;

    float x0 = rect.x;
    float x1 = rect.x + corner.x;
    float x2 = rect.x + rect.w - corner.x;
    float x3 = rect.x + rect.w;

    float y0 = rect.y;
    float y1 = rect.y + corner.y;
    float y2 = rect.y + rect.h - corner.y;
    float y3 = rect.y + rect.h;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= cTranslucent;
    params.fill= s_colors[cTranslucent];
    params.zones= gk::glsl::vec2(corner.x - 1, corner.x - 2);

    m_widgets.begin(params);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x0, y0);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y0);

        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x0, y1);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y1);

        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x0, y2);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y2);

        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x0, y3);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y3);

    m_widgets.restart();
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y0);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x3, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y1);
        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x3, y1);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y2);
        m_widgets.push_texcoord(xb, 0);
        m_widgets.push_vertex( x3, y2);

        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y3);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x3, y3);

    m_widgets.restart();
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y0);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y0);

        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y1);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y1);

    m_widgets.restart();
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x1, y2);
        m_widgets.push_texcoord(0, 0);
        m_widgets.push_vertex( x2, y2);

        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x1, y3);
        m_widgets.push_texcoord(0, yb);
        m_widgets.push_vertex( x2, y3);
    m_widgets.end();
}

void GLCorePainter::drawCircle( const Rect& rect, int fillColorId, int borderColorId ) {
    float xb = rect.w / 2;
    float yb = rect.w / 2;

    float x0 = rect.x;
    float x1 = rect.x + rect.w;

    float y0 = rect.y;
    float y1 = rect.y + rect.h;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2((rect.w / 2) - 1, (rect.w / 2) - 2);

    m_widgets.begin(params);
        m_widgets.push_texcoord(-xb, -yb);
        m_widgets.push_vertex( x0, y0);
        m_widgets.push_texcoord(xb, -yb);
        m_widgets.push_vertex( x1, y0);
        m_widgets.push_texcoord(-xb, yb);
        m_widgets.push_vertex( x0, y1);
        m_widgets.push_texcoord(xb, yb);
        m_widgets.push_vertex( x1, y1);
    m_widgets.end();
}

void GLCorePainter::drawMinus( const Rect& rect, int width, int fillColorId, int borderColorId ) {
    float xb = width;
    float yb = width;

    float xoff = xb ;
    float yoff = yb ;

    float x0 = rect.x + rect.w * 0.1 ;
    float x1 = rect.x + rect.w * 0.9;

    float y1 = rect.y + rect.h * 0.5;
    
    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(width - 1, width - 2);

    m_widgets.begin(params);
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x1 - xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x1 - xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x1, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x1, y1 - yoff);
    m_widgets.end();
}

void GLCorePainter::drawPlus( const Rect& rect, int width, int fillColorId, int borderColorId ) {
    float xb = width;
    float yb = width;

    float xoff = xb ;
    float yoff = yb ;

    float x0 = rect.x + rect.w * 0.1 ;
    float x1 = rect.x + rect.w * 0.5;
    float x2 = rect.x + rect.w * 0.9;

    float y0 = rect.y + rect.h * 0.1;
    float y1 = rect.y + rect.h * 0.5;
    float y2 = rect.y + rect.h * 0.9;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(xb - 1, xb - 2);
    
    m_widgets.begin(params);
    #if 0
        // commente
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 - yoff);
 
        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x1 - xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x1 - xoff, y1 - yoff);

        m_widgets.push_texcoord(0, yb, 0);
        m_widgets.push_vertex( x1, y1);
        
    m_widgets.restart();
        m_widgets.push_texcoord(0, yb, 0);
        m_widgets.push_vertex( x1, y1);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x1 + xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x1 + xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x2 - xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x2 - xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 - yoff);
        
    m_widgets.restart();
    #endif
        // original
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x2 - xoff , y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x2 - xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 + yoff);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 - yoff);

    m_widgets.restart();
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x1 + yoff, y0);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x1 - yoff, y0);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x1 + yoff, y0 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x1 - yoff, y0 + yoff);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x1 + yoff, y2 - yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x1 - yoff, y2 - yoff);

        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x1 + yoff, y2);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x1 - yoff, y2);
    m_widgets.end();
}

void GLCorePainter::drawDownArrow( const Rect& rect, int width, int fillColorId, int borderColorId ) {
    float offset = sqrtf(2.0f) / 2.0f ;

    float xb = width;
    float yb = width;

    float xoff = offset * xb ;
    float yoff = offset * yb ;
    float xoff2 = offset * xb * 2.0f;
    float yoff2 = offset * yb * 2.0f;

    float x0 = rect.x + xoff2;
    float x1 = rect.x + rect.w * 0.5f;
    float x2 = rect.x + rect.w - xoff2;

    float y0 = rect.y + rect.h * 0.1f + yoff2;
    float y1 = rect.y + rect.h * 0.6f;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(xb - 1.f, xb - 2.f);

    m_widgets.begin(params);
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 + yoff2);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x0 - xoff2, y1);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff, y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x0 - xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x1, y0 + yoff2);
        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x1 - xoff2, y0);

        m_widgets.push_texcoord(xb, 2*yb, xb);
        m_widgets.push_vertex( x1, y0 - yoff2);

    m_widgets.restart();
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x2 + xoff2, y1);
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 + yoff2);

        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x2 + xoff, y1 - yoff);
        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x2 - xoff, y1 + yoff);

        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x1 + xoff2, y0);
        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x1, y0 + yoff2);

        m_widgets.push_texcoord(xb, 2*yb, xb);
        m_widgets.push_vertex( x1, y0 - yoff2);
    m_widgets.end();
}

void GLCorePainter::drawUpArrow( const Rect& rect, int width, int fillColorId, int borderColorId ) {
    float offset = sqrtf(2.0f) / 2.0f ;

    float xb = width;
    float yb = width;

    float xoff = offset * xb ;
    float yoff = - offset * yb ;
    float xoff2 = offset * xb * 2.0f ;
    float yoff2 = - offset * yb * 2.0f;

    float x0 = rect.x + xoff2;
    float x1 = rect.x + rect.w * 0.5f;
    float x2 = rect.x + rect.w - xoff2;

    float y0 = rect.y + rect.h * 0.9f + yoff2;
    float y1 = rect.y + rect.h * 0.4f;

    nv::GLCore::widget_params params(&m_widget_program);
    params.borderId= borderColorId;
    params.border= s_colors[borderColorId];
    params.fillId= fillColorId;
    params.fill= s_colors[fillColorId];
    params.zones= gk::glsl::vec2(xb - 1, xb - 2);
    
    m_widgets.begin(params);
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x0, y1 + yoff2);
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x0 - xoff2, y1);

        m_widgets.push_texcoord(-xb, 0, 0);
        m_widgets.push_vertex( x0 + xoff, y1 + yoff);
        m_widgets.push_texcoord(xb, 0, 0);
        m_widgets.push_vertex( x0 - xoff, y1 - yoff);

        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x1, y0 + yoff2);
        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x1 - xoff2, y0);

        m_widgets.push_texcoord(xb, 2*yb, xb);
        m_widgets.push_vertex( x1, y0 - yoff2);

    m_widgets.restart();
        m_widgets.push_texcoord(xb, -yb, 0);
        m_widgets.push_vertex( x2 + xoff2, y1);
        m_widgets.push_texcoord(-xb, -yb, 0);
        m_widgets.push_vertex( x2, y1 + yoff2);

        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x2 + xoff, y1 - yoff);
        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x2 - xoff, y1 + yoff);

        m_widgets.push_texcoord(xb, 0, xb);
        m_widgets.push_vertex( x1 + xoff2, y0);
        m_widgets.push_texcoord(-xb, 0, xb);
        m_widgets.push_vertex( x1, y0 + yoff2);

        m_widgets.push_texcoord(xb, 2*yb, xb);
        m_widgets.push_vertex( x1, y0 - yoff2);
    m_widgets.end();
}

void GLCorePainter::drawDataCurve( const Rect& rect, const int *data, const int n, int colorId )
{
    //~ nv::GLCore::graph_params params(&m_graph_program);
    //~ params.colorId= colorId;
    //~ params.color= s_colors[colorId];
    
    //~ m_curves.begin(params);
    //~ m_curves.end();    
}

}       // namespace
