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

#ifndef SDL_gamepad_c_h_
#define SDL_gamepad_c_h_

#include "SDL_internal.h"

#define SDL_GAMEPAD_CRC_FIELD "crc:"
#define SDL_GAMEPAD_CRC_FIELD_SIZE 4 // hard-coded for speed
#define SDL_GAMEPAD_PLATFORM_FIELD "platform:"
#define SDL_GAMEPAD_PLATFORM_FIELD_SIZE SDL_strlen(SDL_GAMEPAD_PLATFORM_FIELD)

typedef enum
{
    SDL_GAMEPAD_FACE_STYLE_UNKNOWN,
    SDL_GAMEPAD_FACE_STYLE_ABXY,
    SDL_GAMEPAD_FACE_STYLE_AXBY,
    SDL_GAMEPAD_FACE_STYLE_BAYX,
    SDL_GAMEPAD_FACE_STYLE_SONY,
} SDL_GamepadFaceStyle;

extern SDL_GamepadFaceStyle SDL_GetGamepadFaceStyleFromString(const char *string);
extern SDL_GamepadFaceStyle SDL_GetGamepadFaceStyleForGamepadType(SDL_GamepadType type);
extern SDL_GamepadButtonLabel SDL_GetGamepadButtonLabelForFaceStyle(SDL_GamepadFaceStyle face_style, SDL_GamepadButton button);
extern char *SDL_PrivateGetGamepadGUIDFromMappingString(const char *mapping);
extern char *SDL_PrivateGetGamepadNameFromMappingString(const char *mapping);
extern char *SDL_PrivateGetGamepadMappingFromMappingString(const char *mapping);
extern Uint16 SDL_NormalizeGamepadMappingCRC(SDL_GUID *guid, char **mapping);
extern char *SDL_CreateGamepadMappingString(const char *name, const char *mapping, SDL_GUID guid);

// Useful functions and variables from SDL_gamepad.c

extern SDL_GamepadButton SDL_PrivateGetGamepadButtonFromString(const char *str, bool axby, bool baxy);
extern void SDL_CreateMappingStringForSInputGamepad(Uint16 version, Uint8 face_style, char *mapping_string, size_t mapping_string_len);

// Initialization and shutdown functions
extern bool SDL_InitGamepadMappings(void);
extern void SDL_QuitGamepadMappings(void);
extern void SDL_InitGamepadVIDPIDLists(void);
extern void SDL_QuitGamepadVIDPIDLists(void);
extern bool SDL_InitGamepads(void);
extern void SDL_QuitGamepads(void);

extern void SDL_PrivateGamepadAdded(SDL_JoystickID instance_id);
extern void SDL_PrivateGamepadRemoved(SDL_JoystickID instance_id);

// Function to return whether a gamepad should be ignored
extern bool SDL_ShouldIgnoreGamepad(Uint16 vendor_id, Uint16 product_id, Uint16 version, const char *name);

// Handle delayed guide button on a gamepad
extern void SDL_GamepadHandleDelayedGuideButton(SDL_Joystick *joystick);

// Handle system sensor data
extern void SDL_GamepadSensorWatcher(Uint64 timestamp, SDL_SensorID sensor, Uint64 sensor_timestamp, float *data, int num_values);

#endif // SDL_gamepad_c_h_
