/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"

#include "SDL_hints_c.h"
#include "SDL_video_c.h"
#include "SDL_sysvideo.h"

// Hint to treat all window ops as synchronous
static bool syncHint;

static void SDL_SyncHintWatcher(void *userdata, const char *name, const char *oldValue, const char *newValue)
{
    syncHint = SDL_GetStringBoolean(newValue, false);
}

void SDL_InitVideoSyncHint(void)
{
    SDL_AddHintCallback(SDL_HINT_VIDEO_SYNC_WINDOW_OPERATIONS, SDL_SyncHintWatcher, NULL);
}

void SDL_SyncIfRequired(SDL_Window *window)
{
    if (syncHint) {
        SDL_SyncWindow(window);
    }
}

bool SDL_GetVideoSyncHint(void)
{
    return syncHint;
}

bool SDL_SyncWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (_this->SyncWindow) {
        return _this->SyncWindow(_this, window);
    } else {
        return true;
    }
}
