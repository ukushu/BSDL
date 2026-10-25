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
#include "SDL_yuv_sw_c.h"
#include "../video/SDL_pixels_c.h"

#define CHECK_TEXTURE_MAGIC(texture, result)                            \
    CHECK_PARAM(!SDL_ObjectValid(texture, SDL_OBJECT_TYPE_TEXTURE)) {   \
        SDL_InvalidParamError("texture");                               \
        return result;                                                  \
    }


#ifdef SDL_HAVE_YUV
static bool SDL_LockTextureYUV(SDL_Texture *texture, const SDL_Rect *rect,
                              void **pixels, int *pitch)
{
    return SDL_SW_LockYUVTexture(texture->yuv, rect, pixels, pitch);
}
#endif // SDL_HAVE_YUV

static bool SDL_LockTexturePaletteSurface(SDL_Texture *texture, const SDL_Rect *rect, void **pixels, int *pitch)
{
    SDL_Surface *surface = texture->palette_surface;

    *pixels = ((Uint8 *)surface->pixels) + rect->y * surface->pitch + (rect->x * SDL_BITSPERPIXEL(texture->format)) / 8;
    *pitch = surface->pitch;
    return true;
}

static bool SDL_LockTextureNative(SDL_Texture *texture, const SDL_Rect *rect, void **pixels, int *pitch)
{
    texture->locked_rect = *rect;
    *pixels = (void *)((Uint8 *)texture->pixels +
                       rect->y * texture->pitch +
                       rect->x * SDL_BYTESPERPIXEL(texture->format));
    *pitch = texture->pitch;
    return true;
}

bool SDL_LockTexture(SDL_Texture *texture, const SDL_Rect *rect, void **pixels, int *pitch)
{
    SDL_Rect full_rect;

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(texture->access != SDL_TEXTUREACCESS_STREAMING) {
        return SDL_SetError("SDL_LockTexture(): texture must be streaming");
    }

    if (!rect) {
        full_rect.x = 0;
        full_rect.y = 0;
        full_rect.w = texture->w;
        full_rect.h = texture->h;
        rect = &full_rect;
    }

#ifdef SDL_HAVE_YUV
    if (texture->yuv) {
        if (!SDL_FlushRenderCommandsIfTextureNeeded(texture)) {
            return false;
        }
        return SDL_LockTextureYUV(texture, rect, pixels, pitch);
    } else
#endif
    if (texture->palette_surface) {
        return SDL_LockTexturePaletteSurface(texture, rect, pixels, pitch);
    } else if (texture->native) {
        // Calls a real SDL_LockTexture/SDL_UnlockTexture on unlock, flushing then.
        return SDL_LockTextureNative(texture, rect, pixels, pitch);
    } else {
        SDL_Renderer *renderer = texture->renderer;
        if (!SDL_FlushRenderCommandsIfTextureNeeded(texture)) {
            return false;
        }
        return renderer->LockTexture(renderer, texture, rect, pixels, pitch);
    }
}

bool SDL_LockTextureToSurface(SDL_Texture *texture, const SDL_Rect *rect, SDL_Surface **surface)
{
    SDL_Rect real_rect;
    void *pixels = NULL;
    int pitch = 0; // fix static analysis

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(!surface) {
        return SDL_InvalidParamError("surface");
    }

    real_rect.x = 0;
    real_rect.y = 0;
    real_rect.w = texture->w;
    real_rect.h = texture->h;
    if (rect) {
        SDL_GetRectIntersection(rect, &real_rect, &real_rect);
    }

    if (!SDL_LockTexture(texture, &real_rect, &pixels, &pitch)) {
        return false;
    }

    texture->locked_surface = SDL_CreateSurfaceFrom(real_rect.w, real_rect.h, texture->format, pixels, pitch);
    if (!texture->locked_surface) {
        SDL_UnlockTexture(texture);
        return false;
    }
    if (texture->public_palette) {
        SDL_SetSurfacePalette(texture->locked_surface, texture->public_palette);
    }

    *surface = texture->locked_surface;
    return true;
}

#ifdef SDL_HAVE_YUV
static void SDL_UnlockTextureYUV(SDL_Texture *texture)
{
    SDL_Texture *native = texture->native;
    void *native_pixels = NULL;
    int native_pitch = 0;
    SDL_Rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.w = texture->w;
    rect.h = texture->h;

    if (!SDL_LockTexture(native, &rect, &native_pixels, &native_pitch)) {
        return;
    }
    SDL_SW_CopyYUVToRGB(texture->yuv, &rect, native->format,
                        rect.w, rect.h, native_pixels, native_pitch);
    SDL_UnlockTexture(native);
}
#endif // SDL_HAVE_YUV

static void SDL_UnlockTexturePaletteSurface(SDL_Texture *texture)
{
    texture->palette_version = 0;
}

static void SDL_UnlockTextureNative(SDL_Texture *texture)
{
    SDL_Texture *native = texture->native;
    void *native_pixels = NULL;
    int native_pitch = 0;
    const SDL_Rect *rect = &texture->locked_rect;
    const void *pixels = (void *)((Uint8 *)texture->pixels +
                                  rect->y * texture->pitch +
                                  rect->x * SDL_BYTESPERPIXEL(texture->format));
    int pitch = texture->pitch;

    if (!SDL_LockTexture(native, rect, &native_pixels, &native_pitch)) {
        return;
    }
    SDL_ConvertPixels(rect->w, rect->h,
                      texture->format, pixels, pitch,
                      native->format, native_pixels, native_pitch);
    SDL_UnlockTexture(native);
}

void SDL_UnlockTexture(SDL_Texture *texture)
{
    CHECK_TEXTURE_MAGIC(texture,);

    if (texture->access != SDL_TEXTUREACCESS_STREAMING) {
        return;
    }

#ifdef SDL_HAVE_YUV
    if (texture->yuv) {
        SDL_UnlockTextureYUV(texture);
    } else
#endif
    if (texture->palette_surface) {
        SDL_UnlockTexturePaletteSurface(texture);
    } else if (texture->native) {
        SDL_UnlockTextureNative(texture);
    } else {
        SDL_Renderer *renderer = texture->renderer;
        renderer->UnlockTexture(renderer, texture);
    }

    if (texture->locked_surface) {
        SDL_DestroySurface(texture->locked_surface);
        texture->locked_surface = NULL;
    }
}

