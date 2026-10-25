/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_sysvideo.h"

SDL_Window ** SDLCALL SDL_GetWindows(int *count)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *window;
    int num_added = 0;
    int num_windows = 0;

    if (count) {
        *count = 0;
    }

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }

    for (window = _this->windows; window; window = window->next) {
        ++num_windows;
    }

    SDL_Window **windows = (SDL_Window **)SDL_malloc((num_windows + 1) * sizeof(*windows));
    if (!windows) {
        return NULL;
    }

    for (window = _this->windows; window; window = window->next) {
        windows[num_added++] = window;
        if (num_added == num_windows) {
            // Race condition? Multi-threading not supported, ignore it
            break;
        }
    }
    windows[num_added] = NULL;

    if (count) {
        *count = num_added;
    }
    return windows;
}

bool SDL_HasWindows(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    return _this && _this->windows;
}

SDL_WindowID SDL_GetWindowID(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return window->id;
}

SDL_Window *SDL_GetWindowFromID(SDL_WindowID id)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *window;

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    if (id) {
        for (window = _this->windows; window; window = window->next) {
            if (window->id == id) {
                return window;
            }
        }
    }
    SDL_SetError("Invalid window ID");
    return NULL;
}

SDL_Window *SDL_GetWindowParent(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, NULL);

    return window->parent;
}

SDL_PropertiesID SDL_GetWindowProperties(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    if (window->props == 0) {
        window->props = SDL_CreateProperties();
    }
    return window->props;
}

SDL_WindowFlags SDL_GetWindowFlags(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return window->flags | window->pending_flags;
}
