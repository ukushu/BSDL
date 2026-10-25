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
static bool SDL_UpdateTextureYUV(SDL_Texture *texture, const SDL_Rect *rect,
                                const void *pixels, int pitch)
{
    SDL_Texture *native = texture->native;
    SDL_Rect full_rect;
    bool result = true;

    if (!SDL_SW_UpdateYUVTexture(texture->yuv, rect, pixels, pitch)) {
        return false;
    }

    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = texture->w;
    full_rect.h = texture->h;
    rect = &full_rect;

    if (texture->access == SDL_TEXTUREACCESS_STREAMING) {
        // We can lock the texture and copy to it
        void *native_pixels = NULL;
        int native_pitch = 0;

        if (!SDL_LockTexture(native, rect, &native_pixels, &native_pitch)) {
            return false;
        }
        result = SDL_SW_CopyYUVToRGB(texture->yuv, rect, native->format, rect->w, rect->h, native_pixels, native_pitch);
        SDL_UnlockTexture(native);
    } else {
        // Use a temporary buffer for updating
        const int temp_pitch = (((rect->w * SDL_BYTESPERPIXEL(native->format)) + 3) & ~3);
        const size_t alloclen = (size_t)rect->h * temp_pitch;
        if (alloclen > 0) {
            void *temp_pixels = SDL_malloc(alloclen);
            if (!temp_pixels) {
                return false;
            }
            result = SDL_SW_CopyYUVToRGB(texture->yuv, rect, native->format, rect->w, rect->h, temp_pixels, temp_pitch);
            if (result) {
                SDL_UpdateTexture(native, rect, temp_pixels, temp_pitch);
            }
            SDL_free(temp_pixels);
        }
    }
    return result;
}
#endif // SDL_HAVE_YUV

static bool SDL_UpdateTexturePaletteSurface(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_Surface *surface = texture->palette_surface;

    const Uint8 *src = (const Uint8 *)pixels;
    Uint8 *dst = ((Uint8 *)surface->pixels) + rect->y * surface->pitch + (rect->x * SDL_BITSPERPIXEL(texture->format)) / 8;
    int w = ((rect->w * SDL_BITSPERPIXEL(texture->format)) + 7) / 8;
    int h = rect->h;
    while (h--) {
        SDL_memcpy(dst, src, w);
        src += pitch;
        dst += surface->pitch;
    }
    texture->palette_version = 0;
    return true;
}

static bool SDL_UpdateTextureNative(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_Texture *native = texture->native;

    if (texture->access == SDL_TEXTUREACCESS_STREAMING) {
        // We can lock the texture and copy to it
        void *native_pixels = NULL;
        int native_pitch = 0;

        if (!SDL_LockTexture(native, rect, &native_pixels, &native_pitch)) {
            return false;
        }
        SDL_ConvertPixelsAndColorspace(rect->w, rect->h,
                                       texture->format, texture->colorspace, 0, pixels, pitch,
                                       native->format, native->colorspace, 0, native_pixels, native_pitch);
        SDL_UnlockTexture(native);
    } else {
        // Use a temporary buffer for updating
        const int temp_pitch = (((rect->w * SDL_BYTESPERPIXEL(native->format)) + 3) & ~3);
        const size_t alloclen = (size_t)rect->h * temp_pitch;
        if (alloclen > 0) {
            void *temp_pixels = SDL_malloc(alloclen);
            if (!temp_pixels) {
                return false;
            }
            SDL_ConvertPixelsAndColorspace(rect->w, rect->h,
                                           texture->format, texture->colorspace, 0, pixels, pitch,
                                           native->format, native->colorspace, 0, temp_pixels, temp_pitch);
            SDL_UpdateTexture(native, rect, temp_pixels, temp_pitch);
            SDL_free(temp_pixels);
        }
    }
    return true;
}

bool SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_Rect real_rect;

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(!pixels) {
        return SDL_InvalidParamError("pixels");
    }
    CHECK_PARAM(!pitch) {
        return SDL_InvalidParamError("pitch");
    }

    real_rect.x = 0;
    real_rect.y = 0;
    real_rect.w = texture->w;
    real_rect.h = texture->h;
    if (rect) {
        if (!SDL_GetRectIntersection(rect, &real_rect, &real_rect)) {
            return true;
        }
    }

    if (real_rect.w == 0 || real_rect.h == 0) {
        return true; // nothing to do.
#ifdef SDL_HAVE_YUV
    } else if (texture->yuv) {
        return SDL_UpdateTextureYUV(texture, &real_rect, pixels, pitch);
#endif
    } else if (texture->palette_surface) {
        return SDL_UpdateTexturePaletteSurface(texture, &real_rect, pixels, pitch);
    } else if (texture->native) {
        return SDL_UpdateTextureNative(texture, &real_rect, pixels, pitch);
    } else {
        SDL_Renderer *renderer = texture->renderer;
        if (!SDL_FlushRenderCommandsIfTextureNeeded(texture)) {
            return false;
        }
        return renderer->UpdateTexture(renderer, texture, &real_rect, pixels, pitch);
    }
}

#ifdef SDL_HAVE_YUV
static bool SDL_UpdateTextureYUVToNative(SDL_Texture *texture)
{
    SDL_Texture *native = texture->native;
    SDL_Rect rect = { 0, 0, texture->w, texture->h };
    bool result = true;

    if (!rect.w || !rect.h) {
        return true; // nothing to do.
    }

    if (texture->access == SDL_TEXTUREACCESS_STREAMING) {
        void *native_pixels = NULL;
        int native_pitch = 0;

        if (!SDL_LockTexture(native, &rect, &native_pixels, &native_pitch)) {
            return false;
        }
        result = SDL_SW_CopyYUVToRGB(texture->yuv, &rect, native->format, rect.w, rect.h, native_pixels, native_pitch);
        SDL_UnlockTexture(native);
    } else {
        const int temp_pitch = (((rect.w * SDL_BYTESPERPIXEL(native->format)) + 3) & ~3);
        const size_t alloclen = (size_t)rect.h * temp_pitch;
        if (alloclen > 0) {
            void *temp_pixels = SDL_malloc(alloclen);
            if (!temp_pixels) {
                return false;
            }
            result = SDL_SW_CopyYUVToRGB(texture->yuv, &rect, native->format, rect.w, rect.h, temp_pixels, temp_pitch);
            if (result) {
                SDL_UpdateTexture(native, &rect, temp_pixels, temp_pitch);
            }
            SDL_free(temp_pixels);
        }
    }
    return result;
}

static bool SDL_UpdateTextureYUVPlanar(SDL_Texture *texture, const SDL_Rect *rect,
                                      const Uint8 *Yplane, int Ypitch,
                                      const Uint8 *Uplane, int Upitch,
                                      const Uint8 *Vplane, int Vpitch)
{
    if (!SDL_SW_UpdateYUVTexturePlanar(texture->yuv, rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch)) {
        return false;
    }
    return SDL_UpdateTextureYUVToNative(texture);
}

static bool SDL_UpdateTextureNVPlanar(SDL_Texture *texture, const SDL_Rect *rect,
                                     const Uint8 *Yplane, int Ypitch,
                                     const Uint8 *UVplane, int UVpitch)
{
    if (!SDL_SW_UpdateNVTexturePlanar(texture->yuv, rect, Yplane, Ypitch, UVplane, UVpitch)) {
        return false;
    }
    return SDL_UpdateTextureYUVToNative(texture);
}

#endif // SDL_HAVE_YUV

bool SDL_UpdateYUVTexture(SDL_Texture *texture, const SDL_Rect *rect,
                         const Uint8 *Yplane, int Ypitch,
                         const Uint8 *Uplane, int Upitch,
                         const Uint8 *Vplane, int Vpitch)
{
    SDL_Renderer *renderer;
    SDL_Rect real_rect;

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(!Yplane) {
        return SDL_InvalidParamError("Yplane");
    }
    CHECK_PARAM(!Ypitch) {
        return SDL_InvalidParamError("Ypitch");
    }
    CHECK_PARAM(!Uplane) {
        return SDL_InvalidParamError("Uplane");
    }
    CHECK_PARAM(!Upitch) {
        return SDL_InvalidParamError("Upitch");
    }
    CHECK_PARAM(!Vplane) {
        return SDL_InvalidParamError("Vplane");
    }
    CHECK_PARAM(!Vpitch) {
        return SDL_InvalidParamError("Vpitch");
    }

    CHECK_PARAM(texture->format != SDL_PIXELFORMAT_YV12 &&
                texture->format != SDL_PIXELFORMAT_IYUV &&
                texture->format != SDL_PIXELFORMAT_I444 &&
                texture->format != SDL_PIXELFORMAT_I0FL &&
                texture->format != SDL_PIXELFORMAT_I4FL) {
        return SDL_SetError("Texture format must be YV12, IYUV, I444, I0FL, or I4FL");
    }

    real_rect.x = 0;
    real_rect.y = 0;
    real_rect.w = texture->w;
    real_rect.h = texture->h;
    if (rect) {
        SDL_GetRectIntersection(rect, &real_rect, &real_rect);
    }

    if (real_rect.w == 0 || real_rect.h == 0) {
        return true; // nothing to do.
    }

#ifdef SDL_HAVE_YUV
    if (texture->yuv) {
        return SDL_UpdateTextureYUVPlanar(texture, &real_rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch);
    } else
#endif
    {
        SDL_assert(!texture->native);
        renderer = texture->renderer;
        SDL_assert(renderer->UpdateTextureYUV);
        if (renderer->UpdateTextureYUV) {
            if (!SDL_FlushRenderCommandsIfTextureNeeded(texture)) {
                return false;
            }
            return renderer->UpdateTextureYUV(renderer, texture, &real_rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch);
        } else {
            return SDL_Unsupported();
        }
    }
}

bool SDL_UpdateNVTexture(SDL_Texture *texture, const SDL_Rect *rect,
                        const Uint8 *Yplane, int Ypitch,
                        const Uint8 *UVplane, int UVpitch)
{
    SDL_Renderer *renderer;
    SDL_Rect real_rect;

    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(!Yplane) {
        return SDL_InvalidParamError("Yplane");
    }
    CHECK_PARAM(!Ypitch) {
        return SDL_InvalidParamError("Ypitch");
    }
    CHECK_PARAM(!UVplane) {
        return SDL_InvalidParamError("UVplane");
    }
    CHECK_PARAM(!UVpitch) {
        return SDL_InvalidParamError("UVpitch");
    }

    CHECK_PARAM(texture->format != SDL_PIXELFORMAT_NV12 &&
                texture->format != SDL_PIXELFORMAT_NV21 &&
                texture->format != SDL_PIXELFORMAT_P010) {
        return SDL_SetError("Texture format must be NV12, NV21, or P010");
    }

    real_rect.x = 0;
    real_rect.y = 0;
    real_rect.w = texture->w;
    real_rect.h = texture->h;
    if (rect) {
        SDL_GetRectIntersection(rect, &real_rect, &real_rect);
    }

    if (real_rect.w == 0 || real_rect.h == 0) {
        return true; // nothing to do.
    }

#ifdef SDL_HAVE_YUV
    if (texture->yuv) {
        return SDL_UpdateTextureNVPlanar(texture, &real_rect, Yplane, Ypitch, UVplane, UVpitch);
    } else
#endif
    {
        SDL_assert(!texture->native);
        renderer = texture->renderer;
        SDL_assert(renderer->UpdateTextureNV);
        if (renderer->UpdateTextureNV) {
            if (!SDL_FlushRenderCommandsIfTextureNeeded(texture)) {
                return false;
            }
            return renderer->UpdateTextureNV(renderer, texture, &real_rect, Yplane, Ypitch, UVplane, UVpitch);
        } else {
            return SDL_Unsupported();
        }
    }
}

