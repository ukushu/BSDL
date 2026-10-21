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
#include "../video/SDL_pixels_c.h"
#include "../video/SDL_video_c.h"

static bool IsSupportedFormat(SDL_Renderer *renderer, SDL_PixelFormat format)
{
    int i;

    for (i = 0; i < renderer->num_texture_formats; ++i) {
        if (renderer->texture_formats[i] == format) {
            return true;
        }
    }
    return false;
}

static SDL_PixelFormat GetClosestSupportedFormat(SDL_Renderer *renderer, SDL_PixelFormat format, bool hasAlpha)
{
    int i;

    if (format == SDL_PIXELFORMAT_MJPG) {
        // We'll decode to SDL_PIXELFORMAT_NV12 or SDL_PIXELFORMAT_RGBA32
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (renderer->texture_formats[i] == SDL_PIXELFORMAT_NV12) {
                return renderer->texture_formats[i];
            }
        }
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (renderer->texture_formats[i] == SDL_PIXELFORMAT_RGBA32) {
                return renderer->texture_formats[i];
            }
        }
    } else if (SDL_ISPIXELFORMAT_FOURCC(format)) {
        // Look for an exact match
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (renderer->texture_formats[i] == format) {
                return renderer->texture_formats[i];
            }
        }
    } else if (SDL_ISPIXELFORMAT_10BIT(format) || SDL_ISPIXELFORMAT_FLOAT(format)) {
        if (SDL_ISPIXELFORMAT_10BIT(format)) {
            for (i = 0; i < renderer->num_texture_formats; ++i) {
                if (SDL_ISPIXELFORMAT_10BIT(renderer->texture_formats[i])) {
                    return renderer->texture_formats[i];
                }
            }
        }
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (SDL_ISPIXELFORMAT_FLOAT(renderer->texture_formats[i])) {
                return renderer->texture_formats[i];
            }
        }
    } else if (SDL_ISPIXELFORMAT_INDEXED(format)) {
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            // Converting between <8bpp formats is not supported yet
            if (renderer->texture_formats[i] == SDL_PIXELFORMAT_INDEX8) {
                return renderer->texture_formats[i];
            }
        }
    } else {
        int type = SDL_PIXELTYPE(format);
        int layout = SDL_PIXELLAYOUT(format);
        hasAlpha = hasAlpha || SDL_ISPIXELFORMAT_ALPHA(format);

        // We just want to match the first format that has the same channels
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (!SDL_ISPIXELFORMAT_FOURCC(renderer->texture_formats[i]) &&
                SDL_PIXELTYPE(renderer->texture_formats[i]) == type &&
                SDL_PIXELLAYOUT(renderer->texture_formats[i]) == layout &&
                SDL_ISPIXELFORMAT_ALPHA(renderer->texture_formats[i]) == hasAlpha) {
                return renderer->texture_formats[i];
            }
        }
    }

    if (hasAlpha) {
        // The default format may still lack an alpha channel
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (!SDL_ISPIXELFORMAT_FOURCC(renderer->texture_formats[i]) &&
                SDL_ISPIXELFORMAT_ALPHA(renderer->texture_formats[i])) {
                return renderer->texture_formats[i];
            }
        }
    }
    return renderer->texture_formats[0];
}

static bool SDL_CreateFallbackNativeTexture(SDL_Texture *texture)
{
    SDL_Renderer *renderer = texture->renderer;
    SDL_PixelFormat format = texture->format;
    SDL_TextureAccess access = texture->access;
    bool texture_is_fourcc_and_target = (access == SDL_TEXTUREACCESS_TARGET && SDL_ISPIXELFORMAT_FOURCC(format));
    SDL_PixelFormat closest_format = texture_is_fourcc_and_target ? renderer->texture_formats[0] : GetClosestSupportedFormat(renderer, format, false);
    SDL_PropertiesID native_props = SDL_CreateProperties();

    if (!native_props) {
        return false;
    }

    if (format == SDL_PIXELFORMAT_MJPG && closest_format == SDL_PIXELFORMAT_NV12) {
        if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, SDL_COLORSPACE_JPEG)) {
            goto failure;
        }
    } else {
        const SDL_Colorspace default_colorspace = SDL_GetDefaultColorspaceForFormat(closest_format);
        if (SDL_COLORSPACETYPE(texture->colorspace) == SDL_COLORSPACETYPE(default_colorspace) &&
            SDL_COLORSPACETRANSFER(texture->colorspace) == SDL_COLORSPACETRANSFER(default_colorspace)) {
            if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, texture->colorspace)) {
                goto failure;
            }
        } else {
            if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, default_colorspace)) {
                goto failure;
            }
        }
    }
    if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, closest_format)) {
        goto failure;
    }
    if (SDL_ISPIXELFORMAT_INDEXED(format)) {
        // We're going to be uploading pixels frequently as the palette changes
        if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STREAMING)) {
            goto failure;
        }
    } else {
        if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, access)) {
            goto failure;
        }
    }
    if (!SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, texture->w) ||
        !SDL_SetNumberProperty(native_props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, texture->h)) {
        goto failure;
    }

    texture->native = SDL_CreateTextureWithProperties(renderer, native_props);
    SDL_DestroyProperties(native_props);
    return texture->native != NULL;

failure:
    SDL_DestroyProperties(native_props);
    return false;
}

static bool SDL_AllocateFallbackTextureStorage(SDL_Texture *texture)
{
    SDL_PixelFormat format = texture->format;
    SDL_TextureAccess access = texture->access;

    if (format == SDL_PIXELFORMAT_MJPG) {
        // We have a custom decode + upload path for this
    } else if (SDL_ISPIXELFORMAT_FOURCC(format)) {
#ifdef SDL_HAVE_YUV
        texture->yuv = SDL_SW_CreateYUVTexture(format, texture->colorspace, texture->w, texture->h);
#else
        SDL_SetError("SDL not built with YUV support");
#endif
        if (!texture->yuv) {
            return false;
        }
    } else if (SDL_ISPIXELFORMAT_INDEXED(format)) {
        texture->palette_surface = SDL_CreateSurfaceZeroed(texture->w, texture->h, format);
        if (!texture->palette_surface) {
            return false;
        }
    } else if (access == SDL_TEXTUREACCESS_STREAMING) {
        // The pitch is 4 byte aligned
        texture->pitch = (((texture->w * SDL_BYTESPERPIXEL(format)) + 3) & ~3);
        texture->pixels = SDL_calloc(1, (size_t)texture->pitch * texture->h);
        if (!texture->pixels) {
            return false;
        }
    }
    return true;
}

static bool SDL_CreateTextureFallback(SDL_Texture *texture)
{
    SDL_Renderer *renderer = texture->renderer;

    if (!SDL_CreateFallbackNativeTexture(texture)) {
        return false;
    }

    SDL_SetPointerProperty(SDL_GetTextureProperties(texture->native), SDL_PROP_TEXTURE_PARENT_POINTER, texture);

    // Swap textures to have texture before texture->native in the list
    texture->native->next = texture->next;
    if (texture->native->next) {
        texture->native->next->prev = texture->native;
    }
    texture->prev = texture->native->prev;
    if (texture->prev) {
        texture->prev->next = texture;
    }
    texture->native->prev = texture;
    texture->next = texture->native;
    renderer->textures = texture;

    SDL_SetTextureScaleMode(texture->native, texture->scaleMode);

    return SDL_AllocateFallbackTextureStorage(texture);
}

static bool SDL_ValidateTextureCreateParams(SDL_Renderer *renderer, SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
{
    CHECK_PARAM(SDL_BYTESPERPIXEL(format) == 0) {
        SDL_SetError("Invalid texture format");
        return false;
    }
    CHECK_PARAM(SDL_ISPIXELFORMAT_INDEXED(format) && access == SDL_TEXTUREACCESS_TARGET) {
        SDL_SetError("Palettized textures can't be render targets");
        return false;
    }
    CHECK_PARAM(SDL_ISPIXELFORMAT_FOURCC(format) && access == SDL_TEXTUREACCESS_TARGET) {
        SDL_SetError("%s textures can't be render targets", SDL_GetPixelFormatName(format));
        return false;
    }
    CHECK_PARAM(w <= 0 || h <= 0) {
        SDL_SetError("Texture dimensions can't be 0");
        return false;
    }
    int max_texture_size = (int)SDL_GetNumberProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
    CHECK_PARAM(max_texture_size && (w > max_texture_size || h > max_texture_size)) {
        SDL_SetError("Texture dimensions are limited to %dx%d", max_texture_size, max_texture_size);
        return false;
    }
    return true;
}

static void SDL_InitializeTextureState(SDL_Texture *texture, SDL_Renderer *renderer, SDL_PropertiesID props,
                                       SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
{
    SDL_Colorspace default_colorspace = SDL_GetDefaultColorspaceForFormat(format);

    texture->refcount = 1;
    SDL_SetObjectValid(texture, SDL_OBJECT_TYPE_TEXTURE, true);
    texture->colorspace = (SDL_Colorspace)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, default_colorspace);
    texture->format = format;
    texture->access = access;
    texture->w = w;
    texture->h = h;
    texture->color.r = 1.0f;
    texture->color.g = 1.0f;
    texture->color.b = 1.0f;
    texture->color.a = 1.0f;
    texture->blendMode = SDL_ISPIXELFORMAT_ALPHA(format) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE;
    texture->scaleMode = renderer->scale_mode;
    texture->view.pixel_w = w;
    texture->view.pixel_h = h;
    texture->view.viewport.w = -1.0f;
    texture->view.viewport.h = -1.0f;
    texture->view.scale.x = 1.0f;
    texture->view.scale.y = 1.0f;
    texture->view.logical_scale.x = 1.0f;
    texture->view.logical_scale.y = 1.0f;
    texture->view.current_scale.x = 1.0f;
    texture->view.current_scale.y = 1.0f;
    texture->renderer = renderer;
    texture->next = renderer->textures;
    if (renderer->textures) {
        renderer->textures->prev = texture;
    }
    renderer->textures = texture;

    SDL_UpdatePixelViewport(renderer, &texture->view);
    SDL_UpdatePixelClipRect(renderer, &texture->view);

    texture->SDR_white_point = SDL_GetFloatProperty(props, SDL_PROP_TEXTURE_CREATE_SDR_WHITE_POINT_FLOAT, SDL_GetDefaultSDRWhitePoint(texture->colorspace));
    texture->HDR_headroom = SDL_GetFloatProperty(props, SDL_PROP_TEXTURE_CREATE_HDR_HEADROOM_FLOAT, SDL_GetDefaultHDRHeadroom(texture->colorspace));
}

SDL_Texture *SDL_CreateTextureWithProperties(SDL_Renderer *renderer, SDL_PropertiesID props)
{
    SDL_Texture *texture;
    SDL_PixelFormat format = (SDL_PixelFormat)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_UNKNOWN);
    SDL_TextureAccess access = (SDL_TextureAccess)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STATIC);
    int w = (int)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, 0);
    int h = (int)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, 0);
    SDL_Palette *palette = (SDL_Palette *)SDL_GetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, NULL);

    CHECK_RENDERER_MAGIC(renderer, NULL);

    if (!format) {
        format = renderer->texture_formats[0];
    }

    if (!SDL_ValidateTextureCreateParams(renderer, format, access, w, h)) {
        return NULL;
    }

    texture = (SDL_Texture *)SDL_calloc(1, sizeof(*texture));
    if (!texture) {
        return NULL;
    }
    SDL_InitializeTextureState(texture, renderer, props, format, access, w, h);

    if (IsSupportedFormat(renderer, format)) {
        if (!renderer->CreateTexture(renderer, texture, props)) {
            SDL_DestroyTexture(texture);
            return NULL;
        }
    } else {
        if (!SDL_CreateTextureFallback(texture)) {
            SDL_DestroyTexture(texture);
            return NULL;
        }
    }

    if (SDL_ISPIXELFORMAT_INDEXED(texture->format) && palette) {
        if (!SDL_SetTexturePalette(texture, palette)) {
            SDL_DestroyTexture(texture);
            return NULL;
        }
    }

    // Now set the properties for the new texture
    props = SDL_GetTextureProperties(texture);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_COLORSPACE_NUMBER, texture->colorspace);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER, texture->format);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, texture->access);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, texture->w);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, texture->h);
    SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT, texture->SDR_white_point);
    if (texture->HDR_headroom > 0.0f) {
        SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT, texture->HDR_headroom);
    }
    return texture;
}

SDL_Texture *SDL_CreateTexture(SDL_Renderer *renderer, SDL_PixelFormat format, SDL_TextureAccess access, int w, int h)
{
    SDL_Texture *texture;
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, format);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, access);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, w);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, h);
    texture = SDL_CreateTextureWithProperties(renderer, props);
    SDL_DestroyProperties(props);
    return texture;
}

static bool SDL_UpdateTextureFromSurface(SDL_Texture *texture, SDL_Rect *rect, SDL_Surface *surface)
{
    bool direct_update;
    bool result;
    if (surface->format == texture->format &&
        SDL_GetSurfaceColorspace(surface) == texture->colorspace) {
        if (SDL_ISPIXELFORMAT_INDEXED(surface->format)) {
            // Update Texture directly - the color key is handled later
            direct_update = true;
        } else if (SDL_ISPIXELFORMAT_ALPHA(surface->format) && SDL_SurfaceHasColorKey(surface)) {
            /* Surface and Renderer formats are identical.
             * Intermediate conversion is needed to convert color key to alpha (SDL_ConvertColorkeyToAlpha()). */
            direct_update = false;
        } else {
            // Update Texture directly
            direct_update = true;
        }
    } else {
        // Surface and Renderer formats are different, it needs an intermediate conversion.
        direct_update = false;
    }

    if (direct_update) {
        if (SDL_MUSTLOCK(surface)) {
            if (SDL_LockSurface(surface)) {
                result = SDL_UpdateTexture(texture, rect, surface->pixels, surface->pitch);
                SDL_UnlockSurface(surface);
                if (!result) {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            if (!SDL_UpdateTexture(texture, rect, surface->pixels, surface->pitch)) {
                return false;
            }
        }
    } else {
        // Set up a destination surface for the texture update
        SDL_Surface *temp = SDL_ConvertSurfaceAndColorspace(surface, texture->format, texture->public_palette, texture->colorspace, SDL_GetSurfaceProperties(surface));
        if (temp) {
            result = SDL_UpdateTexture(texture, NULL, temp->pixels, temp->pitch);
            SDL_DestroySurface(temp);
            if (!result) {
                return false;
            }
        } else {
            return false;
        }
    }

    if (SDL_ISPIXELFORMAT_INDEXED(texture->format) && SDL_SurfaceHasColorKey(surface)) {
        Uint32 key;
        SDL_Color col;
        SDL_Palette *palette;

        palette = SDL_GetTexturePalette(texture);
        if (!palette || !SDL_GetSurfaceColorKey(surface, &key) || key >= (Uint32)palette->ncolors) {
            return false;
        }

        col = palette->colors[key];
        col.a = SDL_ALPHA_TRANSPARENT;
        SDL_SetPaletteColors(palette, &col, key, 1);
    }

    {
        Uint8 r, g, b, a;
        SDL_BlendMode blendMode;

        SDL_GetSurfaceColorMod(surface, &r, &g, &b);
        SDL_SetTextureColorMod(texture, r, g, b);

        SDL_GetSurfaceAlphaMod(surface, &a);
        SDL_SetTextureAlphaMod(texture, a);

        if (SDL_SurfaceHasColorKey(surface)) {
            // We converted to a texture with alpha format
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        } else {
            SDL_GetSurfaceBlendMode(surface, &blendMode);
            SDL_SetTextureBlendMode(texture, blendMode);
        }
    }

    return true;
}

static SDL_PixelFormat SelectTextureFormatForSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    int i;
    SDL_PixelFormat format = SDL_PIXELFORMAT_UNKNOWN;

    // Try to have the best pixel format for the texture
    // No alpha, but a colorkey => promote to alpha
    if (!SDL_ISPIXELFORMAT_ALPHA(surface->format) && SDL_SurfaceHasColorKey(surface)) {
        if (surface->format == SDL_PIXELFORMAT_XRGB8888) {
            for (i = 0; i < renderer->num_texture_formats; ++i) {
                if (renderer->texture_formats[i] == SDL_PIXELFORMAT_ARGB8888) {
                    format = SDL_PIXELFORMAT_ARGB8888;
                    break;
                }
            }
        } else if (surface->format == SDL_PIXELFORMAT_XBGR8888) {
            for (i = 0; i < renderer->num_texture_formats; ++i) {
                if (renderer->texture_formats[i] == SDL_PIXELFORMAT_ABGR8888) {
                    format = SDL_PIXELFORMAT_ABGR8888;
                    break;
                }
            }
        } else if (surface->format == SDL_PIXELFORMAT_XRGB4444) {
            for (i = 0; i < renderer->num_texture_formats; ++i) {
                if (renderer->texture_formats[i] == SDL_PIXELFORMAT_ARGB4444) {
                    format = SDL_PIXELFORMAT_ARGB4444;
                    break;
                }
            }
        }
    } else {
        // Exact match would be fine
        for (i = 0; i < renderer->num_texture_formats; ++i) {
            if (renderer->texture_formats[i] == surface->format) {
                format = surface->format;
                break;
            }
        }
    }

    // Fallback, choose a valid pixel format
    if (format == SDL_PIXELFORMAT_UNKNOWN) {
        bool needAlpha = SDL_SurfaceHasColorKey(surface);

        // If palette contains alpha values, promotes to alpha format
        if (surface->palette) {
            bool is_opaque, has_alpha_channel;
            SDL_DetectPalette(surface->palette, &is_opaque, &has_alpha_channel);
            if (!is_opaque) {
                needAlpha = true;
            }
        }

        format = GetClosestSupportedFormat(renderer, surface->format, needAlpha);
    }

    return format;
}

SDL_Texture *SDL_CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_PixelFormat format;
    SDL_Texture *texture;
    SDL_PropertiesID props;
    SDL_Colorspace surface_colorspace = SDL_COLORSPACE_UNKNOWN;
    SDL_Colorspace texture_colorspace = SDL_COLORSPACE_UNKNOWN;

    CHECK_RENDERER_MAGIC(renderer, NULL);

    CHECK_PARAM(!SDL_SurfaceValid(surface)) {
        SDL_InvalidParamError("SDL_CreateTextureFromSurface(): surface");
        return NULL;
    }

    format = SelectTextureFormatForSurface(renderer, surface);

    surface_colorspace = SDL_GetSurfaceColorspace(surface);
    texture_colorspace = surface_colorspace;

    if (surface_colorspace == SDL_COLORSPACE_SRGB_LINEAR ||
        SDL_COLORSPACETRANSFER(surface_colorspace) == SDL_TRANSFER_CHARACTERISTICS_PQ ||
        (SDL_ISPIXELFORMAT_FOURCC(surface->format) && !SDL_ISPIXELFORMAT_FOURCC(format))) {
        texture_colorspace = SDL_GetDefaultColorspaceForFormat(format);
    }

    props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, texture_colorspace);
    if (surface_colorspace == texture_colorspace) {
        SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_CREATE_SDR_WHITE_POINT_FLOAT,
                             SDL_GetSurfaceSDRWhitePoint(surface, surface_colorspace));
    }
    SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_CREATE_HDR_HEADROOM_FLOAT,
                         SDL_GetSurfaceHDRHeadroom(surface, surface_colorspace));
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, format);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STATIC);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, surface->w);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, surface->h);
    SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, surface->palette);

    texture = SDL_CreateTextureWithProperties(renderer, props);
    SDL_DestroyProperties(props);
    if (!texture) {
        return NULL;
    }

    if (!SDL_UpdateTextureFromSurface(texture, NULL, surface)) {
        SDL_DestroyTexture(texture);
        return NULL;
    }

    return texture;
}

SDL_Renderer *SDL_GetRendererFromTexture(SDL_Texture *texture)
{
    CHECK_TEXTURE_MAGIC(texture, NULL);

    return texture->renderer;
}
