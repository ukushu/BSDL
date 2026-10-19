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
#include "../events/SDL_events_c.h"

static bool SDL_SendsDisplayChanges(SDL_VideoDevice *device)
{
    return !!(device->device_caps & VIDEO_DEVICE_CAPS_SENDS_DISPLAY_CHANGES);
}

static bool SDL_DriverSendsHDRChanges(SDL_VideoDevice *device)
{
    return !!(device->device_caps & VIDEO_DEVICE_CAPS_SENDS_HDR_CHANGES);
}

static bool SDL_CheckWindowMagic(SDL_Window *window)
{
    if (!SDL_GetVideoDevice()) {
        SDL_UninitializedVideo();
        return false;
    }
    if (!SDL_ObjectValid(window, SDL_OBJECT_TYPE_WINDOW)) {
        SDL_SetError("Invalid window");
        return false;
    }
    return true;
}

void SDL_OnDisplayAdded(SDL_VideoDisplay *display)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    SDL_Window *window;

    // See if any windows have changed to the new display
    for (window = device->windows; window; window = window->next) {
        SDL_CheckWindowDisplayChanged(window);
    }
}

SDL_DisplayID SDL_GetDisplayForWindowInternal(SDL_Window *window)
{
    SDL_DisplayID displayID = 0;

    // An explicit fullscreen display overrides all
    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        displayID = window->current_fullscreen_mode.displayID;
    }

    if (!displayID) {
        displayID = SDL_GetDisplayForWindowPosition(window);
    }
    return displayID;
}

SDL_VideoDisplay *SDL_GetVideoDisplayForWindow(SDL_Window *window)
{
    if (!SDL_CheckWindowMagic(window)) {
        return NULL;
    }

    return SDL_GetVideoDisplay(SDL_GetDisplayForWindowInternal(window));
}

SDL_DisplayData *SDL_GetDisplayDriverDataForWindow(SDL_Window *window)
{
    if (!SDL_CheckWindowMagic(window)) {
        return NULL;
    }

    return SDL_GetDisplayDriverData(SDL_GetDisplayForWindowInternal(window));
}

void SDL_SetDisplayContentScale(SDL_VideoDisplay *display, float scale)
{
    if (scale != display->content_scale) {
        SDL_Window *window;

        display->content_scale = scale;
        SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED, 0, 0);

        // Check the windows on this display
        for (window = display->device->windows; window; window = window->next) {
            if (display->id == window->displayID) {
                SDL_CheckWindowDisplayScaleChanged(window);
            }
        }
    }
}

void SDL_SetWindowHDRProperties(SDL_Window *window, const SDL_HDROutputProperties *HDR, bool send_event)
{
    const SDL_HDROutputProperties HDR_clamped = {
        SDL_max(HDR->SDR_white_level, 1.0f),
        SDL_max(HDR->HDR_headroom, 1.0f)
    };

    if (window->HDR.HDR_headroom != HDR_clamped.HDR_headroom || window->HDR.SDR_white_level != HDR_clamped.SDR_white_level) {
        SDL_PropertiesID window_props = SDL_GetWindowProperties(window);

        SDL_SetFloatProperty(window_props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, HDR_clamped.HDR_headroom);
        SDL_SetFloatProperty(window_props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, HDR_clamped.SDR_white_level);
        SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, HDR_clamped.HDR_headroom > 1.0f);
        SDL_copyp(&window->HDR, &HDR_clamped);

        if (send_event) {
            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_HDR_STATE_CHANGED, HDR_clamped.HDR_headroom > 1.0f, 0);
        }
    }
}

void SDL_SetDisplayHDRProperties(SDL_VideoDisplay *display, const SDL_HDROutputProperties *HDR)
{
    const SDL_HDROutputProperties HDR_clamped = {
        SDL_max(HDR->SDR_white_level, 1.0f),
        SDL_max(HDR->HDR_headroom, 1.0f)
    };
    const bool changed = HDR_clamped.SDR_white_level != display->HDR.SDR_white_level ||
                         HDR_clamped.HDR_headroom != display->HDR.HDR_headroom;

    SDL_copyp(&display->HDR, &HDR_clamped);

    if (changed && !SDL_DriverSendsHDRChanges(display->device)) {
        for (SDL_Window *w = display->device->windows; w; w = w->next) {
            if (SDL_GetDisplayForWindowInternal(w) == display->id) {
                SDL_SetWindowHDRProperties(w, &display->HDR, true);
            }
        }
    }
}

#define SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY "sdl2-compat.window.preferred_fullscreen_display"

SDL_DisplayID SDL_GetDisplayForWindow(SDL_Window *window)
{
    if (!SDL_CheckWindowMagic(window)) {
        return 0;
    }

    /* sdl2-compat calls this function to get a display on which to make the window fullscreen,
     * so pass it the preferred fullscreen display ID in a property.
     */
    SDL_PropertiesID window_props = SDL_GetWindowProperties(window);
    SDL_VideoDisplay *fs_display = SDL_GetVideoDisplayForFullscreenWindow(window);
    if (fs_display) {
        SDL_SetNumberProperty(window_props, SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY, fs_display->id);
    } else {
        SDL_ClearProperty(window_props, SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY);
    }

    return SDL_GetDisplayForWindowInternal(window);
}

void SDL_CheckWindowDisplayChanged(SDL_Window *window)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    if (SDL_SendsDisplayChanges(device)) {
        return;
    }

    SDL_DisplayID displayID = SDL_GetDisplayForWindowPosition(window);

    if (displayID != window->displayID) {
        // See if we are fully committed to the new display
        // 80% is about the right value, tested with 350% scale on the left monitor and 100% scale on the right
        SDL_Rect old_bounds, new_bounds;
        SDL_Rect window_rect;
        SDL_Rect old_overlap, new_overlap;

        if (SDL_GetDisplayBounds(window->displayID, &old_bounds) &&
            SDL_GetDisplayBounds(displayID, &new_bounds)) {
            window_rect.x = window->x;
            window_rect.y = window->y;
            window_rect.w = window->w;
            window_rect.h = window->h;

            if (SDL_GetRectIntersection(&old_bounds, &window_rect, &old_overlap) &&
                SDL_GetRectIntersection(&new_bounds, &window_rect, &new_overlap)) {
                int old_area = old_overlap.w * old_overlap.h;
                int new_area = new_overlap.w * new_overlap.h;
                float new_overlap_ratio = (new_area / ((float)old_area + new_area));
                if (new_overlap_ratio < 0.80) {
                    return;
                }
            }
        }
    }

    if (displayID != window->displayID) {
        int i, display_index;

        // Sanity check our fullscreen windows
        display_index = SDL_GetDisplayIndex(displayID);
        for (i = 0; i < device->num_displays; ++i) {
            SDL_VideoDisplay *display = device->displays[i];

            if (display->fullscreen_window == window) {
                if (display_index != i) {
                    if (display_index < 0) {
                        display_index = i;
                    } else {
                        SDL_VideoDisplay *new_display = device->displays[display_index];

                        // The window was moved to a different display
                        if (new_display->fullscreen_window &&
                            new_display->fullscreen_window != window) {
                            // Uh oh, there's already a fullscreen window here; minimize it
                            SDL_MinimizeWindow(new_display->fullscreen_window);
                        }
                        new_display->fullscreen_window = window;
                        display->fullscreen_window = NULL;
                    }
                }
                break;
            }
        }

        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DISPLAY_CHANGED, (int)displayID, 0);
    }
}

float SDL_GetWindowPixelDensity(SDL_Window *window)
{
    int window_w, window_h, pixel_w, pixel_h;
    float pixel_density = 1.0f;

    if (!SDL_CheckWindowMagic(window)) {
        return 0.0f;
    }

    if (SDL_GetWindowSize(window, &window_w, &window_h) &&
        SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h)) {
        pixel_density = (float)pixel_w / window_w;
    }
    return pixel_density;
}

float SDL_GetWindowDisplayScale(SDL_Window *window)
{
    if (!SDL_CheckWindowMagic(window)) {
        return 0.0f;
    }

    return window->display_scale;
}

void SDL_CheckWindowDisplayScaleChanged(SDL_Window *window)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    float display_scale;

    if (device->GetWindowContentScale) {
        display_scale = device->GetWindowContentScale(device, window);
    } else {
        const float pixel_density = SDL_GetWindowPixelDensity(window);
        const float content_scale = SDL_GetDisplayContentScale(window->displayID);

        display_scale = pixel_density * content_scale;
    }

    if (display_scale != window->display_scale) {
        window->display_scale = display_scale;
        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED, 0, 0);
    }
}
