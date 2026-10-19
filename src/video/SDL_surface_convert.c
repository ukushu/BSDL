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

#include "SDL_pixels_c.h"
#include "SDL_surface_c.h"

static void SDL_ConvertColorkeyToAlpha(SDL_Surface *surface, bool ignore_alpha)
{
    int x, y, bpp;

    if (!SDL_SurfaceValid(surface)) {
        return;
    }

    if (!(surface->map.info.flags & SDL_COPY_COLORKEY) ||
        !SDL_ISPIXELFORMAT_ALPHA(surface->format)) {
        return;
    }

    bpp = SDL_BYTESPERPIXEL(surface->format);

    SDL_LockSurface(surface);

    if (bpp == 2) {
        Uint16 *row, *spot;
        Uint16 ckey = (Uint16)surface->map.info.colorkey;
        Uint16 mask = (Uint16)(~surface->fmt->Amask);

        // Ignore, or not, alpha in colorkey comparison
        if (ignore_alpha) {
            ckey &= mask;
            row = (Uint16 *)surface->pixels;
            for (y = surface->h; y--;) {
                spot = row;
                for (x = surface->w; x--;) {
                    if ((*spot & mask) == ckey) {
                        *spot &= mask;
                    }
                    ++spot;
                }
                row += surface->pitch / 2;
            }
        } else {
            row = (Uint16 *)surface->pixels;
            for (y = surface->h; y--;) {
                spot = row;
                for (x = surface->w; x--;) {
                    if (*spot == ckey) {
                        *spot &= mask;
                    }
                    ++spot;
                }
                row += surface->pitch / 2;
            }
        }
    } else if (bpp == 4) {
        Uint32 *row, *spot;
        Uint32 ckey = surface->map.info.colorkey;
        Uint32 mask = ~surface->fmt->Amask;

        // Ignore, or not, alpha in colorkey comparison
        if (ignore_alpha) {
            ckey &= mask;
            row = (Uint32 *)surface->pixels;
            for (y = surface->h; y--;) {
                spot = row;
                for (x = surface->w; x--;) {
                    if ((*spot & mask) == ckey) {
                        *spot &= mask;
                    }
                    ++spot;
                }
                row += surface->pitch / 4;
            }
        } else {
            row = (Uint32 *)surface->pixels;
            for (y = surface->h; y--;) {
                spot = row;
                for (x = surface->w; x--;) {
                    if (*spot == ckey) {
                        *spot &= mask;
                    }
                    ++spot;
                }
                row += surface->pitch / 4;
            }
        }
    }

    SDL_UnlockSurface(surface);

    SDL_SetSurfaceColorKey(surface, false, 0);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
}

static bool SDL_ConvertSurfaceColorKey(SDL_Surface *surface, SDL_Surface *convert, SDL_Palette *palette, SDL_Colorspace colorspace, SDL_PropertiesID props)
{
    bool set_colorkey_by_color = false;
    bool convert_colorkey = true;

    if (surface->palette) {
        if (palette &&
            surface->palette->ncolors <= palette->ncolors &&
            (SDL_memcmp(surface->palette->colors, palette->colors,
                        surface->palette->ncolors * sizeof(SDL_Color)) == 0)) {
            // The palette is identical, just set the same colorkey
            SDL_SetSurfaceColorKey(convert, true, surface->map.info.colorkey);
        } else if (!palette) {
            if (SDL_ISPIXELFORMAT_ALPHA(convert->format)) {
                // No need to add the colorkey, transparency is in the alpha channel
            } else {
                // Only set the colorkey information
                set_colorkey_by_color = true;
                convert_colorkey = false;
            }
        } else {
            set_colorkey_by_color = true;
        }
    } else {
        set_colorkey_by_color = true;
    }

    if (set_colorkey_by_color) {
        SDL_Surface *tmp;
        SDL_Surface *tmp2;
        int converted_colorkey = 0;

        // Create a dummy surface to get the colorkey converted
        tmp = SDL_CreateSurfaceUninitialized(1, 1, surface->format);
        if (!tmp) {
            return false;
        }

        // Share the palette, if any
        if (surface->palette) {
            SDL_SetSurfacePalette(tmp, surface->palette);
        }

        SDL_FillSurfaceRect(tmp, NULL, surface->map.info.colorkey);

        tmp->map.info.flags &= ~SDL_COPY_COLORKEY;

        // Conversion of the colorkey
        tmp2 = SDL_ConvertSurfaceAndColorspace(tmp, convert->format, palette, colorspace, props);
        if (!tmp2) {
            SDL_DestroySurface(tmp);
            return false;
        }

        // Get the converted colorkey
        SDL_memcpy(&converted_colorkey, tmp2->pixels, tmp2->fmt->bytes_per_pixel);

        SDL_DestroySurface(tmp);
        SDL_DestroySurface(tmp2);

        // Set the converted colorkey on the new surface
        SDL_SetSurfaceColorKey(convert, true, converted_colorkey);

        // This is needed when converting for 3D texture upload
        if (convert_colorkey) {
            SDL_ConvertColorkeyToAlpha(convert, true);
        }
    }
    return true;
}

static bool SDL_CopyConvertedSurfaceMetadata(SDL_Surface *surface, SDL_Surface *convert, Uint32 copy_flags)
{
    SDL_SetSurfaceClipRect(convert, &surface->clip_rect);

    /* Enable alpha blending by default if the new surface has an
     * alpha channel or alpha modulation */
    if (SDL_ISPIXELFORMAT_ALPHA(convert->format) ||
        (copy_flags & SDL_COPY_MODULATE_ALPHA)) {
        SDL_SetSurfaceBlendMode(convert, SDL_BLENDMODE_BLEND);
    }
    if (copy_flags & SDL_COPY_RLE_DESIRED) {
        SDL_SetSurfaceRLE(convert, true);
    }

    // Copy alternate images
    for (int i = 0; i < surface->num_images; ++i) {
        if (!SDL_AddSurfaceAlternateImage(convert, surface->images[i])) {
            return false;
        }
    }

    // Copy properties
    if (surface->props) {
        if (!SDL_CopyProperties(surface->props, SDL_GetSurfaceProperties(convert))) {
            return false;
        }

        // Make sure the new surface doesn't reference an old SDL2 surface.
        SDL_ClearProperty(SDL_GetSurfaceProperties(convert), "sdl2-compat.surface2");
    }
    return true;
}

static bool SDL_BlitSurfaceForConversion(SDL_Surface *surface, const SDL_Rect *rect, SDL_Surface *convert, SDL_Palette *palette, SDL_Colorspace colorspace, SDL_PropertiesID props, Uint32 copy_flags)
{
    SDL_Color copy_color;
    SDL_Rect bounds = { 0, 0, rect->w, rect->h };
    bool result;
    bool palette_ck_transform = false;
    Uint8 palette_ck_value = 0;
    Uint8 *palette_saved_alpha = NULL;
    int palette_saved_alpha_ncolors = 0;

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

    /* Source surface has a palette with no real alpha (0 or OPAQUE).
     * Destination format has alpha.
     * -> set alpha channel to be opaque */
    if (surface->palette && SDL_ISPIXELFORMAT_ALPHA(convert->format)) {
        bool set_opaque = false;

        bool is_opaque, has_alpha_channel;
        SDL_DetectPalette(surface->palette, &is_opaque, &has_alpha_channel);

        if (is_opaque) {
            if (!has_alpha_channel) {
                set_opaque = true;
            }
        }

        // Set opaque and backup palette alpha values
        if (set_opaque) {
            int i;
            palette_saved_alpha_ncolors = surface->palette->ncolors;
            if (palette_saved_alpha_ncolors > 0) {
                palette_saved_alpha = SDL_stack_alloc(Uint8, palette_saved_alpha_ncolors);
                for (i = 0; i < palette_saved_alpha_ncolors; i++) {
                    palette_saved_alpha[i] = surface->palette->colors[i].a;
                    surface->palette->colors[i].a = SDL_ALPHA_OPAQUE;
                }
            }
        }
    }

    // Transform colorkey to alpha. for cases where source palette has duplicate values, and colorkey is one of them
    if (copy_flags & SDL_COPY_COLORKEY) {
        if (surface->palette && !palette) {
            palette_ck_transform = true;
            palette_ck_value = surface->palette->colors[surface->map.info.colorkey].a;
            surface->palette->colors[surface->map.info.colorkey].a = SDL_ALPHA_TRANSPARENT;
        }
    }

    if (surface->pixels || SDL_MUSTLOCK(surface)) {
        result = SDL_BlitSurfaceUnchecked(surface, rect, convert, &bounds);
    } else {
        result = true;
    }

    // Restore colorkey alpha value
    if (palette_ck_transform) {
        surface->palette->colors[surface->map.info.colorkey].a = palette_ck_value;
    }

    // Restore palette alpha values
    if (palette_saved_alpha) {
        int i;
        for (i = 0; i < palette_saved_alpha_ncolors; i++) {
            surface->palette->colors[i].a = palette_saved_alpha[i];
        }
        SDL_stack_free(palette_saved_alpha);
    }

    // Clean up the original surface, and update converted surface
    convert->map.info.r = copy_color.r;
    convert->map.info.g = copy_color.g;
    convert->map.info.b = copy_color.b;
    convert->map.info.a = copy_color.a;
    convert->map.info.flags =
        (copy_flags &
         ~(SDL_COPY_COLORKEY | SDL_COPY_BLEND | SDL_COPY_RLE_DESIRED | SDL_COPY_RLE_COLORKEY |
           SDL_COPY_RLE_ALPHAKEY));
    surface->map.info.r = copy_color.r;
    surface->map.info.g = copy_color.g;
    surface->map.info.b = copy_color.b;
    surface->map.info.a = copy_color.a;
    surface->map.info.flags = copy_flags;
    SDL_InvalidateMap(&surface->map);

    if (!result) {
        return false;
    }

    if (copy_flags & SDL_COPY_COLORKEY) {
        if (!SDL_ConvertSurfaceColorKey(surface, convert, palette, colorspace, props)) {
            return false;
        }
    }
    return true;
}

static SDL_Surface *SDL_ConvertSurfaceRectAndColorspace(SDL_Surface *surface, const SDL_Rect *rect, SDL_PixelFormat format, SDL_Palette *palette, SDL_Colorspace colorspace, SDL_PropertiesID props)
{
    SDL_Palette *temp_palette = NULL;
    SDL_Surface *convert = NULL;
    SDL_Colorspace src_colorspace;
    SDL_PropertiesID src_properties;
    Uint32 copy_flags;
    SDL_Rect bounds;

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        goto error;
    }

    CHECK_PARAM(format == SDL_PIXELFORMAT_UNKNOWN) {
        SDL_InvalidParamError("format");
        goto error;
    }

    // Set the bounds of the new surface
    bounds.x = 0;
    bounds.y = 0;
    bounds.w = surface->w;
    bounds.h = surface->h;
    if (rect) {
        bounds.w = rect->w;
        bounds.h = rect->h;
    } else {
        rect = &bounds;
    }

    // Check for empty destination palette! (results in empty image)
    if (palette) {
        int i;
        for (i = 0; i < palette->ncolors; ++i) {
            if ((palette->colors[i].r != 0xFF) || (palette->colors[i].g != 0xFF) || (palette->colors[i].b != 0xFF)) {
                break;
            }
        }
        if (i == palette->ncolors) {
            SDL_SetError("Empty destination palette");
            goto error;
        }
    } else if (SDL_ISPIXELFORMAT_INDEXED(format)) {
        // Create a dither palette for conversion
        temp_palette = SDL_CreatePalette(1 << SDL_BITSPERPIXEL(format));
        if (temp_palette) {
            SDL_DitherPalette(temp_palette);
            palette = temp_palette;
        }
    }

    src_colorspace = surface->colorspace;
    src_properties = surface->props;

    // Create a new surface with the desired format
    if (surface->pixels || SDL_MUSTLOCK(surface)) {
        convert = SDL_CreateSurfaceUninitialized(rect->w, rect->h, format);
    } else {
        convert = SDL_CreateSurfaceFrom(rect->w, rect->h, format, NULL, 0);
    }
    if (!convert) {
        goto error;
    }
    if (SDL_ISPIXELFORMAT_INDEXED(format)) {
        SDL_SetSurfacePalette(convert, palette);
    }

    if (colorspace == SDL_COLORSPACE_UNKNOWN) {
        colorspace = src_colorspace;
    }
    SDL_SetSurfaceColorspace(convert, colorspace);

    if (SDL_ISPIXELFORMAT_FOURCC(format) || SDL_ISPIXELFORMAT_FOURCC(surface->format)) {
        if (surface->format == SDL_PIXELFORMAT_MJPG && format == SDL_PIXELFORMAT_MJPG) {
            // Just do a straight pixel copy of the JPEG image
            size_t size = (size_t)surface->pitch;
            convert->pixels = SDL_malloc(size);
            if (!convert->pixels) {
                goto error;
            }
            convert->flags &= ~SDL_SURFACE_PREALLOCATED;
            convert->pitch = surface->pitch;
            SDL_memcpy(convert->pixels, surface->pixels, size);

        } else if (!SDL_ConvertPixelsAndColorspace(surface->w, surface->h, surface->format, src_colorspace, src_properties, surface->pixels, surface->pitch, convert->format, colorspace, props, convert->pixels, convert->pitch)) {
            goto error;
        }

        // Save the original copy flags
        copy_flags = surface->map.info.flags;

        goto end;
    }

    // Save the original copy flags
    copy_flags = surface->map.info.flags;
    if (!SDL_BlitSurfaceForConversion(surface, rect, convert, palette, colorspace, props, copy_flags)) {
        goto error;
    }

end:
    if (temp_palette) {
        SDL_DestroyPalette(temp_palette);
    }

    if (!SDL_CopyConvertedSurfaceMetadata(surface, convert, copy_flags)) {
        goto error;
    }

    // We're ready to go!
    return convert;

error:
    if (temp_palette) {
        SDL_DestroyPalette(temp_palette);
    }
    if (convert) {
        SDL_DestroySurface(convert);
    }
    return NULL;
}

SDL_Surface *SDL_ConvertSurfaceAndColorspace(SDL_Surface *surface, SDL_PixelFormat format, SDL_Palette *palette, SDL_Colorspace colorspace, SDL_PropertiesID props)
{
    return SDL_ConvertSurfaceRectAndColorspace(surface, NULL, format, palette, colorspace, props);
}

SDL_Surface *SDL_ConvertSurfaceRect(SDL_Surface *surface, const SDL_Rect *rect, SDL_PixelFormat format)
{
    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    return SDL_ConvertSurfaceRectAndColorspace(surface, rect, format, NULL, SDL_GetDefaultColorspaceForFormat(format), surface->props);
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *surface, SDL_PixelFormat format)
{
    return SDL_ConvertSurfaceRect(surface, NULL, format);
}
