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

#ifdef SDL_VIDEO_DRIVER_WINDOWS
#include "../core/windows/SDL_windows.h"
#endif

#ifdef SDL_VIDEO_DRIVER_X11
#include <X11/Xlib.h>
#endif

typedef struct SDL2_version
{
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDL2_version;

typedef enum
{
    SDL2_SYSWM_UNKNOWN
} SDL2_SYSWM_TYPE;

typedef struct SDL2_SysWMmsg
{
    SDL2_version version;
    SDL2_SYSWM_TYPE subsystem;
    union
    {
#ifdef SDL_VIDEO_DRIVER_WINDOWS
        struct {
            HWND hwnd;
            UINT msg;
            WPARAM wParam;
            LPARAM lParam;
        } win;
#endif
#ifdef SDL_VIDEO_DRIVER_X11
        struct {
            XEvent event;
        } x11;
#endif
        /* Can't have an empty union */
        int dummy;
    } msg;
} SDL2_SysWMmsg;

struct SDL_TemporaryMemory
{
    void *memory;
    struct SDL_TemporaryMemory *prev;
    struct SDL_TemporaryMemory *next;
};

typedef struct SDL_TemporaryMemoryState
{
    SDL_TemporaryMemory *head;
    SDL_TemporaryMemory *tail;
} SDL_TemporaryMemoryState;

static SDL_TLSID SDL_temporary_memory;

static void SDL_CleanupTemporaryMemory(void *data)
{
    SDL_TemporaryMemoryState *state = (SDL_TemporaryMemoryState *)data;

    SDL_FreeTemporaryMemory();
    SDL_free(state);
}

static SDL_TemporaryMemoryState *SDL_GetTemporaryMemoryState(bool create)
{
    SDL_TemporaryMemoryState *state;

    state = (SDL_TemporaryMemoryState *)SDL_GetTLS(&SDL_temporary_memory);
    if (!state) {
        if (!create) {
            return NULL;
        }

        state = (SDL_TemporaryMemoryState *)SDL_calloc(1, sizeof(*state));
        if (!state) {
            return NULL;
        }

        if (!SDL_SetTLS(&SDL_temporary_memory, state, SDL_CleanupTemporaryMemory)) {
            SDL_free(state);
            return NULL;
        }
    }
    return state;
}

static SDL_TemporaryMemory *SDL_GetTemporaryMemoryEntry(SDL_TemporaryMemoryState *state, const void *mem)
{
    SDL_TemporaryMemory *entry;

    // Start from the end, it's likely to have been recently allocated
    for (entry = state->tail; entry; entry = entry->prev) {
        if (mem == entry->memory) {
            return entry;
        }
    }
    return NULL;
}

static void SDL_LinkTemporaryMemoryEntry(SDL_TemporaryMemoryState *state, SDL_TemporaryMemory *entry)
{
    entry->prev = state->tail;
    entry->next = NULL;

    if (state->tail) {
        state->tail->next = entry;
    } else {
        state->head = entry;
    }
    state->tail = entry;
}

static void SDL_FreeTemporaryMemoryList(SDL_TemporaryMemory *entry)
{
    while (entry) {
        SDL_TemporaryMemory *next = entry->next;
        SDL_free(entry->memory);
        SDL_free(entry);
        entry = next;
    }
}

static void SDL_UnlinkTemporaryMemoryEntry(SDL_TemporaryMemoryState *state, SDL_TemporaryMemory *entry)
{
    if (state->head == entry) {
        state->head = entry->next;
    }
    if (state->tail == entry) {
        state->tail = entry->prev;
    }

    if (entry->prev) {
        entry->prev->next = entry->next;
    }
    if (entry->next) {
        entry->next->prev = entry->prev;
    }

    entry->prev = NULL;
    entry->next = NULL;
}

void SDL_AttachTemporaryMemory(SDL_TemporaryMemory **event_memory, const void *mem)
{
    SDL_TemporaryMemoryState *state;
    SDL_TemporaryMemory *entry;

    state = SDL_GetTemporaryMemoryState(false);
    if (!state) {
        return;
    }

    entry = SDL_GetTemporaryMemoryEntry(state, mem);
    if (entry) {
        SDL_UnlinkTemporaryMemoryEntry(state, entry);
        entry->next = *event_memory;
        *event_memory = entry;
    }
}

static void SDL_TransferSysWMMemoryToEvent(SDL_TemporaryMemory **event_memory, SDL_Event *event)
{
    SDL2_SysWMmsg **wmmsg = (SDL2_SysWMmsg **)((&event->common) + 1);
    SDL2_SysWMmsg *mem = SDL_AllocateTemporaryMemory(sizeof(*mem));
    if (mem) {
        SDL_copyp(mem, *wmmsg);
        *wmmsg = mem;
        SDL_AttachTemporaryMemory(event_memory, mem);
    }
}

void SDL_TransferTemporaryMemoryToEvent(SDL_TemporaryMemory **event_memory, SDL_Event *event)
{
    switch (event->type) {
    case SDL_EVENT_TEXT_EDITING:
        SDL_AttachTemporaryMemory(event_memory, event->edit.text);
        break;
    case SDL_EVENT_TEXT_EDITING_CANDIDATES:
        SDL_AttachTemporaryMemory(event_memory, event->edit_candidates.candidates);
        break;
    case SDL_EVENT_TEXT_INPUT:
        SDL_AttachTemporaryMemory(event_memory, event->text.text);
        break;
    case SDL_EVENT_DROP_BEGIN:
    case SDL_EVENT_DROP_FILE:
    case SDL_EVENT_DROP_TEXT:
    case SDL_EVENT_DROP_COMPLETE:
    case SDL_EVENT_DROP_POSITION:
        SDL_AttachTemporaryMemory(event_memory, event->drop.source);
        SDL_AttachTemporaryMemory(event_memory, event->drop.data);
        break;
    case SDL_EVENT_CLIPBOARD_UPDATE:
        SDL_AttachTemporaryMemory(event_memory, event->clipboard.mime_types);
        break;
    case SDL2_SYSWMEVENT:
        // We need to copy the stack pointer into temporary memory
        SDL_TransferSysWMMemoryToEvent(event_memory, event);
        break;
    default:
        break;
    }
}

void SDL_TransferTemporaryMemoryFromEvent(SDL_TemporaryMemory **event_memory)
{
    SDL_TemporaryMemoryState *state;
    SDL_TemporaryMemory *entry, *next;

    if (!*event_memory) {
        return;
    }

    state = SDL_GetTemporaryMemoryState(true);
    if (!state) {
        SDL_FreeTemporaryMemoryList(*event_memory);
        *event_memory = NULL;
        return;
    }

    for (entry = *event_memory; entry; entry = next) {
        next = entry->next;
        SDL_LinkTemporaryMemoryEntry(state, entry);
    }
    *event_memory = NULL;
}

static void *SDL_FreeLater(void *memory)
{
    SDL_TemporaryMemoryState *state;

    if (memory == NULL) {
        return NULL;
    }

    // Make sure we're not adding this to the list twice
    //SDL_assert(!SDL_ClaimTemporaryMemory(memory));

    state = SDL_GetTemporaryMemoryState(true);
    if (!state) {
        SDL_free(memory);
        return NULL;
    }

    SDL_TemporaryMemory *entry = (SDL_TemporaryMemory *)SDL_malloc(sizeof(*entry));
    if (!entry) {
        SDL_free(memory);
        return NULL;
    }

    entry->memory = memory;

    SDL_LinkTemporaryMemoryEntry(state, entry);

    return memory;
}

void *SDL_AllocateTemporaryMemory(size_t size)
{
    return SDL_FreeLater(SDL_malloc(size));
}

const char *SDL_CreateTemporaryString(const char *string)
{
    if (string) {
        return (const char *)SDL_FreeLater(SDL_strdup(string));
    }
    return NULL;
}

void *SDL_ClaimTemporaryMemory(const void *mem)
{
    SDL_TemporaryMemoryState *state;

    state = SDL_GetTemporaryMemoryState(false);
    if (state && mem) {
        SDL_TemporaryMemory *entry = SDL_GetTemporaryMemoryEntry(state, mem);
        if (entry) {
            SDL_UnlinkTemporaryMemoryEntry(state, entry);
            SDL_free(entry);
            return (void *)mem;
        }
    }
    return NULL;
}

void SDL_FreeTemporaryMemory(void)
{
    SDL_TemporaryMemoryState *state;

    state = SDL_GetTemporaryMemoryState(false);
    if (!state) {
        return;
    }

    while (state->head) {
        SDL_TemporaryMemory *entry = state->head;

        SDL_UnlinkTemporaryMemoryEntry(state, entry);
        SDL_free(entry->memory);
        SDL_free(entry);
    }
}
