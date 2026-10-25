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

#include "SDL_sysjoystick.h"
#include "SDL_joystick_c.h"

static void SDL_LoadVIDPIDListFromHint(const char *hint, int *num_entries, int *max_entries, Uint32 **entries)
{
    Uint32 entry;
    char *spot;
    char *file = NULL;

    if (hint && *hint == '@') {
        spot = file = (char *)SDL_LoadFile(hint + 1, NULL);
    } else {
        spot = (char *)hint;
    }

    if (!spot) {
        return;
    }

    while ((spot = SDL_strstr(spot, "0x")) != NULL) {
        entry = (Uint16)SDL_strtol(spot, &spot, 0);
        entry <<= 16;
        spot = SDL_strstr(spot, "0x");
        if (!spot) {
            break;
        }
        entry |= (Uint16)SDL_strtol(spot, &spot, 0);

        if (*num_entries == *max_entries) {
            int new_max_entries = *max_entries + 16;
            Uint32 *new_entries = (Uint32 *)SDL_realloc(*entries, new_max_entries * sizeof(**entries));
            if (!new_entries) {
                // Out of memory, go with what we have already
                break;
            }
            *entries = new_entries;
            *max_entries = new_max_entries;
        }
        (*entries)[(*num_entries)++] = entry;
    }

    SDL_free(file);
}

void SDL_LoadVIDPIDListFromHints(SDL_vidpid_list *list, const char *included_list, const char *excluded_list)
{
    if (list->included_entries) {
        SDL_free(list->included_entries);
        list->included_entries = NULL;
        list->max_included_entries = 0;
    }

    // Empty the list
    list->num_included_entries = 0;
    list->num_excluded_entries = 0;

    // Add the initial entries
    if (list->num_initial_entries > 0) {
        if (list->num_included_entries < list->num_initial_entries) {
            Uint32 *entries = (Uint32 *)SDL_malloc(list->num_initial_entries * sizeof(*entries));
            if (entries) {
                SDL_memcpy(entries, list->initial_entries, list->num_initial_entries * sizeof(*entries));
                list->included_entries = entries;
                list->num_included_entries = list->num_initial_entries;
                list->max_included_entries = list->num_initial_entries;
            }
        }
    }

    // Add the included entries from the hint
    SDL_LoadVIDPIDListFromHint(included_list, &list->num_included_entries, &list->max_included_entries, &list->included_entries);

    // Add the excluded entries from the hint
    SDL_LoadVIDPIDListFromHint(excluded_list, &list->num_excluded_entries, &list->max_excluded_entries, &list->excluded_entries);
}

static void SDLCALL SDL_VIDPIDIncludedHintChanged(void *userdata, const char *name, const char *oldValue, const char *hint)
{
    SDL_vidpid_list *list = (SDL_vidpid_list *)userdata;
    const char *included_list = hint;
    const char *excluded_list = NULL;

    if (!list->initialized) {
        return;
    }

    if (list->excluded_hint_name) {
        excluded_list = SDL_GetHint(list->excluded_hint_name);
    }
    SDL_LoadVIDPIDListFromHints(list, included_list, excluded_list);
}

static void SDLCALL SDL_VIDPIDExcludedHintChanged(void *userdata, const char *name, const char *oldValue, const char *hint)
{
    SDL_vidpid_list *list = (SDL_vidpid_list *)userdata;
    const char *included_list = NULL;
    const char *excluded_list = hint;

    if (!list->initialized) {
        return;
    }

    if (list->included_hint_name) {
        included_list = SDL_GetHint(list->included_hint_name);
    }
    SDL_LoadVIDPIDListFromHints(list, included_list, excluded_list);
}

void SDL_LoadVIDPIDList(SDL_vidpid_list *list)
{
    const char *included_list = NULL;
    const char *excluded_list = NULL;

    if (list->included_hint_name) {
        SDL_AddHintCallback(list->included_hint_name, SDL_VIDPIDIncludedHintChanged, list);
    }

    if (list->excluded_hint_name) {
        SDL_AddHintCallback(list->excluded_hint_name, SDL_VIDPIDExcludedHintChanged, list);
    }

    list->initialized = true;

    if (list->included_hint_name) {
        included_list = SDL_GetHint(list->included_hint_name);
        if (!included_list) {
            included_list = SDL_getenv_unsafe(list->included_hint_name);
        }
    }
    if (list->excluded_hint_name) {
        excluded_list = SDL_GetHint(list->excluded_hint_name);
        if (!excluded_list) {
            excluded_list = SDL_getenv_unsafe(list->excluded_hint_name);
        }
    }
    SDL_LoadVIDPIDListFromHints(list, included_list, excluded_list);
}

bool SDL_VIDPIDInList(Uint16 vendor_id, Uint16 product_id, const SDL_vidpid_list *list)
{
    int i;
    Uint32 vidpid = MAKE_VIDPID(vendor_id, product_id);

    for (i = 0; i < list->num_excluded_entries; ++i) {
        if (vidpid == list->excluded_entries[i]) {
            return false;
        }
    }
    for (i = 0; i < list->num_included_entries; ++i) {
        if (vidpid == list->included_entries[i]) {
            return true;
        }
    }
    return false;
}

void SDL_FreeVIDPIDList(SDL_vidpid_list *list)
{
    if (list->included_hint_name) {
        SDL_RemoveHintCallback(list->included_hint_name, SDL_VIDPIDIncludedHintChanged, list);
    }

    if (list->excluded_hint_name) {
        SDL_RemoveHintCallback(list->excluded_hint_name, SDL_VIDPIDExcludedHintChanged, list);
    }

    if (list->included_entries) {
        SDL_free(list->included_entries);
        list->included_entries = NULL;
        list->num_included_entries = 0;
        list->max_included_entries = 0;
    }

    if (list->excluded_entries) {
        SDL_free(list->excluded_entries);
        list->excluded_entries = NULL;
        list->num_excluded_entries = 0;
        list->max_excluded_entries = 0;
    }

    list->initialized = false;
}
