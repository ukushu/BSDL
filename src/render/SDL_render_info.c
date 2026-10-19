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

SDL_Renderer *SDL_GetRenderer(SDL_Window *window)
{
    return (SDL_Renderer *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_RENDERER_POINTER, NULL);
}

SDL_Window *SDL_GetRenderWindow(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);
    return renderer->window;
}

const char *SDL_GetRendererName(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    return SDL_GetPersistentString(renderer->name);
}

SDL_PropertiesID SDL_GetRendererProperties(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, 0);

    if (renderer->props == 0) {
        renderer->props = SDL_CreateProperties();
    }
    return renderer->props;
}

bool SDL_GetRenderOutputSize(SDL_Renderer *renderer, int *w, int *h)
{
    if (w) {
        *w = 0;
    }
    if (h) {
        *h = 0;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (renderer->GetOutputSize) {
        return renderer->GetOutputSize(renderer, w, h);
    } else if (renderer->window) {
        return SDL_GetWindowSizeInPixels(renderer->window, w, h);
    } else {
        // We don't have any output size, this might be an offscreen-only renderer
        return true;
    }
}

bool SDL_GetCurrentRenderOutputSize(SDL_Renderer *renderer, int *w, int *h)
{
    if (w) {
        *w = 0;
    }
    if (h) {
        *h = 0;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    const SDL_RenderViewState *view = renderer->view;
    if (w) {
        *w = view->pixel_w;
    }
    if (h) {
        *h = view->pixel_h;
    }
    return true;
}

