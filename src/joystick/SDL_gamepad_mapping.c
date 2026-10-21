/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_gamepad_c.h"
#include "SDL_joystick_c.h"

bool SDL_ParseGamepadMappingOutput(const char *mapping, const char *szGameButton, SDL_GamepadBinding *bind)
{
    SDL_GamepadButton button;
    SDL_GamepadAxis axis;
    char half_axis_output = 0;
    bool axby_mapping = false;
    bool baxy_mapping = false;

    if (*szGameButton == '+' || *szGameButton == '-') {
        half_axis_output = *szGameButton++;
    }

    if (SDL_strstr(mapping, ",hint:SDL_GAMECONTROLLER_USE_GAMECUBE_LABELS:=1") != NULL) {
        axby_mapping = true;
    }
    if (SDL_strstr(mapping, ",hint:SDL_GAMECONTROLLER_USE_BUTTON_LABELS:=1") != NULL) {
        baxy_mapping = true;
    }

    // FIXME: We fix these up when loading the mapping, does this ever get hit?
    //SDL_assert(!axby_mapping && !baxy_mapping);

    axis = SDL_GetGamepadAxisFromString(szGameButton);
    button = SDL_PrivateGetGamepadButtonFromString(szGameButton, axby_mapping, baxy_mapping);
    if (axis != SDL_GAMEPAD_AXIS_INVALID) {
        bind->output_type = SDL_GAMEPAD_BINDTYPE_AXIS;
        bind->output.axis.axis = axis;
        if (axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER || axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
            bind->output.axis.axis_min = 0;
            bind->output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        } else {
            if (half_axis_output == '+') {
                bind->output.axis.axis_min = 0;
                bind->output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
            } else if (half_axis_output == '-') {
                bind->output.axis.axis_min = 0;
                bind->output.axis.axis_max = SDL_JOYSTICK_AXIS_MIN;
            } else {
                bind->output.axis.axis_min = SDL_JOYSTICK_AXIS_MIN;
                bind->output.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
            }
        }
    } else if (button != SDL_GAMEPAD_BUTTON_INVALID) {
        bind->output_type = SDL_GAMEPAD_BINDTYPE_BUTTON;
        bind->output.button = button;
    } else {
        return false;
    }
    return true;
}

bool SDL_ParseGamepadMappingInput(const char *szJoystickButton, SDL_GamepadBinding *bind)
{
    bool invert_input = false;
    char half_axis_input = 0;

    if (*szJoystickButton == '+' || *szJoystickButton == '-') {
        half_axis_input = *szJoystickButton++;
    }
    if (szJoystickButton[SDL_strlen(szJoystickButton) - 1] == '~') {
        invert_input = true;
    }

    if (szJoystickButton[0] == 'a' && SDL_isdigit((unsigned char)szJoystickButton[1])) {
        bind->input_type = SDL_GAMEPAD_BINDTYPE_AXIS;
        bind->input.axis.axis = SDL_atoi(&szJoystickButton[1]);
        if (half_axis_input == '+') {
            bind->input.axis.axis_min = 0;
            bind->input.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        } else if (half_axis_input == '-') {
            bind->input.axis.axis_min = 0;
            bind->input.axis.axis_max = SDL_JOYSTICK_AXIS_MIN;
        } else {
            bind->input.axis.axis_min = SDL_JOYSTICK_AXIS_MIN;
            bind->input.axis.axis_max = SDL_JOYSTICK_AXIS_MAX;
        }
        if (invert_input) {
            int tmp = bind->input.axis.axis_min;
            bind->input.axis.axis_min = bind->input.axis.axis_max;
            bind->input.axis.axis_max = tmp;
        }
    } else if (szJoystickButton[0] == 'b' && SDL_isdigit((unsigned char)szJoystickButton[1])) {
        bind->input_type = SDL_GAMEPAD_BINDTYPE_BUTTON;
        bind->input.button = SDL_atoi(&szJoystickButton[1]);
    } else if (szJoystickButton[0] == 'h' && SDL_isdigit((unsigned char)szJoystickButton[1]) &&
               szJoystickButton[2] == '.' && SDL_isdigit((unsigned char)szJoystickButton[3])) {
        int hat = SDL_atoi(&szJoystickButton[1]);
        int mask = SDL_atoi(&szJoystickButton[3]);
        bind->input_type = SDL_GAMEPAD_BINDTYPE_HAT;
        bind->input.hat.hat = hat;
        bind->input.hat.hat_mask = mask;
    } else {
        return false;
    }
    return true;
}
/*
 * grab the guid string from a mapping string
 */
char *SDL_PrivateGetGamepadGUIDFromMappingString(const char *pMapping)
{
    const char *pFirstComma = SDL_strchr(pMapping, ',');
    if (pFirstComma) {
        char *pchGUID = (char *)SDL_malloc(pFirstComma - pMapping + 1);
        if (!pchGUID) {
            return NULL;
        }
        SDL_memcpy(pchGUID, pMapping, pFirstComma - pMapping);
        pchGUID[pFirstComma - pMapping] = '\0';

        // Convert old style GUIDs to the new style in 2.0.5
#if defined(SDL_PLATFORM_WIN32) || defined(SDL_PLATFORM_WINGDK)
        if (SDL_strlen(pchGUID) == 32 &&
            SDL_memcmp(&pchGUID[20], "504944564944", 12) == 0) {
            SDL_memcpy(&pchGUID[20], "000000000000", 12);
            SDL_memcpy(&pchGUID[16], &pchGUID[4], 4);
            SDL_memcpy(&pchGUID[8], &pchGUID[0], 4);
            SDL_memcpy(&pchGUID[0], "03000000", 8);
        }
#elif defined(SDL_PLATFORM_MACOS)
        if (SDL_strlen(pchGUID) == 32 &&
            SDL_memcmp(&pchGUID[4], "000000000000", 12) == 0 &&
            SDL_memcmp(&pchGUID[20], "000000000000", 12) == 0) {
            SDL_memcpy(&pchGUID[20], "000000000000", 12);
            SDL_memcpy(&pchGUID[8], &pchGUID[0], 4);
            SDL_memcpy(&pchGUID[0], "03000000", 8);
        }
#endif
        return pchGUID;
    }
    return NULL;
}

/*
 * grab the name string from a mapping string
 */
char *SDL_PrivateGetGamepadNameFromMappingString(const char *pMapping)
{
    const char *pFirstComma, *pSecondComma;
    char *pchName;

    pFirstComma = SDL_strchr(pMapping, ',');
    if (!pFirstComma) {
        return NULL;
    }

    pSecondComma = SDL_strchr(pFirstComma + 1, ',');
    if (!pSecondComma) {
        return NULL;
    }

    pchName = (char *)SDL_malloc(pSecondComma - pFirstComma);
    if (!pchName) {
        return NULL;
    }
    SDL_memcpy(pchName, pFirstComma + 1, pSecondComma - pFirstComma);
    pchName[pSecondComma - pFirstComma - 1] = 0;
    return pchName;
}

/*
 * grab the button mapping string from a mapping string
 */
char *SDL_PrivateGetGamepadMappingFromMappingString(const char *pMapping)
{
    const char *pFirstComma, *pSecondComma;
    char *result;
    size_t length;

    pFirstComma = SDL_strchr(pMapping, ',');
    if (!pFirstComma) {
        return NULL;
    }

    pSecondComma = SDL_strchr(pFirstComma + 1, ',');
    if (!pSecondComma) {
        return NULL;
    }

    // Skip whitespace
    while (SDL_isspace(pSecondComma[1])) {
        ++pSecondComma;
    }

    result = SDL_strdup(pSecondComma + 1); // mapping is everything after the 3rd comma
    if (!result) {
        return NULL;
    }

    // Trim whitespace
    length = SDL_strlen(result);
    while (length > 0 && SDL_isspace(result[length - 1])) {
        --length;
    }
    result[length] = '\0';

    return result;
}

bool SDL_NormalizeGamepadMappingCRC(SDL_GUID *guid, char **mapping, Uint16 *crc)
{
    char *pchMapping = *mapping;
    Uint16 mapping_crc;

    // Fix up the GUID and the mapping with the CRC, if needed
    SDL_GetJoystickGUIDInfo(*guid, NULL, NULL, NULL, &mapping_crc);
    if (mapping_crc) {
        // Make sure the mapping has the CRC
        char *new_mapping;
        const char *optional_comma;
        size_t mapping_length;
        char *crc_end = "";
        char *crc_string = SDL_strstr(pchMapping, SDL_GAMEPAD_CRC_FIELD);
        char saved_crc_char = '\0';
        if (crc_string) {
            crc_end = SDL_strchr(crc_string, ',');
            if (crc_end) {
                ++crc_end;
            } else {
                crc_end = "";
            }
            saved_crc_char = *crc_string;
            *crc_string = '\0';
        }

        // Make sure there's a comma before the CRC
        mapping_length = SDL_strlen(pchMapping);
        if (mapping_length == 0 || pchMapping[mapping_length - 1] == ',') {
            optional_comma = "";
        } else {
            optional_comma = ",";
        }

        if (SDL_asprintf(&new_mapping, "%s%s%s%.4x,%s", pchMapping, optional_comma, SDL_GAMEPAD_CRC_FIELD, mapping_crc, crc_end) < 0) {
            if (crc_string) {
                *crc_string = saved_crc_char;
            }
            return false;
        }
        if (crc_string) {
            *crc_string = saved_crc_char;
        }
        SDL_free(pchMapping);
        pchMapping = new_mapping;
    } else {
        // Make sure the GUID has the CRC, for matching purposes
        char *crc_string = SDL_strstr(pchMapping, SDL_GAMEPAD_CRC_FIELD);
        if (crc_string) {
            mapping_crc = (Uint16)SDL_strtol(crc_string + SDL_GAMEPAD_CRC_FIELD_SIZE, NULL, 16);
            if (mapping_crc) {
                SDL_SetJoystickGUIDCRC(guid, mapping_crc);
            }
        }
    }

    *mapping = pchMapping;
    *crc = mapping_crc;
    return true;
}

/*
 * Create a mapping string; callers must hold the joystick lock.
 */
char *SDL_CreateGamepadMappingString(const char *name, const char *mapping, SDL_GUID guid)
{
    char *pMappingString, *pPlatformString;
    char pchGUID[33];
    size_t needed;
    bool need_platform = false;
    const char *platform = NULL;

    SDL_AssertJoysticksLocked();

    SDL_GUIDToString(guid, pchGUID, sizeof(pchGUID));

    // allocate enough memory for GUID + ',' + name + ',' + mapping + \0
    needed = SDL_strlen(pchGUID) + 1 + SDL_strlen(name) + 1 + SDL_strlen(mapping) + 1;

    if (!SDL_strstr(mapping, SDL_GAMEPAD_PLATFORM_FIELD)) {
        // add memory for ',' + platform:PLATFORM
        need_platform = true;
        if (mapping[SDL_strlen(mapping) - 1] != ',') {
            needed += 1;
        }
        platform = SDL_GetPlatform();
        needed += SDL_GAMEPAD_PLATFORM_FIELD_SIZE + SDL_strlen(platform) + 1;
    }

    pMappingString = (char *)SDL_malloc(needed);
    if (!pMappingString) {
        return NULL;
    }

    (void)SDL_snprintf(pMappingString, needed, "%s,%s,%s", pchGUID, name, mapping);

    if (need_platform) {
        if (mapping[SDL_strlen(mapping) - 1] != ',') {
            SDL_strlcat(pMappingString, ",", needed);
        }
        SDL_strlcat(pMappingString, SDL_GAMEPAD_PLATFORM_FIELD, needed);
        SDL_strlcat(pMappingString, platform, needed);
        SDL_strlcat(pMappingString, ",", needed);
    }

    // Make sure multiple platform strings haven't made their way into the mapping
    pPlatformString = SDL_strstr(pMappingString, SDL_GAMEPAD_PLATFORM_FIELD);
    if (pPlatformString) {
        pPlatformString = SDL_strstr(pPlatformString + 1, SDL_GAMEPAD_PLATFORM_FIELD);
        if (pPlatformString) {
            *pPlatformString = '\0';
        }
    }
    return pMappingString;
}
