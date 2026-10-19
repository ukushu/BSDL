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
#include "SDL_sysjoystick.h"

/**
 *  Get the number of touchpads on a gamepad.
 */
int SDL_GetNumGamepadTouchpads(SDL_Gamepad *gamepad)
{
    int result = 0;

    SDL_LockJoysticks();
    {
        SDL_Joystick *joystick = SDL_GetGamepadJoystick(gamepad);
        if (joystick) {
            result = joystick->ntouchpads;
        }
    }
    SDL_UnlockJoysticks();

    return result;
}

/**
 *  Get the number of supported simultaneous fingers on a touchpad on a gamepad.
 */
int SDL_GetNumGamepadTouchpadFingers(SDL_Gamepad *gamepad, int touchpad)
{
    int result = 0;

    SDL_LockJoysticks();
    {
        SDL_Joystick *joystick = SDL_GetGamepadJoystick(gamepad);
        if (joystick) {
            if (touchpad >= 0 && touchpad < joystick->ntouchpads) {
                result = joystick->touchpads[touchpad].nfingers;
            }
        }
    }
    SDL_UnlockJoysticks();

    return result;
}

/**
 *  Get the current state of a finger on a touchpad on a gamepad.
 */
bool SDL_GetGamepadTouchpadFinger(SDL_Gamepad *gamepad, int touchpad, int finger, bool *down, float *x, float *y, float *pressure)
{
    bool result = false;

    SDL_LockJoysticks();
    {
        SDL_Joystick *joystick = SDL_GetGamepadJoystick(gamepad);
        if (joystick) {
            if (touchpad >= 0 && touchpad < joystick->ntouchpads) {
                SDL_JoystickTouchpadInfo *touchpad_info = &joystick->touchpads[touchpad];
                if (finger >= 0 && finger < touchpad_info->nfingers) {
                    SDL_JoystickTouchpadFingerInfo *info = &touchpad_info->fingers[finger];

                    if (down) {
                        *down = info->down;
                    }
                    if (x) {
                        *x = info->x;
                    }
                    if (y) {
                        *y = info->y;
                    }
                    if (pressure) {
                        *pressure = info->pressure;
                    }
                    result = true;
                } else {
                    result = SDL_InvalidParamError("finger");
                }
            } else {
                result = SDL_InvalidParamError("touchpad");
            }
        }
    }
    SDL_UnlockJoysticks();

    return result;
}

bool SDL_GamepadHasSensor(SDL_Gamepad *gamepad, SDL_SensorType type)
{
    return SDL_JoystickHasSensor(SDL_GetGamepadJoystick(gamepad), type);
}

bool SDL_SetGamepadSensorEnabled(SDL_Gamepad *gamepad, SDL_SensorType type, bool enabled)
{
    return SDL_SetJoystickSensorEnabled(SDL_GetGamepadJoystick(gamepad), type, enabled);
}

bool SDL_GamepadSensorEnabled(SDL_Gamepad *gamepad, SDL_SensorType type)
{
    return SDL_JoystickSensorEnabled(SDL_GetGamepadJoystick(gamepad), type);
}

float SDL_GetGamepadSensorDataRate(SDL_Gamepad *gamepad, SDL_SensorType type)
{
    return SDL_GetJoystickSensorDataRate(SDL_GetGamepadJoystick(gamepad), type);
}

bool SDL_GetGamepadSensorData(SDL_Gamepad *gamepad, SDL_SensorType type, float *data, int num_values)
{
    return SDL_GetJoystickSensorData(SDL_GetGamepadJoystick(gamepad), type, data, num_values);
}

bool SDL_GamepadHasCapSense(SDL_Gamepad *gamepad, SDL_GamepadCapSenseType type)
{
    bool result = false;

    SDL_LockJoysticks();
    {
        SDL_Joystick *joystick = SDL_GetGamepadJoystick(gamepad);
        if (joystick) {
            for (int i = 0; i < joystick->ncapsenses; ++i) {
                if (joystick->capsenses[i].type == type) {
                    result = true;
                    break;
                }
            }
        }
    }
    SDL_UnlockJoysticks();

    return result;
}

bool SDL_GetGamepadCapSense(SDL_Gamepad *gamepad, SDL_GamepadCapSenseType type)
{
    bool result = false;

    SDL_LockJoysticks();
    {
        SDL_Joystick *joystick = SDL_GetGamepadJoystick(gamepad);
        if (joystick) {
            for (int i = 0; i < joystick->ncapsenses; ++i) {
                if (joystick->capsenses[i].type == type) {
                    result = joystick->capsenses[i].down;
                    break;
                }
            }
        }
    }
    SDL_UnlockJoysticks();

    return result;
}

