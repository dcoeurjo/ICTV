
#include <cstdio>
#include <algorithm>

#include "Widgets/nvWidgets.h"


namespace nv {

int UIFont::getFontHeight( ) const
{
    return m_line_skip;
}

const UIGlyph& UIFont::getGlyph( const int g ) const
{
    int id= (g < 128) ? g : '?';
    return m_glyphs[id];
}

const UIFontTexture *UIFont::getFontTexture( ) const
{
    return &m_texture;
}

Rect UIFont::getTextRect( const char *text ) const
{
    Rect r(0, 0, 0, m_line_skip);
    int w= 0;
    for(int i= 0; text[i] != 0; i++)
    {
        if(text[i] == '\n')
        {
            r.w= std::max(r.w, w);
            r.h+= m_line_skip;
            w= 0;
            continue;
        }
        
        w+= getGlyph(text[i]).advance;
    }
    
    r.w= std::max(r.w, w) +2;
    return r;
}

Rect UIFont::getTextRect( const int n, const char *options[] ) const
{
    Rect r;
    for(int i= 0; i < n; i++)
    {
        Rect o= getTextRect(options[i]);
        r.w= std::max(r.w, o.w);
        r.h+= o.h;
    }
    
    return r;
}

int UIFont::getTextLineWidth( const char *text ) const
{
    int w= 0;
    for(int i= 0; text[i] != 0; i++)
    {
        if(text[i] == '\n')
            break;
        
        w+= getGlyph(text[i]).advance;
    }
    
    return w +2;
}

int UIFont::getTextLineWidthAt( const char *text, const int at ) const
{
    int w= 0;
    for(int i= 0; text[i] != 0; i++)
    {
        if(i == at || text[i] == '\n')
            break;
        
        w+= getGlyph(text[i]).advance;
    }
    
    //~ return w +2;
    return w;
}

int UIFont::getPickedCharNb( const char *text, const Point& p ) const
{
    int w= 0;
    int i= 0;
    for(; text[i] != 0; i++)
    {
        if(text[i] == '\n')
            break;
        if(p.x < w)
            return i;
        
        w+= getGlyph(text[i]).advance;
        //~ printf("advance '%c' %d, %d\n", text[i], getGlyph(text[i]).advance, w);
    }
    
    return i;
}

}       // namespace
