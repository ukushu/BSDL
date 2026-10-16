/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"
#include "SDL_sysrender.h"
#include "SDL_render_debug_font.h"

#define SDL_DEBUG_FONT_GLYPHS_PER_ROW 14

static bool CreateDebugTextAtlas(SDL_Renderer *renderer)
{
    static const SDL_Color colors[] = {
        { 255, 255, 255, SDL_ALPHA_TRANSPARENT },
        { 255, 255, 255, SDL_ALPHA_OPAQUE }
    };

    SDL_assert(renderer->debug_char_texture_atlas == NULL);  // don't double-create it!

    const int charWidth = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    const int charHeight = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    // actually make each glyph two pixels taller/wider, to prevent scaling artifacts.
    const int rows = (SDL_DEBUG_FONT_NUM_GLYPHS / SDL_DEBUG_FONT_GLYPHS_PER_ROW) + 1;
    SDL_Surface *atlas = SDL_CreateSurfaceZeroed((charWidth + 2) * SDL_DEBUG_FONT_GLYPHS_PER_ROW, rows * (charHeight + 2), SDL_PIXELFORMAT_INDEX8);
    if (!atlas) {
        return false;
    }

    SDL_Palette *palette = SDL_CreateSurfacePalette(atlas);
    if (!palette || !SDL_SetPaletteColors(palette, colors, 0, 2)) {
        SDL_DestroySurface(atlas);
        return false;
    }

    const int pitch = atlas->pitch;
    SDL_memset(atlas->pixels, '\0', atlas->h * atlas->pitch);

    int column = 0;
    int row = 0;
    for (int glyph = 0; glyph < SDL_DEBUG_FONT_NUM_GLYPHS; glyph++) {
        // find top-left of this glyph in destination surface. The +2's account for glyph padding.
        Uint8 *linepos = (((Uint8 *)atlas->pixels) + ((row * (charHeight + 2) + 1) * pitch)) + ((column * (charWidth + 2) + 1) * sizeof (Uint8));
        const Uint8 *charpos = SDL_RenderDebugTextFontData + (glyph * 8);

        // Draw the glyph to the surface...
        for (int iy = 0; iy < charHeight; iy++) {
            Uint8 *curpos = linepos;
            for (int ix = 0; ix < charWidth; ix++) {
                *curpos++ = (*charpos >> ix) & 1;
            }
            linepos += pitch;
            ++charpos;
        }

        // move to next position (and if too far, start the next row).
        column++;
        if (column >= SDL_DEBUG_FONT_GLYPHS_PER_ROW) {
            row++;
            column = 0;
        }
    }

    SDL_assert((row < rows) || ((row == rows) && (column == 0)));  // make sure we didn't overflow the surface.

    // Convert temp surface into texture
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, atlas);
    if (texture) {
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_PIXELART);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        renderer->debug_char_texture_atlas = texture;
    }
    SDL_DestroySurface(atlas);

    return texture != NULL;
}

static bool DrawDebugCharacter(SDL_Renderer *renderer, float x, float y, Uint32 c)
{
    SDL_assert(renderer->debug_char_texture_atlas != NULL);   // should have been created by now!

    const int charWidth = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    const int charHeight = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;

    // Character index in cache
    Uint32 ci = c;
    if ((ci <= 32) || ((ci >= 127) && (ci <= 160))) {
        return true;  // these are just completely blank chars, don't bother doing anything.
    } else if (ci >= SDL_DEBUG_FONT_NUM_GLYPHS) {
        ci = SDL_DEBUG_FONT_NUM_GLYPHS - 1;  // use our "not a valid/supported character" glyph.
    } else if (ci < 127) {
        ci -= 33;     // adjust for the 33 blank glyphs at the start
    } else {
        ci -= 67;     // adjust for the 33 blank glyphs at the start AND the 34 gap in the middle.
    }

    const float src_x = (float) (((ci % SDL_DEBUG_FONT_GLYPHS_PER_ROW) * (charWidth + 2)) + 1);
    const float src_y = (float) (((ci / SDL_DEBUG_FONT_GLYPHS_PER_ROW) * (charHeight + 2)) + 1);

    // Draw texture onto destination
    const SDL_FRect srect = { src_x, src_y, (float) charWidth, (float) charHeight };
    const SDL_FRect drect = { x, y, (float) charWidth, (float) charHeight };
    return SDL_RenderTexture(renderer, renderer->debug_char_texture_atlas, &srect, &drect);
}

bool SDL_RenderDebugText(SDL_Renderer *renderer, float x, float y, const char *s)
{
    CHECK_PARAM(!SDL_ObjectValid(renderer, SDL_OBJECT_TYPE_RENDERER)) {
        SDL_InvalidParamError("renderer");
        return false;
    }
    CHECK_PARAM(renderer->destroyed) {
        SDL_SetError("Renderer's window has been destroyed, can't use further");
        return false;
    }

    // Allocate a texture atlas for this renderer if needed.
    if (!renderer->debug_char_texture_atlas) {
        if (!CreateDebugTextAtlas(renderer)) {
            return false;
        }
    }

    bool result = true;

    Uint8 r, g, b, a;
    result &= SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
    result &= SDL_SetTextureColorMod(renderer->debug_char_texture_atlas, r, g, b);
    result &= SDL_SetTextureAlphaMod(renderer->debug_char_texture_atlas, a);

    float curx = x;
    Uint32 ch;

    while (result && ((ch = SDL_StepUTF8(&s, NULL)) != 0)) {
        result &= DrawDebugCharacter(renderer, curx, y, ch);
        curx += SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE;
    }

    return result;
}

bool SDL_RenderDebugTextFormat(SDL_Renderer *renderer, float x, float y, SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    // fast path to avoid unnecessary allocation and copy. If you're going through the dynapi, there's a good chance
    // you _always_ hit this path, since it probably had to process varargs before calling into the jumptable.
    if (SDL_strcmp(fmt, "%s") == 0) {
        const char *str = va_arg(ap, const char *);
        va_end(ap);
        return SDL_RenderDebugText(renderer, x, y, str);
    }

    char *str = NULL;
    const int rc = SDL_vasprintf(&str, fmt, ap);
    va_end(ap);

    if (rc == -1) {
        return false;
    }

    const bool retval = SDL_RenderDebugText(renderer, x, y, str);
    SDL_free(str);
    return retval;
}

