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
#include "../events/SDL_mouse_c.h"

void SDL_UpdateWindowGrab(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    bool keyboard_grabbed, mouse_grabbed;

    if (window->flags & SDL_WINDOW_INPUT_FOCUS) {
        if (SDL_GetMouse()->relative_mode || (window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
            mouse_grabbed = true;
        } else {
            mouse_grabbed = false;
        }

        if (window->flags & SDL_WINDOW_KEYBOARD_GRABBED) {
            keyboard_grabbed = true;
        } else {
            keyboard_grabbed = false;
        }
    } else {
        mouse_grabbed = false;
        keyboard_grabbed = false;
    }

    if (mouse_grabbed || keyboard_grabbed) {
        if (_this->grabbed_window && (_this->grabbed_window != window)) {
            // stealing a grab from another window!
            _this->grabbed_window->flags &= ~(SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED);
            if (_this->SetWindowMouseGrab) {
                _this->SetWindowMouseGrab(_this, _this->grabbed_window, false);
            }
            if (_this->SetWindowKeyboardGrab) {
                _this->SetWindowKeyboardGrab(_this, _this->grabbed_window, false);
            }
        }
        _this->grabbed_window = window;
    } else if (_this->grabbed_window == window) {
        _this->grabbed_window = NULL; // ungrabbing input.
    }

    if (_this->SetWindowMouseGrab) {
        if (!_this->SetWindowMouseGrab(_this, window, mouse_grabbed)) {
            window->flags &= ~SDL_WINDOW_MOUSE_GRABBED;
        }
    }
    if (_this->SetWindowKeyboardGrab) {
        if (!_this->SetWindowKeyboardGrab(_this, window, keyboard_grabbed)) {
            window->flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
        }
    }

    if (_this->grabbed_window && !(_this->grabbed_window->flags & (SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED))) {
        _this->grabbed_window = NULL;
    }
}

bool SDL_SetWindowKeyboardGrab(SDL_Window *window, bool grabbed)
{

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        if (grabbed) {
            window->pending_flags |= SDL_WINDOW_KEYBOARD_GRABBED;
        } else {
            window->pending_flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
        }
        return true;
    }

    if (!!grabbed == !!(window->flags & SDL_WINDOW_KEYBOARD_GRABBED)) {
        return true;
    }
    if (grabbed) {
        window->flags |= SDL_WINDOW_KEYBOARD_GRABBED;
    } else {
        window->flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
    }
    SDL_UpdateWindowGrab(window);

    if (grabbed && !(window->flags & SDL_WINDOW_KEYBOARD_GRABBED)) {
        return false;
    }
    return true;
}

bool SDL_SetWindowMouseGrab(SDL_Window *window, bool grabbed)
{

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        if (grabbed) {
            window->pending_flags |= SDL_WINDOW_MOUSE_GRABBED;
        } else {
            window->pending_flags &= ~SDL_WINDOW_MOUSE_GRABBED;
        }
        return true;
    }

    if (!!grabbed == !!(window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
        return true;
    }
    if (grabbed) {
        window->flags |= SDL_WINDOW_MOUSE_GRABBED;
    } else {
        window->flags &= ~SDL_WINDOW_MOUSE_GRABBED;
    }
    SDL_UpdateWindowGrab(window);

    if (grabbed && !(window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
        return false;
    }
    return true;
}

bool SDL_GetWindowKeyboardGrab(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    return window == _this->grabbed_window && (_this->grabbed_window->flags & SDL_WINDOW_KEYBOARD_GRABBED);
}

bool SDL_GetWindowMouseGrab(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    return window == _this->grabbed_window && (_this->grabbed_window->flags & SDL_WINDOW_MOUSE_GRABBED);
}

SDL_Window *SDL_GetGrabbedWindow(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    if (_this && _this->grabbed_window &&
        (_this->grabbed_window->flags & (SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED)) != 0) {
        return _this->grabbed_window;
    } else {
        return NULL;
    }
}

bool SDL_SetWindowMouseRect(SDL_Window *window, const SDL_Rect *rect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->SetWindowMouseRect) {
        return SDL_Unsupported();
    }

    SDL_Rect zero = { 0, 0, 0, 0 };
    if (!rect) {
        rect = &zero;
    }

    if (SDL_memcmp(&window->mouse_rect, rect, sizeof(*rect)) == 0) {
        return true;
    }
    SDL_memcpy(&window->mouse_rect, rect, sizeof(*rect));

    return _this->SetWindowMouseRect(_this, window);
}

const SDL_Rect *SDL_GetWindowMouseRect(SDL_Window *window)
{

    CHECK_WINDOW_MAGIC(window, NULL);

    if (SDL_RectEmpty(&window->mouse_rect)) {
        return NULL;
    } else {
        return &window->mouse_rect;
    }
}

bool SDL_SetWindowRelativeMouseMode(SDL_Window *window, bool enabled)
{

    CHECK_WINDOW_MAGIC(window, false);

    /* If the app toggles relative mode directly, it probably shouldn't
     * also be emulating it using repeated mouse warps, so disable
     * mouse warp emulation by default.
     */
    SDL_DisableMouseWarpEmulation();

    if (enabled == SDL_GetWindowRelativeMouseMode(window)) {
        return true;
    }

    if (enabled) {
        window->flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
    } else {
        window->flags &= ~SDL_WINDOW_MOUSE_RELATIVE_MODE;
    }

    if (!SDL_UpdateRelativeMouseMode()) {
        if (enabled) {
            window->flags &= ~SDL_WINDOW_MOUSE_RELATIVE_MODE;
        } else {
            window->flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
        }
        return false;
    }
    return true;
}

bool SDL_GetWindowRelativeMouseMode(SDL_Window *window)
{

    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_MOUSE_RELATIVE_MODE) {
        return true;
    } else {
        return false;
    }
}
