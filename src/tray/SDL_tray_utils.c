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

#include "../events/SDL_events_c.h"
#include "../video/SDL_sysvideo.h"
#include "SDL_tray_utils.h"

static int active_trays = 0;

void SDL_RegisterTray(SDL_Tray *tray)
{
    SDL_SetObjectValid(tray, SDL_OBJECT_TYPE_TRAY, true);

    ++active_trays;
}

void SDL_UnregisterTray(SDL_Tray *tray)
{
    SDL_assert(SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY));

    SDL_SetObjectValid(tray, SDL_OBJECT_TYPE_TRAY, false);

    --active_trays;
    if (active_trays > 0) {
        return;
    }

    if (!SDL_GetHintBoolean(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, true)) {
        return;
    }

    int toplevel_count = 0;
    SDL_Window **windows = SDL_GetWindows(NULL);
    if (windows) {
        for (int i = 0; windows[i]; ++i) {
            SDL_Window *window = windows[i];
            if (!window->parent && !(window->flags & SDL_WINDOW_HIDDEN)) {
                ++toplevel_count;
            }
        }
        SDL_free(windows);
    }

    if (toplevel_count == 0) {
        SDL_SendQuit();
    }
}

void SDL_CleanupTrays(void)
{
    if (active_trays == 0) {
        return;
    }

    void **trays = (void **)SDL_malloc(active_trays * sizeof(*trays));
    if (!trays) {
        return;
    }

    int count = SDL_GetObjects(SDL_OBJECT_TYPE_TRAY, trays, active_trays);
    SDL_assert(count == active_trays);
    for (int i = 0; i < count; ++i) {
        SDL_DestroyTray((SDL_Tray *)trays[i]);
    }
    SDL_free(trays);

    SDL_ClearProperty(SDL_GetGlobalProperties(), SDL_PROP_TRAY_CLEANUP);
}

bool SDL_HasActiveTrays(void)
{
    return (active_trays > 0);
}

int SDL_GetActiveTrayCount(void)
{
    return active_trays;
}

/* Click contexts are synchronous, main-thread-only, and may be nested. */
typedef struct SDL_TrayClickContext
{
    SDL_Tray *tray;
    const SDL_Point *position;
    const char *activation_token;
} SDL_TrayClickContext;

static const SDL_TrayClickContext *current_click;

bool SDL_InvokeTrayClick(SDL_Tray *tray, SDL_TrayClickCallback callback, void *userdata,
                         const SDL_Point *position, const char *activation_token)
{
    const SDL_TrayClickContext context = { tray, position, activation_token };
    const SDL_TrayClickContext *previous = current_click;
    current_click = &context;
    bool show_menu = callback ? callback(userdata, tray) : true;
    current_click = previous;
    return show_menu;
}

bool SDL_GetTrayClickPosition(SDL_Tray *tray, int *x, int *y)
{
    if (!SDL_IsMainThread()) {
        return SDL_SetError("This function should be called on the main thread");
    }
    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        return SDL_InvalidParamError("tray");
    }
    if (!x || !y) {
        return SDL_InvalidParamError(!x ? "x" : "y");
    }
    if (!current_click || current_click->tray != tray || !current_click->position) {
        return false;
    }
    *x = current_click->position->x;
    *y = current_click->position->y;
    return true;
}

const char *SDL_GetTrayActivationToken(void)
{
    return current_click ? current_click->activation_token : NULL;
}

const char *SDL_GetTrayClickActivationToken(SDL_Tray *tray)
{
    if (!SDL_IsMainThread()) {
        SDL_SetError("This function should be called on the main thread");
        return NULL;
    }
    if (!SDL_ObjectValid(tray, SDL_OBJECT_TYPE_TRAY)) {
        SDL_InvalidParamError("tray");
        return NULL;
    }
    return current_click && current_click->tray == tray ? current_click->activation_token : NULL;
}
