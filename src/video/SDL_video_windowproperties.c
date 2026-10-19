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

bool SDL_SetWindowTitle(SDL_Window *window, const char *title)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (title == window->title) {
        return true;
    }
    if (!title) {
        title = "";
    }
    if (window->title && SDL_strcmp(title, window->title) == 0) {
        return true;
    }

    SDL_free(window->title);

    window->title = SDL_strdup(title);

    if (_this->SetWindowTitle) {
        _this->SetWindowTitle(_this, window);
    }
    return true;
}

const char *SDL_GetWindowTitle(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, "");

    return window->title ? window->title : "";
}

bool SDL_SetWindowIcon(SDL_Window *window, SDL_Surface *icon)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    CHECK_PARAM(!icon) {
        return SDL_InvalidParamError("icon");
    }

    SDL_DestroySurface(window->icon);

    // Convert the icon into ARGB8888
    window->icon = SDL_ConvertSurface(icon, SDL_PIXELFORMAT_ARGB8888);
    if (!window->icon) {
        return false;
    }

    if (!_this->SetWindowIcon) {
        return SDL_Unsupported();
    }

    return _this->SetWindowIcon(_this, window, window->icon);
}

bool SDL_SetWindowPosition(SDL_Window *window, int x, int y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_DisplayID original_displayID;

    CHECK_WINDOW_MAGIC(window, false);

    const int w = window->last_size_pending ? window->pending.w : window->windowed.w;
    const int h = window->last_size_pending ? window->pending.h : window->windowed.h;

        original_displayID = SDL_GetDisplayForWindowInternal(window);
    window->pending_displayID = 0;

    if (SDL_WINDOWPOS_ISUNDEFINED(x)) {
        x = window->windowed.x;
    }
    if (SDL_WINDOWPOS_ISUNDEFINED(y)) {
        y = window->windowed.y;
    }
    if (SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISCENTERED(y)) {
        SDL_DisplayID displayID = original_displayID;
        SDL_Rect bounds;

        if (SDL_WINDOWPOS_ISCENTERED(x) && (x & 0xFFFF)) {
            displayID = (x & 0xFFFF);
        } else if (SDL_WINDOWPOS_ISCENTERED(y) && (y & 0xFFFF)) {
            displayID = (y & 0xFFFF);
        }
        if (displayID == 0 || SDL_GetDisplayIndex(displayID) < 0) {
            displayID = SDL_GetPrimaryDisplay();
        }

        window->pending_displayID = displayID;

        SDL_zero(bounds);
        if (!SDL_GetDisplayUsableBounds(displayID, &bounds) || w > bounds.w || h > bounds.h) {
            if (!SDL_GetDisplayBounds(displayID, &bounds)) {
                return false;
            }
        }
        if (SDL_WINDOWPOS_ISCENTERED(x)) {
            x = bounds.x + (bounds.w - w) / 2;
        }
        if (SDL_WINDOWPOS_ISCENTERED(y)) {
            y = bounds.y + (bounds.h - h) / 2;
        }
    } else {
        /* See if the requested window position matches the origin of any displays and set
         * the pending fullscreen display ID if it does. This needs to be set early in case
         * the window is prevented from moving to the exact origin due to struts.
         */
        window->pending_displayID = SDL_GetDisplayAtOrigin(x, y);
    }

    window->pending.x = x;
    window->pending.y = y;

    /* Windows are placed at the coordinates received while in fullscreen after leaving fullscreen.
     * Asynchronous backends need special handling in this case.
     */
    if (!_this->SyncWindow && (window->flags & SDL_WINDOW_FULLSCREEN)) {
        window->floating.x = window->windowed.x = x;
        window->floating.y = window->windowed.y = y;
    }
    window->undefined_x = false;
    window->undefined_y = false;
    window->last_position_pending = true;

    if (_this->SetWindowPosition) {
        const bool result = _this->SetWindowPosition(_this, window);
        if (result) {
            SDL_SyncIfRequired(window);
        }
        return result;
    }

    return SDL_Unsupported();
}

bool SDL_SetWindowBordered(SDL_Window *window, bool bordered)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowBordered) {
        return SDL_Unsupported();
    }

    const bool want = bordered;
    const bool have = !(window->flags & SDL_WINDOW_BORDERLESS);
    if (want != have) {
        if (want) {
            window->flags &= ~SDL_WINDOW_BORDERLESS;
        } else {
            window->flags |= SDL_WINDOW_BORDERLESS;
        }
        _this->SetWindowBordered(_this, window, want);
    }

    return true;
}

bool SDL_SetWindowResizable(SDL_Window *window, bool resizable)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowResizable) {
        return SDL_Unsupported();
    }

    const bool want = resizable;
    const bool have = ((window->flags & SDL_WINDOW_RESIZABLE) != 0);
    if (want != have) {
        if (want) {
            window->flags |= SDL_WINDOW_RESIZABLE;
        } else {
            window->flags &= ~SDL_WINDOW_RESIZABLE;
            SDL_copyp(&window->windowed, &window->floating);
        }
        _this->SetWindowResizable(_this, window, want);
    }

    return true;
}

bool SDL_SetWindowAlwaysOnTop(SDL_Window *window, bool on_top)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowAlwaysOnTop) {
        return SDL_Unsupported();
    }

    const bool want = on_top;
    const bool have = ((window->flags & SDL_WINDOW_ALWAYS_ON_TOP) != 0);
    if (want != have) {
        if (want) {
            window->flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        } else {
            window->flags &= ~SDL_WINDOW_ALWAYS_ON_TOP;
        }
        _this->SetWindowAlwaysOnTop(_this, window, want);
    }

    return true;
}

bool SDL_GetWindowBordersSize(SDL_Window *window, int *top, int *left, int *bottom, int *right)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int dummy = 0;

    if (!top) {
        top = &dummy;
    }
    if (!left) {
        left = &dummy;
    }
    if (!right) {
        right = &dummy;
    }
    if (!bottom) {
        bottom = &dummy;
    }

    // Always initialize, so applications don't have to care
    *top = *left = *bottom = *right = 0;

    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->GetWindowBordersSize) {
        return SDL_Unsupported();
    }

    return _this->GetWindowBordersSize(_this, window, top, left, bottom, right);
}


bool SDL_SetWindowOpacity(SDL_Window *window, float opacity)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    bool result;

    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->SetWindowOpacity) {
        return SDL_Unsupported();
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    } else if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    result = _this->SetWindowOpacity(_this, window, opacity);
    if (result) {
        window->opacity = opacity;
    }

    return result;
}

float SDL_GetWindowOpacity(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, -1.0f);

    return window->opacity;
}

bool SDL_SetWindowFillDocument(SDL_Window *window, bool fill)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    const bool want = (fill != false); // normalize the flag.
    const bool have = ((window->flags & SDL_WINDOW_FILL_DOCUMENT) != 0);
    if ((want != have) && (_this->SetWindowFillDocument)) {
        if (!_this->SetWindowFillDocument(_this, window, want)) {
            return false;
        } else if (want) {
            window->flags |= SDL_WINDOW_FILL_DOCUMENT;
        } else {
            window->flags &= ~SDL_WINDOW_FILL_DOCUMENT;
        }
    }

    return true;
}
