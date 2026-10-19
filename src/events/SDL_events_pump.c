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
#include "../SDL_hints_c.h"
#include "../audio/SDL_audio_c.h"
#include "../camera/SDL_camera_c.h"
#include "../timer/SDL_timer_c.h"
#include "../core/linux/SDL_udev.h"
#ifndef SDL_JOYSTICK_DISABLED
#include "../joystick/SDL_joystick_c.h"
#endif
#ifndef SDL_SENSOR_DISABLED
#include "../sensor/SDL_sensor_c.h"
#endif
#ifdef HAVE_DBUS_DBUS_H
#include "core/linux/SDL_dbus.h"
#endif
#ifdef SDL_PLATFORM_ANDROID
#include "../core/android/SDL_android.h"
#include "../video/android/SDL_androidevents.h"
#endif
#ifdef SDL_PLATFORM_UNIX
#include "../tray/SDL_tray_utils.h"
#endif

#define ENUMERATION_POLL_INTERVAL_NS (3 * SDL_NS_PER_SECOND)
#define EVENT_POLL_INTERVAL_NS SDL_MS_TO_NS(1)
#define TRAY_POLL_INTERVAL_NS SDL_MS_TO_NS(50)
#define DBUS_POLL_INTERVAL_NS (3 * SDL_NS_PER_SECOND)

#ifndef SDL_JOYSTICK_DISABLED

static bool SDL_update_joysticks = true;

static void SDLCALL SDL_AutoUpdateJoysticksChanged(void *userdata, const char *name, const char *oldValue, const char *hint)
{
    SDL_update_joysticks = SDL_GetStringBoolean(hint, true);
}

#endif // !SDL_JOYSTICK_DISABLED

#ifndef SDL_SENSOR_DISABLED

static bool SDL_update_sensors = true;

static void SDLCALL SDL_AutoUpdateSensorsChanged(void *userdata, const char *name, const char *oldValue, const char *hint)
{
    SDL_update_sensors = SDL_GetStringBoolean(hint, true);
}

#endif // !SDL_SENSOR_DISABLED

void SDL_InitEventPumping(void)
{
#ifndef SDL_JOYSTICK_DISABLED
    SDL_AddHintCallback(SDL_HINT_AUTO_UPDATE_JOYSTICKS, SDL_AutoUpdateJoysticksChanged, NULL);
#endif
#ifndef SDL_SENSOR_DISABLED
    SDL_AddHintCallback(SDL_HINT_AUTO_UPDATE_SENSORS, SDL_AutoUpdateSensorsChanged, NULL);
#endif
}

void SDL_QuitEventPumping(void)
{
#ifndef SDL_JOYSTICK_DISABLED
    SDL_RemoveHintCallback(SDL_HINT_AUTO_UPDATE_JOYSTICKS, SDL_AutoUpdateJoysticksChanged, NULL);
#endif
#ifndef SDL_SENSOR_DISABLED
    SDL_RemoveHintCallback(SDL_HINT_AUTO_UPDATE_SENSORS, SDL_AutoUpdateSensorsChanged, NULL);
#endif
}

void SDL_PumpEventMaintenance(void)
{
#ifdef SDL_USE_LIBUDEV
    SDL_UDEV_Poll();
#endif

#ifndef SDL_AUDIO_DISABLED
    SDL_UpdateAudio();
#endif

#ifndef SDL_CAMERA_DISABLED
    SDL_UpdateCamera();
#endif

#ifndef SDL_SENSOR_DISABLED
    // Check for sensor state change
    if (SDL_update_sensors) {
        SDL_UpdateSensors();
    }
#endif

#ifndef SDL_JOYSTICK_DISABLED
    // Check for joystick state change
    if (SDL_update_joysticks) {
        SDL_UpdateJoysticks();
    }
#endif

    SDL_SendPendingPenProximity();

    SDL_UpdateCursorAnimation();

    SDL_UpdateTrays();

    SDL_SendPendingSignalEvents(); // in case we had a signal handler fire, etc.
}

// Run the system dependent event loops
static void SDL_PumpEventsInternal(bool push_sentinel)
{
    // This should only be called on the main thread, check in debug builds
    SDL_assert(SDL_IsMainThread());

    // Free any temporary memory from old events
    SDL_FreeTemporaryMemory();

    // Release any keys held down from last frame
    SDL_ReleaseAutoReleaseKeys();

    // Run any pending main thread callbacks
    SDL_RunMainThreadCallbacks();

#ifdef SDL_USE_LIBDBUS
    // DBus event processing is independent of the video subsystem
    SDL_DBus_PumpEvents();
#endif

#ifdef SDL_PLATFORM_ANDROID
    // Android event processing is independent of the video subsystem
    Android_PumpEvents(0);
#else
    // Get events from the video subsystem
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (_this) {
        _this->PumpEvents(_this);
    }
#endif

    SDL_PumpEventMaintenance();

    if (push_sentinel && SDL_EventEnabled(SDL_EVENT_POLL_SENTINEL)) {
        SDL_Event sentinel;

        // Make sure we don't already have a sentinel in the queue, and add one to the end
        if (SDL_GetPendingEventSentinels() > 0) {
            SDL_PeepEventsInternal(&sentinel, 1, SDL_GETEVENT, SDL_EVENT_POLL_SENTINEL, SDL_EVENT_POLL_SENTINEL, true);
        }

        sentinel.type = SDL_EVENT_POLL_SENTINEL;
        sentinel.common.timestamp = 0;
        SDL_PushEvent(&sentinel);
    }
}

void SDL_PumpEvents(void)
{
    SDL_PumpEventsInternal(false);
}

// Public functions

bool SDL_PollEvent(SDL_Event *event)
{
    return SDL_WaitEventTimeoutNS(event, 0);
}

#ifndef SDL_PLATFORM_ANDROID

static Sint64 SDL_events_get_polling_interval(void)
{
    Sint64 poll_intervalNS = SDL_MAX_SINT64;

#ifndef SDL_JOYSTICK_DISABLED
    if (SDL_WasInit(SDL_INIT_JOYSTICK) && SDL_update_joysticks) {
        if (SDL_JoysticksOpened()) {
            // If we have joysticks open, we need to poll rapidly for events
            poll_intervalNS = SDL_min(poll_intervalNS, EVENT_POLL_INTERVAL_NS);
        } else {
            // If not, just poll every few seconds to enumerate new joysticks
            poll_intervalNS = SDL_min(poll_intervalNS, ENUMERATION_POLL_INTERVAL_NS);
        }
    }
#endif

#ifndef SDL_SENSOR_DISABLED
    if (SDL_WasInit(SDL_INIT_SENSOR) && SDL_update_sensors && SDL_SensorsOpened()) {
        // If we have sensors open, we need to poll rapidly for events
        poll_intervalNS = SDL_min(poll_intervalNS, EVENT_POLL_INTERVAL_NS);
    }
#endif

#ifdef SDL_PLATFORM_UNIX
    if (SDL_HasActiveTrays()) {
        // Tray events on *nix platforms run separately from window system events, and need periodic polling
        poll_intervalNS = SDL_min(poll_intervalNS, TRAY_POLL_INTERVAL_NS);
    }
#endif

#ifdef SDL_USE_LIBDBUS
    // Wake periodically to pump DBus events
    poll_intervalNS = SDL_min(poll_intervalNS, DBUS_POLL_INTERVAL_NS);
#endif
    return poll_intervalNS;
}

static int SDL_WaitEventTimeout_Device(SDL_VideoDevice *_this, SDL_Window *wakeup_window, SDL_Event *event, Uint64 start, Sint64 timeoutNS)
{
    Sint64 loop_timeoutNS = timeoutNS;
    Sint64 poll_intervalNS = SDL_events_get_polling_interval();

    for (;;) {
        int status;
        /* Pump events on entry and each time we wake to ensure:
           a) All pending events are batch processed after waking up from a wait
           b) Waiting can be completely skipped if events are already available to be pumped
           c) Periodic processing that takes place in some platform PumpEvents() functions happens
           d) Signals received in WaitEventTimeout() are turned into SDL events
        */
        SDL_PumpEventsInternal(true);

        status = SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST);
        if (status < 0) {
            // Got an error: return
            break;
        }
        if (status > 0) {
            // There is an event, we can return.
            return 1;
        }
        // No events found in the queue, call WaitEventTimeout to wait for an event.
        if (timeoutNS > 0) {
            Sint64 elapsed = SDL_GetTicksNS() - start;
            if (elapsed >= timeoutNS) {
                return 0;
            }
            loop_timeoutNS = (timeoutNS - elapsed);
        }
        // Adjust the timeout for any polling requirements we currently have.
        if (poll_intervalNS != SDL_MAX_SINT64) {
            if (loop_timeoutNS >= 0) {
                loop_timeoutNS = SDL_min(loop_timeoutNS, poll_intervalNS);
            } else {
                loop_timeoutNS = poll_intervalNS;
            }
        }
        SDL_SetAtomicPointer(&_this->wakeup_window, wakeup_window);
        status = _this->WaitEventTimeout(_this, loop_timeoutNS);
        SDL_SetAtomicPointer(&_this->wakeup_window, NULL);
        if (status == 0 && poll_intervalNS != SDL_MAX_SINT64 && loop_timeoutNS == poll_intervalNS) {
            // We may have woken up to poll. Try again
            continue;
        } else if (status <= 0) {
            // There is either an error or the timeout is elapsed: return
            return status;
        }
        /* An event was found and pumped into the SDL events queue. Continue the loop
          to let SDL_PeepEvents pick it up .*/
    }
    return 0;
}

static SDL_Window *SDL_find_active_window(SDL_VideoDevice *_this)
{
    SDL_Window *window;
    for (window = _this->windows; window; window = window->next) {
        if (!window->is_destroying) {
            return window;
        }
    }
    return NULL;
}

#endif // !SDL_PLATFORM_ANDROID

bool SDL_WaitEvent(SDL_Event *event)
{
    return SDL_WaitEventTimeoutNS(event, -1);
}

bool SDL_WaitEventTimeout(SDL_Event *event, Sint32 timeoutMS)
{
    Sint64 timeoutNS;

    if (timeoutMS > 0) {
        timeoutNS = SDL_MS_TO_NS(timeoutMS);
    } else {
        timeoutNS = timeoutMS;
    }
    return SDL_WaitEventTimeoutNS(event, timeoutNS);
}

bool SDL_WaitEventTimeoutNS(SDL_Event *event, Sint64 timeoutNS)
{
    Uint64 start, expiration;
    bool include_sentinel = (timeoutNS == 0);
    int result;

    if (timeoutNS > 0) {
        start = SDL_GetTicksNS();
        expiration = start + timeoutNS;
    } else {
        start = 0;
        expiration = 0;
    }

    // If there isn't a poll sentinel event pending, pump events and add one
    if (SDL_GetPendingEventSentinels() == 0) {
        SDL_PumpEventsInternal(true);
    }

    // First check for existing events
    result = SDL_PeepEventsInternal(event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST, include_sentinel);
    if (result < 0) {
        return false;
    }
    if (include_sentinel) {
        if (event) {
            if (event->type == SDL_EVENT_POLL_SENTINEL) {
                // Reached the end of a poll cycle, and not willing to wait
                return false;
            }
        } else {
            // Need to peek the next event to check for sentinel
            SDL_Event dummy;

            if (SDL_PeepEventsInternal(&dummy, 1, SDL_PEEKEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST, true) &&
                dummy.type == SDL_EVENT_POLL_SENTINEL) {
                SDL_PeepEventsInternal(&dummy, 1, SDL_GETEVENT, SDL_EVENT_POLL_SENTINEL, SDL_EVENT_POLL_SENTINEL, true);
                // Reached the end of a poll cycle, and not willing to wait
                return false;
            }
        }
    }
    if (result == 0) {
        if (timeoutNS == 0) {
            // No events available, and not willing to wait
            return false;
        }
    } else {
        // Has existing events
        if (!event) {
            // The application expects callbacks to run even if events are pending
            SDL_RunMainThreadCallbacks();
        }
        return true;
    }
    // We should have completely handled timeoutNS == 0 above
    SDL_assert(timeoutNS != 0);

#ifdef SDL_PLATFORM_ANDROID
    for (;;) {
        SDL_PumpEventsInternal(true);

        if (SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) > 0) {
            return true;
        }

        Uint64 delay = -1;
        if (timeoutNS > 0) {
            Uint64 now = SDL_GetTicksNS();
            if (now >= expiration) {
                // Timeout expired and no events
                return false;
            }
            delay = (expiration - now);
        }
        Android_PumpEvents(delay);
    }
#else
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (_this && _this->WaitEventTimeout && _this->SendWakeupEvent) {
        // Look if a shown window is available to send the wakeup event.
        SDL_Window *wakeup_window = SDL_find_active_window(_this);
        if (wakeup_window) {
            result = SDL_WaitEventTimeout_Device(_this, wakeup_window, event, start, timeoutNS);
            if (result > 0) {
                return true;
            } else if (result == 0) {
                return false;
            } else {
                /* There may be implementation-defined conditions where the backend cannot
                 * reliably wait for the next event. If that happens, fall back to polling.
                 */
            }
        }
    }

    for (;;) {
        SDL_PumpEventsInternal(true);

        if (SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST) > 0) {
            return true;
        }

        Uint64 delay = EVENT_POLL_INTERVAL_NS;
        if (timeoutNS > 0) {
            Uint64 now = SDL_GetTicksNS();
            if (now >= expiration) {
                // Timeout expired and no events
                return false;
            }
            delay = SDL_min((expiration - now), delay);
        }
        SDL_DelayNS(delay);
    }
#endif // SDL_PLATFORM_ANDROID
}

