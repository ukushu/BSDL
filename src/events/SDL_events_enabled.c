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

#include "SDL_events_c.h"

typedef struct
{
    Uint32 bits[8];
} SDL_DisabledEventBlock;

static SDL_DisabledEventBlock *SDL_disabled_events[256];
static SDL_AtomicInt SDL_userevents;

void SDL_QuitEventEnabled(void)
{
    for (int i = 0; i < SDL_arraysize(SDL_disabled_events); ++i) {
        SDL_free(SDL_disabled_events[i]);
        SDL_disabled_events[i] = NULL;
    }
}

void SDL_SetEventEnabled(Uint32 type, bool enabled)
{
    bool current_state;
    Uint8 hi = ((type >> 8) & 0xff);
    Uint8 lo = (type & 0xff);

    if (SDL_disabled_events[hi] &&
        (SDL_disabled_events[hi]->bits[lo / 32] & (1U << (lo & 31)))) {
        current_state = false;
    } else {
        current_state = true;
    }

    if ((enabled != false) != current_state) {
        if (enabled) {
            SDL_assert(SDL_disabled_events[hi] != NULL);
            SDL_disabled_events[hi]->bits[lo / 32] &= ~(1U << (lo & 31));

            // Gamepad events depend on joystick events
            switch (type) {
            case SDL_EVENT_GAMEPAD_ADDED:
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_ADDED, true);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_REMOVED, true);
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_AXIS_MOTION, true);
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_HAT_MOTION, true);
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_BUTTON_DOWN, true);
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_BUTTON_UP, true);
                break;
            case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
                SDL_SetEventEnabled(SDL_EVENT_JOYSTICK_UPDATE_COMPLETE, true);
                break;
            default:
                break;
            }
        } else {
            // Disable this event type and discard pending events
            if (!SDL_disabled_events[hi]) {
                SDL_disabled_events[hi] = (SDL_DisabledEventBlock *)SDL_calloc(1, sizeof(SDL_DisabledEventBlock));
            }
            // Out of memory, nothing we can do...
            if (SDL_disabled_events[hi]) {
                SDL_disabled_events[hi]->bits[lo / 32] |= (1U << (lo & 31));
                SDL_FlushEvent(type);
            }
        }

        /* turn off drag'n'drop support if we've disabled the events.
           This might change some UI details at the OS level. */
        if (type == SDL_EVENT_DROP_FILE || type == SDL_EVENT_DROP_TEXT) {
            SDL_ToggleDragAndDropSupport();
        }
    }
}

bool SDL_EventEnabled(Uint32 type)
{
    Uint8 hi = ((type >> 8) & 0xff);
    Uint8 lo = (type & 0xff);

    if (SDL_disabled_events[hi] &&
        (SDL_disabled_events[hi]->bits[lo / 32] & (1U << (lo & 31)))) {
        return false;
    } else {
        return true;
    }
}

Uint32 SDL_RegisterEvents(int numevents)
{
    Uint32 event_base = 0;

    if (numevents > 0) {
        int value = SDL_AddAtomicInt(&SDL_userevents, numevents);
        if (value >= 0 && value <= (SDL_EVENT_LAST - SDL_EVENT_USER)) {
            event_base = (Uint32)(SDL_EVENT_USER + value);
        }
    }
    return event_base;
}
