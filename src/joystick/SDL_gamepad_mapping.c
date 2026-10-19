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

    // Trim whitespace
    length = SDL_strlen(result);
    while (length > 0 && SDL_isspace(result[length - 1])) {
        --length;
    }
    result[length] = '\0';

    return result;
}

Uint16 SDL_NormalizeGamepadMappingCRC(SDL_GUID *guid, char **mapping)
{
    char *pchMapping = *mapping;
    Uint16 crc;

    // Fix up the GUID and the mapping with the CRC, if needed
    SDL_GetJoystickGUIDInfo(*guid, NULL, NULL, NULL, &crc);
    if (crc) {
        // Make sure the mapping has the CRC
        char *new_mapping;
        const char *optional_comma;
        size_t mapping_length;
        char *crc_end = "";
        char *crc_string = SDL_strstr(pchMapping, SDL_GAMEPAD_CRC_FIELD);
        if (crc_string) {
            crc_end = SDL_strchr(crc_string, ',');
            if (crc_end) {
                ++crc_end;
            } else {
                crc_end = "";
            }
            *crc_string = '\0';
        }

        // Make sure there's a comma before the CRC
        mapping_length = SDL_strlen(pchMapping);
        if (mapping_length == 0 || pchMapping[mapping_length - 1] == ',') {
            optional_comma = "";
        } else {
            optional_comma = ",";
        }

        if (SDL_asprintf(&new_mapping, "%s%s%s%.4x,%s", pchMapping, optional_comma, SDL_GAMEPAD_CRC_FIELD, crc, crc_end) >= 0) {
            SDL_free(pchMapping);
            pchMapping = new_mapping;
        }
    } else {
        // Make sure the GUID has the CRC, for matching purposes
        char *crc_string = SDL_strstr(pchMapping, SDL_GAMEPAD_CRC_FIELD);
        if (crc_string) {
            crc = (Uint16)SDL_strtol(crc_string + SDL_GAMEPAD_CRC_FIELD_SIZE, NULL, 16);
            if (crc) {
                SDL_SetJoystickGUIDCRC(guid, crc);
            }
        }
    }

    *mapping = pchMapping;
    return crc;
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
