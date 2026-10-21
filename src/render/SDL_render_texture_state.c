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
#include "SDL_render_internal.h"

#define CHECK_TEXTURE_MAGIC(texture, result)                            \
    CHECK_PARAM(!SDL_ObjectValid(texture, SDL_OBJECT_TYPE_TEXTURE)) {   \
        SDL_InvalidParamError("texture");                               \
        return result;                                                  \
    }


bool SDL_RendererSupportsBlendMode(SDL_Renderer *renderer, SDL_BlendMode blendMode)
{
    switch (blendMode) {
    // These are required to be supported by all renderers
    case SDL_BLENDMODE_NONE:
    case SDL_BLENDMODE_BLEND:
    case SDL_BLENDMODE_BLEND_PREMULTIPLIED:
    case SDL_BLENDMODE_ADD:
    case SDL_BLENDMODE_ADD_PREMULTIPLIED:
    case SDL_BLENDMODE_MOD:
    case SDL_BLENDMODE_MUL:
        return true;

    default:
        return renderer->SupportsBlendMode && renderer->SupportsBlendMode(renderer, blendMode);
    }
}

bool SDL_SetTextureColorMod(SDL_Texture *texture, Uint8 r, Uint8 g, Uint8 b)
{
    const float fR = (float)r / 255.0f;
    const float fG = (float)g / 255.0f;
    const float fB = (float)b / 255.0f;

    return SDL_SetTextureColorModFloat(texture, fR, fG, fB);
}

bool SDL_SetTextureColorModFloat(SDL_Texture *texture, float r, float g, float b)
{
    CHECK_TEXTURE_MAGIC(texture, false);

    texture->color.r = r;
    texture->color.g = g;
    texture->color.b = b;
    if (texture->native) {
        return SDL_SetTextureColorModFloat(texture->native, r, g, b);
    }
    return true;
}

bool SDL_GetTextureColorMod(SDL_Texture *texture, Uint8 *r, Uint8 *g, Uint8 *b)
{
    float fR = 1.0f, fG = 1.0f, fB = 1.0f;

    if (!SDL_GetTextureColorModFloat(texture, &fR, &fG, &fB)) {
        if (r) {
            *r = 255;
        }
        if (g) {
            *g = 255;
        }
        if (b) {
            *b = 255;
        }
        return false;
    }

    if (r) {
        *r = (Uint8)SDL_roundf(SDL_clamp(fR, 0.0f, 1.0f) * 255.0f);
    }
    if (g) {
        *g = (Uint8)SDL_roundf(SDL_clamp(fG, 0.0f, 1.0f) * 255.0f);
    }
    if (b) {
        *b = (Uint8)SDL_roundf(SDL_clamp(fB, 0.0f, 1.0f) * 255.0f);
    }
    return true;
}

bool SDL_GetTextureColorModFloat(SDL_Texture *texture, float *r, float *g, float *b)
{
    SDL_FColor color;

    if (r) {
        *r = 1.0f;
    }
    if (g) {
        *g = 1.0f;
    }
    if (b) {
        *b = 1.0f;
    }

    CHECK_TEXTURE_MAGIC(texture, false);

    color = texture->color;

    if (r) {
        *r = color.r;
    }
    if (g) {
        *g = color.g;
    }
    if (b) {
        *b = color.b;
    }
    return true;
}

bool SDL_SetTextureAlphaMod(SDL_Texture *texture, Uint8 alpha)
{
    const float fA = (float)alpha / 255.0f;

    return SDL_SetTextureAlphaModFloat(texture, fA);
}

bool SDL_SetTextureAlphaModFloat(SDL_Texture *texture, float alpha)
{
    CHECK_TEXTURE_MAGIC(texture, false);

    texture->color.a = alpha;
    if (texture->native) {
        return SDL_SetTextureAlphaModFloat(texture->native, alpha);
    }
    return true;
}

bool SDL_GetTextureAlphaMod(SDL_Texture *texture, Uint8 *alpha)
{
    float fA = 1.0f;

    if (!SDL_GetTextureAlphaModFloat(texture, &fA)) {
        if (alpha) {
            *alpha = 255;
        }
        return false;
    }

    if (alpha) {
        *alpha = (Uint8)SDL_roundf(SDL_clamp(fA, 0.0f, 1.0f) * 255.0f);
    }
    return true;
}

bool SDL_GetTextureAlphaModFloat(SDL_Texture *texture, float *alpha)
{
    if (alpha) {
        *alpha = 1.0f;
    }

    CHECK_TEXTURE_MAGIC(texture, false);

    if (alpha) {
        *alpha = texture->color.a;
    }
    return true;
}

bool SDL_SetTextureBlendMode(SDL_Texture *texture, SDL_BlendMode blendMode)
{
    SDL_Renderer *renderer;

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(blendMode == SDL_BLENDMODE_INVALID) {
        return SDL_InvalidParamError("blendMode");
    }

    renderer = texture->renderer;
    if (!SDL_RendererSupportsBlendMode(renderer, blendMode)) {
        return SDL_Unsupported();
    }
    texture->blendMode = blendMode;
    if (texture->native) {
        return SDL_SetTextureBlendMode(texture->native, blendMode);
    }
    return true;
}

bool SDL_GetTextureBlendMode(SDL_Texture *texture, SDL_BlendMode *blendMode)
{
    if (blendMode) {
        *blendMode = SDL_BLENDMODE_INVALID;
    }

    CHECK_TEXTURE_MAGIC(texture, false);

    if (blendMode) {
        *blendMode = texture->blendMode;
    }
    return true;
}

bool SDL_SetTextureScaleMode(SDL_Texture *texture, SDL_ScaleMode scaleMode)
{
    CHECK_TEXTURE_MAGIC(texture, false);

    switch (scaleMode) {
    case SDL_SCALEMODE_NEAREST:
    case SDL_SCALEMODE_PIXELART:
    case SDL_SCALEMODE_LINEAR:
        break;
    default:
        return SDL_InvalidParamError("scaleMode");
    }

    texture->scaleMode = scaleMode;

    if (texture->native) {
        return SDL_SetTextureScaleMode(texture->native, scaleMode);
    }
    return true;
}

bool SDL_GetTextureScaleMode(SDL_Texture *texture, SDL_ScaleMode *scaleMode)
{
    if (scaleMode) {
        *scaleMode = SDL_SCALEMODE_INVALID;
    }

    CHECK_TEXTURE_MAGIC(texture, false);

    if (scaleMode) {
        *scaleMode = texture->scaleMode;
    }
    return true;
}

bool SDL_SetRenderTextureAddressMode(SDL_Renderer *renderer, SDL_TextureAddressMode u_mode, SDL_TextureAddressMode v_mode)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->texture_address_mode_u = u_mode;
    renderer->texture_address_mode_v = v_mode;
    return true;
}

bool SDL_GetRenderTextureAddressMode(SDL_Renderer *renderer, SDL_TextureAddressMode *u_mode, SDL_TextureAddressMode *v_mode)
{
    if (u_mode) {
        *u_mode = SDL_TEXTURE_ADDRESS_INVALID;
    }
    if (v_mode) {
        *v_mode = SDL_TEXTURE_ADDRESS_INVALID;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (u_mode) {
        *u_mode = renderer->texture_address_mode_u;
    }
    if (v_mode) {
        *v_mode = renderer->texture_address_mode_v;
    }
    return true;
}

bool SDL_SetDefaultTextureScaleMode(SDL_Renderer *renderer, SDL_ScaleMode scale_mode)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->scale_mode = scale_mode;

    return true;
}

bool SDL_GetDefaultTextureScaleMode(SDL_Renderer *renderer, SDL_ScaleMode *scale_mode)
{
    if (scale_mode) {
        *scale_mode = SDL_SCALEMODE_LINEAR;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (scale_mode) {
        *scale_mode = renderer->scale_mode;
    }
    return true;
}

SDL_PropertiesID SDL_GetTextureProperties(SDL_Texture *texture)
{
    CHECK_TEXTURE_MAGIC(texture, 0);

    if (texture->props == 0) {
        texture->props = SDL_CreateProperties();
    }
    return texture->props;
}

bool SDL_GetTextureSize(SDL_Texture *texture, float *w, float *h)
{
    if (w) {
        *w = 0;
    }
    if (h) {
        *h = 0;
    }

    CHECK_TEXTURE_MAGIC(texture, false);

    if (w) {
        *w = (float)texture->w;
    }
    if (h) {
        *h = (float)texture->h;
    }
    return true;
}

static void DetachTexturePalette(SDL_Texture *texture)
{
    if (texture->public_palette) {
        SDL_Renderer *renderer = texture->renderer;
        SDL_DestroyPalette(texture->public_palette);

        if (!texture->native) {
            // Clean up the texture palette
            --texture->palette->refcount;
            if (texture->palette->refcount == 0) {
                SDL_FlushRenderCommandsIfPaletteNeeded(renderer, texture->palette);
                renderer->DestroyPalette(renderer, texture->palette);
                SDL_RemoveFromHashTable(renderer->palettes, texture->public_palette);
            }
            texture->palette = NULL;
        }
    }
}

static bool AttachTexturePalette(SDL_Texture *texture, SDL_Palette *palette)
{
    SDL_Renderer *renderer = texture->renderer;

    texture->public_palette = palette;
    texture->palette_version = 0;

    if (texture->public_palette) {
        ++texture->public_palette->refcount;

        if (!texture->native) {
            if (SDL_FindInHashTable(renderer->palettes, palette, (const void **)&texture->palette)) {
                ++texture->palette->refcount;
            } else {
                SDL_TexturePalette *texture_palette = (SDL_TexturePalette *)SDL_calloc(1, sizeof(*texture_palette));
                if (!texture_palette) {
                    SDL_DestroyPalette(texture->public_palette);
                    texture->public_palette = NULL;
                    return false;
                }
                if (!renderer->CreatePalette(renderer, texture_palette)) {
                    renderer->DestroyPalette(renderer, texture_palette);
                    SDL_free(texture_palette);
                    SDL_DestroyPalette(texture->public_palette);
                    texture->public_palette = NULL;
                    return false;
                }
                texture->palette = texture_palette;
                texture->palette->refcount = 1;

                if (!SDL_InsertIntoHashTable(renderer->palettes, palette, texture->palette, false)) {
                    SDL_SetTexturePalette(texture, NULL);
                    return false;
                }
            }
        }

        if (!texture->native && renderer->ChangeTexturePalette) {
            renderer->ChangeTexturePalette(renderer, texture);
        }
    }
    return true;
}

bool SDL_SetTexturePalette(SDL_Texture *texture, SDL_Palette *palette)
{
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(!SDL_ISPIXELFORMAT_INDEXED(texture->format)) {
        return SDL_SetError("Texture isn't palettized format");
    }

    CHECK_PARAM(palette && palette->ncolors > (1 << SDL_BITSPERPIXEL(texture->format))) {
        return SDL_SetError("Palette doesn't match surface format");
    }

    if (palette != texture->public_palette) {
        DetachTexturePalette(texture);
        if (!AttachTexturePalette(texture, palette)) {
            return false;
        }
        if (texture->palette_surface) {
            SDL_SetSurfacePalette(texture->palette_surface, palette);
        }
    }
    return true;
}

SDL_Palette *SDL_GetTexturePalette(SDL_Texture *texture)
{
    CHECK_TEXTURE_MAGIC(texture, NULL);

    return texture->public_palette;
}

bool SDL_UpdateTexturePalette(SDL_Texture *texture)
{
    SDL_Renderer *renderer = texture->renderer;
    SDL_Palette *public = texture->public_palette;

    if (!SDL_ISPIXELFORMAT_INDEXED(texture->format)) {
        return true;
    }

    if (!public) {
        return SDL_SetError("Texture doesn't have a palette");
    }

    if (texture->native) {
        // Keep the native texture in sync with palette updates
        if (texture->palette_version == public->version) {
            return true;
        }

        if (!SDL_FlushRenderCommandsIfTextureNeeded(texture->native)) {
            return false;
        }

        SDL_Surface *surface;
        bool result = SDL_LockTextureToSurface(texture->native, NULL, &surface);
        if (result) {
            result = SDL_BlitSurface(texture->palette_surface, NULL, surface, NULL);
            SDL_UnlockTexture(texture->native);
        }
        if (!result) {
            return false;
        }
        texture->palette_version = public->version;
        return true;
    }

    SDL_TexturePalette *palette = texture->palette;
    if (palette->version != public->version) {
        // Keep the native palette in sync with palette updates
        if (!SDL_FlushRenderCommandsIfPaletteNeeded(renderer, palette)) {
            return false;
        }

        if (!renderer->UpdatePalette(renderer, palette, public->ncolors, public->colors)) {
            return false;
        }

        palette->version = public->version;
    }

    palette->last_command_generation = renderer->render_command_generation;
    return true;
}
