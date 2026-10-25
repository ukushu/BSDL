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
#include "SDL_sysjoystick.h"
#include "SDL_joystick_c.h"

void SDL_GetJoystickGUIDInfo(SDL_GUID guid, Uint16 *vendor, Uint16 *product, Uint16 *version, Uint16 *crc16)
{
    Uint16 *guid16 = (Uint16 *)guid.data;
    Uint16 bus = SDL_Swap16LE(guid16[0]);

    if ((bus < ' ' || bus == SDL_HARDWARE_BUS_VIRTUAL) && guid16[3] == 0x0000 && guid16[5] == 0x0000) {
        /* This GUID fits the standard form:
         * 16-bit bus
         * 16-bit CRC16 of the joystick name (can be zero)
         * 16-bit vendor ID
         * 16-bit zero
         * 16-bit product ID
         * 16-bit zero
         * 16-bit version
         * 8-bit driver identifier ('h' for HIDAPI, 'x' for XInput, etc.)
         * 8-bit driver-dependent type info
         */
        if (vendor) {
            *vendor = SDL_Swap16LE(guid16[2]);
        }
        if (product) {
            *product = SDL_Swap16LE(guid16[4]);
        }
        if (version) {
            *version = SDL_Swap16LE(guid16[6]);
        }
        if (crc16) {
            *crc16 = SDL_Swap16LE(guid16[1]);
        }
    } else if (bus < ' ' || bus == SDL_HARDWARE_BUS_VIRTUAL) {
        /* This GUID fits the unknown VID/PID form:
         * 16-bit bus
         * 16-bit CRC16 of the joystick name (can be zero)
         * 11 characters of the joystick name, null terminated
         */
        if (vendor) {
            *vendor = 0;
        }
        if (product) {
            *product = 0;
        }
        if (version) {
            *version = 0;
        }
        if (crc16) {
            *crc16 = SDL_Swap16LE(guid16[1]);
        }
    } else {
        if (vendor) {
            *vendor = 0;
        }
        if (product) {
            *product = 0;
        }
        if (version) {
            *version = 0;
        }
        if (crc16) {
            *crc16 = 0;
        }
    }
}

SDL_GUID SDL_CreateJoystickGUID(Uint16 bus, Uint16 vendor, Uint16 product, Uint16 version, const char *vendor_name, const char *product_name, Uint8 driver_signature, Uint8 driver_data)
{
    SDL_GUID guid;
    Uint16 *guid16 = (Uint16 *)guid.data;
    Uint16 crc = 0;

    SDL_zero(guid);

    if (vendor_name && *vendor_name && product_name && *product_name) {
        crc = SDL_crc16(crc, vendor_name, SDL_strlen(vendor_name));
        crc = SDL_crc16(crc, " ", 1);
        crc = SDL_crc16(crc, product_name, SDL_strlen(product_name));
    } else if (product_name) {
        crc = SDL_crc16(crc, product_name, SDL_strlen(product_name));
    }

    // We only need 16 bits for each of these; space them out to fill 128.
    // Byteswap so devices get same GUID on little/big endian platforms.
    *guid16++ = SDL_Swap16LE(bus);
    *guid16++ = SDL_Swap16LE(crc);

    if (vendor) {
        *guid16++ = SDL_Swap16LE(vendor);
        *guid16++ = 0;
        *guid16++ = SDL_Swap16LE(product);
        *guid16++ = 0;
        *guid16++ = SDL_Swap16LE(version);
        guid.data[14] = driver_signature;
        guid.data[15] = driver_data;
    } else {
        size_t available_space = sizeof(guid.data) - 4;

        if (driver_signature) {
            available_space -= 2;
            guid.data[14] = driver_signature;
            guid.data[15] = driver_data;
        }
        if (product_name) {
            SDL_strlcpy((char *)guid16, product_name, available_space);
        }
    }
    return guid;
}

SDL_GUID SDL_CreateJoystickGUIDForName(const char *name)
{
    return SDL_CreateJoystickGUID(SDL_HARDWARE_BUS_UNKNOWN, 0, 0, 0, NULL, name, 0, 0);
}

void SDL_SetJoystickGUIDVendor(SDL_GUID *guid, Uint16 vendor)
{
    Uint16 *guid16 = (Uint16 *)guid->data;

    guid16[2] = SDL_Swap16LE(vendor);
}

void SDL_SetJoystickGUIDProduct(SDL_GUID *guid, Uint16 product)
{
    Uint16 *guid16 = (Uint16 *)guid->data;

    guid16[4] = SDL_Swap16LE(product);
}

void SDL_SetJoystickGUIDVersion(SDL_GUID *guid, Uint16 version)
{
    Uint16 *guid16 = (Uint16 *)guid->data;

    guid16[6] = SDL_Swap16LE(version);
}

void SDL_SetJoystickGUIDCRC(SDL_GUID *guid, Uint16 crc)
{
    Uint16 *guid16 = (Uint16 *)guid->data;

    guid16[1] = SDL_Swap16LE(crc);
}

bool SDL_JoystickGUIDUsesVersion(SDL_GUID guid)
{
    Uint16 vendor, product;

    if (SDL_IsJoystickMFI(guid)) {
        // The version bits are used as button capability mask
        return false;
    }

    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, NULL, NULL);
    if (vendor && product) {
        return true;
    }
    return false;
}

bool SDL_IsJoystickXInput(SDL_GUID guid)
{
    return (guid.data[14] == 'x') ? true : false;
}

bool SDL_IsJoystickWGI(SDL_GUID guid)
{
    return (guid.data[14] == 'w') ? true : false;
}

bool SDL_IsJoystickGameInput(SDL_GUID guid)
{
    return (guid.data[14] == 'g') ? true : false;
}

bool SDL_IsJoystickHIDAPI(SDL_GUID guid)
{
    return (guid.data[14] == 'h') ? true : false;
}

bool SDL_IsJoystickMFI(SDL_GUID guid)
{
    return (guid.data[14] == 'm') ? true : false;
}

bool SDL_IsJoystickRAWINPUT(SDL_GUID guid)
{
    return (guid.data[14] == 'r') ? true : false;
}

bool SDL_IsJoystickVIRTUAL(SDL_GUID guid)
{
    return (guid.data[14] == 'v') ? true : false;
}

