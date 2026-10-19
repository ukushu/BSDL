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

#define CHECK_DISPLAY_MAGIC(display, result)                            \
    CHECK_PARAM(!display) {                                             \
        return result;                                                  \
    }

static int SDLCALL cmpmodes(const void *A, const void *B)
{
    const SDL_DisplayMode *a = (const SDL_DisplayMode *)A;
    const SDL_DisplayMode *b = (const SDL_DisplayMode *)B;
    int a_refresh_rate = (int)(a->refresh_rate * 100);
    int b_refresh_rate = (int)(b->refresh_rate * 100);
    int a_pixel_density = (int)(a->pixel_density * 100);
    int b_pixel_density = (int)(b->pixel_density * 100);

    if (a->w != b->w) {
        return b->w - a->w;
    } else if (a->h != b->h) {
        return b->h - a->h;
    } else if (SDL_BITSPERPIXEL(a->format) != SDL_BITSPERPIXEL(b->format)) {
        return SDL_BITSPERPIXEL(b->format) - SDL_BITSPERPIXEL(a->format);
    } else if (SDL_PIXELLAYOUT(a->format) != SDL_PIXELLAYOUT(b->format)) {
        return SDL_PIXELLAYOUT(b->format) - SDL_PIXELLAYOUT(a->format);
    } else if (a_refresh_rate != b_refresh_rate) {
        return b_refresh_rate - a_refresh_rate;
    } else if (a_pixel_density != b_pixel_density) {
        return a_pixel_density - b_pixel_density;
    }
    return 0;
}

void SDL_FinalizeDisplayMode(SDL_DisplayMode *mode)
{
    // Make sure all the fields are set up correctly
    if (mode->pixel_density <= 0.0f) {
        mode->pixel_density = 1.0f;
    }

    if (mode->refresh_rate_numerator > 0) {
        if (mode->refresh_rate_denominator <= 0) {
            mode->refresh_rate_denominator = 1;
        }
        mode->refresh_rate = (float)mode->refresh_rate_numerator / mode->refresh_rate_denominator;
    } else {
        SDL_CalculateFraction(mode->refresh_rate, &mode->refresh_rate_numerator, &mode->refresh_rate_denominator);
    }
    mode->refresh_rate = SDL_roundf(mode->refresh_rate * 100) / 100.0f;
}

static void SDL_UpdateFullscreenDisplayModes(SDL_VideoDisplay *display)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (display->num_fullscreen_modes == 0 && _this->GetDisplayModes) {
        _this->GetDisplayModes(_this, display);
    }
}

// Return the matching mode as a pointer into our current mode list
const SDL_DisplayMode *SDL_GetFullscreenModeMatch(const SDL_DisplayMode *mode)
{
    SDL_VideoDisplay *display;
    SDL_DisplayMode fullscreen_mode;

    if (mode->w <= 0 || mode->h <= 0) {
        // Use the desktop mode
        return NULL;
    }

    SDL_memcpy(&fullscreen_mode, mode, sizeof(fullscreen_mode));
    if (fullscreen_mode.displayID == 0) {
        fullscreen_mode.displayID = SDL_GetPrimaryDisplay();
    }
    SDL_FinalizeDisplayMode(&fullscreen_mode);

    mode = NULL;

    display = SDL_GetVideoDisplay(fullscreen_mode.displayID);
    if (display) {
        SDL_UpdateFullscreenDisplayModes(display);

        // Search for an exact match
        if (!mode) {
            for (int i = 0; i < display->num_fullscreen_modes; ++i) {
                if (SDL_memcmp(&fullscreen_mode, &display->fullscreen_modes[i], sizeof(fullscreen_mode)) == 0) {
                    mode = &display->fullscreen_modes[i];
                    break;
                }
            }
        }

        // Search for a mode with the same characteristics
        if (!mode) {
            for (int i = 0; i < display->num_fullscreen_modes; ++i) {
                if (cmpmodes(&fullscreen_mode, &display->fullscreen_modes[i]) == 0) {
                    mode = &display->fullscreen_modes[i];
                    break;
                }
            }
        }
    }
    return mode;
}

bool SDL_AddFullscreenDisplayMode(SDL_VideoDisplay *display, const SDL_DisplayMode *mode)
{
    SDL_DisplayMode *modes;
    SDL_DisplayMode new_mode;
    int i, nmodes;

    // Finalize the mode for the display
    SDL_memcpy(&new_mode, mode, sizeof(new_mode));
    new_mode.displayID = display->id;
    SDL_FinalizeDisplayMode(&new_mode);

    // Make sure we don't already have the mode in the list
    modes = display->fullscreen_modes;
    nmodes = display->num_fullscreen_modes;
    for (i = 0; i < nmodes; ++i) {
        if (cmpmodes(&new_mode, &modes[i]) == 0) {
            return false;
        }
    }

    // Go ahead and add the new mode
    if (nmodes == display->max_fullscreen_modes) {
        modes = (SDL_DisplayMode *)SDL_malloc((display->max_fullscreen_modes + 32) * sizeof(*modes));
        if (!modes) {
            return false;
        }

        if (display->fullscreen_modes) {
            // Copy the list and update the current mode pointer, if necessary.
            SDL_memcpy(modes, display->fullscreen_modes, nmodes * sizeof(*modes));
            for (i = 0; i < nmodes; ++i) {
                if (display->current_mode == &display->fullscreen_modes[i]) {
                    display->current_mode = &modes[i];
                }
            }

            SDL_free(display->fullscreen_modes);
        }

        display->fullscreen_modes = modes;
        display->max_fullscreen_modes += 32;
    }
    SDL_memcpy(&modes[display->num_fullscreen_modes++], &new_mode, sizeof(new_mode));

    // Re-sort video modes
    SDL_qsort(display->fullscreen_modes, display->num_fullscreen_modes,
              sizeof(SDL_DisplayMode), cmpmodes);

    return true;
}

void SDL_ResetFullscreenDisplayModes(SDL_VideoDisplay *display)
{
    int i;

    for (i = display->num_fullscreen_modes; i--;) {
        SDL_free(display->fullscreen_modes[i].internal);
        display->fullscreen_modes[i].internal = NULL;
    }
    SDL_free(display->fullscreen_modes);
    display->fullscreen_modes = NULL;
    display->num_fullscreen_modes = 0;
    display->max_fullscreen_modes = 0;
    display->current_mode = &display->desktop_mode;
}

SDL_DisplayMode **SDL_GetFullscreenDisplayModes(SDL_DisplayID displayID, int *count)
{
    int i;
    int num_modes;
    SDL_DisplayMode **result;
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    if (count) {
        *count = 0;
    }

    CHECK_DISPLAY_MAGIC(display, NULL);

    SDL_UpdateFullscreenDisplayModes(display);

    num_modes = display->num_fullscreen_modes;
    result = (SDL_DisplayMode **)SDL_malloc((num_modes + 1) * sizeof(*result) + num_modes * sizeof(**result));
    if (result) {
        SDL_DisplayMode *modes = (SDL_DisplayMode *)((Uint8 *)result + ((num_modes + 1) * sizeof(*result)));
        if (num_modes) {
            SDL_memcpy(modes, display->fullscreen_modes, num_modes * sizeof(*modes));
        }
        for (i = 0; i < num_modes; ++i) {
            result[i] = modes++;
        }
        result[i] = NULL;

        if (count) {
            *count = num_modes;
        }
    } else {
        if (count) {
            *count = 0;
        }
    }
    return result;
}

bool SDL_GetClosestFullscreenDisplayMode(SDL_DisplayID displayID, int w, int h, float refresh_rate, bool include_high_density_modes, SDL_DisplayMode *result)
{
    CHECK_PARAM(!result) {
        return SDL_InvalidParamError("closest"); // Parameter `result` is called `closest` in the header.
    }

    const SDL_DisplayMode *mode, *closest = NULL;
    float aspect_ratio;
    int i;
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    SDL_zerop(result);

    CHECK_DISPLAY_MAGIC(display, false);

    if (h > 0) {
        aspect_ratio = (float)w / h;
    } else {
        aspect_ratio = 1.0f;
    }

    if (refresh_rate == 0.0f) {
        refresh_rate = display->desktop_mode.refresh_rate;
    }

    SDL_UpdateFullscreenDisplayModes(display);

    for (i = 0; i < display->num_fullscreen_modes; ++i) {
        mode = &display->fullscreen_modes[i];

        if (w > mode->w) {
            // Out of sorted modes large enough here
            break;
        }
        if (h > mode->h) {
            /* Wider, but not tall enough, due to a different aspect ratio.
             * This mode must be skipped, but closer modes may still follow */
            continue;
        }
        if (mode->pixel_density > 1.0f && !include_high_density_modes) {
            continue;
        }
        if (closest) {
            float current_aspect_ratio = (float)mode->w / mode->h;
            float closest_aspect_ratio = (float)closest->w / closest->h;
            if (SDL_fabsf(aspect_ratio - closest_aspect_ratio) < SDL_fabsf(aspect_ratio - current_aspect_ratio)) {
                // The mode we already found has a better aspect ratio match
                continue;
            }

            if (mode->w == closest->w && mode->h == closest->h) {
                if (SDL_fabsf(closest->refresh_rate - refresh_rate) < SDL_fabsf(mode->refresh_rate - refresh_rate)) {
                    /* We already found a mode and the new mode is further from our
                     * refresh rate target */
                    continue;
                }
                if (SDL_BYTESPERPIXEL(closest->format) > SDL_BYTESPERPIXEL(mode->format)) {
                    // Prefer the highest color depth
                    continue;
                }
            }
        }

        closest = mode;
    }
    if (!closest) {
        return SDL_SetError("Couldn't find any matching video modes");
    }

    SDL_copyp(result, closest);

    return true;
}

static bool DisplayModeChanged(const SDL_DisplayMode *old_mode, const SDL_DisplayMode *new_mode)
{
    return ((old_mode->displayID && old_mode->displayID != new_mode->displayID) ||
            (old_mode->format && old_mode->format != new_mode->format) ||
            (old_mode->w && old_mode->h && (old_mode->w != new_mode->w ||old_mode->h != new_mode->h)) ||
            (old_mode->pixel_density != 0.0f && old_mode->pixel_density != new_mode->pixel_density) ||
            (old_mode->refresh_rate != 0.0f && old_mode->refresh_rate != new_mode->refresh_rate));
}

void SDL_SetDesktopDisplayMode(SDL_VideoDisplay *display, const SDL_DisplayMode *mode)
{
    SDL_DisplayMode last_mode;

    if (display->fullscreen_active) {
        // This is a temporary mode change, don't save the desktop mode
        return;
    }

    SDL_copyp(&last_mode, &display->desktop_mode);

    SDL_free(display->desktop_mode.internal);
    SDL_copyp(&display->desktop_mode, mode);
    display->desktop_mode.displayID = display->id;
    SDL_FinalizeDisplayMode(&display->desktop_mode);

    if (DisplayModeChanged(&last_mode, &display->desktop_mode)) {
        SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED, mode->w, mode->h);
        if (display->current_mode == &display->desktop_mode) {
            SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED, mode->w, mode->h);
        }
    }
}

const SDL_DisplayMode *SDL_GetDesktopDisplayMode(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, NULL);

    return &display->desktop_mode;
}

void SDL_SetCurrentDisplayMode(SDL_VideoDisplay *display, const SDL_DisplayMode *mode)
{
    SDL_DisplayMode last_mode;

    if (display->current_mode) {
        SDL_copyp(&last_mode, display->current_mode);
    } else {
        SDL_zero(last_mode);
    }

    display->current_mode = mode;

    if (DisplayModeChanged(&last_mode, mode)) {
        SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED, mode->w, mode->h);
    }
}

const SDL_DisplayMode *SDL_GetCurrentDisplayMode(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, NULL);

    return display->current_mode;
}

bool SDL_SetDisplayModeForDisplay(SDL_VideoDisplay *display, SDL_DisplayMode *mode)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    /* Mode switching is being emulated per-window; nothing to do and cannot fail,
     * except for XWayland, which still needs the actual mode setting call since
     * it's emulated via the XRandR interface.
     */
    if ((_this->device_caps & VIDEO_DEVICE_CAPS_MODE_SWITCHING_EMULATED) && SDL_strcmp(_this->name, "x11") != 0) {
        return true;
    }

    if (!mode) {
        mode = &display->desktop_mode;
    }

    // On RISC OS, it's necessary to switch from the desktop to single-tasking
    // fullscreen so that it can handle switching back to the desktop correctly.
#ifndef SDL_PLATFORM_RISCOS
    if (mode == display->current_mode) {
        return true;
    }
#endif

    // Actually change the display mode
    if (_this->SetDisplayMode) {
        bool result;

        _this->setting_display_mode = true;
        result = _this->SetDisplayMode(_this, display, mode);
        _this->setting_display_mode = false;
        if (!result) {
            return false;
        }
    }

    SDL_SetCurrentDisplayMode(display, mode);

    return true;
}
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
extern bool Cocoa_SetWindowFullscreenSpace(SDL_Window *window, bool state, bool blocking);
#endif

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
