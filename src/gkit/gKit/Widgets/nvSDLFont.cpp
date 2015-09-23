
#include <math.h>
#include <cassert>

#include "SDLPlatform.h"
#include "SDLttfPlatform.h"

#include "IOFileSystem.h"
#include "Logger.h"
#include "Widgets/nvSDLFont.h"


namespace nv {

SdlFont::SdlFont( const char *filename, const int size )
    :
    UIFont(),
    m_filename(filename),
    m_size(size),
    m_sdl_texture(NULL),
    m_font_height(0)
{}    


SdlFont::~SdlFont( )
{
    if(m_sdl_texture != NULL)
        SDL_FreeSurface(m_sdl_texture);
}


static
int power2( const int value )
{
    int p= 1;
    while(p < value)
        p+= p;
    return p;
}

int SdlFont::init_cache( TTF_Font *font )
{
    if(font == NULL)
        return -1;

    // fill the glyphs cache, replace non printable characters with a '?'
    // 1. compute texture area
    int area= 0;
    SDL_Color color= { 255, 255, 255, 0 };
    for(int i= 0; i < 128; i++)
    {
        const int g= isprint(i) ? i : '?';

        TTF_GlyphMetrics(font, g,
            &m_glyphs[i].xmin, &m_glyphs[i].xmax,
            &m_glyphs[i].ymin, &m_glyphs[i].ymax,
            &m_glyphs[i].advance);

        SDL_Surface *glyph= TTF_RenderGlyph_Blended(font, g, color);
        m_glyphs[i].tex_w= glyph->w;
        m_glyphs[i].tex_h= glyph->h;

        area+= glyph->w * glyph->h;
        SDL_FreeSurface(glyph);
    }

    // 2. pick up a texture size
    const int width= power2((int) (sqrtf(2.f * area) + .5f));
    const int height= width;

    SDL_Surface *surface= SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
        0x000000FF,
        0x0000FF00,
        0x00FF0000,
        0xFF000000
    #else
        0xFF000000,
        0x00FF0000,
        0x0000FF00,
        0x000000FF
    #endif
    );
    assert(surface != NULL);

    // 3. pack glyphs
    int x= 0;
    int y= 0;
    for(int i= 0; i < 128; i++)
    {
        const int g= isprint(i) ? i : '?';
        //~ SDL_Surface *glyph= TTF_RenderGlyph_Solid(font, g, color);
        SDL_Surface *glyph= TTF_RenderGlyph_Blended(font, g, color);

        // place current glyph
        if(x + glyph->w > width)
        {
            y+= m_font_height;
            x= 0;
        }
        assert(y < height);

        // texcoords
        m_glyphs[i].tex_xmin= (float) x / (float) width;
        m_glyphs[i].tex_xmax= (float) (x + glyph->w) / (float) width;
        // flip texture ...
        m_glyphs[i].tex_ymax= (float) y / (float) height;
        m_glyphs[i].tex_ymin= (float) (y + glyph->h) / (float) height;

        // blit glyph
        SDL_Rect surface_area;
        surface_area.x= x;
        surface_area.y= y;
        surface_area.w= glyph->w;
        surface_area.h= glyph->h;

        SDL_Rect glyph_area;
        glyph_area.x= 0;
        glyph_area.y= 0;
        glyph_area.w= glyph->w;
        glyph_area.h= glyph->h;

        // sdl 2, sdl_ttf, glfont.c cf. http://hg.libsdl.org/SDL_ttf/file/79c69974d263/glfont.c
        SDL_SetSurfaceAlphaMod(glyph, SDL_ALPHA_OPAQUE);
        SDL_SetSurfaceBlendMode(glyph, SDL_BLENDMODE_NONE);

        SDL_BlitSurface(glyph, &glyph_area, surface, &surface_area);

        // place next glyph
        x+= glyph->w;
        SDL_FreeSurface(glyph);
    }

    //~ SDL_SaveBMP(surface, "fontfixed.bmp");
    m_sdl_texture= surface;
    m_texture.data= surface->pixels;
    m_texture.width= surface->w;
    m_texture.height= surface->h;
    return 0;
}

int SdlFont::init( )
{
    // load font
    if(TTF_WasInit() == 0)
        TTF_Init();

    TTF_Font *font= TTF_OpenFont(gk::IOFileSystem::findFile(m_filename).c_str(), m_size);
    if(font == NULL)
    {
        ERROR("error reading font '%s': %s\n", m_filename.c_str(), TTF_GetError());
        return -1;
    }

    MESSAGE("loading ttf font '%s'...\n", TTF_FontFaceFamilyName(font));

    TTF_SetFontKerning(font, 0);
    TTF_SetFontHinting(font, TTF_HINTING_LIGHT);

    // get metrics
    m_line_skip= TTF_FontLineSkip(font);
    m_font_height= TTF_FontHeight(font);

    // cache glyphs
    init_cache(font);

    // release
    TTF_CloseFont(font);
    TTF_Quit();

    return (m_texture.data != NULL) ? 0 : -1;
}

}       // namespace
