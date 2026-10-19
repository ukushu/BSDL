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

static bool SDL_SendsFullscreenDimensions(SDL_VideoDevice *device)
{
    return !!(device->device_caps & VIDEO_DEVICE_CAPS_SENDS_FULLSCREEN_DIMENSIONS);
}

static bool SDL_CheckWindowMagicForFullscreen(SDL_Window *window)
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

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
extern bool Cocoa_IsWindowInFullscreenSpace(SDL_Window *window);
extern bool Cocoa_IsWindowInFullscreenSpaceTransition(SDL_Window *window);
extern bool Cocoa_SetWindowFullscreenSpace(SDL_Window *window, bool state, bool blocking);
#endif

bool SDL_SetWindowFullscreenMode(SDL_Window *window, const SDL_DisplayMode *mode)
{
    if (!SDL_CheckWindowMagicForFullscreen(window)) {
        return false;
    }
    if (SDL_WINDOW_IS_POPUP(window)) {
        return SDL_SetError("Operation invalid on popup windows");
    }

    if (mode) {
        if (!SDL_GetFullscreenModeMatch(mode)) {
            return SDL_SetError("Invalid fullscreen display mode");
        }

        // Save the mode so we can look up the closest match later
        SDL_copyp(&window->requested_fullscreen_mode, mode);
    } else {
        SDL_zero(window->requested_fullscreen_mode);
    }

    /* Copy to the current mode now, in case an asynchronous fullscreen window request
     * is in progress. It will be overwritten if a new request is made.
     */
    SDL_copyp(&window->current_fullscreen_mode, &window->requested_fullscreen_mode);

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
    /* If this is called while in the middle of a Cocoa fullscreen spaces transition,
     * wait until the transition has completed, or the window can wind up in a weird,
     * broken state if a mode switch occurs while in a fullscreen space.
     */
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    if (SDL_strcmp(device->name, "cocoa") == 0 && Cocoa_IsWindowInFullscreenSpaceTransition(window)) {
        SDL_SyncWindow(window);
    }
#endif
    if (SDL_WINDOW_FULLSCREEN_VISIBLE(window)) {
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_UPDATE, true);
        SDL_SyncIfRequired(window);
    }

    return true;
}

const SDL_DisplayMode *SDL_GetWindowFullscreenMode(SDL_Window *window)
{
    if (!SDL_CheckWindowMagicForFullscreen(window)) {
        return NULL;
    }
    if (SDL_WINDOW_IS_POPUP(window)) {
        SDL_SetError("Operation invalid on popup windows");
        return NULL;
    }

    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        return SDL_GetFullscreenModeMatch(&window->current_fullscreen_mode);
    } else {
        return SDL_GetFullscreenModeMatch(&window->requested_fullscreen_mode);
    }
}

static SDL_VideoDisplay *SDL_GetFullscreenDisplay(SDL_Window *window, bool entering)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    if (entering) {
        return SDL_GetVideoDisplayForFullscreenWindow(window);
    }

    for (int i = 0; i < device->num_displays; ++i) {
        SDL_VideoDisplay *display = device->displays[i];
        if (display->fullscreen_window == window) {
            return display;
        }
    }
    return NULL;
}

static bool SDL_EnterFullscreen(SDL_Window *window, SDL_VideoDisplay *display, SDL_DisplayMode *mode, SDL_FullscreenOp fullscreen, bool commit)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    int mode_w = 0, mode_h = 0;
    bool resized = false;

    // Hide any other fullscreen window on this display
    if (display->fullscreen_window && display->fullscreen_window != window) {
        SDL_MinimizeWindow(display->fullscreen_window);
    }

    display->fullscreen_active = window->fullscreen_exclusive;

    if (!SDL_SetDisplayModeForDisplay(display, mode)) {
        return false;
    }
    if (commit) {
        SDL_FullscreenResult ret = SDL_FULLSCREEN_SUCCEEDED;
        if (device->SetWindowFullscreen) {
            ret = device->SetWindowFullscreen(device, window, display, fullscreen);
        } else {
            resized = true;
        }

        if (ret == SDL_FULLSCREEN_SUCCEEDED) {
            // Window is fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
            if (!(window->flags & SDL_WINDOW_FULLSCREEN)) {
                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_ENTER_FULLSCREEN, 0, 0);
            }
        } else if (ret == SDL_FULLSCREEN_FAILED) {
            display->fullscreen_active = false;
            return false;
        }
    }

    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        display->fullscreen_window = window;

        /* Android may not resize the window to exactly what our fullscreen mode is,
         * especially on windowed Android environments like the Chromebook or Samsung DeX.
         * Given this, we shouldn't use the mode size. Android's SetWindowFullscreen
         * will generate the window event for us with the proper final size.
         *
         * This is also unnecessary on Cocoa, Wayland, Win32, and X11 (will send SDL_EVENT_WINDOW_RESIZED).
         */
        if (!SDL_SendsFullscreenDimensions(device)) {
            SDL_Rect displayRect;

            if (mode) {
                mode_w = mode->w;
                mode_h = mode->h;
                SDL_GetDisplayBounds(mode->displayID, &displayRect);
            } else {
                mode_w = display->desktop_mode.w;
                mode_h = display->desktop_mode.h;
                SDL_GetDisplayBounds(display->id, &displayRect);
            }

            if (window->w != mode_w || window->h != mode_h) {
                resized = true;
            }

            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MOVED, displayRect.x, displayRect.y);

            if (resized) {
                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESIZED, mode_w, mode_h);
            } else {
                SDL_OnWindowResized(window);
            }
        }
    }
    return true;
}

static bool SDL_LeaveFullscreen(SDL_Window *window, SDL_VideoDisplay *display, bool commit)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    bool resized = false;

    // Restore the desktop mode
    if (display) {
        display->fullscreen_active = false;
        SDL_SetDisplayModeForDisplay(display, NULL);
    }
    if (commit) {
        SDL_FullscreenResult ret = SDL_FULLSCREEN_SUCCEEDED;
        if (device->SetWindowFullscreen) {
            SDL_VideoDisplay *fullscreen_display = display ? display : SDL_GetVideoDisplayForFullscreenWindow(window);
            if (fullscreen_display) {
                ret = device->SetWindowFullscreen(device, window, fullscreen_display, SDL_FULLSCREEN_OP_LEAVE);
            }
        } else {
            resized = true;
        }

        if (ret == SDL_FULLSCREEN_SUCCEEDED) {
            // Window left fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
            if (window->flags & SDL_WINDOW_FULLSCREEN) {
                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_LEAVE_FULLSCREEN, 0, 0);
            }
        } else if (ret == SDL_FULLSCREEN_FAILED) {
            return false;
        }
    }

    if (!(window->flags & SDL_WINDOW_FULLSCREEN)) {
        if (display) {
            display->fullscreen_window = NULL;
        }

        if (!SDL_SendsFullscreenDimensions(device)) {
            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MOVED, window->windowed.x, window->windowed.y);
            if (resized) {
                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESIZED, window->windowed.w, window->windowed.h);
            } else {
                SDL_OnWindowResized(window);
            }
        }
    }
    return true;
}

bool SDL_UpdateFullscreenMode(SDL_Window *window, SDL_FullscreenOp fullscreen, bool commit)
{
    SDL_VideoDevice *device = SDL_GetVideoDevice();
    SDL_VideoDisplay *display = NULL;
    SDL_DisplayMode *mode = NULL;
    int i;

    if (!SDL_CheckWindowMagicForFullscreen(window)) {
        return false;
    }

    window->fullscreen_exclusive = false;
    window->update_fullscreen_on_display_changed = false;

    // If we are in the process of hiding don't go back to fullscreen
    if (window->is_destroying || window->is_hiding) {
        fullscreen = SDL_FULLSCREEN_OP_LEAVE;
    }

    // Get the correct display for this operation
    display = SDL_GetFullscreenDisplay(window, fullscreen != SDL_FULLSCREEN_OP_LEAVE);
    if (fullscreen) {
        if (!display) {
            // This should never happen, but it did...
            goto done;
        }
    }

    if (fullscreen) {
        mode = (SDL_DisplayMode *)SDL_GetWindowFullscreenMode(window);
        if (mode) {
            window->fullscreen_exclusive = true;
        } else {
            // Make sure the current mode is zeroed for fullscreen desktop.
            SDL_zero(window->current_fullscreen_mode);
        }
    }

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
    /* if the window is going away and no resolution change is necessary,
       do nothing, or else we may trigger an ugly double-transition
     */
    if (SDL_strcmp(device->name, "cocoa") == 0) { // don't do this for X11, etc
        if (window->is_destroying && !window->last_fullscreen_exclusive_display) {
            window->fullscreen_exclusive = false;
            if (display) {
                display->fullscreen_window = NULL;
            }
            goto done;
        }
        if (commit) {
            // If we're switching between a fullscreen Space and exclusive fullscreen, we need to get back to normal first.
            if (fullscreen && Cocoa_IsWindowInFullscreenSpace(window) && !window->last_fullscreen_exclusive_display && window->fullscreen_exclusive) {
                if (!Cocoa_SetWindowFullscreenSpace(window, false, true)) {
                    goto error;
                }
            } else if (fullscreen && window->last_fullscreen_exclusive_display && !window->fullscreen_exclusive) {
                for (i = 0; i < device->num_displays; ++i) {
                    SDL_VideoDisplay *last_display = device->displays[i];
                    if (last_display->fullscreen_window == window) {
                        SDL_SetDisplayModeForDisplay(last_display, NULL);
                        if (device->SetWindowFullscreen) {
                            device->SetWindowFullscreen(device, window, last_display, false);
                        }
                        last_display->fullscreen_window = NULL;
                    }
                }
            }

            if (Cocoa_SetWindowFullscreenSpace(window, !!fullscreen, SDL_GetVideoSyncHint())) {
                goto done;
            }
        }
    }
#endif

    if (display) {
        // Restore the video mode on other displays if needed
        for (i = 0; i < device->num_displays; ++i) {
            SDL_VideoDisplay *other = device->displays[i];
            if (other != display && other->fullscreen_window == window) {
                SDL_SetDisplayModeForDisplay(other, NULL);
                other->fullscreen_window = NULL;
            }
        }
    }

    if (fullscreen) {
        if (!SDL_EnterFullscreen(window, display, mode, fullscreen, commit)) {
            goto error;
        }
    } else if (!SDL_LeaveFullscreen(window, display, commit)) {
        goto error;
    }

done:
    window->last_fullscreen_exclusive_display = display && (window->flags & SDL_WINDOW_FULLSCREEN) && window->fullscreen_exclusive ? display->id : 0;
    return true;

error:
    if (fullscreen) {
        // Something went wrong and the window is no longer fullscreen.
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, commit);
    }
    return false;
}
