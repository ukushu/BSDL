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

#define CHECK_WINDOW_MAGIC(window, result)                              \
    CHECK_PARAM(!_this) {                                               \
        SDL_UninitializedVideo();                                       \
        return result;                                                  \
    }                                                                   \
    CHECK_PARAM(!SDL_ObjectValid(window, SDL_OBJECT_TYPE_WINDOW)) {     \
        SDL_SetError("Invalid window");                                 \
        return result;                                                  \
    }

#define CHECK_WINDOW_NOT_POPUP(window, result)                         \
    CHECK_PARAM(SDL_WINDOW_IS_POPUP(window)) {                          \
        SDL_SetError("Operation invalid on popup windows");            \
        return result;                                                  \
    }

bool SDL_FlashWindow(SDL_Window *window, SDL_FlashOperation operation)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (_this->FlashWindow) {
        return _this->FlashWindow(_this, window, operation);
    }

    return SDL_Unsupported();
}

bool SDL_SetWindowProgressState(SDL_Window *window, SDL_ProgressState state)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    CHECK_PARAM(state < SDL_PROGRESS_STATE_NONE || state > SDL_PROGRESS_STATE_ERROR) {
        return SDL_InvalidParamError("state");
    }

    window->progress_state = state;

    if (_this->ApplyWindowProgress) {
        if (!_this->ApplyWindowProgress(_this, window)) {
            return false;
        }
    }

    return true;
}

SDL_ProgressState SDL_GetWindowProgressState(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, SDL_PROGRESS_STATE_INVALID);
    CHECK_WINDOW_NOT_POPUP(window, SDL_PROGRESS_STATE_INVALID);

    return window->progress_state;
}

bool SDL_SetWindowProgressValue(SDL_Window *window, float value)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    value = SDL_clamp(value, 0.0f, 1.f);

    window->progress_value = value;

    if (_this->ApplyWindowProgress) {
        if (!_this->ApplyWindowProgress(_this, window)) {
            return false;
        }
    }

    return true;
}

float SDL_GetWindowProgressValue(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, -1.0f);
    CHECK_WINDOW_NOT_POPUP(window, -1.0f);

    return window->progress_value;
}
