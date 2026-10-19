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
#include "../video/SDL_video_c.h"

static bool SDL_RenderVectorFromWindow(SDL_Renderer *renderer, float window_dx, float window_dy, float *dx, float *dy)
{
    // Convert from window coordinates to pixels within the window
    window_dx *= renderer->dpi_scale.x;
    window_dy *= renderer->dpi_scale.y;

    // Convert from pixels within the window to pixels within the view
    const SDL_RenderViewState *view = &renderer->main_view;
    if (view->logical_presentation_mode != SDL_LOGICAL_PRESENTATION_DISABLED) {
        const SDL_FRect *src = &view->logical_src_rect;
        const SDL_FRect *dst = &view->logical_dst_rect;
        window_dx = (window_dx * src->w) / dst->w;
        window_dy = (window_dy * src->h) / dst->h;
    }

    window_dx /= view->scale.x;
    window_dy /= view->scale.y;

    *dx = window_dx;
    *dy = window_dy;
    return true;
}

bool SDL_RenderCoordinatesFromWindow(SDL_Renderer *renderer, float window_x, float window_y, float *x, float *y)
{
    float render_x, render_y;

    CHECK_RENDERER_MAGIC(renderer, false);

    // Convert from window coordinates to pixels within the window
    render_x = window_x * renderer->dpi_scale.x;
    render_y = window_y * renderer->dpi_scale.y;

    // Convert from pixels within the window to pixels within the view
    const SDL_RenderViewState *view = &renderer->main_view;
    if (view->logical_presentation_mode != SDL_LOGICAL_PRESENTATION_DISABLED) {
        const SDL_FRect *src = &view->logical_src_rect;
        const SDL_FRect *dst = &view->logical_dst_rect;
        render_x = ((render_x - dst->x) * src->w) / dst->w;
        render_y = ((render_y - dst->y) * src->h) / dst->h;
    }

    render_x = (render_x / view->scale.x) - view->viewport.x;
    render_y = (render_y / view->scale.y) - view->viewport.y;

    if (x) {
        *x = render_x;
    }
    if (y) {
        *y = render_y;
    }
    return true;
}

bool SDL_RenderCoordinatesToWindow(SDL_Renderer *renderer, float x, float y, float *window_x, float *window_y)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    const SDL_RenderViewState *view = &renderer->main_view;
    x = (view->viewport.x + x) * view->scale.x;
    y = (view->viewport.y + y) * view->scale.y;

    // Convert from render coordinates to pixels within the window
    if (view->logical_presentation_mode != SDL_LOGICAL_PRESENTATION_DISABLED) {
        const SDL_FRect *src = &view->logical_src_rect;
        const SDL_FRect *dst = &view->logical_dst_rect;
        x = dst->x + ((x * dst->w) / src->w);
        y = dst->y + ((y * dst->h) / src->h);
    }

    // Convert from pixels within the window to window coordinates
    x /= renderer->dpi_scale.x;
    y /= renderer->dpi_scale.y;

    if (window_x) {
        *window_x = x;
    }
    if (window_y) {
        *window_y = y;
    }
    return true;
}

bool SDL_ConvertEventToRenderCoordinates(SDL_Renderer *renderer, SDL_Event *event)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        SDL_Window *window = SDL_GetWindowFromID(event->motion.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->motion.x, event->motion.y, &event->motion.x, &event->motion.y);
            SDL_RenderVectorFromWindow(renderer, event->motion.xrel, event->motion.yrel, &event->motion.xrel, &event->motion.yrel);
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        SDL_Window *window = SDL_GetWindowFromID(event->button.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->button.x, event->button.y, &event->button.x, &event->button.y);
        }
    } else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        SDL_Window *window = SDL_GetWindowFromID(event->wheel.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->wheel.mouse_x,
                                            event->wheel.mouse_y,
                                            &event->wheel.mouse_x,
                                            &event->wheel.mouse_y);
        }
    } else if (event->type == SDL_EVENT_FINGER_DOWN ||
               event->type == SDL_EVENT_FINGER_UP ||
               event->type == SDL_EVENT_FINGER_CANCELED ||
               event->type == SDL_EVENT_FINGER_MOTION) {
        // FIXME: Are these events guaranteed to be window relative?
        if (renderer->window) {
            int w, h;
            if (!SDL_GetWindowSize(renderer->window, &w, &h)) {
                return false;
            }
            SDL_RenderCoordinatesFromWindow(renderer, event->tfinger.x * w, event->tfinger.y * h, &event->tfinger.x, &event->tfinger.y);
            SDL_RenderVectorFromWindow(renderer, event->tfinger.dx * w, event->tfinger.dy * h, &event->tfinger.dx, &event->tfinger.dy);
        }
    } else if (event->type == SDL_EVENT_PEN_MOTION) {
        SDL_Window *window = SDL_GetWindowFromID(event->pmotion.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->pmotion.x, event->pmotion.y, &event->pmotion.x, &event->pmotion.y);
        }
    } else if ((event->type == SDL_EVENT_PEN_DOWN) || (event->type == SDL_EVENT_PEN_UP)) {
        SDL_Window *window = SDL_GetWindowFromID(event->ptouch.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->ptouch.x, event->ptouch.y, &event->ptouch.x, &event->ptouch.y);
        }
    } else if ((event->type == SDL_EVENT_PEN_BUTTON_DOWN) || (event->type == SDL_EVENT_PEN_BUTTON_UP)) {
        SDL_Window *window = SDL_GetWindowFromID(event->pbutton.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->pbutton.x, event->pbutton.y, &event->pbutton.x, &event->pbutton.y);
        }
    } else if (event->type == SDL_EVENT_PEN_AXIS) {
        SDL_Window *window = SDL_GetWindowFromID(event->paxis.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->paxis.x, event->paxis.y, &event->paxis.x, &event->paxis.y);
        }
    } else if (event->type == SDL_EVENT_DROP_POSITION ||
               event->type == SDL_EVENT_DROP_FILE ||
               event->type == SDL_EVENT_DROP_TEXT ||
               event->type == SDL_EVENT_DROP_COMPLETE) {
        SDL_Window *window = SDL_GetWindowFromID(event->drop.windowID);
        if (window == renderer->window) {
            SDL_RenderCoordinatesFromWindow(renderer, event->drop.x, event->drop.y, &event->drop.x, &event->drop.y);
        }
    }
    return true;
}

bool SDL_SetRenderLogicalPresentation(SDL_Renderer *renderer, int w, int h, SDL_RendererLogicalPresentation mode)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    SDL_RenderViewState *view = renderer->view;
    if (mode == SDL_LOGICAL_PRESENTATION_DISABLED) {
        view->logical_w = 0;
        view->logical_h = 0;
    } else {
        view->logical_w = w;
        view->logical_h = h;
    }
    view->logical_presentation_mode = mode;

    SDL_UpdateLogicalPresentation(renderer);

    return true;
}

bool SDL_GetRenderLogicalPresentation(SDL_Renderer *renderer, int *w, int *h, SDL_RendererLogicalPresentation *mode)
{
    #define SETVAL(ptr, val) if (ptr) { *ptr = val; }

    SETVAL(w, 0);
    SETVAL(h, 0);
    SETVAL(mode, SDL_LOGICAL_PRESENTATION_DISABLED);

    CHECK_RENDERER_MAGIC(renderer, false);

    const SDL_RenderViewState *view = renderer->view;
    SETVAL(w, view->logical_w);
    SETVAL(h, view->logical_h);
    SETVAL(mode, view->logical_presentation_mode);

    #undef SETVAL

    return true;
}

bool SDL_GetRenderLogicalPresentationRect(SDL_Renderer *renderer, SDL_FRect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (rect) {
        SDL_copyp(rect, &renderer->view->logical_dst_rect);
    }
    return true;
}

bool SDL_GetRenderSafeArea(SDL_Renderer *renderer, SDL_Rect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (renderer->target || !renderer->window) {
        // The entire viewport is safe for rendering
        return SDL_GetRenderViewport(renderer, rect);
    }

    if (rect) {
        // Get the window safe rect
        SDL_Rect safe;
        if (!SDL_GetWindowSafeArea(renderer->window, &safe)) {
            return false;
        }

        // Convert the coordinates into the render space
        float minx = (float)safe.x;
        float miny = (float)safe.y;
        float maxx = (float)safe.x + safe.w;
        float maxy = (float)safe.y + safe.h;
        if (!SDL_RenderCoordinatesFromWindow(renderer, minx, miny, &minx, &miny) ||
            !SDL_RenderCoordinatesFromWindow(renderer, maxx, maxy, &maxx, &maxy)) {
            return false;
        }

        rect->x = (int)SDL_ceilf(minx);
        rect->y = (int)SDL_ceilf(miny);
        rect->w = (int)SDL_ceilf(maxx - minx);
        rect->h = (int)SDL_ceilf(maxy - miny);

        // Clip with the viewport
        SDL_Rect viewport;
        if (!SDL_GetRenderViewport(renderer, &viewport)) {
            return false;
        }
        if (!SDL_GetRectIntersection(rect, &viewport, rect)) {
            return SDL_SetError("No safe area within viewport");
        }
    }
    return true;
}

bool SDL_SetRenderViewport(SDL_Renderer *renderer, const SDL_Rect *rect)
{
    if (rect) {
        SDL_FRect frect;
        SDL_RectToFRect(rect, &frect);
        return SDL_SetRenderViewportFloat(renderer, &frect);
    } else {
        return SDL_SetRenderViewportFloat(renderer, NULL);
    }
}

bool SDL_GetRenderViewport(SDL_Renderer *renderer, SDL_Rect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    SDL_FRect frect;
    if (SDL_GetRenderViewportFloat(renderer, &frect)) {
        if (rect) {
            rect->x = (int)SDL_floorf(frect.x);
            rect->y = (int)SDL_floorf(frect.y);
            rect->w = (int)SDL_ceilf(frect.w);
            rect->h = (int)SDL_ceilf(frect.h);
        }
        return true;
    } else {
        return false;
    }
}

bool SDL_SetRenderViewportFloat(SDL_Renderer *renderer, const SDL_FRect *rect)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    SDL_RenderViewState *view = renderer->view;
    if (rect) {
        if ((rect->w < 0.0f) || (rect->h < 0.0f)) {
            return SDL_SetError("rect has a negative size");
        }
        SDL_copyp(&view->viewport, rect);
    } else {
        view->viewport.x = view->viewport.y = 0.0f;
        view->viewport.w = view->viewport.h = -1.0f;
    }
    SDL_UpdatePixelViewport(renderer, view);

    return SDL_QueueCmdSetViewport(renderer);
}

bool SDL_GetRenderViewportFloat(SDL_Renderer *renderer, SDL_FRect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (rect) {
        const SDL_RenderViewState *view = renderer->view;
        rect->x = view->viewport.x;
        rect->y = view->viewport.y;
        if (view->viewport.w >= 0.0f) {
            rect->w = view->viewport.w;
        } else {
            rect->w = view->pixel_w / view->current_scale.x;
        }
        if (view->viewport.h >= 0.0f) {
            rect->h = view->viewport.h;
        } else {
            rect->h = view->pixel_h / view->current_scale.y;
        }
    }
    return true;
}

bool SDL_RenderViewportSet(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    const SDL_RenderViewState *view = renderer->view;
    return (view->viewport.w >= 0.0f && view->viewport.h >= 0.0f);
}


void SDL_GetRenderViewportSize(SDL_Renderer *renderer, SDL_FRect *rect)
{
    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;

    rect->x = 0.0f;
    rect->y = 0.0f;

    if (view->viewport.w >= 0.0f) {
        rect->w = view->viewport.w;
    } else {
        rect->w = view->pixel_w / scale_x;
    }

    if (view->viewport.h >= 0.0f) {
        rect->h = view->viewport.h;
    } else {
        rect->h = view->pixel_h / scale_y;
    }
}


bool SDL_SetRenderClipRect(SDL_Renderer *renderer, const SDL_Rect *rect)
{
    if (rect) {
        SDL_FRect frect;
        SDL_RectToFRect(rect, &frect);
        return SDL_SetRenderClipRectFloat(renderer, &frect);
    } else {
        return SDL_SetRenderClipRectFloat(renderer, NULL);
    }
}

bool SDL_GetRenderClipRect(SDL_Renderer *renderer, SDL_Rect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    SDL_FRect frect;
    if (SDL_GetRenderClipRectFloat(renderer, &frect)) {
        rect->x = (int)SDL_floorf(frect.x);
        rect->y = (int)SDL_floorf(frect.y);
        rect->w = (int)SDL_ceilf(frect.w);
        rect->h = (int)SDL_ceilf(frect.h);
        return true;
    } else {
        return false;
    }
}

bool SDL_SetRenderClipRectFloat(SDL_Renderer *renderer, const SDL_FRect *rect)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    SDL_RenderViewState *view = renderer->view;
    if (rect && rect->w >= 0.0f && rect->h >= 0.0f) {
        view->clipping_enabled = true;
        SDL_copyp(&view->clip_rect, rect);
    } else {
        view->clipping_enabled = false;
        SDL_zero(view->clip_rect);
    }
    SDL_UpdatePixelClipRect(renderer, view);

    return SDL_QueueCmdSetClipRect(renderer);
}

bool SDL_GetRenderClipRectFloat(SDL_Renderer *renderer, SDL_FRect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (rect) {
        SDL_copyp(rect, &renderer->view->clip_rect);
    }
    return true;
}

bool SDL_RenderClipEnabled(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, false);
    return renderer->view->clipping_enabled;
}

bool SDL_SetRenderScale(SDL_Renderer *renderer, float scaleX, float scaleY)
{
    bool result = true;

    CHECK_RENDERER_MAGIC(renderer, false);

    SDL_RenderViewState *view = renderer->view;

    if ((view->scale.x == scaleX) && (view->scale.y == scaleY)) {
        return true;
    }

    view->scale.x = scaleX;
    view->scale.y = scaleY;
    view->current_scale.x = scaleX * view->logical_scale.x;
    view->current_scale.y = scaleY * view->logical_scale.y;
    SDL_UpdatePixelViewport(renderer, view);
    SDL_UpdatePixelClipRect(renderer, view);

    // The scale affects the existing viewport and clip rectangle
    result &= SDL_QueueCmdSetViewport(renderer);
    result &= SDL_QueueCmdSetClipRect(renderer);
    return result;
}

bool SDL_GetRenderScale(SDL_Renderer *renderer, float *scaleX, float *scaleY)
{
    if (scaleX) {
        *scaleX = 1.0f;
    }
    if (scaleY) {
        *scaleY = 1.0f;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    const SDL_RenderViewState *view = renderer->view;

    if (scaleX) {
        *scaleX = view->scale.x;
    }
    if (scaleY) {
        *scaleY = view->scale.y;
    }
    return true;
}
