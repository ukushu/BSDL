/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_sysvideo.h"
#include "SDL_video_c.h"
#include "../events/SDL_events_c.h"

SDL_DisplayID SDL_ResolveWindowPosition(int *window_x, int *window_y, int w, int h, bool *undefined_x, bool *undefined_y)
{
    int x = *window_x;
    int y = *window_y;
    SDL_DisplayID displayID = 0;

    if (SDL_WINDOWPOS_ISUNDEFINED(x) || SDL_WINDOWPOS_ISUNDEFINED(y) ||
        SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISCENTERED(y)) {
        SDL_Rect bounds;

        if ((SDL_WINDOWPOS_ISUNDEFINED(x) || SDL_WINDOWPOS_ISCENTERED(x)) && (x & 0xFFFF)) {
            displayID = (x & 0xFFFF);
        } else if ((SDL_WINDOWPOS_ISUNDEFINED(y) || SDL_WINDOWPOS_ISCENTERED(y)) && (y & 0xFFFF)) {
            displayID = (y & 0xFFFF);
        }
        if (displayID == 0 || SDL_GetDisplayIndex(displayID) < 0) {
            displayID = SDL_GetPrimaryDisplay();
        }

        SDL_zero(bounds);
        SDL_GetDisplayUsableBounds(displayID, &bounds);
        if (w > bounds.w || h > bounds.h) {
            // This window is larger than the usable bounds, just center on the display
            SDL_GetDisplayBounds(displayID, &bounds);
        }
        if (SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISUNDEFINED(x)) {
            if (SDL_WINDOWPOS_ISUNDEFINED(x)) {
                *undefined_x = true;
            }
            x = bounds.x + (bounds.w - w) / 2;
        }
        if (SDL_WINDOWPOS_ISCENTERED(y) || SDL_WINDOWPOS_ISUNDEFINED(y)) {
            if (SDL_WINDOWPOS_ISUNDEFINED(y)) {
                *undefined_y = true;
            }
            y = bounds.y + (bounds.h - h) / 2;
        }
    }

    *window_x = x;
    *window_y = y;
    return displayID;
}

/**
 * If x, y are outside of rect, snaps them to the closest point inside rect
 * (between rect->x, rect->y, inclusive, and rect->x + w, rect->y + h, exclusive)
 */
static void SDL_GetClosestPointOnRect(const SDL_Rect *rect, SDL_Point *point)
{
    const int right = rect->x + rect->w - 1;
    const int bottom = rect->y + rect->h - 1;

    if (point->x < rect->x) {
        point->x = rect->x;
    } else if (point->x > right) {
        point->x = right;
    }

    if (point->y < rect->y) {
        point->y = rect->y;
    } else if (point->y > bottom) {
        point->y = bottom;
    }
}

static SDL_DisplayID GetDisplayForRect(int x, int y, int w, int h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int i, dist;
    SDL_DisplayID closest = 0;
    int closest_dist = 0x7FFFFFFF;
    SDL_Point closest_point_on_display;
    SDL_Point delta;
    SDL_Point center;
    center.x = x + w / 2;
    center.y = y + h / 2;

    if (_this) {
        for (i = 0; i < _this->num_displays; ++i) {
            SDL_VideoDisplay *display = _this->displays[i];
            SDL_Rect display_rect;
            SDL_GetDisplayBounds(display->id, &display_rect);

            // Check if the window is fully enclosed
            if (SDL_GetRectEnclosingPoints(&center, 1, &display_rect, NULL)) {
                return display->id;
            }

            // Snap window center to the display rect
            closest_point_on_display = center;
            SDL_GetClosestPointOnRect(&display_rect, &closest_point_on_display);

            delta.x = center.x - closest_point_on_display.x;
            delta.y = center.y - closest_point_on_display.y;
            dist = (delta.x * delta.x + delta.y * delta.y);
            if (dist < closest_dist) {
                closest = display->id;
                closest_dist = dist;
            }
        }
    }

    if (closest == 0) {
        SDL_SetError("Couldn't find any displays");
    }

    return closest;
}

void SDL_RelativeToGlobalForWindow(SDL_Window *window, int rel_x, int rel_y, int *abs_x, int *abs_y)
{
    SDL_Window *w;

    if (SDL_WINDOW_IS_POPUP(window)) {
        // Calculate the total offset of the popup from the parents
        for (w = window->parent; w; w = w->parent) {
            rel_x += w->x;
            rel_y += w->y;

            if (!SDL_WINDOW_IS_POPUP(w)) {
                break;
            }
        }
    }

    if (abs_x) {
        *abs_x = rel_x;
    }
    if (abs_y) {
        *abs_y = rel_y;
    }
}

void SDL_GlobalToRelativeForWindow(SDL_Window *window, int abs_x, int abs_y, int *rel_x, int *rel_y)
{
    SDL_Window *w;

    if (SDL_WINDOW_IS_POPUP(window)) {
        // Convert absolute window coordinates to relative for a popup
        for (w = window->parent; w; w = w->parent) {
            abs_x -= w->x;
            abs_y -= w->y;

            if (!SDL_WINDOW_IS_POPUP(w)) {
                break;
            }
        }
    }

    if (rel_x) {
        *rel_x = abs_x;
    }
    if (rel_y) {
        *rel_y = abs_y;
    }
}

SDL_DisplayID SDL_GetDisplayForPoint(const SDL_Point *point)
{
    CHECK_PARAM(!point) {
        SDL_InvalidParamError("point");
        return 0;
    }

    return GetDisplayForRect(point->x, point->y, 1, 1);
}

SDL_DisplayID SDL_GetDisplayForRect(const SDL_Rect *rect)
{
    CHECK_PARAM(!rect) {
        SDL_InvalidParamError("rect");
        return 0;
    }

    return GetDisplayForRect(rect->x, rect->y, rect->w, rect->h);
}

SDL_DisplayID SDL_GetDisplayAtOrigin(int x, int y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    for (int i = 0; i < _this->num_displays; ++i) {
        SDL_Rect rect;
        const SDL_DisplayID cur_id = _this->displays[i]->id;
        if (SDL_GetDisplayBounds(cur_id, &rect)) {
            if (x == rect.x && y == rect.y) {
                return cur_id;
            }
        }
    }

    return 0;
}

SDL_DisplayID SDL_GetDisplayForWindowPosition(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int x, y;
    SDL_DisplayID displayID = 0;

    CHECK_WINDOW_MAGIC(window, 0);

    if (_this->GetDisplayForWindow) {
        displayID = _this->GetDisplayForWindow(_this, window);
    }

    /* A backend implementation may fail to get a display for the window
     * (for example if the window is off-screen), but other code may expect it
     * to succeed in that situation, so we fall back to a generic position-
     * based implementation in that case. */
    SDL_RelativeToGlobalForWindow(window, window->x, window->y, &x, &y);

    if (!displayID) {
        /* Fullscreen windows may be larger than the display if they were moved between differently sized
         * displays and the new position was received before the new size or vice versa. Using the center
         * of the window rect in this case can report the wrong display, so use the origin.
         */
        if (window->flags & SDL_WINDOW_FULLSCREEN) {
            displayID = GetDisplayForRect(x, y, 1, 1);
        } else {
            displayID = GetDisplayForRect(x, y, window->w, window->h);
        }
    }
    if (!displayID) {
        // Use the primary display for a window if we can't find it anywhere else
        displayID = SDL_GetPrimaryDisplay();
    }
    return displayID;
}

bool SDL_GetWindowSizeInPixels(SDL_Window *window, int *w, int *h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int filter;

    CHECK_WINDOW_MAGIC(window, false);

    if (!w) {
        w = &filter;
    }
    if (!h) {
        h = &filter;
    }

    if (_this->GetWindowSizeInPixels) {
        _this->GetWindowSizeInPixels(_this, window, w, h);
    } else {
        SDL_DisplayID displayID = SDL_GetDisplayForWindowInternal(window);
        const SDL_DisplayMode *mode;

        SDL_GetWindowSize(window, w, h);

        if ((window->flags & SDL_WINDOW_FULLSCREEN) && SDL_GetWindowFullscreenMode(window)) {
            mode = SDL_GetCurrentDisplayMode(displayID);
        } else {
            mode = SDL_GetDesktopDisplayMode(displayID);
        }
        if (mode) {
            *w = (int)SDL_ceilf(*w * mode->pixel_density);
            *h = (int)SDL_ceilf(*h * mode->pixel_density);
        }
    }
    return true;
}

bool SDL_GetWindowPosition(SDL_Window *window, int *x, int *y)
{
    CHECK_WINDOW_MAGIC(window, false);

#ifdef SDL_PLATFORM_MACOS
    // On newer MacBooks, the fullscreen window might be placed below the camera notch, so use the actual window position
    bool use_display_origin = false;
#else
    // Fullscreen windows are always at their display's origin
    bool use_display_origin = ((window->flags & SDL_WINDOW_FULLSCREEN) != 0);
#endif
    if (use_display_origin) {
        SDL_DisplayID displayID;

        if (x) {
            *x = 0;
        }
        if (y) {
            *y = 0;
        }

        /* Find the window's monitor and update to the
           monitor offset. */
        displayID = SDL_GetDisplayForWindowInternal(window);
        if (displayID != 0) {
            SDL_Rect bounds;

            SDL_zero(bounds);

            SDL_GetDisplayBounds(displayID, &bounds);
            if (x) {
                *x = bounds.x;
            }
            if (y) {
                *y = bounds.y;
            }
        }
    } else {
        const bool use_pending = (window->flags & SDL_WINDOW_HIDDEN) && window->last_position_pending;
        if (x) {
            *x = use_pending ? window->pending.x : window->x;
        }
        if (y) {
            *y = use_pending ? window->pending.y : window->y;
        }
    }
    return true;
}

SDL_VideoDisplay *SDL_GetVideoDisplayForFullscreenWindow(SDL_Window *window)
{
    SDL_DisplayID displayID = 0;

    CHECK_WINDOW_MAGIC(window, NULL);

    // An explicit fullscreen display overrides all
    if (window->current_fullscreen_mode.displayID) {
        displayID = window->current_fullscreen_mode.displayID;
    }

    /* This is used to handle the very common pattern of SDL_SetWindowPosition()
     * followed immediately by SDL_SetWindowFullscreen() to make the window fullscreen
     * desktop on a specific display. If the backend doesn't support changing the
     * window position, or an async window manager hasn't yet actually moved the window,
     * the current position won't be updated at the time of the fullscreen call.
     */
    if (!displayID) {
        displayID = window->pending_displayID;
    }
    if (!displayID) {
        // Use the pending position and dimensions, if available, otherwise, use the current.
        const int x = window->last_position_pending ? window->pending.x : window->x;
        const int y = window->last_position_pending ? window->pending.y : window->y;
        const int w = window->last_size_pending ? window->pending.w : window->w;
        const int h = window->last_size_pending ? window->pending.h : window->h;

        // Check if the window is exactly at the origin of a display. Otherwise, fall back to the generic check.
        displayID = SDL_GetDisplayAtOrigin(x, y);
        if (!displayID) {
            displayID = GetDisplayForRect(x, y, w, h);
        }
    }
    if (!displayID) {
        // Use the primary display for a window if we can't find it anywhere else
        displayID = SDL_GetPrimaryDisplay();
    }
    return SDL_GetVideoDisplay(displayID);
}
