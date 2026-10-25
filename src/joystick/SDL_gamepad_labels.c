/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_gamepad_c.h"

SDL_GamepadFaceStyle SDL_GetGamepadFaceStyleFromString(const char *string)
{
    if (SDL_strcmp(string, "abxy") == 0) {
        return SDL_GAMEPAD_FACE_STYLE_ABXY;
    } else if (SDL_strcmp(string, "axby") == 0) {
        return SDL_GAMEPAD_FACE_STYLE_AXBY;
    } else if (SDL_strcmp(string, "bayx") == 0) {
        return SDL_GAMEPAD_FACE_STYLE_BAYX;
    } else if (SDL_strcmp(string, "sony") == 0) {
        return SDL_GAMEPAD_FACE_STYLE_SONY;
    } else {
        return SDL_GAMEPAD_FACE_STYLE_UNKNOWN;
    }
}

SDL_GamepadFaceStyle SDL_GetGamepadFaceStyleForGamepadType(SDL_GamepadType type)
{
    switch (type) {
    case SDL_GAMEPAD_TYPE_PS3:
    case SDL_GAMEPAD_TYPE_PS4:
    case SDL_GAMEPAD_TYPE_PS5:
        return SDL_GAMEPAD_FACE_STYLE_SONY;
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
    case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
        return SDL_GAMEPAD_FACE_STYLE_BAYX;
    case SDL_GAMEPAD_TYPE_GAMECUBE:
        return SDL_GAMEPAD_FACE_STYLE_AXBY;
    default:
        return SDL_GAMEPAD_FACE_STYLE_ABXY;
    }
}

SDL_GamepadButtonLabel SDL_GetGamepadButtonLabelForFaceStyle(SDL_GamepadFaceStyle face_style, SDL_GamepadButton button)
{
    SDL_GamepadButtonLabel label = SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN;

    switch (face_style) {
    case SDL_GAMEPAD_FACE_STYLE_ABXY:
        switch (button) {
        case SDL_GAMEPAD_BUTTON_SOUTH: label = SDL_GAMEPAD_BUTTON_LABEL_A; break;
        case SDL_GAMEPAD_BUTTON_EAST: label = SDL_GAMEPAD_BUTTON_LABEL_B; break;
        case SDL_GAMEPAD_BUTTON_WEST: label = SDL_GAMEPAD_BUTTON_LABEL_X; break;
        case SDL_GAMEPAD_BUTTON_NORTH: label = SDL_GAMEPAD_BUTTON_LABEL_Y; break;
        default: break;
        }
        break;
    case SDL_GAMEPAD_FACE_STYLE_AXBY:
        switch (button) {
        case SDL_GAMEPAD_BUTTON_SOUTH: label = SDL_GAMEPAD_BUTTON_LABEL_A; break;
        case SDL_GAMEPAD_BUTTON_EAST: label = SDL_GAMEPAD_BUTTON_LABEL_X; break;
        case SDL_GAMEPAD_BUTTON_WEST: label = SDL_GAMEPAD_BUTTON_LABEL_B; break;
        case SDL_GAMEPAD_BUTTON_NORTH: label = SDL_GAMEPAD_BUTTON_LABEL_Y; break;
        default: break;
        }
        break;
    case SDL_GAMEPAD_FACE_STYLE_BAYX:
        switch (button) {
        case SDL_GAMEPAD_BUTTON_SOUTH: label = SDL_GAMEPAD_BUTTON_LABEL_B; break;
        case SDL_GAMEPAD_BUTTON_EAST: label = SDL_GAMEPAD_BUTTON_LABEL_A; break;
        case SDL_GAMEPAD_BUTTON_WEST: label = SDL_GAMEPAD_BUTTON_LABEL_Y; break;
        case SDL_GAMEPAD_BUTTON_NORTH: label = SDL_GAMEPAD_BUTTON_LABEL_X; break;
        default: break;
        }
        break;
    case SDL_GAMEPAD_FACE_STYLE_SONY:
        switch (button) {
        case SDL_GAMEPAD_BUTTON_SOUTH: label = SDL_GAMEPAD_BUTTON_LABEL_CROSS; break;
        case SDL_GAMEPAD_BUTTON_EAST: label = SDL_GAMEPAD_BUTTON_LABEL_CIRCLE; break;
        case SDL_GAMEPAD_BUTTON_WEST: label = SDL_GAMEPAD_BUTTON_LABEL_SQUARE; break;
        case SDL_GAMEPAD_BUTTON_NORTH: label = SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE; break;
        default: break;
        }
        break;
    default:
        break;
    }
    return label;
}

SDL_GamepadButtonLabel SDL_GetGamepadButtonLabelForType(SDL_GamepadType type, SDL_GamepadButton button)
{
    return SDL_GetGamepadButtonLabelForFaceStyle(SDL_GetGamepadFaceStyleForGamepadType(type), button);
}
