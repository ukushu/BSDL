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

#include "SDL_joystick_c.h"
#include "SDL_gamepad_c.h"
#include "../SDL_hints_c.h"

#ifdef SDL_PLATFORM_WIN32
#include "../core/windows/SDL_windows.h"
#endif

static SDL_vidpid_list SDL_allowed_gamepads = {
    SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES_EXCEPT, 0, 0, NULL,
    NULL, 0, 0, NULL,
    0, NULL,
    false
};
static SDL_vidpid_list SDL_ignored_gamepads = {
    SDL_HINT_GAMECONTROLLER_IGNORE_DEVICES, 0, 0, NULL,
    NULL, 0, 0, NULL,
    0, NULL,
    false
};

/*
    List of words in gamepad names that indicate that the gamepad should not be detected.
    See also `initial_blacklist_devices` in SDL_joystick.c
*/

enum SDL_GamepadBlacklistWordsPosition {
    GAMEPAD_BLACKLIST_BEGIN,
    GAMEPAD_BLACKLIST_END,
    GAMEPAD_BLACKLIST_ANYWHERE,
};

struct SDL_GamepadBlacklistWords {
    const char* str;
    enum SDL_GamepadBlacklistWordsPosition pos;
};

static const struct SDL_GamepadBlacklistWords SDL_gamepad_blacklist_words[] = {
#ifdef SDL_PLATFORM_LINUX
    {" Motion Sensors", GAMEPAD_BLACKLIST_END}, // Don't treat the PS3 and PS4 motion controls as a separate gamepad
    {" IMU",            GAMEPAD_BLACKLIST_END}, // Don't treat the Nintendo IMU as a separate gamepad
    {" Touchpad",       GAMEPAD_BLACKLIST_END}, // "Sony Interactive Entertainment DualSense Wireless Controller Touchpad"

    // Don't treat the Wii extension controls as a separate gamepad
    {" Accelerometer",  GAMEPAD_BLACKLIST_END},
    {" IR",             GAMEPAD_BLACKLIST_END},
    {" Motion Plus",    GAMEPAD_BLACKLIST_END},
    {" Nunchuk",        GAMEPAD_BLACKLIST_END},
#endif

    // The Google Pixel fingerprint sensor, as well as other fingerprint sensors, reports itself as a joystick
    {"uinput-",         GAMEPAD_BLACKLIST_BEGIN},

    // The IR receiver on the NVIDIA Shield TV
    {"gpio_ir_recv",    GAMEPAD_BLACKLIST_BEGIN},

    {"Synaptics ",      GAMEPAD_BLACKLIST_ANYWHERE}, // "Synaptics TM2768-001", "SynPS/2 Synaptics TouchPad"
    {"Trackpad",        GAMEPAD_BLACKLIST_ANYWHERE},
    {"Clickpad",        GAMEPAD_BLACKLIST_ANYWHERE},
    // "Usb Keyboard Usb Keyboard Consumer Control", "Framework Laptop 16 Keyboard Module - ISO System Control"
    {" Keyboard",       GAMEPAD_BLACKLIST_ANYWHERE},
    {" Laptop ",        GAMEPAD_BLACKLIST_ANYWHERE}, // "Framework Laptop 16 Numpad Module System Control"
    {"Mouse ",          GAMEPAD_BLACKLIST_BEGIN}, // "Mouse passthrough"
    {" Pen",            GAMEPAD_BLACKLIST_END}, // "Wacom One by Wacom S Pen"
    {" Finger",         GAMEPAD_BLACKLIST_END}, // "Wacom HID 495F Finger"
    {" System Control", GAMEPAD_BLACKLIST_END}, // "hid-over-i2c 0107 System Control"
    {" LED ",           GAMEPAD_BLACKLIST_ANYWHERE}, // "ASRock LED Controller"
    {" Thelio ",        GAMEPAD_BLACKLIST_ANYWHERE}, // "System76 Thelio Io 2"
};

void SDL_InitGamepadVIDPIDLists(void)
{
    SDL_AssertJoysticksLocked();
    SDL_LoadVIDPIDList(&SDL_allowed_gamepads);
    SDL_LoadVIDPIDList(&SDL_ignored_gamepads);
}

void SDL_QuitGamepadVIDPIDLists(void)
{
    SDL_AssertJoysticksLocked();
    SDL_FreeVIDPIDList(&SDL_allowed_gamepads);
    SDL_FreeVIDPIDList(&SDL_ignored_gamepads);
}

/*
 * Return true if the gamepad should be ignored by SDL
 */
bool SDL_ShouldIgnoreGamepad(Uint16 vendor_id, Uint16 product_id, Uint16 version, const char *name)
{
    if (name) {
        for (int i = 0; i < SDL_arraysize(SDL_gamepad_blacklist_words); i++) {
            const struct SDL_GamepadBlacklistWords *blacklist_word = &SDL_gamepad_blacklist_words[i];

            switch (blacklist_word->pos) {
                case GAMEPAD_BLACKLIST_BEGIN:
                    if (SDL_startswith(name, blacklist_word->str)) {
                        return true;
                    }
                    break;

                case GAMEPAD_BLACKLIST_END:
                    if (SDL_endswith(name, blacklist_word->str)) {
                        return true;
                    }
                    break;

                case GAMEPAD_BLACKLIST_ANYWHERE:
                    if (SDL_strstr(name, blacklist_word->str) != NULL) {
                        if (SDL_startswith(name, "PG-")) {
                            // Ipega gamepads have modes with keyboard keys in addition to gamepad controls
                            break;
                        }
                        return true;
                    }
                    break;
            }
        }
    }

    const char *hint = SDL_getenv_unsafe("SDL_GAMECONTROLLER_ALLOW_STEAM_VIRTUAL_GAMEPAD");
    bool allow_steam_virtual_gamepad = SDL_GetStringBoolean(hint, false);
#ifdef SDL_PLATFORM_WIN32
    if (allow_steam_virtual_gamepad && WIN_IsWine()) {
        // We are launched by Steam and running under Proton or Wine
        // We can't tell whether this controller is a Steam Virtual Gamepad,
        // so assume that is doing the appropriate filtering of controllers
        // and anything we see here is fine to use.
        return false;
    }
#endif // SDL_PLATFORM_WIN32

    if (SDL_IsJoystickSteamVirtualGamepad(vendor_id, product_id, version)) {
        return !allow_steam_virtual_gamepad;
    }

    if (SDL_allowed_gamepads.num_included_entries > 0) {
        if (SDL_VIDPIDInList(vendor_id, product_id, &SDL_allowed_gamepads)) {
            return false;
        }
        return true;
    } else {
        if (SDL_VIDPIDInList(vendor_id, product_id, &SDL_ignored_gamepads)) {
            return true;
        }
        return false;
    }
}
