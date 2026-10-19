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
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#include "SDL_sysrender.h"
#include "SDL_render_internal.h"

void SDL_UpdateColorScale(SDL_Renderer *renderer)
{
    float SDR_white_point;
    if (renderer->target) {
        SDR_white_point = renderer->target->SDR_white_point;
    } else {
        SDR_white_point = renderer->SDR_white_point;
    }
    renderer->color_scale = renderer->desired_color_scale * SDR_white_point;
}

void SDL_UpdateHDRProperties(SDL_Renderer *renderer)
{
    SDL_PropertiesID window_props;
    SDL_PropertiesID renderer_props;

    window_props = SDL_GetWindowProperties(renderer->window);
    if (!window_props) {
        return;
    }

    renderer_props = SDL_GetRendererProperties(renderer);
    if (!renderer_props) {
        return;
    }

    if (renderer->output_colorspace == SDL_COLORSPACE_SRGB_LINEAR ||
        renderer->output_colorspace == SDL_COLORSPACE_HDR10) {
        renderer->SDR_white_point = SDL_GetFloatProperty(window_props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, 1.0f);
        renderer->HDR_headroom = SDL_GetFloatProperty(window_props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, 1.0f);
    } else {
        renderer->SDR_white_point = 1.0f;
        renderer->HDR_headroom = 1.0f;
    }

    if (renderer->HDR_headroom > 1.0f) {
        SDL_SetBooleanProperty(renderer_props, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, true);
    } else {
        SDL_SetBooleanProperty(renderer_props, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false);
    }
    SDL_SetFloatProperty(renderer_props, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, renderer->SDR_white_point);
    SDL_SetFloatProperty(renderer_props, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, renderer->HDR_headroom);

    SDL_UpdateColorScale(renderer);
}

bool SDL_SetRenderDrawColor(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    const float fR = (float)r / 255.0f;
    const float fG = (float)g / 255.0f;
    const float fB = (float)b / 255.0f;
    const float fA = (float)a / 255.0f;

    return SDL_SetRenderDrawColorFloat(renderer, fR, fG, fB, fA);
}

bool SDL_SetRenderDrawColorFloat(SDL_Renderer *renderer, float r, float g, float b, float a)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->color.r = r;
    renderer->color.g = g;
    renderer->color.b = b;
    renderer->color.a = a;
    return true;
}

bool SDL_GetRenderDrawColor(SDL_Renderer *renderer, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    float fR, fG, fB, fA;

    if (!SDL_GetRenderDrawColorFloat(renderer, &fR, &fG, &fB, &fA)) {
        if (r) {
            *r = 0;
        }
        if (g) {
            *g = 0;
        }
        if (b) {
            *b = 0;
        }
        if (a) {
            *a = 0;
        }
        return false;
    }

    if (r) {
        *r = (Uint8)(fR * 255.0f);
    }
    if (g) {
        *g = (Uint8)(fG * 255.0f);
    }
    if (b) {
        *b = (Uint8)(fB * 255.0f);
    }
    if (a) {
        *a = (Uint8)(fA * 255.0f);
    }
    return true;
}

bool SDL_GetRenderDrawColorFloat(SDL_Renderer *renderer, float *r, float *g, float *b, float *a)
{
    SDL_FColor color;

    if (r) {
        *r = 0.0f;
    }
    if (g) {
        *g = 0.0f;
    }
    if (b) {
        *b = 0.0f;
    }
    if (a) {
        *a = 0.0f;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    color = renderer->color;

    if (r) {
        *r = color.r;
    }
    if (g) {
        *g = color.g;
    }
    if (b) {
        *b = color.b;
    }
    if (a) {
        *a = color.a;
    }
    return true;
}

bool SDL_SetRenderColorScale(SDL_Renderer *renderer, float scale)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->desired_color_scale = scale;
    SDL_UpdateColorScale(renderer);
    return true;
}

bool SDL_GetRenderColorScale(SDL_Renderer *renderer, float *scale)
{
    if (scale) {
        *scale = 1.0f;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (scale) {
        *scale = renderer->desired_color_scale;
    }
    return true;
}

bool SDL_SetRenderDrawBlendMode(SDL_Renderer *renderer, SDL_BlendMode blendMode)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(blendMode == SDL_BLENDMODE_INVALID) {
        return SDL_InvalidParamError("blendMode");
    }

    if (!SDL_RendererSupportsBlendMode(renderer, blendMode)) {
        return SDL_Unsupported();
    }

    renderer->blendMode = blendMode;
    return true;
}

bool SDL_GetRenderDrawBlendMode(SDL_Renderer *renderer, SDL_BlendMode *blendMode)
{
    if (blendMode) {
        *blendMode = SDL_BLENDMODE_INVALID;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (blendMode) {
        *blendMode = renderer->blendMode;
    }
    return true;
}

