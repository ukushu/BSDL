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

typedef enum
{
    SDL_MAIN_CALLBACK_WAITING,
    SDL_MAIN_CALLBACK_COMPLETE,
    SDL_MAIN_CALLBACK_CANCELED,
} SDL_MainThreadCallbackState;

typedef struct SDL_MainThreadCallbackEntry
{
    SDL_MainThreadCallback callback;
    void *userdata;
    SDL_AtomicInt state;
    SDL_Semaphore *semaphore;
    struct SDL_MainThreadCallbackEntry *next;
} SDL_MainThreadCallbackEntry;

static SDL_Mutex *SDL_main_callbacks_lock;
static SDL_MainThreadCallbackEntry *SDL_main_callbacks_head;
static SDL_MainThreadCallbackEntry *SDL_main_callbacks_tail;

static SDL_MainThreadCallbackEntry *SDL_CreateMainThreadCallback(SDL_MainThreadCallback callback, void *userdata, bool wait_complete)
{
    SDL_MainThreadCallbackEntry *entry = (SDL_MainThreadCallbackEntry *)SDL_malloc(sizeof(*entry));
    if (!entry) {
        return NULL;
    }

    entry->callback = callback;
    entry->userdata = userdata;
    SDL_SetAtomicInt(&entry->state, SDL_MAIN_CALLBACK_WAITING);
    if (wait_complete) {
        entry->semaphore = SDL_CreateSemaphore(0);
        if (!entry->semaphore) {
            SDL_free(entry);
            return NULL;
        }
    } else {
        entry->semaphore = NULL;
    }
    entry->next = NULL;

    return entry;
}

static void SDL_DestroyMainThreadCallback(SDL_MainThreadCallbackEntry *entry)
{
    if (entry->semaphore) {
        SDL_DestroySemaphore(entry->semaphore);
    }
    SDL_free(entry);
}

void SDL_InitMainThreadCallbacks(void)
{
    SDL_main_callbacks_lock = SDL_CreateMutex();
    SDL_assert(SDL_main_callbacks_head == NULL &&
               SDL_main_callbacks_tail == NULL);
}

void SDL_QuitMainThreadCallbacks(void)
{
    SDL_MainThreadCallbackEntry *entry;

    SDL_LockMutex(SDL_main_callbacks_lock);
    {
        entry = SDL_main_callbacks_head;
        SDL_main_callbacks_head = NULL;
        SDL_main_callbacks_tail = NULL;
    }
    SDL_UnlockMutex(SDL_main_callbacks_lock);

    while (entry) {
        SDL_MainThreadCallbackEntry *next = entry->next;

        if (entry->semaphore) {
            // Let the waiting thread know this is canceled
            SDL_SetAtomicInt(&entry->state, SDL_MAIN_CALLBACK_CANCELED);
            SDL_SignalSemaphore(entry->semaphore);
        } else {
            // Nobody's waiting for this, clean it up
            SDL_DestroyMainThreadCallback(entry);
        }
        entry = next;
    }

    SDL_DestroyMutex(SDL_main_callbacks_lock);
    SDL_main_callbacks_lock = NULL;
}

void SDL_RunMainThreadCallbacks(void)
{
    SDL_MainThreadCallbackEntry *entry;

    SDL_LockMutex(SDL_main_callbacks_lock);
    {
        entry = SDL_main_callbacks_head;
        SDL_main_callbacks_head = NULL;
        SDL_main_callbacks_tail = NULL;
    }
    SDL_UnlockMutex(SDL_main_callbacks_lock);

    while (entry) {
        SDL_MainThreadCallbackEntry *next = entry->next;

        entry->callback(entry->userdata);

        if (entry->semaphore) {
            // Let the waiting thread know this is done
            SDL_SetAtomicInt(&entry->state, SDL_MAIN_CALLBACK_COMPLETE);
            SDL_SignalSemaphore(entry->semaphore);
        } else {
            // Nobody's waiting for this, clean it up
            SDL_DestroyMainThreadCallback(entry);
        }
        entry = next;
    }
}

bool SDL_RunOnMainThread(SDL_MainThreadCallback callback, void *userdata, bool wait_complete)
{
    if (SDL_IsMainThread() || !SDL_WasInit(SDL_INIT_EVENTS)) {
        // No need to queue the callback
        callback(userdata);
        return true;
    }

    SDL_MainThreadCallbackEntry *entry = SDL_CreateMainThreadCallback(callback, userdata, wait_complete);
    if (!entry) {
        return false;
    }

    SDL_LockMutex(SDL_main_callbacks_lock);
    {
        if (SDL_main_callbacks_tail) {
            SDL_main_callbacks_tail->next = entry;
            SDL_main_callbacks_tail = entry;
        } else {
            SDL_main_callbacks_head = entry;
            SDL_main_callbacks_tail = entry;
        }
    }
    SDL_UnlockMutex(SDL_main_callbacks_lock);

    // If the main thread is waiting for events, wake it up
    SDL_SendWakeupEvent();

    if (!wait_complete) {
        // Queued for execution, wait not requested
        return true;
    }

    SDL_WaitSemaphore(entry->semaphore);

    switch (SDL_GetAtomicInt(&entry->state)) {
    case SDL_MAIN_CALLBACK_COMPLETE:
        // Execution complete!
        SDL_DestroyMainThreadCallback(entry);
        return true;

    case SDL_MAIN_CALLBACK_CANCELED:
        // The callback was canceled on the main thread
        SDL_DestroyMainThreadCallback(entry);
        return SDL_SetError("Callback canceled");

    default:
        // Probably hit a deadlock in the callback
        // We can't destroy the entry as the semaphore will be signaled
        // if it ever comes back, just leak it here.
        return SDL_SetError("Callback timed out");
    }
}

