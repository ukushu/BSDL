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

// General event handling code for SDL

#include "SDL_events_c.h"
#include "SDL_windowevents_c.h"
#include "../SDL_hints_c.h"
#include "../timer/SDL_timer_c.h"
#include "../video/SDL_sysvideo.h"

#ifdef SDL_PLATFORM_ANDROID
#include "../core/android/SDL_android.h"
#include "../video/android/SDL_androidevents.h"
#endif


// An arbitrary limit so we don't have unbounded growth
#define SDL_MAX_QUEUED_EVENTS 65535

// Make sure the type in the SDL_Event aligns properly across the union
SDL_COMPILE_TIME_ASSERT(SDL_Event_type, sizeof(Uint32) == sizeof(SDL_EventType));

static SDL_AtomicInt SDL_sentinel_pending;

int SDL_GetPendingEventSentinels(void)
{
    return SDL_GetAtomicInt(&SDL_sentinel_pending);
}

typedef struct SDL_EventEntry
{
    SDL_Event event;
    SDL_TemporaryMemory *memory;
    struct SDL_EventEntry *prev;
    struct SDL_EventEntry *next;
} SDL_EventEntry;

static struct
{
    SDL_Mutex *lock;
    bool active;
    SDL_AtomicInt count;
    int max_events_seen;
    SDL_EventEntry *head;
    SDL_EventEntry *tail;
    SDL_EventEntry *free;
} SDL_EventQ = { NULL, false, { 0 }, 0, NULL, NULL, NULL };


SDL_Mutex *SDL_event_lock = NULL; // This needs to support recursive locks

void SDL_CreateEventLock(void)
{
    if (!SDL_event_lock) {
        SDL_event_lock = SDL_CreateMutex();
    }
}

void SDL_DestroyEventLock(void)
{
    if (SDL_event_lock) {
        SDL_DestroyMutex(SDL_event_lock);
        SDL_event_lock = NULL;
    }
}

static void SDLCALL SDL_PollSentinelChanged(void *userdata, const char *name, const char *oldValue, const char *hint)
{
    SDL_SetEventEnabled(SDL_EVENT_POLL_SENTINEL, SDL_GetStringBoolean(hint, true));
}

void SDL_StopEventLoop(void)
{
    const char *report = SDL_GetHint("SDL_EVENT_QUEUE_STATISTICS");
    SDL_EventEntry *entry;
    SDL_Mutex *lock = SDL_EventQ.lock;

    SDL_LockMutex(lock);

    SDL_EventQ.active = false;

    if (report && SDL_atoi(report)) {
        SDL_Log("SDL EVENT QUEUE: Maximum events in-flight: %d",
                SDL_EventQ.max_events_seen);
    }

    // Clean out EventQ
    for (entry = SDL_EventQ.head; entry;) {
        SDL_EventEntry *next = entry->next;
        SDL_TransferTemporaryMemoryFromEvent(&entry->memory);
        SDL_free(entry);
        entry = next;
    }
    for (entry = SDL_EventQ.free; entry;) {
        SDL_EventEntry *next = entry->next;
        SDL_free(entry);
        entry = next;
    }

    SDL_SetAtomicInt(&SDL_EventQ.count, 0);
    SDL_EventQ.max_events_seen = 0;
    SDL_EventQ.head = NULL;
    SDL_EventQ.tail = NULL;
    SDL_EventQ.free = NULL;
    SDL_SetAtomicInt(&SDL_sentinel_pending, 0);

    SDL_QuitEventEnabled();

    SDL_QuitEventWatchers();
    SDL_QuitWindowEventWatch();

    SDL_EventQ.lock = NULL;

    SDL_UnlockMutex(lock);
    SDL_DestroyMutex(lock);
}

// This function (and associated calls) may be called more than once
bool SDL_StartEventLoop(void)
{
    /* We'll leave the event queue alone, since we might have gotten
       some important events at launch (like SDL_EVENT_DROP_FILE)

       FIXME: Does this introduce any other bugs with events at startup?
     */

    // Create the lock and set ourselves active
#ifndef SDL_THREADS_DISABLED
    if (!SDL_EventQ.lock) {
        SDL_EventQ.lock = SDL_CreateMutex();
        if (SDL_EventQ.lock == NULL) {
            return false;
        }
    }
    SDL_LockMutex(SDL_EventQ.lock);
#endif // !SDL_THREADS_DISABLED

    SDL_EventQ.active = true;

#ifndef SDL_THREADS_DISABLED
    SDL_UnlockMutex(SDL_EventQ.lock);
#endif
    return true;
}

// Add an event to the event queue -- called with the queue locked
static int SDL_AddEvent(SDL_Event *event)
{
    SDL_EventEntry *entry;
    const int initial_count = SDL_GetAtomicInt(&SDL_EventQ.count);
    int final_count;

    if (initial_count >= SDL_MAX_QUEUED_EVENTS) {
        SDL_SetError("Event queue is full (%d events)", initial_count);
        return 0;
    }

    if (SDL_EventQ.free == NULL) {
        entry = (SDL_EventEntry *)SDL_malloc(sizeof(*entry));
        if (entry == NULL) {
            return 0;
        }
    } else {
        entry = SDL_EventQ.free;
        SDL_EventQ.free = entry->next;
    }

    if (SDL_EventLoggingVerbosity > 0) {
        SDL_LogEvent(event);
    }

    SDL_copyp(&entry->event, event);
    if (event->type == SDL_EVENT_POLL_SENTINEL) {
        SDL_AddAtomicInt(&SDL_sentinel_pending, 1);
    }
    entry->memory = NULL;
    SDL_TransferTemporaryMemoryToEvent(&entry->memory, &entry->event);

    if (SDL_EventQ.tail) {
        SDL_EventQ.tail->next = entry;
        entry->prev = SDL_EventQ.tail;
        SDL_EventQ.tail = entry;
        entry->next = NULL;
    } else {
        SDL_assert(!SDL_EventQ.head);
        SDL_EventQ.head = entry;
        SDL_EventQ.tail = entry;
        entry->prev = NULL;
        entry->next = NULL;
    }

    final_count = SDL_AddAtomicInt(&SDL_EventQ.count, 1) + 1;
    if (final_count > SDL_EventQ.max_events_seen) {
        SDL_EventQ.max_events_seen = final_count;
    }

    return 1;
}

// Remove an event from the queue -- called with the queue locked
static void SDL_CutEvent(SDL_EventEntry *entry)
{
    SDL_TransferTemporaryMemoryFromEvent(&entry->memory);

    if (entry->prev) {
        entry->prev->next = entry->next;
    }
    if (entry->next) {
        entry->next->prev = entry->prev;
    }

    if (entry == SDL_EventQ.head) {
        SDL_assert(entry->prev == NULL);
        SDL_EventQ.head = entry->next;
    }
    if (entry == SDL_EventQ.tail) {
        SDL_assert(entry->next == NULL);
        SDL_EventQ.tail = entry->prev;
    }

    if (entry->event.type == SDL_EVENT_POLL_SENTINEL) {
        SDL_AddAtomicInt(&SDL_sentinel_pending, -1);
    }

    entry->next = SDL_EventQ.free;
    SDL_EventQ.free = entry;
    SDL_assert(SDL_GetAtomicInt(&SDL_EventQ.count) > 0);
    SDL_AddAtomicInt(&SDL_EventQ.count, -1);
}

void SDL_SendWakeupEvent(void)
{
#ifdef SDL_PLATFORM_ANDROID
    Android_SendLifecycleEvent(SDL_ANDROID_LIFECYCLE_WAKE);
#else
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (_this == NULL || !_this->SendWakeupEvent) {
        return;
    }

    // We only want to do this once while waiting for an event, so set it to NULL atomically here
    SDL_Window *wakeup_window = (SDL_Window *)SDL_SetAtomicPointer(&_this->wakeup_window, NULL);
    if (wakeup_window) {
        _this->SendWakeupEvent(_this, wakeup_window);
    }
#endif
}

// Read queued events while SDL_EventQ.lock is held.
static int SDL_PeepQueuedEvents(SDL_Event *events, int numevents, SDL_EventAction action,
                                Uint32 minType, Uint32 maxType, bool include_sentinel)
{
    int used = 0;
    int sentinels_expected = 0;
    SDL_EventEntry *entry, *next;

    for (entry = SDL_EventQ.head; entry && (events == NULL || used < numevents); entry = next) {
        next = entry->next;
        const Uint32 type = entry->event.type;
        if (minType <= type && type <= maxType) {
            if (events) {
                SDL_copyp(&events[used], &entry->event);

                if (action == SDL_GETEVENT) {
                    SDL_CutEvent(entry);
                }
            }
            if (type == SDL_EVENT_POLL_SENTINEL) {
                if (!include_sentinel) {
                    continue;
                }
                if (events == NULL || action != SDL_GETEVENT) {
                    ++sentinels_expected;
                }
                if (SDL_GetAtomicInt(&SDL_sentinel_pending) > sentinels_expected) {
                    continue;
                }
            }
            ++used;
        }
    }
    return used;
}

// Lock the event queue, take a peep at it, and unlock it
int SDL_PeepEventsInternal(SDL_Event *events, int numevents, SDL_EventAction action,
                                  Uint32 minType, Uint32 maxType, bool include_sentinel)
{
    int i, used;

    // Lock the event queue
    used = 0;

    SDL_LockMutex(SDL_EventQ.lock);
    {
        // Don't look after we've quit
        if (!SDL_EventQ.active) {
            // We get a few spurious events at shutdown, so don't warn then
            if (action == SDL_GETEVENT) {
                SDL_SetError("The event system has been shut down");
            }
            SDL_UnlockMutex(SDL_EventQ.lock);
            return -1;
        }
        if (action == SDL_ADDEVENT) {
            CHECK_PARAM(!events) {
                SDL_UnlockMutex(SDL_EventQ.lock);
                SDL_InvalidParamError("events");
                return -1;
            }
            for (i = 0; i < numevents; ++i) {
                used += SDL_AddEvent(&events[i]);
            }
        } else {
            used = SDL_PeepQueuedEvents(events, numevents, action, minType, maxType, include_sentinel);
        }
    }
    SDL_UnlockMutex(SDL_EventQ.lock);

    if (used > 0 && action == SDL_ADDEVENT) {
        SDL_SendWakeupEvent();
    }

    return used;
}
int SDL_PeepEvents(SDL_Event *events, int numevents, SDL_EventAction action,
                   Uint32 minType, Uint32 maxType)
{
    return SDL_PeepEventsInternal(events, numevents, action, minType, maxType, false);
}

bool SDL_HasEvent(Uint32 type)
{
    return SDL_HasEvents(type, type);
}

bool SDL_HasEvents(Uint32 minType, Uint32 maxType)
{
    bool found = false;

    SDL_LockMutex(SDL_EventQ.lock);
    {
        if (SDL_EventQ.active) {
            for (SDL_EventEntry *entry = SDL_EventQ.head; entry; entry = entry->next) {
                const Uint32 type = entry->event.type;
                if (minType <= type && type <= maxType) {
                    found = true;
                    break;
                }
            }
        }
    }
    SDL_UnlockMutex(SDL_EventQ.lock);

    return found;
}

void SDL_FlushEvent(Uint32 type)
{
    SDL_FlushEvents(type, type);
}

void SDL_FlushEvents(Uint32 minType, Uint32 maxType)
{
    SDL_EventEntry *entry, *next;
    Uint32 type;

    // Don't pump events here: flushing during a resize may trigger nested resize events.

    // Lock the event queue
    SDL_LockMutex(SDL_EventQ.lock);
    {
        // Don't look after we've quit
        if (!SDL_EventQ.active) {
            SDL_UnlockMutex(SDL_EventQ.lock);
            return;
        }
        for (entry = SDL_EventQ.head; entry; entry = next) {
            next = entry->next;
            type = entry->event.type;
            if (minType <= type && type <= maxType) {
                SDL_CutEvent(entry);
            }
        }
    }
    SDL_UnlockMutex(SDL_EventQ.lock);
}

bool SDL_PushEvent(SDL_Event *event)
{
    if (!event->common.timestamp) {
        event->common.timestamp = SDL_GetTicksNS();
    }

    if (!SDL_EventEnabled(event->common.type)) {
        SDL_ClearError();
        return false;
    }

    if (!SDL_DispatchEventWatchers(event)) {
        SDL_ClearError();
        return false;
    }

    if (SDL_PeepEvents(event, 1, SDL_ADDEVENT, 0, 0) <= 0) {
        return false;
    }

    return true;
}

void SDL_FilterEvents(SDL_EventFilter filter, void *userdata)
{
    SDL_LockMutex(SDL_EventQ.lock);
    {
        SDL_EventEntry *entry, *next;
        for (entry = SDL_EventQ.head; entry; entry = next) {
            next = entry->next;
            if (!filter(userdata, &entry->event)) {
                SDL_CutEvent(entry);
            }
        }
    }
    SDL_UnlockMutex(SDL_EventQ.lock);
}

void SDL_SendAppEvent(SDL_EventType eventType)
{
    if (SDL_EventEnabled(eventType)) {
        SDL_Event event;
        event.type = eventType;
        event.common.timestamp = 0;

        switch (eventType) {
        case SDL_EVENT_TERMINATING:
        case SDL_EVENT_LOW_MEMORY:
        case SDL_EVENT_WILL_ENTER_BACKGROUND:
        case SDL_EVENT_DID_ENTER_BACKGROUND:
        case SDL_EVENT_WILL_ENTER_FOREGROUND:
        case SDL_EVENT_DID_ENTER_FOREGROUND:
            // We won't actually queue this event, it needs to be handled in this call stack by an event watcher
            if (SDL_EventLoggingVerbosity > 0) {
                SDL_LogEvent(&event);
            }
            SDL_DispatchEventWatchers(&event);
            break;
        default:
            SDL_PushEvent(&event);
            break;
        }
    }
}

void SDL_SendKeymapChangedEvent(void)
{
    SDL_SendAppEvent(SDL_EVENT_KEYMAP_CHANGED);
}

void SDL_SendLocaleChangedEvent(void)
{
    SDL_SendAppEvent(SDL_EVENT_LOCALE_CHANGED);
}

void SDL_SendSystemThemeChangedEvent(void)
{
    SDL_SendAppEvent(SDL_EVENT_SYSTEM_THEME_CHANGED);
}

bool SDL_InitEvents(void)
{
#ifdef SDL_PLATFORM_ANDROID
    Android_InitEvents();
#endif
    SDL_InitEventPumping();
    SDL_InitEventLogging();
    SDL_AddHintCallback(SDL_HINT_POLL_SENTINEL, SDL_PollSentinelChanged, NULL);
    SDL_InitMainThreadCallbacks();
    if (!SDL_StartEventLoop()) {
        SDL_QuitEventLogging();
        return false;
    }

    SDL_InitQuit();

    return true;
}

void SDL_QuitEvents(void)
{
    SDL_QuitQuit();
    SDL_StopEventLoop();
    SDL_QuitMainThreadCallbacks();
    SDL_RemoveHintCallback(SDL_HINT_POLL_SENTINEL, SDL_PollSentinelChanged, NULL);
    SDL_QuitEventLogging();
    SDL_QuitEventPumping();
#ifdef SDL_PLATFORM_ANDROID
    Android_QuitEvents();
#endif
}
