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
#include "SDL_gamepad_c.h"

static const char *map_StringForGamepadType[] = {
    "unknown",
    "standard",
    "xbox360",
    "xboxone",
    "ps3",
    "ps4",
    "ps5",
    "switchpro",
    "joyconleft",
    "joyconright",
    "joyconpair",
    "gamecube",
    "steam"
};
SDL_COMPILE_TIME_ASSERT(map_StringForGamepadType, SDL_arraysize(map_StringForGamepadType) == SDL_GAMEPAD_TYPE_COUNT);

/*
 * convert a string to its enum equivalent
 */
SDL_GamepadType SDL_GetGamepadTypeFromString(const char *str)
{
    int i;

    if (!str || str[0] == '\0') {
        return SDL_GAMEPAD_TYPE_UNKNOWN;
    }

    if (*str == '+' || *str == '-') {
        ++str;
    }

    for (i = 0; i < SDL_arraysize(map_StringForGamepadType); ++i) {
        if (SDL_strcasecmp(str, map_StringForGamepadType[i]) == 0) {
            return (SDL_GamepadType)i;
        }
    }
    return SDL_GAMEPAD_TYPE_UNKNOWN;
}

/*
 * convert an enum to its string equivalent
 */
const char *SDL_GetGamepadStringForType(SDL_GamepadType type)
{
    if (type >= SDL_GAMEPAD_TYPE_STANDARD && type < SDL_GAMEPAD_TYPE_COUNT) {
        return map_StringForGamepadType[type];
    }
    return NULL;
}

static const char *map_StringForGamepadAxis[] = {
    "leftx",
    "lefty",
    "rightx",
    "righty",
    "lefttrigger",
    "righttrigger"
};
SDL_COMPILE_TIME_ASSERT(map_StringForGamepadAxis, SDL_arraysize(map_StringForGamepadAxis) == SDL_GAMEPAD_AXIS_COUNT);

/*
 * convert a string to its enum equivalent
 */
SDL_GamepadAxis SDL_GetGamepadAxisFromString(const char *str)
{
    int i;

    if (!str || str[0] == '\0') {
        return SDL_GAMEPAD_AXIS_INVALID;
    }

    if (*str == '+' || *str == '-') {
        ++str;
    }

    for (i = 0; i < SDL_arraysize(map_StringForGamepadAxis); ++i) {
        if (SDL_strcasecmp(str, map_StringForGamepadAxis[i]) == 0) {
            return (SDL_GamepadAxis)i;
        }
    }
    return SDL_GAMEPAD_AXIS_INVALID;
}

/*
 * convert an enum to its string equivalent
 */
const char *SDL_GetGamepadStringForAxis(SDL_GamepadAxis axis)
{
    if (axis > SDL_GAMEPAD_AXIS_INVALID && axis < SDL_GAMEPAD_AXIS_COUNT) {
        return map_StringForGamepadAxis[axis];
    }
    return NULL;
}

static const char *map_StringForGamepadButton[] = {
    "a",
    "b",
    "x",
    "y",
    "back",
    "guide",
    "start",
    "leftstick",
    "rightstick",
    "leftshoulder",
    "rightshoulder",
    "dpup",
    "dpdown",
    "dpleft",
    "dpright",
    "misc1",
    "paddle1",
    "paddle2",
    "paddle3",
    "paddle4",
    "touchpad",
    "misc2",
    "misc3",
    "misc4",
    "misc5",
    "misc6"
};
SDL_COMPILE_TIME_ASSERT(map_StringForGamepadButton, SDL_arraysize(map_StringForGamepadButton) == SDL_GAMEPAD_BUTTON_COUNT);

/*
 * convert a string to its enum equivalent
 */
SDL_GamepadButton SDL_PrivateGetGamepadButtonFromString(const char *str, bool axby, bool baxy)
{
    int i;

    if (!str || str[0] == '\0') {
        return SDL_GAMEPAD_BUTTON_INVALID;
    }

    for (i = 0; i < SDL_arraysize(map_StringForGamepadButton); ++i) {
        if (SDL_strcasecmp(str, map_StringForGamepadButton[i]) == 0) {
            if (axby) {
                // Need to swap face buttons
                switch (i) {
                case SDL_GAMEPAD_BUTTON_EAST:
                    return SDL_GAMEPAD_BUTTON_WEST;
                case SDL_GAMEPAD_BUTTON_WEST:
                    return SDL_GAMEPAD_BUTTON_EAST;
                default:
                    break;
                }
            } else if (baxy) {
                // Need to swap face buttons
                switch (i) {
                case SDL_GAMEPAD_BUTTON_SOUTH:
                    return SDL_GAMEPAD_BUTTON_EAST;
                case SDL_GAMEPAD_BUTTON_EAST:
                    return SDL_GAMEPAD_BUTTON_SOUTH;
                case SDL_GAMEPAD_BUTTON_WEST:
                    return SDL_GAMEPAD_BUTTON_NORTH;
                case SDL_GAMEPAD_BUTTON_NORTH:
                    return SDL_GAMEPAD_BUTTON_WEST;
                default:
                    break;
                }
            }
            return (SDL_GamepadButton)i;
        }
    }
    return SDL_GAMEPAD_BUTTON_INVALID;
}
SDL_GamepadButton SDL_GetGamepadButtonFromString(const char *str)
{
    return SDL_PrivateGetGamepadButtonFromString(str, false, false);
}

/*
 * convert an enum to its string equivalent
 */
const char *SDL_GetGamepadStringForButton(SDL_GamepadButton button)
{
    if (button > SDL_GAMEPAD_BUTTON_INVALID && button < SDL_GAMEPAD_BUTTON_COUNT) {
        return map_StringForGamepadButton[button];
    }
    return NULL;
}

