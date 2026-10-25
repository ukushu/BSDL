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

#include "SDL_sysvideo.h"
#include "SDL_video_c.h"
#include "SDL_RLEaccel_c.h"
#include "SDL_pixels_c.h"
#include "SDL_rotate.h"
#include "SDL_stb_c.h"
#include "SDL_yuv_c.h"
#include "../render/SDL_sysrender.h"

#include "SDL_surface_c.h"


// Check to make sure we can safely check multiplication of surface w and pitch and it won't overflow size_t
SDL_COMPILE_TIME_ASSERT(surface_size_assumptions,
                        sizeof(int) == sizeof(Sint32) && sizeof(size_t) >= sizeof(Sint32));

SDL_COMPILE_TIME_ASSERT(can_indicate_overflow, SDL_SIZE_MAX > SDL_MAX_SINT32);

// Magic!
static char SDL_surface_magic;

// Public routines

bool SDL_SurfaceValid(SDL_Surface *surface)
{
    return (surface && surface->reserved == &SDL_surface_magic);
}

void SDL_UpdateSurfaceLockFlag(SDL_Surface *surface)
{
    // We need to mark the surface as needing unlock while locked
    if ((surface->flags & SDL_SURFACE_LOCKED) ||
        (surface->internal_flags & SDL_INTERNAL_SURFACE_RLEACCEL)) {
        surface->flags |= SDL_SURFACE_LOCK_NEEDED;
    } else {
        surface->flags &= ~SDL_SURFACE_LOCK_NEEDED;
    }
}

/*
 * Calculate the pad-aligned scanline width of a surface.
 *
 * for FOURCC, use SDL_CalculateYUVSize()
 */
static bool SDL_CalculateRGBSize(Uint32 format, size_t width, size_t height, size_t *size, size_t *pitch, bool minimal)
{
    if (SDL_BITSPERPIXEL(format) >= 8) {
        if (!SDL_size_mul_check_overflow(width, SDL_BYTESPERPIXEL(format), pitch)) {
            return SDL_SetError("width * bpp would overflow");
        }
    } else {
        if (!SDL_size_mul_check_overflow(width, SDL_BITSPERPIXEL(format), pitch)) {
            return SDL_SetError("width * bpp would overflow");
        }
        if (!SDL_size_add_check_overflow(*pitch, 7, pitch)) {
            return SDL_SetError("aligning pitch would overflow");
        }
        *pitch /= 8;
    }
    if (!minimal) {
        // 4-byte aligning for speed
        if (!SDL_size_add_check_overflow(*pitch, 3, pitch)) {
            return SDL_SetError("aligning pitch would overflow");
        }
        *pitch &= ~3;
    }

    if (!SDL_size_mul_check_overflow(height, *pitch, size)) {
        return SDL_SetError("height * pitch would overflow");
    }

    return true;
}

bool SDL_CalculateSurfaceSize(SDL_PixelFormat format, int width, int height, size_t *size, size_t *pitch, bool minimalPitch)
{
    size_t p = 0, sz = 0;

    if (size) {
        *size = 0;
    }

    if (pitch) {
        *pitch = 0;
    }

    if (SDL_ISPIXELFORMAT_FOURCC(format)) {
        if (format == SDL_PIXELFORMAT_MJPG) {
            // We don't know in advance what it will be, we'll figure it out later.
            return true;
        }

        if (!SDL_CalculateYUVSize(format, width, height, &sz, &p)) {
            // Overflow...
            return false;
        }
    } else {
        if (!SDL_CalculateRGBSize(format, width, height, &sz, &p, minimalPitch)) {
            // Overflow...
            return false;
        }
    }

    if (size) {
        *size = sz;
    }

    if (pitch) {
        *pitch = p;
    }

    return true;
}

static bool SDL_InitializeSurface(SDL_Surface *surface, int width, int height, SDL_PixelFormat format, SDL_Colorspace colorspace, SDL_PropertiesID props, void *pixels, int pitch, bool onstack)
{
    SDL_zerop(surface);

    surface->flags = SDL_SURFACE_PREALLOCATED;
    surface->format = format;
    surface->w = width;
    surface->h = height;
    surface->pixels = pixels;
    surface->pitch = pitch;
    surface->reserved = &SDL_surface_magic;

    if (onstack) {
        surface->internal_flags |= SDL_INTERNAL_SURFACE_STACK;
    }

    surface->fmt = SDL_GetPixelFormatDetails(format);
    if (!surface->fmt) {
        SDL_DestroySurface(surface);
        return false;
    }

    // Initialize the clip rect
    surface->clip_rect.w = width;
    surface->clip_rect.h = height;

    // Allocate an empty mapping
    surface->map.info.r = 0xFF;
    surface->map.info.g = 0xFF;
    surface->map.info.b = 0xFF;
    surface->map.info.a = 0xFF;

    if (colorspace == SDL_COLORSPACE_UNKNOWN) {
        surface->colorspace = SDL_GetDefaultColorspaceForFormat(format);
    } else {
        surface->colorspace = colorspace;
    }

    if (props) {
        if (!SDL_CopyProperties(props, SDL_GetSurfaceProperties(surface))) {
            SDL_DestroySurface(surface);
            return false;
        }
    }

    // By default surfaces with an alpha mask are set up for blending
    if (SDL_ISPIXELFORMAT_ALPHA(surface->format)) {
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    }

    // The surface is ready to go
    surface->refcount = 1;
    return true;
}

/*
 * Create an empty surface of the appropriate depth using the given format
 */
static SDL_Surface *SDL_CreateSurfaceInternal(int width, int height, SDL_PixelFormat format, bool clear_surface)
{
    size_t pitch, size;
    SDL_Surface *surface;

    CHECK_PARAM(width < 0) {
        SDL_InvalidParamError("width");
        return NULL;
    }

    CHECK_PARAM(height < 0) {
        SDL_InvalidParamError("height");
        return NULL;
    }

    CHECK_PARAM(format == SDL_PIXELFORMAT_UNKNOWN) {
        SDL_InvalidParamError("format");
        return NULL;
    }

    if (!SDL_CalculateSurfaceSize(format, width, height, &size, &pitch, false /* not minimal pitch */)) {
        // Overflow...
        return NULL;
    }

    // Allocate and initialize the surface
    surface = (SDL_Surface *)SDL_malloc(sizeof(*surface));
    if (!surface) {
        return NULL;
    }

    if (!SDL_InitializeSurface(surface, width, height, format, SDL_COLORSPACE_UNKNOWN, 0, NULL, (int)pitch, false)) {
        return NULL;
    }

    if (surface->w && surface->h && format != SDL_PIXELFORMAT_MJPG) {
        surface->flags &= ~SDL_SURFACE_PREALLOCATED;
        if (SDL_GetHintBoolean("SDL_SURFACE_MALLOC", false)) {
            if (clear_surface) {
                surface->pixels = SDL_calloc(1, size);
            } else {
                surface->pixels = SDL_malloc(size);
            }
        } else {
            surface->flags |= SDL_SURFACE_SIMD_ALIGNED;
            if (clear_surface) {
                surface->pixels = SDL_aligned_alloc_zero(SDL_GetSIMDAlignment(), size);
            } else {
                surface->pixels = SDL_aligned_alloc(SDL_GetSIMDAlignment(), size);
            }
        }
        if (!surface->pixels) {
            SDL_DestroySurface(surface);
            return NULL;
        }
    }
    return surface;
}

SDL_Surface *SDL_CreateSurfaceZeroed(int width, int height, SDL_PixelFormat format)
{
    return SDL_CreateSurfaceInternal(width, height, format, true);
}

SDL_Surface *SDL_CreateSurfaceUninitialized(int width, int height, SDL_PixelFormat format)
{
    return SDL_CreateSurfaceInternal(width, height, format, false);
}

SDL_Surface *SDL_CreateSurface(int width, int height, SDL_PixelFormat format)
{
    return SDL_CreateSurfaceZeroed(width, height, format);
}


/*
 * Create an RGB surface from an existing memory buffer using the given
 * enum SDL_PIXELFORMAT_* format
 */
SDL_Surface *SDL_CreateSurfaceFrom(int width, int height, SDL_PixelFormat format, void *pixels, int pitch)
{
    CHECK_PARAM(width < 0) {
        SDL_InvalidParamError("width");
        return NULL;
    }

    CHECK_PARAM(height < 0) {
        SDL_InvalidParamError("height");
        return NULL;
    }

    CHECK_PARAM(format == SDL_PIXELFORMAT_UNKNOWN) {
        SDL_InvalidParamError("format");
        return NULL;
    }

    if (pitch == 0 && !pixels) {
        // The application will fill these in later with valid values
    } else {
        size_t minimalPitch;

        if (!SDL_CalculateSurfaceSize(format, width, height, NULL, &minimalPitch, true /* minimal pitch */)) {
            // Overflow...
            return NULL;
        }

        CHECK_PARAM(pitch < 0 || (size_t)pitch < minimalPitch) {
            SDL_InvalidParamError("pitch");
            return NULL;
        }
    }

    // Allocate and initialize the surface
    SDL_Surface *surface = (SDL_Surface *)SDL_malloc(sizeof(*surface));
    if (!surface ||
        !SDL_InitializeSurface(surface, width, height, format, SDL_COLORSPACE_UNKNOWN, 0, pixels, pitch, false)) {
        return NULL;
    }
    return surface;
}

SDL_PropertiesID SDL_GetSurfaceProperties(SDL_Surface *surface)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return 0;
    }

    if (!surface->props) {
        surface->props = SDL_CreateProperties();
    }
    return surface->props;
}

bool SDL_SetSurfaceColorspace(SDL_Surface *surface, SDL_Colorspace colorspace)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    surface->colorspace = colorspace;
    return true;
}

SDL_Colorspace SDL_GetSurfaceColorspace(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return SDL_COLORSPACE_UNKNOWN;
    }

    return surface->colorspace;
}

float SDL_GetDefaultSDRWhitePoint(SDL_Colorspace colorspace)
{
    return SDL_GetSurfaceSDRWhitePoint(NULL, colorspace);
}

float SDL_GetSurfaceSDRWhitePoint(SDL_Surface *surface, SDL_Colorspace colorspace)
{
    SDL_TransferCharacteristics transfer = SDL_COLORSPACETRANSFER(colorspace);

    if (transfer == SDL_TRANSFER_CHARACTERISTICS_LINEAR ||
        transfer == SDL_TRANSFER_CHARACTERISTICS_PQ) {
        SDL_PropertiesID props;
        float default_value = 1.0f;

        if (SDL_SurfaceValid(surface)) {
            props = surface->props;
        } else {
            props = 0;
        }
        if (transfer == SDL_TRANSFER_CHARACTERISTICS_PQ) {
            /* The older standards use an SDR white point of 100 nits.
             * ITU-R BT.2408-6 recommends using an SDR white point of 203 nits.
             * This is the default Chrome uses, and what a lot of game content
             * assumes, so we'll go with that.
             */
            const float DEFAULT_PQ_SDR_WHITE_POINT = 203.0f;
            default_value = DEFAULT_PQ_SDR_WHITE_POINT;
        }
        return SDL_GetFloatProperty(props, SDL_PROP_SURFACE_SDR_WHITE_POINT_FLOAT, default_value);
    }
    return 1.0f;
}

float SDL_GetDefaultHDRHeadroom(SDL_Colorspace colorspace)
{
    return SDL_GetSurfaceHDRHeadroom(NULL, colorspace);
}

float SDL_GetSurfaceHDRHeadroom(SDL_Surface *surface, SDL_Colorspace colorspace)
{
    SDL_TransferCharacteristics transfer = SDL_COLORSPACETRANSFER(colorspace);

    if (transfer == SDL_TRANSFER_CHARACTERISTICS_LINEAR ||
        transfer == SDL_TRANSFER_CHARACTERISTICS_PQ) {
        SDL_PropertiesID props;
        float default_value = 0.0f;

        if (SDL_SurfaceValid(surface)) {
            props = surface->props;
        } else {
            props = 0;
        }
        return SDL_GetFloatProperty(props, SDL_PROP_SURFACE_HDR_HEADROOM_FLOAT, default_value);
    }
    return 1.0f;
}

SDL_Palette *SDL_CreateSurfacePalette(SDL_Surface *surface)
{
    SDL_Palette *palette;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    CHECK_PARAM(!SDL_ISPIXELFORMAT_INDEXED(surface->format)) {
        SDL_SetError("The surface is not indexed format");
        return NULL;
    }

    palette = SDL_CreatePalette((1 << SDL_BITSPERPIXEL(surface->format)));
    if (!palette) {
        return NULL;
    }

    if (palette->ncolors == 2) {
        // Create a black and white bitmap palette
        palette->colors[0].r = 0xFF;
        palette->colors[0].g = 0xFF;
        palette->colors[0].b = 0xFF;
        palette->colors[1].r = 0x00;
        palette->colors[1].g = 0x00;
        palette->colors[1].b = 0x00;
    }

    if (!SDL_SetSurfacePalette(surface, palette)) {
        SDL_DestroyPalette(palette);
        return NULL;
    }

    // The surface has retained the palette, we can remove the reference here
    SDL_assert(palette->refcount == 2);
    SDL_DestroyPalette(palette);
    return palette;
}

bool SDL_SetSurfacePalette(SDL_Surface *surface, SDL_Palette *palette)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    CHECK_PARAM(palette && !SDL_ISPIXELFORMAT_INDEXED(surface->format)) {
        return SDL_SetError("Surface doesn't use a palette");
    }

    CHECK_PARAM(palette && palette->ncolors > (1 << SDL_BITSPERPIXEL(surface->format))) {
        return SDL_SetError("Palette doesn't match surface format");
    }

    if (palette != surface->palette) {
        if (surface->palette) {
            SDL_DestroyPalette(surface->palette);
        }

        surface->palette = palette;

        if (surface->palette) {
            ++surface->palette->refcount;
        }
    }

    SDL_InvalidateMap(&surface->map);

    return true;
}

SDL_Palette *SDL_GetSurfacePalette(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return NULL;
    }

    return surface->palette;
}

bool SDL_AddSurfaceAlternateImage(SDL_Surface *surface, SDL_Surface *image)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    CHECK_PARAM(!SDL_SurfaceValid(image)) {
        return SDL_InvalidParamError("image");
    }

    SDL_Surface **images = (SDL_Surface **)SDL_realloc(surface->images, (surface->num_images + 1) * sizeof(*images));
    if (!images) {
        return false;
    }
    images[surface->num_images] = image;
    surface->images = images;
    ++surface->num_images;
    ++image->refcount;
    return true;
}

bool SDL_SurfaceHasAlternateImages(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return false;
    }

    return (surface->num_images > 0);
}

SDL_Surface **SDL_GetSurfaceImages(SDL_Surface *surface, int *count)
{
    if (count) {
        *count = 0;
    }

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    int num_images = 1 + surface->num_images;
    SDL_Surface **images = (SDL_Surface **)SDL_malloc((num_images + 1) * sizeof(*images));
    if (!images) {
        return NULL;
    }
    images[0] = surface;
    if (surface->num_images > 0) {
        SDL_memcpy(&images[1], surface->images, (surface->num_images * sizeof(images[1])));
    }
    images[num_images] = NULL;

    if (count) {
        *count = num_images;
    }
    return images;
}

SDL_Surface *SDL_GetSurfaceImage(SDL_Surface *surface, float display_scale)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    if (!SDL_SurfaceHasAlternateImages(surface)) {
        ++surface->refcount;
        return surface;
    }

    // This surface has high DPI images, pick the best one available, or scale one to the correct size
    SDL_Surface **images = SDL_GetSurfaceImages(surface, NULL);
    if (!images) {
        // Failure, fall back to the existing surface
        ++surface->refcount;
        return surface;
    }

    // Find closest image. Images that are larger than the
    // desired size are preferred over images that are smaller.
    SDL_Surface *closest = NULL;
    int desired_w = (int)SDL_round(surface->w * display_scale);
    int desired_h = (int)SDL_round(surface->h * display_scale);
    int desired_size = desired_w * desired_h;
    int closest_distance = -1;
    int closest_size = -1;
    for (int i = 0; images[i]; ++i) {
        SDL_Surface *candidate = images[i];
        int size = candidate->w * candidate->h;
        int delta_w = (candidate->w - desired_w);
        int delta_h = (candidate->h - desired_h);
        int distance = (delta_w * delta_w) + (delta_h * delta_h);
        if (closest_distance < 0 || distance < closest_distance ||
            (size > desired_size && closest_size < desired_size)) {
            closest = candidate;
            closest_distance = distance;
            closest_size = size;
        }
    }
    SDL_free(images);
    SDL_assert(closest != NULL);    // We should always have at least one surface

    if (closest->w == desired_w && closest->h == desired_h) {
        ++closest->refcount;
        return closest;
    }

    // We need to scale the image to the correct size. To maintain good image quality, downscaling
    // is done in steps, never reducing the width and height by more than half each time.
    SDL_Surface *scaled = closest;
    do {
        int next_scaled_w = SDL_max(desired_w, (scaled->w + 1) / 2);
        int next_scaled_h = SDL_max(desired_h, (scaled->h + 1) / 2);
        SDL_Surface *next_scaled = SDL_ScaleSurface(scaled, next_scaled_w, next_scaled_h, SDL_SCALEMODE_LINEAR);
        if (scaled != closest) {
            SDL_DestroySurface(scaled);
        }
        scaled = next_scaled;
        if (!scaled) {
            // Failure, fall back to the closest surface
            ++closest->refcount;
            return closest;
        }
    } while (scaled->w != desired_w || scaled->h != desired_h);

    return scaled;
}

void SDL_RemoveSurfaceAlternateImages(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return;
    }

    if (surface->num_images > 0) {
        for (int i = 0; i < surface->num_images; ++i) {
            SDL_DestroySurface(surface->images[i]);
        }
        SDL_free(surface->images);
        surface->images = NULL;
        surface->num_images = 0;
    }
}

bool SDL_SetSurfaceRLE(SDL_Surface *surface, bool enabled)
{
    int flags;

    CHECK_PARAM(!SDL_SurfaceValid(surface) || SDL_ISPIXELFORMAT_FOURCC(surface->format)) {
        return SDL_InvalidParamError("surface");
    }

    flags = surface->map.info.flags;
    if (enabled) {
        surface->map.info.flags |= SDL_COPY_RLE_DESIRED;
    } else {
        surface->map.info.flags &= ~SDL_COPY_RLE_DESIRED;
    }
    if (surface->map.info.flags != flags) {
        SDL_InvalidateMap(&surface->map);
    }
    return true;
}

bool SDL_SurfaceHasRLE(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return false;
    }

    if (!(surface->map.info.flags & SDL_COPY_RLE_DESIRED)) {
        return false;
    }

    return true;
}

bool SDL_SetSurfaceColorKey(SDL_Surface *surface, bool enabled, Uint32 key)
{
    int flags;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    CHECK_PARAM(surface->palette && key >= ((Uint32)surface->palette->ncolors)) {
        return SDL_InvalidParamError("key");
    }

    flags = surface->map.info.flags;
    if (enabled) {
        surface->map.info.flags |= SDL_COPY_COLORKEY;
        surface->map.info.colorkey = key;
    } else {
        surface->map.info.flags &= ~SDL_COPY_COLORKEY;
    }
    if (surface->map.info.flags != flags) {
        SDL_InvalidateMap(&surface->map);
    }

    return true;
}

bool SDL_SurfaceHasColorKey(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return false;
    }

    if (!(surface->map.info.flags & SDL_COPY_COLORKEY)) {
        return false;
    }

    return true;
}

bool SDL_GetSurfaceColorKey(SDL_Surface *surface, Uint32 *key)
{
    if (key) {
        *key = 0;
    }

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    CHECK_PARAM(!(surface->map.info.flags & SDL_COPY_COLORKEY)) {
        return SDL_SetError("Surface doesn't have a colorkey");
    }

    if (key) {
        *key = surface->map.info.colorkey;
    }
    return true;
}

/* This is a fairly slow function to switch from colorkey to alpha
   NB: it doesn't handle bpp 1 or 3, because they have no alpha channel */
bool SDL_SetSurfaceColorMod(SDL_Surface *surface, Uint8 r, Uint8 g, Uint8 b)
{
    int flags;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    surface->map.info.r = r;
    surface->map.info.g = g;
    surface->map.info.b = b;

    flags = surface->map.info.flags;
    if (r != 0xFF || g != 0xFF || b != 0xFF) {
        surface->map.info.flags |= SDL_COPY_MODULATE_COLOR;
    } else {
        surface->map.info.flags &= ~SDL_COPY_MODULATE_COLOR;
    }
    if (surface->map.info.flags != flags) {
        SDL_InvalidateMap(&surface->map);
    }
    return true;
}

bool SDL_GetSurfaceColorMod(SDL_Surface *surface, Uint8 *r, Uint8 *g, Uint8 *b)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        if (r) {
            *r = 255;
        }
        if (g) {
            *g = 255;
        }
        if (b) {
            *b = 255;
        }
        return SDL_InvalidParamError("surface");
    }

    if (r) {
        *r = surface->map.info.r;
    }
    if (g) {
        *g = surface->map.info.g;
    }
    if (b) {
        *b = surface->map.info.b;
    }
    return true;
}

bool SDL_SetSurfaceAlphaMod(SDL_Surface *surface, Uint8 alpha)
{
    int flags;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    surface->map.info.a = alpha;

    flags = surface->map.info.flags;
    if (alpha != 0xFF) {
        surface->map.info.flags |= SDL_COPY_MODULATE_ALPHA;
    } else {
        surface->map.info.flags &= ~SDL_COPY_MODULATE_ALPHA;
    }
    if (surface->map.info.flags != flags) {
        SDL_InvalidateMap(&surface->map);
    }
    return true;
}

bool SDL_GetSurfaceAlphaMod(SDL_Surface *surface, Uint8 *alpha)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        if (alpha) {
            *alpha = 255;
        }
        return SDL_InvalidParamError("surface");
    }

    if (alpha) {
        *alpha = surface->map.info.a;
    }
    return true;
}

bool SDL_SetSurfaceBlendMode(SDL_Surface *surface, SDL_BlendMode blendMode)
{
    int flags;
    bool result = true;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    CHECK_PARAM(blendMode == SDL_BLENDMODE_INVALID) {
        return SDL_InvalidParamError("blendMode");
    }

    flags = surface->map.info.flags;
    surface->map.info.flags &= ~SDL_COPY_BLEND_MASK;
    switch (blendMode) {
    case SDL_BLENDMODE_NONE:
        break;
    case SDL_BLENDMODE_BLEND:
        surface->map.info.flags |= SDL_COPY_BLEND;
        break;
    case SDL_BLENDMODE_BLEND_PREMULTIPLIED:
        surface->map.info.flags |= SDL_COPY_BLEND_PREMULTIPLIED;
        break;
    case SDL_BLENDMODE_ADD:
        surface->map.info.flags |= SDL_COPY_ADD;
        break;
    case SDL_BLENDMODE_ADD_PREMULTIPLIED:
        surface->map.info.flags |= SDL_COPY_ADD_PREMULTIPLIED;
        break;
    case SDL_BLENDMODE_MOD:
        surface->map.info.flags |= SDL_COPY_MOD;
        break;
    case SDL_BLENDMODE_MUL:
        surface->map.info.flags |= SDL_COPY_MUL;
        break;
    default:
        result = SDL_Unsupported();
        break;
    }

    if (surface->map.info.flags != flags) {
        SDL_InvalidateMap(&surface->map);
    }

    return result;
}

bool SDL_GetSurfaceBlendMode(SDL_Surface *surface, SDL_BlendMode *blendMode)
{
    if (blendMode) {
        *blendMode = SDL_BLENDMODE_INVALID;
    }

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    if (!blendMode) {
        return true;
    }

    switch (surface->map.info.flags & SDL_COPY_BLEND_MASK) {
    case SDL_COPY_BLEND:
        *blendMode = SDL_BLENDMODE_BLEND;
        break;
    case SDL_COPY_BLEND_PREMULTIPLIED:
        *blendMode = SDL_BLENDMODE_BLEND_PREMULTIPLIED;
        break;
    case SDL_COPY_ADD:
        *blendMode = SDL_BLENDMODE_ADD;
        break;
    case SDL_COPY_ADD_PREMULTIPLIED:
        *blendMode = SDL_BLENDMODE_ADD_PREMULTIPLIED;
        break;
    case SDL_COPY_MOD:
        *blendMode = SDL_BLENDMODE_MOD;
        break;
    case SDL_COPY_MUL:
        *blendMode = SDL_BLENDMODE_MUL;
        break;
    default:
        *blendMode = SDL_BLENDMODE_NONE;
        break;
    }
    return true;
}

bool SDL_SetSurfaceClipRect(SDL_Surface *surface, const SDL_Rect *rect)
{
    SDL_Rect full_rect;

    // Don't do anything if there's no surface to act on
    if (!SDL_SurfaceValid(surface)) {
        return false;
    }

    // Set up the full surface rectangle
    full_rect.x = 0;
    full_rect.y = 0;
    full_rect.w = surface->w;
    full_rect.h = surface->h;

    // Set the clipping rectangle
    if (!rect) {
        surface->clip_rect = full_rect;
        return true;
    }
    return SDL_GetRectIntersection(rect, &full_rect, &surface->clip_rect);
}

bool SDL_GetSurfaceClipRect(SDL_Surface *surface, SDL_Rect *rect)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        if (rect) {
            SDL_zerop(rect);
        }
        return SDL_InvalidParamError("surface");
    }
    CHECK_PARAM(!rect) {
        return SDL_InvalidParamError("rect");
    }
    *rect = surface->clip_rect;
    return true;
}

/*
 * Lock a surface to directly access the pixels
 */
bool SDL_LockSurface(SDL_Surface *surface)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }

    if (!surface->locked) {
#ifdef SDL_HAVE_RLE
        // Perform the lock
        if (surface->internal_flags & SDL_INTERNAL_SURFACE_RLEACCEL) {
            SDL_UnRLESurface(surface);
        }
#endif
    }

    // Increment the surface lock count, for recursive locks
    ++surface->locked;
    surface->flags |= SDL_SURFACE_LOCKED;
    SDL_UpdateSurfaceLockFlag(surface);

    // Ready to go..
    return true;
}

/*
 * Unlock a previously locked surface
 */
void SDL_UnlockSurface(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return;
    }

    // Only perform an unlock if we are locked
    if (!surface->locked || (--surface->locked > 0)) {
        return;
    }

    surface->flags &= ~SDL_SURFACE_LOCKED;
    SDL_UpdateSurfaceLockFlag(surface);
}

static bool SDL_FlipSurfaceHorizontal(SDL_Surface *surface)
{
    bool isstack;
    Uint8 *row, *a, *b, *tmp;
    int i, j, bpp;

    if (SDL_BITSPERPIXEL(surface->format) < 8) {
        // We could implement this if needed, but we'd have to flip sets of bits within a byte
        return SDL_Unsupported();
    }

    if (surface->h <= 0) {
        return true;
    }

    if (surface->w <= 1) {
        return true;
    }

    bpp = SDL_BYTESPERPIXEL(surface->format);
    row = (Uint8 *)surface->pixels;
    tmp = SDL_small_alloc(Uint8, surface->pitch, &isstack);
    if (!tmp) {
        return false;
    }
    for (i = surface->h; i--; ) {
        a = row;
        b = a + (surface->w - 1) * bpp;
        for (j = surface->w / 2; j--; ) {
            SDL_memcpy(tmp, a, bpp);
            SDL_memcpy(a, b, bpp);
            SDL_memcpy(b, tmp, bpp);
            a += bpp;
            b -= bpp;
        }
        row += surface->pitch;
    }
    SDL_small_free(tmp, isstack);
    return true;
}

static bool SDL_FlipSurfaceVertical(SDL_Surface *surface)
{
    bool isstack;
    Uint8 *a, *b, *tmp;
    int i;

    if (surface->h <= 1) {
        return true;
    }

    a = (Uint8 *)surface->pixels;
    b = a + (surface->h - 1) * surface->pitch;
    tmp = SDL_small_alloc(Uint8, surface->pitch, &isstack);
    if (!tmp) {
        return false;
    }
    for (i = surface->h / 2; i--; ) {
        SDL_memcpy(tmp, a, surface->pitch);
        SDL_memcpy(a, b, surface->pitch);
        SDL_memcpy(b, tmp, surface->pitch);
        a += surface->pitch;
        b -= surface->pitch;
    }
    SDL_small_free(tmp, isstack);
    return true;
}

bool SDL_FlipSurface(SDL_Surface *surface, SDL_FlipMode flip)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        return SDL_InvalidParamError("surface");
    }
    if (!surface->pixels) {
        return true;
    }

    bool result = true;
    switch (flip) {
    case SDL_FLIP_HORIZONTAL:
        result = SDL_FlipSurfaceHorizontal(surface);
        break;
    case SDL_FLIP_VERTICAL:
        result = SDL_FlipSurfaceVertical(surface);
        break;
    case SDL_FLIP_HORIZONTAL_AND_VERTICAL:
        result &= SDL_FlipSurfaceHorizontal(surface);
        result &= SDL_FlipSurfaceVertical(surface);
        break;
    default:
        result = SDL_InvalidParamError("flip");
        break;
    }
    return result;
}

SDL_Surface *SDL_RotateSurface(SDL_Surface *surface, float angle)
{
    SDL_Surface *rotated = NULL;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    SDL_Rect rect_dest;
    double cangle, sangle;
    SDL_FPoint center = { surface->w * 0.5f, surface->h * 0.5f };
    SDLgfx_rotozoomSurfaceSizeTrig(surface->w, surface->h, angle, &center, &rect_dest, &cangle, &sangle);

    // This function requires a 32-bit surface or 8-bit surface with a colorkey
    if ((SDL_BITSPERPIXEL(surface->format) == 32 && SDL_PIXELLAYOUT(surface->format) == SDL_PACKEDLAYOUT_8888) ||
        (surface->format == SDL_PIXELFORMAT_INDEX8 && SDL_SurfaceHasColorKey(surface))) {
        rotated = SDLgfx_rotateSurface(surface, angle, 1, 0, 0, &rect_dest, cangle, sangle, &center);
    } else {
        SDL_Surface *convert = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        if (convert) {
            SDL_Surface *tmp = SDLgfx_rotateSurface(convert, angle, 1, 0, 0, &rect_dest, cangle, sangle, &center);
            if (tmp) {
                rotated = SDL_ConvertSurfaceAndColorspace(tmp, surface->format, surface->palette, surface->colorspace, surface->props);
                SDL_DestroySurface(tmp);
            }
            SDL_DestroySurface(convert);
        }
    }

    if (rotated) {
        if (SDL_HasProperty(surface->props, SDL_PROP_SURFACE_ROTATION_FLOAT)) {
            const float rotation = (SDL_GetNumberProperty(surface->props, SDL_PROP_SURFACE_ROTATION_FLOAT, 0) - angle);
            SDL_SetFloatProperty(SDL_GetSurfaceProperties(rotated), SDL_PROP_SURFACE_ROTATION_FLOAT, rotation);
        }
    }

    return rotated;
}

SDL_Surface *SDL_DuplicateSurface(SDL_Surface *surface)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    return SDL_ConvertSurfaceAndColorspace(surface, surface->format, surface->palette, surface->colorspace, surface->props);
}

SDL_Surface *SDL_ScaleSurface(SDL_Surface *surface, int width, int height, SDL_ScaleMode scaleMode)
{
    SDL_Surface *convert = NULL;
    Uint32 copy_flags;
    SDL_Color copy_color;
    bool rc;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        goto error;
    }

    if (SDL_ISPIXELFORMAT_FOURCC(surface->format)) {
        // We can't directly scale a YUV surface (yet!)
        SDL_Surface *tmp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ARGB8888);
        if (!tmp) {
            return NULL;
        }

        SDL_Surface *scaled = SDL_ScaleSurface(tmp, width, height, scaleMode);
        SDL_DestroySurface(tmp);
        if (!scaled) {
            return NULL;
        }

        SDL_Surface *result = SDL_ConvertSurfaceAndColorspace(scaled, surface->format, NULL, surface->colorspace, surface->props);
        SDL_DestroySurface(scaled);
        return result;
    }

    if (SDL_ISPIXELFORMAT_INDEXED(surface->format)) {
        // Linear scaling requires conversion to RGBA and then slow pixel color lookup
        scaleMode = SDL_SCALEMODE_NEAREST;
    }

    // Create a new surface with the desired size
    if (surface->pixels || SDL_MUSTLOCK(surface)) {
        convert = SDL_CreateSurfaceUninitialized(width, height, surface->format);
    } else {
        convert = SDL_CreateSurfaceFrom(width, height, surface->format, NULL, 0);
    }
    if (!convert) {
        goto error;
    }
    SDL_SetSurfacePalette(convert, surface->palette);
    SDL_SetSurfaceColorspace(convert, surface->colorspace);
    SDL_SetSurfaceRLE(convert, SDL_SurfaceHasRLE(surface));

    if (surface->pixels || SDL_MUSTLOCK(surface)) {
        // Save the original copy flags
        copy_flags = surface->map.info.flags;
        copy_color.r = surface->map.info.r;
        copy_color.g = surface->map.info.g;
        copy_color.b = surface->map.info.b;
        copy_color.a = surface->map.info.a;
        surface->map.info.r = 0xFF;
        surface->map.info.g = 0xFF;
        surface->map.info.b = 0xFF;
        surface->map.info.a = 0xFF;
        surface->map.info.flags = (copy_flags & (SDL_COPY_RLE_COLORKEY | SDL_COPY_RLE_ALPHAKEY));
        SDL_InvalidateMap(&surface->map);

        rc = SDL_BlitSurfaceScaled(surface, NULL, convert, NULL, scaleMode);

        // Clean up the original surface, and update converted surface
        convert->map.info.r = copy_color.r;
        convert->map.info.g = copy_color.g;
        convert->map.info.b = copy_color.b;
        convert->map.info.a = copy_color.a;
        convert->map.info.flags = (copy_flags & ~(SDL_COPY_RLE_COLORKEY | SDL_COPY_RLE_ALPHAKEY));
        surface->map.info.r = copy_color.r;
        surface->map.info.g = copy_color.g;
        surface->map.info.b = copy_color.b;
        surface->map.info.a = copy_color.a;
        surface->map.info.flags = copy_flags;
        SDL_InvalidateMap(&surface->map);
    } else {
        rc = true;
    }

    // SDL_BlitSurfaceScaled failed, and so the conversion
    if (!rc) {
        goto error;
    }

    // We're ready to go!
    return convert;

error:
    if (convert) {
        SDL_DestroySurface(convert);
    }
    return NULL;
}

SDL_Surface *SDL_DuplicatePixels(int width, int height, SDL_PixelFormat format, SDL_Colorspace colorspace, void *pixels, int pitch)
{
    SDL_Surface *surface;
    if (pixels) {
        surface = SDL_CreateSurfaceUninitialized(width, height, format);
    } else {
        surface = SDL_CreateSurfaceFrom(width, height, format, NULL, 0);
    }
    if (!surface) {
        return NULL;
    }
    SDL_SetSurfaceColorspace(surface, colorspace);

    if (surface->pixels) {
        int length = width * SDL_BYTESPERPIXEL(format);
        Uint8 *src = (Uint8 *)pixels;
        Uint8 *dst = (Uint8 *)surface->pixels;
        int rows = height;
        while (rows--) {
            SDL_memcpy(dst, src, length);
            dst += surface->pitch;
            src += pitch;
        }
    }
    return surface;
}

bool SDL_ConvertPixelsAndColorspace(int width, int height,
                      SDL_PixelFormat src_format, SDL_Colorspace src_colorspace, SDL_PropertiesID src_properties, const void *src, int src_pitch,
                      SDL_PixelFormat dst_format, SDL_Colorspace dst_colorspace, SDL_PropertiesID dst_properties, void *dst, int dst_pitch)
{
    SDL_Surface src_surface;
    SDL_Surface dst_surface;
    SDL_Rect rect;
    void *nonconst_src = (void *)src;
    bool result;

    CHECK_PARAM(!src) {
        return SDL_InvalidParamError("src");
    }
    CHECK_PARAM(!src_pitch) {
        return SDL_InvalidParamError("src_pitch");
    }
    CHECK_PARAM(!dst) {
        return SDL_InvalidParamError("dst");
    }
    CHECK_PARAM(!dst_pitch) {
        return SDL_InvalidParamError("dst_pitch");
    }

    if (src_colorspace == SDL_COLORSPACE_UNKNOWN) {
        src_colorspace = SDL_GetDefaultColorspaceForFormat(src_format);
    }
    if (dst_colorspace == SDL_COLORSPACE_UNKNOWN) {
        dst_colorspace = SDL_GetDefaultColorspaceForFormat(dst_format);
    }

    if (src_format == SDL_PIXELFORMAT_MJPG) {
        return SDL_ConvertPixels_STB(width, height, src_format, src_colorspace, src_properties, src, src_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
    }

#ifdef SDL_HAVE_YUV
    if (SDL_ISPIXELFORMAT_FOURCC(src_format) && SDL_ISPIXELFORMAT_FOURCC(dst_format)) {
        return SDL_ConvertPixels_YUV_to_YUV(width, height, src_format, src_colorspace, src_properties, src, src_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
    } else if (SDL_ISPIXELFORMAT_FOURCC(src_format)) {
        return SDL_ConvertPixels_YUV_to_RGB(width, height, src_format, src_colorspace, src_properties, src, src_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
    } else if (SDL_ISPIXELFORMAT_FOURCC(dst_format)) {
        return SDL_ConvertPixels_RGB_to_YUV(width, height, src_format, src_colorspace, src_properties, src, src_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
    }
#else
    if (SDL_ISPIXELFORMAT_FOURCC(src_format) || SDL_ISPIXELFORMAT_FOURCC(dst_format)) {
        return SDL_SetError("SDL not built with YUV support");
    }
#endif

    // Fast path for same format copy
    if (src_format == dst_format && src_colorspace == dst_colorspace) {
        if (src_pitch == dst_pitch) {
            SDL_memcpy(dst, src, height * src_pitch);
        } else {
            int i;
            const int bpp = SDL_BYTESPERPIXEL(src_format);
            width *= bpp;
            for (i = height; i--;) {
                SDL_memcpy(dst, src, width);
                src = (const Uint8 *)src + src_pitch;
                dst = (Uint8 *)dst + dst_pitch;
            }
        }
        return true;
    }

    if (!SDL_InitializeSurface(&src_surface, width, height, src_format, src_colorspace, src_properties, nonconst_src, src_pitch, true)) {
        return false;
    }
    SDL_SetSurfaceBlendMode(&src_surface, SDL_BLENDMODE_NONE);

    if (!SDL_InitializeSurface(&dst_surface, width, height, dst_format, dst_colorspace, dst_properties, dst, dst_pitch, true)) {
        return false;
    }

    // Set up the rect and go!
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    result = SDL_BlitSurfaceUnchecked(&src_surface, &rect, &dst_surface, &rect);

    SDL_DestroySurface(&src_surface);
    SDL_DestroySurface(&dst_surface);

    return result;
}

bool SDL_ConvertPixels(int width, int height, SDL_PixelFormat src_format, const void *src, int src_pitch, SDL_PixelFormat dst_format, void *dst, int dst_pitch)
{
    return SDL_ConvertPixelsAndColorspace(width, height,
                      src_format, SDL_COLORSPACE_UNKNOWN, 0, src, src_pitch,
                      dst_format, SDL_COLORSPACE_UNKNOWN, 0, dst, dst_pitch);
}

/*
 * Free a surface created by the above function.
 */
void SDL_DestroySurface(SDL_Surface *surface)
{
    if (!SDL_SurfaceValid(surface)) {
        return;
    }
    if (surface->internal_flags & SDL_INTERNAL_SURFACE_DONTFREE) {
        return;
    }
    if (--surface->refcount > 0) {
        return;
    }

    SDL_RemoveSurfaceAlternateImages(surface);

    SDL_DestroyProperties(surface->props);

    SDL_InvalidateMap(&surface->map);

    while (surface->locked > 0) {
        SDL_UnlockSurface(surface);
    }
#ifdef SDL_HAVE_RLE
    if (surface->internal_flags & SDL_INTERNAL_SURFACE_RLEACCEL) {
        SDL_UnRLESurface(surface);
    }
#endif
    SDL_SetSurfacePalette(surface, NULL);

    if (surface->flags & SDL_SURFACE_PREALLOCATED) {
        // Don't free
    } else if (surface->flags & SDL_SURFACE_SIMD_ALIGNED) {
        // Free aligned
        SDL_aligned_free(surface->pixels);
    } else {
        // Normal
        SDL_free(surface->pixels);
    }

    surface->reserved = NULL;

    if (!(surface->internal_flags & SDL_INTERNAL_SURFACE_STACK)) {
        SDL_free(surface);
    }
}

SDL_Surface *SDL_LoadSurface_IO(SDL_IOStream *src, bool closeio)
{
    CHECK_PARAM(!src) {
        SDL_InvalidParamError("src");
        return NULL;
    }

    if (SDL_IsBMP(src)) {
        return SDL_LoadBMP_IO(src, closeio);
    } else if (SDL_IsPNG(src)) {
        return SDL_LoadPNG_IO(src, closeio);
    } else if (SDL_IsJPG(src)) {
        return SDL_LoadJPG_IO(src, closeio);
    } else {
        if (closeio) {
            SDL_CloseIO(src);
        }
        SDL_SetError("Unsupported image format");
        return NULL;
    }
}

SDL_Surface *SDL_LoadSurface(const char *file)
{
    SDL_IOStream *stream = SDL_IOFromFile(file, "rb");
    if (!stream) {
        return NULL;
    }

    return SDL_LoadSurface_IO(stream, true);
}
