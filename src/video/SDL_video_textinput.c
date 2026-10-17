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
#include "../SDL_hints_c.h"

#define CHECK_WINDOW_MAGIC(window, result)                              \
    CHECK_PARAM(!_this) {                                               \
        SDL_UninitializedVideo();                                       \
        return result;                                                  \
    }                                                                   \
    CHECK_PARAM(!SDL_ObjectValid(window, SDL_OBJECT_TYPE_WINDOW)) {     \
        SDL_SetError("Invalid window");                                 \
        return result;                                                  \
    }

static bool AutoShowingScreenKeyboard(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    const char *hint = SDL_GetHint(SDL_HINT_ENABLE_SCREEN_KEYBOARD);
    if (!hint) {
        // This hint is currently only used by the X11 video driver
        if (SDL_strcmp(_this->name, "x11") == 0) {
            hint = SDL_GetHint(SDL_HINT_ENABLE_STEAM_SCREEN_KEYBOARD);
        }
    }
    if (((!hint || SDL_strcasecmp(hint, "auto") == 0) && !SDL_HasKeyboard()) ||
        SDL_GetStringBoolean(hint, false)) {
        return true;
    } else {
        return false;
    }
}

bool SDL_StartTextInput(SDL_Window *window)
{
    return SDL_StartTextInputWithProperties(window, 0);
}

bool SDL_StartTextInputWithProperties(SDL_Window *window, SDL_PropertiesID props)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (window->text_input_props) {
        SDL_DestroyProperties(window->text_input_props);
        window->text_input_props = 0;
    }

    if (props) {
        window->text_input_props = SDL_CreateProperties();
        if (!window->text_input_props) {
            return false;
        }
        if (!SDL_CopyProperties(props, window->text_input_props)) {
            return false;
        }
    }

    if (_this->SetTextInputProperties) {
        _this->SetTextInputProperties(_this, window, props);
    }

    if (!window->text_input_active) {
        // Start the text input system
        if (_this->StartTextInput) {
            if (!_this->StartTextInput(_this, window, props)) {
                return false;
            }
        }
        window->text_input_active = true;
    }

    // Show the on-screen keyboard, if desired
    if (AutoShowingScreenKeyboard() && !SDL_ScreenKeyboardShown(window)) {
        if (_this->ShowScreenKeyboard) {
            _this->ShowScreenKeyboard(_this, window, props);
        }
    }
    return true;
}

bool SDL_TextInputActive(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    return window->text_input_active;
}

bool SDL_StopTextInput(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (window->text_input_active) {
        // Stop the text input system
        if (_this->StopTextInput) {
            _this->StopTextInput(_this, window);
        }
        window->text_input_active = false;
    }

    // Hide the on-screen keyboard, if desired
    if (AutoShowingScreenKeyboard() && SDL_ScreenKeyboardShown(window)) {
        if (_this->HideScreenKeyboard) {
            _this->HideScreenKeyboard(_this, window);
        }
    }
    return true;
}

bool SDL_SetTextInputArea(SDL_Window *window, const SDL_Rect *rect, int cursor)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (rect) {
        SDL_copyp(&window->text_input_rect, rect);
        window->text_input_cursor = cursor;
    } else {
        SDL_zero(window->text_input_rect);
        window->text_input_cursor = 0;
    }

    if (_this && _this->UpdateTextInputArea) {
        if (!_this->UpdateTextInputArea(_this, window)) {
            return false;
        }
    }
    return true;
}

bool SDL_GetTextInputArea(SDL_Window *window, SDL_Rect *rect, int *cursor)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (rect) {
        SDL_copyp(rect, &window->text_input_rect);
    }
    if (cursor) {
        *cursor = window->text_input_cursor;
    }
    return true;
}

bool SDL_ClearComposition(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (_this->ClearComposition) {
        return _this->ClearComposition(_this, window);
    }
    return true;
}

bool SDL_HasScreenKeyboardSupport(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (_this && _this->HasScreenKeyboardSupport) {
        return _this->HasScreenKeyboardSupport(_this);
    }
    return false;
}

bool SDL_ScreenKeyboardShown(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    return _this->screen_keyboard_shown;
}

void SDL_SendScreenKeyboardShown(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this || _this->screen_keyboard_shown) {
        return;
    }

    _this->screen_keyboard_shown = true;

    if (SDL_EventEnabled(SDL_EVENT_SCREEN_KEYBOARD_SHOWN)) {
        SDL_Event event;
        event.type = SDL_EVENT_SCREEN_KEYBOARD_SHOWN;
        event.common.timestamp = 0;
        SDL_PushEvent(&event);
    }
}

void SDL_SendScreenKeyboardHidden(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this || !_this->screen_keyboard_shown) {
        return;
    }

    _this->screen_keyboard_shown = false;

    if (SDL_EventEnabled(SDL_EVENT_SCREEN_KEYBOARD_HIDDEN)) {
        SDL_Event event;
        event.type = SDL_EVENT_SCREEN_KEYBOARD_HIDDEN;
        event.common.timestamp = 0;
        SDL_PushEvent(&event);
    }
}

