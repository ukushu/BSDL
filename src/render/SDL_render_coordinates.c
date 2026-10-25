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

void SDL_UpdatePixelViewport(SDL_Renderer *renderer, SDL_RenderViewState *view)
{
    view->pixel_viewport.x = (int)SDL_floorf((view->viewport.x * view->current_scale.x) + view->logical_offset.x);
    view->pixel_viewport.y = (int)SDL_floorf((view->viewport.y * view->current_scale.y) + view->logical_offset.y);
    if (view->viewport.w >= 0) {
        view->pixel_viewport.w = (int)SDL_ceilf(view->viewport.w * view->current_scale.x);
    } else {
        view->pixel_viewport.w = view->pixel_w;
    }
    if (view->viewport.h >= 0) {
        view->pixel_viewport.h = (int)SDL_ceilf(view->viewport.h * view->current_scale.y);
    } else {
        view->pixel_viewport.h = view->pixel_h;
    }
}

void SDL_UpdatePixelClipRect(SDL_Renderer *renderer, SDL_RenderViewState *view)
{
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    view->pixel_clip_rect.x = (int)SDL_floorf(view->clip_rect.x * scale_x);
    view->pixel_clip_rect.y = (int)SDL_floorf(view->clip_rect.y * scale_y);
    view->pixel_clip_rect.w = (int)SDL_ceilf(view->clip_rect.w * scale_x);
    view->pixel_clip_rect.h = (int)SDL_ceilf(view->clip_rect.h * scale_y);
}

void SDL_UpdateMainViewDimensions(SDL_Renderer *renderer)
{
    int window_w = 0, window_h = 0;

    if (renderer->window) {
        SDL_GetWindowSize(renderer->window, &window_w, &window_h);
    }

    SDL_GetRenderOutputSize(renderer, &renderer->main_view.pixel_w, &renderer->main_view.pixel_h);

    if (window_w > 0 && window_h > 0) {
        renderer->dpi_scale.x = (float)renderer->main_view.pixel_w / window_w;
        renderer->dpi_scale.y = (float)renderer->main_view.pixel_h / window_h;
    } else {
        renderer->dpi_scale.x = 1.0f;
        renderer->dpi_scale.y = 1.0f;
    }
    SDL_UpdatePixelViewport(renderer, &renderer->main_view);
}

static SDL_FRect CalculateLogicalDestinationRect(float logical_w, float logical_h, float output_w, float output_h, SDL_RendererLogicalPresentation mode)
{
    SDL_FRect rect;
    const float want_aspect = logical_w / logical_h;
    const float real_aspect = output_w / output_h;

    if ((logical_w <= 0.0f) || (logical_h <= 0.0f)) {
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.w = output_w;
        rect.h = output_h;
    } else if (mode == SDL_LOGICAL_PRESENTATION_INTEGER_SCALE) {
        float scale;
        if (want_aspect > real_aspect) {
            scale = (float)((int)output_w / (int)logical_w); // This an integer division!
        } else {
            scale = (float)((int)output_h / (int)logical_h); // This an integer division!
        }

        if (scale < 1.0f) {
            scale = 1.0f;
        }

        rect.w = SDL_floorf(logical_w * scale);
        rect.x = (output_w - rect.w) / 2.0f;
        rect.h = SDL_floorf(logical_h * scale);
        rect.y = (output_h - rect.h) / 2.0f;
    } else if (mode == SDL_LOGICAL_PRESENTATION_STRETCH || SDL_fabsf(want_aspect - real_aspect) < 0.0001f) {
        rect.x = 0.0f;
        rect.y = 0.0f;
        rect.w = output_w;
        rect.h = output_h;
    } else if (want_aspect > real_aspect) {
        if (mode == SDL_LOGICAL_PRESENTATION_LETTERBOX) {
            // We want a wider aspect ratio than is available - letterbox it
            const float scale = output_w / logical_w;
            rect.x = 0.0f;
            rect.w = output_w;
            rect.h = SDL_floorf(logical_h * scale);
            rect.y = (output_h - rect.h) / 2.0f;
        } else { // SDL_LOGICAL_PRESENTATION_OVERSCAN
            /* We want a wider aspect ratio than is available,
               zoom so logical height matches the real height
               and the width will grow off the screen
             */
            const float scale = output_h / logical_h;
            rect.y = 0.0f;
            rect.h = output_h;
            rect.w = SDL_floorf(logical_w * scale);
            rect.x = (output_w - rect.w) / 2.0f;
        }
    } else {
        if (mode == SDL_LOGICAL_PRESENTATION_LETTERBOX) {
            // We want a narrower aspect ratio than is available - use side-bars
            const float scale = output_h / logical_h;
            rect.y = 0.0f;
            rect.h = output_h;
            rect.w = SDL_floorf(logical_w * scale);
            rect.x = (output_w - rect.w) / 2.0f;
        } else { // SDL_LOGICAL_PRESENTATION_OVERSCAN
            /* We want a narrower aspect ratio than is available,
               zoom so logical width matches the real width
               and the height will grow off the screen
             */
            const float scale = output_w / logical_w;
            rect.x = 0.0f;
            rect.w = output_w;
            rect.h = SDL_floorf(logical_h * scale);
            rect.y = (output_h - rect.h) / 2.0f;
        }
    }
    return rect;
}

void SDL_UpdateLogicalPresentation(SDL_Renderer *renderer)
{
    SDL_RenderViewState *view = renderer->view;
    const bool is_main_view = (view == &renderer->main_view);
    const float logical_w = view->logical_w;
    const float logical_h = view->logical_h;
    int iwidth, iheight;

    if (is_main_view) {
        SDL_GetRenderOutputSize(renderer, &iwidth, &iheight);
    } else {
        SDL_assert(renderer->target != NULL);
        iwidth = (int)renderer->target->w;
        iheight = (int)renderer->target->h;
    }

    view->logical_src_rect.x = 0.0f;
    view->logical_src_rect.y = 0.0f;
    view->logical_src_rect.w = logical_w;
    view->logical_src_rect.h = logical_h;

    if (view->logical_presentation_mode == SDL_LOGICAL_PRESENTATION_DISABLED) {
        view->logical_dst_rect.x = 0.0f;
        view->logical_dst_rect.y = 0.0f;
        view->logical_dst_rect.w = iwidth;
        view->logical_dst_rect.h = iheight;
        view->logical_offset.x = view->logical_offset.y = 0.0f;
        view->logical_scale.x = view->logical_scale.y = 1.0f;
        view->current_scale.x = view->scale.x;  // skip the multiplications against 1.0f.
        view->current_scale.y = view->scale.y;
    } else {
        view->logical_dst_rect = CalculateLogicalDestinationRect(logical_w, logical_h, (float)iwidth, (float)iheight, view->logical_presentation_mode);
        view->logical_scale.x = (logical_w > 0.0f) ? view->logical_dst_rect.w / logical_w : 0.0f;
        view->logical_scale.y = (logical_h > 0.0f) ? view->logical_dst_rect.h / logical_h : 0.0f;
        view->current_scale.x = view->scale.x * view->logical_scale.x;
        view->current_scale.y = view->scale.y * view->logical_scale.y;
        view->logical_offset.x = view->logical_dst_rect.x;
        view->logical_offset.y = view->logical_dst_rect.y;
    }

    if (is_main_view) {
        // This makes sure the dpi_scale is right. It also sets pixel_w and pixel_h, but we're going to change them directly below here.
        SDL_UpdateMainViewDimensions(renderer);
    }

    view->pixel_w = (int)view->logical_dst_rect.w;
    view->pixel_h = (int)view->logical_dst_rect.h;
    SDL_UpdatePixelViewport(renderer, view);
    SDL_UpdatePixelClipRect(renderer, view);
    SDL_QueueCmdSetViewport(renderer);
    SDL_QueueCmdSetClipRect(renderer);
}
