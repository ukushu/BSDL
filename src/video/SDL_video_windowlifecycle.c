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
#include "../events/SDL_keyboard_c.h"
#include "../events/SDL_mouse_c.h"
#include "../events/SDL_windowevents_c.h"
#include "../render/SDL_sysrender.h"

#ifdef SDL_VIDEO_DRIVER_UIKIT
extern void SDL_UpdateLifecycleObserver(void);
#endif

static void SDL_UnlinkWindow(SDL_VideoDevice *_this, SDL_Window *window)
{
    // Unlink the window from its siblings.
    SDL_UpdateWindowHierarchy(window, NULL);

    // Unlink the window from the global window list
    if (window->next) {
        window->next->prev = window->prev;
    }
    if (window->prev) {
        window->prev->next = window->next;
    } else {
        _this->windows = window->next;
    }
}

void SDL_DestroyWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window,);

    window->is_destroying = true;

    // Destroy any child windows of this window
    while (window->first_child) {
        SDL_DestroyWindow(window->first_child);
    }

    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DESTROYED, 0, 0);

    SDL_DestroyWindowSurface(window);

    SDL_Renderer *renderer = SDL_GetRenderer(window);
    if (renderer) {
        SDL_DestroyRendererWithoutFreeing(renderer);
    }

    // Restore video mode, etc.
    SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, true);
    if (!(window->flags & SDL_WINDOW_EXTERNAL)) {
        SDL_HideWindow(window);
    }

    SDL_DestroyProperties(window->text_input_props);

    SDL_DestroyWindowTexture(_this, window);
    SDL_DestroyProperties(window->props);

    /* Clear the modal status, but don't unset the parent just yet, as it
     * may be needed later in the destruction process if a backend needs
     * to update the input focus.
     */
    if (_this->SetWindowModal && (window->flags & SDL_WINDOW_MODAL)) {
        _this->SetWindowModal(_this, window, false);
    }

    // Make sure the destroyed window isn't referenced by any display as a fullscreen window.
    for (int i = 0; i < _this->num_displays; ++i) {
        if (_this->displays[i]->fullscreen_window == window) {
            _this->displays[i]->fullscreen_window = NULL;
        }
    }

    // Make sure this window no longer has focus
    if (SDL_GetKeyboardFocus() == window) {
        SDL_SetKeyboardFocus(NULL);
    }
    if ((window->flags & SDL_WINDOW_MOUSE_CAPTURE)) {
        SDL_UpdateMouseCapture(true);
    }
    if (SDL_GetMouseFocus() == window) {
        SDL_SetMouseFocus(NULL);
    }

    // Make no context current if this is the current context window
    if (window->flags & SDL_WINDOW_OPENGL) {
        if (_this->current_glwin == window) {
            SDL_GL_MakeCurrent(window, NULL);
        }
    }

    if (_this->DestroyWindow) {
        _this->DestroyWindow(_this, window);
    }

    // Unload the graphics libraries after the window is destroyed, which may clean up EGL surfaces
    if (window->flags & SDL_WINDOW_OPENGL) {
        SDL_GL_UnloadLibrary();
    }
    if (window->flags & SDL_WINDOW_VULKAN) {
        SDL_Vulkan_UnloadLibrary();
    }

    if (_this->grabbed_window == window) {
        _this->grabbed_window = NULL; // ungrabbing input.
    }

    if (_this->current_glwin == window) {
        _this->current_glwin = NULL;
    }

    SDL_CompareAndSwapAtomicPointer(&_this->wakeup_window, window, NULL);

    // Now invalidate magic
    SDL_SetObjectValid(window, SDL_OBJECT_TYPE_WINDOW, false);

    // Free memory associated with the window
    SDL_free(window->title);
    SDL_DestroySurface(window->icon);

    SDL_UnlinkWindow(_this, window);

    SDL_free(window->renderers);
    SDL_free(window);

#ifdef SDL_VIDEO_DRIVER_UIKIT
    SDL_UpdateLifecycleObserver();
#endif
}
