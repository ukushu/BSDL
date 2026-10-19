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
#include "../video/SDL_pixels_c.h"

void SDL_SetupRendererColorspace(SDL_Renderer *renderer, SDL_PropertiesID props)
{
#ifdef SDL_PLATFORM_VISIONOS
    // The RealityKit texture always renders in linear colorspace
    renderer->output_colorspace = SDL_COLORSPACE_SRGB_LINEAR;
#else
    renderer->output_colorspace = (SDL_Colorspace)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_CREATE_OUTPUT_COLORSPACE_NUMBER, SDL_COLORSPACE_SRGB);
#endif
    renderer->current_colorspace = renderer->output_colorspace;
}

void SDL_ConvertToLinear(SDL_Renderer *renderer, SDL_FColor *color)
{
    color->r = SDL_sRGBtoLinear(color->r);
    color->g = SDL_sRGBtoLinear(color->g);
    color->b = SDL_sRGBtoLinear(color->b);
    if (renderer->current_colorspace == SDL_COLORSPACE_HDR10) {
        SDL_ConvertColor709to2020(&color->r, &color->g, &color->b);
    }
}
