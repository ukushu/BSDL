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
#include "../events/SDL_keyboard_c.h"
#include "../events/SDL_mouse_c.h"
#include "../events/SDL_windowevents_c.h"

bool SDL_ShowWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *child;
    CHECK_WINDOW_MAGIC(window, false);

    if (!(window->flags & SDL_WINDOW_HIDDEN)) {
        return true;
    }

    // If the parent is hidden, set the flag to restore this when the parent is shown
    if (window->parent && (window->parent->flags & SDL_WINDOW_HIDDEN)) {
        window->restore_on_show = true;
        return true;
    }

    if (_this->ShowWindow) {
        _this->ShowWindow(_this, window);
    } else {
        SDL_SetMouseFocus(window);
        SDL_SetKeyboardFocus(window);
    }
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_SHOWN, 0, 0);

    // Restore child windows
    for (child = window->first_child; child; child = child->next_sibling) {
        if (!child->restore_on_show && (child->flags & SDL_WINDOW_HIDDEN)) {
            break;
        }
        SDL_ShowWindow(child);
        child->restore_on_show = false;
    }
    return true;
}

bool SDL_HideWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *child;
    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->restore_on_show = false;
        return true;
    }

    // Hide all child windows
    for (child = window->first_child; child; child = child->next_sibling) {
        if (child->flags & SDL_WINDOW_HIDDEN) {
            break;
        }
        SDL_HideWindow(child);
        child->restore_on_show = true;
    }

    // Store the flags for restoration later.
    const SDL_WindowFlags pending_mask = (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_KEYBOARD_GRABBED | SDL_WINDOW_MOUSE_GRABBED);
    window->pending_flags = (window->flags & pending_mask);

    window->is_hiding = true;
    if (_this->HideWindow) {
        _this->HideWindow(_this, window);
    } else {
        SDL_SetMouseFocus(NULL);
        SDL_SetKeyboardFocus(NULL);
    }
    window->is_hiding = false;
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_HIDDEN, 0, 0);
    return true;
}

bool SDL_RaiseWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        return true;
    }
    if (_this->RaiseWindow) {
        _this->RaiseWindow(_this, window);
    }
    return true;
}

bool SDL_MaximizeWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->MaximizeWindow) {
        return SDL_Unsupported();
    }

    if (!(window->flags & SDL_WINDOW_RESIZABLE)) {
        return SDL_SetError("A window without the 'SDL_WINDOW_RESIZABLE' flag can't be maximized");
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags |= SDL_WINDOW_MAXIMIZED;
        return true;
    }

    _this->MaximizeWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

bool SDL_MinimizeWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->MinimizeWindow) {
        return SDL_Unsupported();
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags |= SDL_WINDOW_MINIMIZED;
        return true;
    }

    _this->MinimizeWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

bool SDL_RestoreWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->RestoreWindow) {
        return SDL_Unsupported();
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags &= ~(SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED);
        return true;
    }

    _this->RestoreWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

