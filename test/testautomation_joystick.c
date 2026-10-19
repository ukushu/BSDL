/**
 * Joystick test suite
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>
#include "../src/joystick/usb_ids.h"
#include "testautomation_suites.h"

/* ================= Test Case Implementation ================== */

/* Fixture */

/* Create a 32-bit writable surface for blitting tests */
static void SDLCALL joystickSetUp(void **arg)
{
    SDL_InitSubSystem(SDL_INIT_GAMEPAD);

    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
}

static void SDLCALL joystickTearDown(void *arg)
{
    SDL_ResetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS);

    SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
}


/* Test case functions */

/**
 * Check virtual joystick creation
 *
 * \sa SDL_AttachVirtualJoystick
 */
static int SDLCALL joystick_testVirtual(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_Joystick *joystick = NULL;
    SDL_Gamepad *gamepad = NULL;
    SDL_JoystickID device_id;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
    desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
    desc.vendor_id = USB_VENDOR_NVIDIA;
    desc.product_id = USB_PRODUCT_NVIDIA_SHIELD_CONTROLLER_V103;
    desc.name = "Virtual NVIDIA SHIELD Controller";
    device_id = SDL_AttachVirtualJoystick(&desc);
    SDLTest_AssertCheck(device_id > 0, "SDL_AttachVirtualJoystick() -> %" SDL_PRIs32 " (expected > 0)", device_id);
    SDLTest_AssertCheck(SDL_IsJoystickVirtual(device_id), "SDL_IsJoystickVirtual()");
    if (device_id > 0) {
        joystick = SDL_OpenJoystick(device_id);
        SDLTest_AssertCheck(joystick != NULL, "SDL_OpenJoystick()");
        if (joystick) {
            {
                const char *dname = SDL_GetJoystickName(joystick);
                SDLTest_AssertCheck(SDL_strcmp(dname, desc.name) == 0, "SDL_GetJoystickName() -> \"%s\" (expected \"%s\")", dname, desc.name);
            }
            {
                Uint16 vendor_id = SDL_GetJoystickVendor(joystick);
                SDLTest_AssertCheck(vendor_id == desc.vendor_id, "SDL_GetJoystickVendor() -> 0x%04x (expected 0x%04x)", vendor_id, desc.vendor_id);
            }
            {
                Uint16 product_id = SDL_GetJoystickProduct(joystick);
                SDLTest_AssertCheck(product_id == desc.product_id, "SDL_GetJoystickProduct() -> 0x%04x (expected 0x%04x)", product_id, desc.product_id);
            }
            {
                Uint16 product_version = SDL_GetJoystickProductVersion(joystick);
                SDLTest_AssertCheck(product_version == 0, "SDL_GetJoystickProductVersion() -> 0x%04x (expected 0x%04x)", product_version, 0);
            }
            {
                Uint16 firmware_Version = SDL_GetJoystickFirmwareVersion(joystick);
                SDLTest_AssertCheck(firmware_Version == 0, "SDL_GetJoystickFirmwareVersion() -> 0x%04x (expected 0x%04x)", firmware_Version, 0);
            }
            {
                const char *serial = SDL_GetJoystickSerial(joystick);
                SDLTest_AssertCheck(serial == NULL, "SDL_GetJoystickSerial() -> %s (expected %s)", serial, "(null)");
            }
            {
                SDL_JoystickType type = SDL_GetJoystickType(joystick);
                SDLTest_AssertCheck(type == desc.type, "SDL_GetJoystickType() -> %d (expected %d)", type, desc.type);
            }
            {
                Uint16 naxes = SDL_GetNumJoystickAxes(joystick);
                SDLTest_AssertCheck(naxes == desc.naxes, "SDL_GetNumJoystickAxes() -> 0x%04x (expected 0x%04x)", naxes, desc.naxes);
            }
            {
                int nballs = SDL_GetNumJoystickBalls(joystick);
                SDLTest_AssertCheck(nballs == 0, "SDL_GetNumJoystickBalls() -> %d (expected %d)", nballs, 0);
            }
            {
                int nhats = SDL_GetNumJoystickHats(joystick);
                SDLTest_AssertCheck(nhats == desc.nhats, "SDL_GetNumJoystickHats() -> %d (expected %d)", nhats, desc.nhats);
            }
            {
                int nbuttons = SDL_GetNumJoystickButtons(joystick);
                SDLTest_AssertCheck(nbuttons == desc.nbuttons, "SDL_GetNumJoystickButtons() -> %d (expected %d)", nbuttons, desc.nbuttons);
            }

            SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, true), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, true)");
            SDL_UpdateJoysticks();
            SDLTest_AssertCheck(SDL_GetJoystickButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH) == true, "SDL_GetJoystickButton(SDL_GAMEPAD_BUTTON_SOUTH) == true");

            SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, false), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, false)");
            SDL_UpdateJoysticks();
            SDLTest_AssertCheck(SDL_GetJoystickButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH) == false, "SDL_GetJoystickButton(SDL_GAMEPAD_BUTTON_SOUTH) == false");

            gamepad = SDL_OpenGamepad(SDL_GetJoystickID(joystick));
            SDLTest_AssertCheck(gamepad != NULL, "SDL_OpenGamepad() succeeded");
            if (gamepad) {
                SDL_Gamepad *again = SDL_OpenGamepad(device_id);
                SDLTest_AssertCheck(again == gamepad, "Opening the same gamepad returns the existing instance");
                if (again) {
                    SDL_CloseGamepad(again);
                }
                SDLTest_AssertCheck(SDL_GetGamepadFromID(device_id) == gamepad,
                                    "Closing the second reference keeps the first gamepad open");
                {
                    const char *name = SDL_GetGamepadName(gamepad);
                    SDLTest_AssertCheck(name && SDL_strcmp(name, desc.name) == 0, "SDL_GetGamepadName() -> \"%s\" (expected \"%s\")", name, desc.name);
                }
                {
                    Uint16 vendor_id = SDL_GetGamepadVendor(gamepad);
                    SDLTest_AssertCheck(vendor_id == desc.vendor_id, "SDL_GetGamepadVendor() -> 0x%04x (expected 0x%04x)", vendor_id, desc.vendor_id);
                }
                {
                    Uint16 product_id = SDL_GetGamepadProduct(gamepad);
                    SDLTest_AssertCheck(product_id == desc.product_id, "SDL_GetGamepadProduct() -> 0x%04x (expected 0x%04x)", product_id, desc.product_id);
                }

                /* Set an explicit mapping with a different name */
                SDL_SetGamepadMapping(SDL_GetJoystickID(joystick), "ff008316550900001072000000007601,Virtual Gamepad,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,");
                {
                    const char *name = SDL_GetGamepadName(gamepad);
                    SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Gamepad") == 0, "SDL_GetGamepadName() ->\"%s\" (expected \"%s\")", name, "Virtual Gamepad");
                }
                {
                    SDL_GamepadButtonLabel label = SDL_GetGamepadButtonLabel(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
                    SDLTest_AssertCheck(label == SDL_GAMEPAD_BUTTON_LABEL_A, "SDL_GetGamepadButtonLabel(SDL_GAMEPAD_BUTTON_SOUTH) -> %d (expected %d [%s])",
                                        label, SDL_GAMEPAD_BUTTON_LABEL_A, "SDL_GAMEPAD_BUTTON_LABEL_A");
                }
                SDLTest_AssertCheck(SDL_GetGamepadButtonLabel(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_A, "SDL_GetGamepadButtonLabel(SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_A");

                /* Set the south button and verify that the gamepad responds */
                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, true), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, true)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == true, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == true");

                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, false), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, false)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == false, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == false");

                /* Set an explicit mapping with legacy GameCube style buttons */
                SDL_SetGamepadMapping(SDL_GetJoystickID(joystick), "ff008316550900001072000000007601,Virtual Nintendo GameCube,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,hint:SDL_GAMECONTROLLER_USE_GAMECUBE_LABELS:=1,");
                {
                    const char *name = SDL_GetGamepadName(gamepad);
                    SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Nintendo GameCube") == 0, "SDL_GetGamepadName() -> \"%s\" (expected \"%s\")", name, "Virtual Nintendo GameCube");
                }
                SDLTest_AssertCheck(SDL_GetGamepadButtonLabel(gamepad, SDL_GAMEPAD_BUTTON_EAST) == SDL_GAMEPAD_BUTTON_LABEL_X, "SDL_GetGamepadButtonLabel(SDL_GAMEPAD_BUTTON_EAST) == SDL_GAMEPAD_BUTTON_LABEL_X");

                /* Set the east button and verify that the gamepad responds, mapping "B" to the west button */
                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_EAST, true), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_EAST, true)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST) == true, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_WEST) == true");

                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_EAST, false), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_EAST, false)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_WEST) == false, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_WEST) == false");

                /* Set an explicit mapping with legacy Nintendo style buttons */
                SDL_SetGamepadMapping(SDL_GetJoystickID(joystick), "ff008316550900001072000000007601,Virtual Nintendo Gamepad,a:b1,b:b0,x:b3,y:b2,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,hint:SDL_GAMECONTROLLER_USE_BUTTON_LABELS:=1,");
                {
                    const char *name = SDL_GetGamepadName(gamepad);
                    SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Nintendo Gamepad") == 0, "SDL_GetGamepadName() -> \"%s\" (expected \"%s\")", name, "Virtual Nintendo Gamepad");
                }
                SDLTest_AssertCheck(SDL_GetGamepadButtonLabel(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_B, "SDL_GetGamepadButtonLabel(SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_B");

                /* Set the south button and verify that the gamepad responds */
                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, true), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, true)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == true, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == true");

                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, false), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, false)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == false, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == false");

                /* Set an explicit mapping with PS4 style buttons */
                SDL_SetGamepadMapping(SDL_GetJoystickID(joystick), "ff008316550900001072000000007601,Virtual PS4 Gamepad,type:ps4,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,");
                {
                    const char *name = SDL_GetGamepadName(gamepad);
                    SDLTest_AssertCheck(SDL_strcmp(name, "Virtual PS4 Gamepad") == 0, "SDL_GetGamepadName() -> \"%s\" (expected \"%s\")", name, "Virtual PS4 Gamepad");
                }
                SDLTest_AssertCheck(SDL_GetGamepadButtonLabel(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_CROSS, "SDL_GetGamepadButtonLabel(SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_CROSS");

                /* Set the south button and verify that the gamepad responds */
                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, true), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, true)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == true, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == true");

                SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, SDL_GAMEPAD_BUTTON_SOUTH, false), "SDL_SetJoystickVirtualButton(SDL_GAMEPAD_BUTTON_SOUTH, false)");
                SDL_UpdateJoysticks();
                SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH) == false, "SDL_GetGamepadButton(SDL_GAMEPAD_BUTTON_SOUTH) == false");

                SDL_CloseGamepad(gamepad);
            }

            SDL_CloseJoystick(joystick);
        }
        SDLTest_AssertCheck(SDL_DetachVirtualJoystick(device_id), "SDL_DetachVirtualJoystick()");
    }
    SDLTest_AssertCheck(!SDL_IsJoystickVirtual(device_id), "!SDL_IsJoystickVirtual()");

    return TEST_COMPLETED;
}

/** Check the initial values of a two-axis virtual joystick. */
static int SDLCALL joystick_twoAxisInitialState(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_JoystickID device_id;
    SDL_Joystick *joystick;
    Sint16 value;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_UNKNOWN;
    desc.naxes = 2;
    device_id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(device_id > 0, "Attach two-axis joystick: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    joystick = SDL_OpenJoystick(device_id);
    SDLTest_AssertCheck(joystick != NULL, "Open two-axis joystick: %s", SDL_GetError());
    if (joystick) {
        for (int axis = 0; axis < 2; ++axis) {
            value = 1;
            SDLTest_AssertCheck(SDL_GetJoystickAxisInitialState(joystick, axis, &value),
                                "Axis %d has an initial state", axis);
            SDLTest_AssertCheck(value == 0, "Axis %d starts centered at zero, got %d", axis, value);
        }
        SDL_CloseJoystick(joystick);
    }
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(device_id), "Detach two-axis joystick");
    return TEST_COMPLETED;
}

static int SDLCALL joystick_virtualGUIDFields(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_JoystickID id;
    SDL_GUID guid;
    Uint16 vendor = 0, product = 0, version = 0, crc = 0;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.vendor_id = 0x1234;
    desc.product_id = 0x5678;
    desc.name = "Virtual GUID test";
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach virtual GUID joystick: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    guid = SDL_GetJoystickGUIDForID(id);
    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, &version, &crc);
    SDLTest_AssertCheck(vendor == desc.vendor_id && product == desc.product_id && version == 0,
                        "Virtual GUID preserves vendor, product and version fields");
    SDLTest_AssertCheck(crc == SDL_crc16(0, desc.name, SDL_strlen(desc.name)),
                        "Virtual GUID preserves the name CRC");
    SDLTest_AssertCheck(guid.data[14] == 'v' && guid.data[15] == desc.type,
                        "Virtual GUID preserves driver signature and joystick type");
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach virtual GUID joystick");
    return TEST_COMPLETED;
}

static int SDLCALL joystick_typeClassification(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_JoystickID id;
    SDL_Joystick *joystick;
    const char *old_hint = SDL_GetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES);
    char *saved_hint = old_hint ? SDL_strdup(old_hint) : NULL;
    const char *old_excluded_hint = SDL_GetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED);
    char *saved_excluded_hint = old_excluded_hint ? SDL_strdup(old_excluded_hint) : NULL;

    if ((old_hint && !saved_hint) || (old_excluded_hint && !saved_excluded_hint)) {
        SDLTest_AssertCheck(false, "Save existing wheel hints");
        SDL_free(saved_hint);
        SDL_free(saved_excluded_hint);
        return TEST_ABORTED;
    }

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.vendor_id = 0x046d;
    desc.product_id = 0xc29b; // Logitech G27, in the built-in wheel list
    desc.name = "Virtual known wheel";
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach known wheel: %s", SDL_GetError())) {
        SDL_free(saved_hint);
        SDL_free(saved_excluded_hint);
        return TEST_ABORTED;
    }
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_WHEEL,
                        "Built-in VID/PID classifies a wheel by ID");
    joystick = SDL_OpenJoystick(id);
    if (SDLTest_AssertCheck(joystick != NULL, "Open known wheel: %s", SDL_GetError())) {
        SDLTest_AssertCheck(SDL_GetJoystickType(joystick) == SDL_JOYSTICK_TYPE_WHEEL,
                            "Built-in VID/PID classifies an open wheel");
        SDL_CloseJoystick(joystick);
    }
    SDLTest_AssertCheck(SDL_SetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED, "0x046d/0xc29b"),
                        "Exclude built-in wheel VID/PID");
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_GAMEPAD,
                        "Exclusion hint overrides built-in wheel classification");
    if (saved_excluded_hint) {
        SDL_SetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED, saved_excluded_hint);
    } else {
        SDL_ResetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED);
    }
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_WHEEL,
                        "Restored exclusion hint restores built-in wheel classification");
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach known wheel");

    desc.vendor_id = 0x1234;
    desc.product_id = 0xabcd;
    desc.name = "Virtual hinted wheel";
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach hinted wheel: %s", SDL_GetError())) {
        SDL_free(saved_hint);
        SDL_free(saved_excluded_hint);
        return TEST_ABORTED;
    }
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_GAMEPAD,
                        "Unknown VID/PID initially keeps virtual type");
    SDLTest_AssertCheck(SDL_SetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES, "0x1234/0xabcd"),
                        "Add wheel VID/PID with hint");
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_WHEEL,
                        "Updated wheel hint changes classification");
    if (saved_hint) {
        SDL_SetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES, saved_hint);
    } else {
        SDL_ResetHint(SDL_HINT_JOYSTICK_WHEEL_DEVICES);
    }
    SDLTest_AssertCheck(SDL_GetJoystickTypeForID(id) == SDL_JOYSTICK_TYPE_GAMEPAD,
                        "Restored wheel hint restores virtual type");
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach hinted wheel");
    SDL_free(saved_hint);
    SDL_free(saved_excluded_hint);
    return TEST_COMPLETED;
}

/**
 * Check gamepad mappings
 */
static int SDLCALL joystick_testMappings(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_Gamepad *gamepad = NULL;
    SDL_JoystickID device_id;

    /* Add a mapping for the virtual controller in advance */
    SDL_AddGamepadMapping("ff000000550900001472000000007601,Virtual Gamepad,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,");

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
    desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
    desc.vendor_id = USB_VENDOR_NVIDIA;
    desc.product_id = USB_PRODUCT_NVIDIA_SHIELD_CONTROLLER_V104;
    desc.name = "Virtual NVIDIA SHIELD Controller";
    device_id = SDL_AttachVirtualJoystick(&desc);
    SDLTest_AssertCheck(device_id > 0, "SDL_AttachVirtualJoystick() -> %" SDL_PRIs32 " (expected > 0)", device_id);
    SDLTest_AssertCheck(SDL_IsJoystickVirtual(device_id), "SDL_IsJoystickVirtual()");

    gamepad = SDL_OpenGamepad(device_id);
    SDLTest_AssertCheck(gamepad != NULL, "SDL_OpenGamepad() succeeded");
    if (gamepad) {
        /* Verify that the gamepad picked up the predefined mapping */
        {
            const char *name = SDL_GetGamepadName(gamepad);
            SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Gamepad") == 0, "SDL_GetGamepadName() ->\"%s\" (expected \"%s\")", name, "Virtual Gamepad");
        }

        /* Verify that the gamepad picks up a new mapping with no CRC */
        SDL_AddGamepadMapping("ff000000550900001472000000007601,Virtual Gamepad V2,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,");
        {
            const char *name = SDL_GetGamepadName(gamepad);
            SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Gamepad V2") == 0, "SDL_GetGamepadName() ->\"%s\" (expected \"%s\")", name, "Virtual Gamepad V2");
        }

        /* Verify that the gamepad picks up a new mapping with valid CRC */
        SDL_AddGamepadMapping("ff008316550900001472000000007601,Virtual Gamepad V3,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,dpright:b14,misc1:b15,paddle1:b16,paddle2:b17,paddle3:b18,paddle4:b19,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,");
        {
            const char *name = SDL_GetGamepadName(gamepad);
            SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Gamepad V3") == 0, "SDL_GetGamepadName() ->\"%s\" (expected \"%s\")", name, "Virtual Gamepad V3");
        }

        SDL_CloseGamepad(gamepad);
    }
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(device_id), "SDL_DetachVirtualJoystick()");

    /* Try matching mappings with a new CRC */
    desc.name = "Virtual NVIDIA SHIELD Controller V2";
    device_id = SDL_AttachVirtualJoystick(&desc);
    SDLTest_AssertCheck(device_id > 0, "SDL_AttachVirtualJoystick() -> %" SDL_PRIs32 " (expected > 0)", device_id);
    SDLTest_AssertCheck(SDL_IsJoystickVirtual(device_id), "SDL_IsJoystickVirtual()");

    gamepad = SDL_OpenGamepad(device_id);
    SDLTest_AssertCheck(gamepad != NULL, "SDL_OpenGamepad() succeeded");
    if (gamepad) {
        {
            const char *name = SDL_GetGamepadName(gamepad);
            SDLTest_AssertCheck(name && SDL_strcmp(name, "Virtual Gamepad V2") == 0, "SDL_GetGamepadName() ->\"%s\" (expected \"%s\")", name, "Virtual Gamepad V2");
        }
        SDL_CloseGamepad(gamepad);
    }
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(device_id), "SDL_DetachVirtualJoystick()");

    return TEST_COMPLETED;
}

static int SDLCALL joystick_gamepadButtonLabelsForType(void *arg)
{
    SDLTest_AssertCheck(SDL_GetGamepadButtonLabelForType(SDL_GAMEPAD_TYPE_UNKNOWN, SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_A, "Unknown type uses ABXY south label");
    SDLTest_AssertCheck(SDL_GetGamepadButtonLabelForType(SDL_GAMEPAD_TYPE_GAMECUBE, SDL_GAMEPAD_BUTTON_EAST) == SDL_GAMEPAD_BUTTON_LABEL_X, "GameCube uses AXBY east label");
    SDLTest_AssertCheck(SDL_GetGamepadButtonLabelForType(SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO, SDL_GAMEPAD_BUTTON_SOUTH) == SDL_GAMEPAD_BUTTON_LABEL_B, "Switch Pro uses BAYX south label");
    SDLTest_AssertCheck(SDL_GetGamepadButtonLabelForType(SDL_GAMEPAD_TYPE_PS4, SDL_GAMEPAD_BUTTON_NORTH) == SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE, "PS4 uses Sony north label");
    SDLTest_AssertCheck(SDL_GetGamepadButtonLabelForType(SDL_GAMEPAD_TYPE_PS5, SDL_GAMEPAD_BUTTON_EAST) == SDL_GAMEPAD_BUTTON_LABEL_CIRCLE, "PS5 uses Sony east label");
    return TEST_COMPLETED;
}

static int SDLCALL joystick_mappingElementParsing(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_JoystickID id;
    SDL_Joystick *joystick = NULL;
    SDL_Gamepad *gamepad = NULL;
    SDL_GamepadBinding **bindings = NULL;
    char guid[33];
    char mapping[256];
    int count = 0;
    bool leftx = false, trigger = false, button = false, hat = false;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.naxes = 2;
    desc.nbuttons = 1;
    desc.nhats = 1;
    desc.vendor_id = 0xF00D;
    desc.product_id = 0xBEEF;
    desc.name = "Mapping Parser Virtual Gamepad";
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach mapping parser joystick: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    SDL_GUIDToString(SDL_GetJoystickGUIDForID(id), guid, sizeof(guid));
    SDL_snprintf(mapping, sizeof(mapping), "%s,Mapping Parser,leftx:+a0~,righttrigger:-a1,a:b0,b:h0.1,", guid);
    if (!SDLTest_AssertCheck(SDL_AddGamepadMapping(mapping) >= 0, "Add axis, button, and hat mapping: %s", SDL_GetError())) {
        goto done;
    }
    gamepad = SDL_OpenGamepad(id);
    if (!SDLTest_AssertCheck(gamepad != NULL, "Open mapping parser gamepad: %s", SDL_GetError())) {
        goto done;
    }
    bindings = SDL_GetGamepadBindings(gamepad, &count);
    if (!SDLTest_AssertCheck(bindings != NULL, "Read parsed gamepad bindings: %s", SDL_GetError())) {
        goto done;
    }
    SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) == 0,
                        "Mapped trigger starts at its logical zero point");
    for (int i = 0; i < count; ++i) {
        const SDL_GamepadBinding *bind = bindings[i];
        if (bind->output_type == SDL_GAMEPAD_BINDTYPE_AXIS && bind->output.axis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
            leftx = bind->input_type == SDL_GAMEPAD_BINDTYPE_AXIS && bind->input.axis.axis == 0 &&
                    bind->input.axis.axis_min == SDL_JOYSTICK_AXIS_MAX && bind->input.axis.axis_max == 0;
        } else if (bind->output_type == SDL_GAMEPAD_BINDTYPE_AXIS && bind->output.axis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
            trigger = bind->input_type == SDL_GAMEPAD_BINDTYPE_AXIS && bind->input.axis.axis == 1 &&
                      bind->input.axis.axis_min == 0 && bind->input.axis.axis_max == SDL_JOYSTICK_AXIS_MIN;
        } else if (bind->output_type == SDL_GAMEPAD_BINDTYPE_BUTTON && bind->output.button == SDL_GAMEPAD_BUTTON_SOUTH) {
            button = bind->input_type == SDL_GAMEPAD_BINDTYPE_BUTTON && bind->input.button == 0;
        } else if (bind->output_type == SDL_GAMEPAD_BINDTYPE_BUTTON && bind->output.button == SDL_GAMEPAD_BUTTON_EAST) {
            hat = bind->input_type == SDL_GAMEPAD_BINDTYPE_HAT && bind->input.hat.hat == 0 && bind->input.hat.hat_mask == 1;
        }
    }
    SDLTest_AssertCheck(leftx && trigger && button && hat,
                        "Parser preserves half-axis inversion, negative half-axis, button, and hat bindings");
    joystick = SDL_GetGamepadJoystick(gamepad);
    if (SDLTest_AssertCheck(joystick != NULL, "Get virtual joystick for mapped-axis checks")) {
        SDLTest_AssertCheck(SDL_SetJoystickVirtualAxis(joystick, 0, SDL_JOYSTICK_AXIS_MAX), "Set mapped axis at positive limit: %s", SDL_GetError());
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) == SDL_JOYSTICK_AXIS_MIN,
                            "Inverted positive half-axis maps its upper endpoint");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualAxis(joystick, 0, 0), "Set mapped axis at zero: %s", SDL_GetError());
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) == SDL_JOYSTICK_AXIS_MAX,
                            "Inverted positive half-axis maps its lower endpoint");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualAxis(joystick, 0, -1), "Move mapped axis outside its half range: %s", SDL_GetError());
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) == 0,
                            "Leaving the mapped half range resets the gamepad axis");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualHat(joystick, 0, SDL_HAT_UP), "Set mapped hat direction: %s", SDL_GetError());
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST), "Mapped hat direction presses the gamepad button");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualHat(joystick, 0, SDL_HAT_CENTERED), "Center mapped hat: %s", SDL_GetError());
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(!SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST), "Centering the mapped hat releases the gamepad button");
    }

done:
    SDL_free(bindings);
    SDL_CloseGamepad(gamepad);
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach mapping parser joystick: %s", SDL_GetError());
    return TEST_COMPLETED;
}

static int SDLCALL joystick_sinputMapping(void *arg)
{
    SDL_GUID guid;
    char *mapping;
    const Uint16 version = 8639; // Both sticks, two bumpers, dual-stage triggers, four paddles, three meta buttons, two touchpads, four misc buttons.

    SDL_zero(guid);
    guid.data[0] = 3; // USB bus
    guid.data[4] = (Uint8)(USB_VENDOR_RASPBERRYPI & 0xff);
    guid.data[5] = (Uint8)(USB_VENDOR_RASPBERRYPI >> 8);
    guid.data[8] = (Uint8)(USB_PRODUCT_HANDHELDLEGEND_SINPUT_GENERIC & 0xff);
    guid.data[9] = (Uint8)(USB_PRODUCT_HANDHELDLEGEND_SINPUT_GENERIC >> 8);
    guid.data[12] = (Uint8)(version & 0xff);
    guid.data[13] = (Uint8)(version >> 8);
    guid.data[14] = 'h'; // HIDAPI
    guid.data[15] = 4 << 5; // Sony face style

    mapping = SDL_GetGamepadMappingForGUID(guid);
    if (SDLTest_AssertCheck(mapping != NULL, "Generate SInput mapping: %s", SDL_GetError())) {
        SDLTest_AssertCheck(SDL_strstr(mapping, "face:sony,") != NULL, "SInput face style is preserved");
        SDLTest_AssertCheck(SDL_strstr(mapping, "leftx:a0,lefty:a1,rightx:a2,righty:a3,") != NULL,
                            "SInput analog sticks retain axis order");
        SDLTest_AssertCheck(SDL_strstr(mapping, "lefttrigger:a4,righttrigger:a5,") != NULL,
                            "SInput dual-stage triggers retain analog axes");
        SDLTest_AssertCheck(SDL_strstr(mapping, "paddle2:") && SDL_strstr(mapping, "paddle1:") &&
                            SDL_strstr(mapping, "paddle4:") && SDL_strstr(mapping, "paddle3:") &&
                            SDL_strstr(mapping, "touchpad:") && SDL_strstr(mapping, "guide:"),
                            "SInput digital controls are present");
    }
    SDL_free(mapping);
    return TEST_COMPLETED;
}

static int SDLCALL joystick_gamepadNameConversions(void *arg)
{
    for (int i = SDL_GAMEPAD_TYPE_STANDARD; i < SDL_GAMEPAD_TYPE_COUNT; ++i) {
        const char *name = SDL_GetGamepadStringForType((SDL_GamepadType)i);
        SDLTest_AssertCheck(name && SDL_GetGamepadTypeFromString(name) == (SDL_GamepadType)i,
                            "Gamepad type %d round-trips through its name", i);
    }
    for (int i = 0; i < SDL_GAMEPAD_AXIS_COUNT; ++i) {
        const char *name = SDL_GetGamepadStringForAxis((SDL_GamepadAxis)i);
        SDLTest_AssertCheck(name && SDL_GetGamepadAxisFromString(name) == (SDL_GamepadAxis)i,
                            "Gamepad axis %d round-trips through its name", i);
    }
    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i) {
        const char *name = SDL_GetGamepadStringForButton((SDL_GamepadButton)i);
        SDLTest_AssertCheck(name && SDL_GetGamepadButtonFromString(name) == (SDL_GamepadButton)i,
                            "Gamepad button %d round-trips through its name", i);
    }

    SDLTest_AssertCheck(SDL_GetGamepadTypeFromString("+PS5") == SDL_GAMEPAD_TYPE_PS5, "Type parsing accepts a sign and ignores case");
    SDLTest_AssertCheck(SDL_GetGamepadAxisFromString("-LEFTX") == SDL_GAMEPAD_AXIS_LEFTX, "Axis parsing accepts a sign and ignores case");
    SDLTest_AssertCheck(SDL_GetGamepadButtonFromString("A") == SDL_GAMEPAD_BUTTON_SOUTH, "Button parsing ignores case");
    SDLTest_AssertCheck(SDL_GetGamepadStringForType(SDL_GAMEPAD_TYPE_UNKNOWN) == NULL, "Unknown type has no public name");
    SDLTest_AssertCheck(SDL_GetGamepadStringForAxis(SDL_GAMEPAD_AXIS_INVALID) == NULL, "Invalid axis has no name");
    SDLTest_AssertCheck(SDL_GetGamepadStringForButton(SDL_GAMEPAD_BUTTON_INVALID) == NULL, "Invalid button has no name");
    return TEST_COMPLETED;
}

static int SDLCALL joystick_mappingLabelHints(void *arg)
{
    const struct {
        const char *guid;
        const char *mapping;
        const char *expected;
    } cases[] = {
        { "03000000adde00000100000000000000",
          "03000000adde00000100000000000000,GameCube labels,a:b0,b:b1,x:b2,y:b3,hint:SDL_GAMECONTROLLER_USE_GAMECUBE_LABELS:=1,",
          ",a:b0,x:b1,b:b2,y:b3,hint:!SDL_GAMECONTROLLER_USE_GAMECUBE_LABELS:=1," },
        { "03000000adde00000200000000000000",
          "03000000adde00000200000000000000,Nintendo labels,a:b0,b:b1,x:b2,y:b3,hint:SDL_GAMECONTROLLER_USE_BUTTON_LABELS:=1,",
          ",b:b0,a:b1,y:b2,x:b3,hint:!SDL_GAMECONTROLLER_USE_BUTTON_LABELS:=1," }
    };

    for (int i = 0; i < SDL_arraysize(cases); ++i) {
        char *mapping;
        if (!SDLTest_AssertCheck(SDL_AddGamepadMapping(cases[i].mapping) >= 0,
                                 "Add labeled mapping %d: %s", i, SDL_GetError())) {
            return TEST_ABORTED;
        }
        mapping = SDL_GetGamepadMappingForGUID(SDL_StringToGUID(cases[i].guid));
        if (!SDLTest_AssertCheck(mapping != NULL, "Read labeled mapping %d: %s", i, SDL_GetError())) {
            return TEST_ABORTED;
        }
        SDLTest_AssertCheck(SDL_strstr(mapping, cases[i].expected) != NULL,
                            "Mapping %d preserves positional buttons and negates the label hint: %s", i, mapping);
        SDL_free(mapping);
    }
    return TEST_COMPLETED;
}

/**
 * Missing joystick axes must not become half-pressed gamepad triggers.
 */
static int SDLCALL joystick_testMissingAxes(void *arg)
{
    SDL_VirtualJoystickDesc joystick_desc;
    SDL_JoystickID joystick_id = 0;
    SDL_Gamepad *gamepad = NULL;
    SDL_Joystick *joystick = NULL;
    char guid_string[33] = {0};
    char mapping_string[256] = {0};

    SDL_INIT_INTERFACE(&joystick_desc);
    joystick_desc.type = SDL_JOYSTICK_TYPE_FLIGHT_STICK;
    joystick_desc.naxes = 2;
    joystick_desc.nbuttons = 2;
    joystick_desc.name = "Virtual two-axis joystick";
    joystick_id = SDL_AttachVirtualJoystick(&joystick_desc);
    SDLTest_AssertCheck(joystick_id != 0, "SDL_AttachVirtualJoystick()");
    if (!joystick_id) {
        return TEST_ABORTED;
    }

    SDL_GUIDToString(SDL_GetJoystickGUIDForID(joystick_id), guid_string, sizeof(guid_string));
    /* Axis 2 is the first invalid index; axis 5 matches the DOS fallback mapping.
     * A second right-trigger binding checks that valid bindings are still read. */
    SDL_snprintf(mapping_string, sizeof(mapping_string),
                 "%s,Virtual two-axis joystick,leftx:a0,lefty:a1,a:b0,"
                 "lefttrigger:a2,righttrigger:a5,righttrigger:b1,", guid_string);
    SDLTest_AssertCheck(SDL_SetGamepadMapping(joystick_id, mapping_string), "SDL_SetGamepadMapping()");
    gamepad = SDL_OpenGamepad(joystick_id);
    SDLTest_AssertCheck(gamepad != NULL, "SDL_OpenGamepad()");
    if (gamepad) {
        joystick = SDL_GetGamepadJoystick(gamepad);
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) == 0,
                            "Missing left trigger is released");
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) == 0,
                            "Missing right trigger is released");

        SDLTest_AssertCheck(SDL_SetJoystickVirtualAxis(joystick, 0, -16384), "Set X axis");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualAxis(joystick, 1, 16384), "Set Y axis");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, 0, true), "Press button 0");
        SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, 1, true), "Press button 1");
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) == -16384, "X axis is unchanged");
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) == 16384, "Y axis is unchanged");
        SDLTest_AssertCheck(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH), "Button 0 is pressed");
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) == SDL_JOYSTICK_AXIS_MAX,
                            "Valid button binding works after a missing axis");
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) == 0,
                            "Missing left trigger stays released while other controls move");

        SDLTest_AssertCheck(SDL_SetJoystickVirtualButton(joystick, 1, false), "Release button 1");
        SDL_UpdateJoysticks();
        SDLTest_AssertCheck(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) == 0,
                            "Right trigger returns to released");
        SDL_CloseGamepad(gamepad);
    }
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(joystick_id), "SDL_DetachVirtualJoystick()");
    return TEST_COMPLETED;
}

/* ================= Test References ================== */

static int SDLCALL joystick_openReuse(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    SDL_JoystickID id;
    SDL_Joystick *first, *second;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_UNKNOWN;
    desc.naxes = 2;
    desc.nbuttons = 1;
    desc.nhats = 1;
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach virtual joystick for open reuse: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    first = SDL_OpenJoystick(id);
    if (!SDLTest_AssertCheck(first != NULL, "Open virtual joystick: %s", SDL_GetError())) {
        SDL_DetachVirtualJoystick(id);
        return TEST_ABORTED;
    }
    second = SDL_OpenJoystick(id);
    SDLTest_AssertCheck(second == first, "Opening an active joystick reuses the same object");
    SDL_CloseJoystick(first);
    if (second) {
        SDLTest_AssertCheck(SDL_JoystickConnected(second), "Second reference remains connected");
        SDLTest_AssertCheck(SDL_GetNumJoystickAxes(second) == 2, "Axis state remains available");
        SDLTest_AssertCheck(SDL_GetNumJoystickButtons(second) == 1, "Button state remains available");
        SDLTest_AssertCheck(SDL_GetNumJoystickHats(second) == 1, "Hat state remains available");
        SDL_CloseJoystick(second);
    }
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach virtual joystick after closing both references: %s", SDL_GetError());
    return TEST_COMPLETED;
}

typedef struct JoystickRumbleCalls
{
    int body_count;
    int trigger_count;
    Uint16 body_low, body_high;
    Uint16 trigger_left, trigger_right;
} JoystickRumbleCalls;

static bool SDLCALL joystick_onRumble(void *userdata, Uint16 low, Uint16 high)
{
    JoystickRumbleCalls *calls = (JoystickRumbleCalls *)userdata;
    ++calls->body_count;
    calls->body_low = low;
    calls->body_high = high;
    return true;
}

static bool SDLCALL joystick_onRumbleTriggers(void *userdata, Uint16 left, Uint16 right)
{
    JoystickRumbleCalls *calls = (JoystickRumbleCalls *)userdata;
    ++calls->trigger_count;
    calls->trigger_left = left;
    calls->trigger_right = right;
    return true;
}

static int SDLCALL joystick_rumbleExpiration(void *arg)
{
    SDL_VirtualJoystickDesc desc;
    JoystickRumbleCalls calls = { 0 };
    SDL_JoystickID id;
    SDL_Joystick *joystick;

    SDL_INIT_INTERFACE(&desc);
    desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
    desc.userdata = &calls;
    desc.Rumble = joystick_onRumble;
    desc.RumbleTriggers = joystick_onRumbleTriggers;
    id = SDL_AttachVirtualJoystick(&desc);
    if (!SDLTest_AssertCheck(id > 0, "Attach rumble joystick: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }
    joystick = SDL_OpenJoystick(id);
    if (!SDLTest_AssertCheck(joystick != NULL, "Open rumble joystick: %s", SDL_GetError())) {
        SDL_DetachVirtualJoystick(id);
        return TEST_ABORTED;
    }

    SDLTest_AssertCheck(SDL_RumbleJoystick(joystick, 100, 200, 1), "Start body rumble: %s", SDL_GetError());
    SDLTest_AssertCheck(SDL_RumbleJoystickTriggers(joystick, 300, 400, 1), "Start trigger rumble: %s", SDL_GetError());
    SDLTest_AssertCheck(calls.body_count == 1 && calls.body_low == 100 && calls.body_high == 200,
                        "Body callback receives initial rumble values");
    SDLTest_AssertCheck(calls.trigger_count == 1 && calls.trigger_left == 300 && calls.trigger_right == 400,
                        "Trigger callback receives initial rumble values");

    SDL_Delay(20);
    SDL_UpdateJoysticks();
    SDLTest_AssertCheck(calls.body_count == 2 && calls.body_low == 0 && calls.body_high == 0,
                        "Expired body rumble sends one stop callback");
    SDLTest_AssertCheck(calls.trigger_count == 2 && calls.trigger_left == 0 && calls.trigger_right == 0,
                        "Expired trigger rumble sends one stop callback");
    SDL_UpdateJoysticks();
    SDLTest_AssertCheck(calls.body_count == 2 && calls.trigger_count == 2,
                        "Stopped rumble is not sent again");

    SDL_CloseJoystick(joystick);
    SDLTest_AssertCheck(SDL_DetachVirtualJoystick(id), "Detach rumble joystick: %s", SDL_GetError());
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference joystickTestOpenReuse = {
    joystick_openReuse, "joystick_openReuse", "Opening a joystick twice preserves its input state until both closes", TEST_ENABLED
};

static const SDLTest_TestCaseReference joystickTestRumbleExpiration = {
    joystick_rumbleExpiration, "joystick_rumbleExpiration", "Virtual joystick rumble stops once after its duration expires", TEST_ENABLED
};

/* Joystick routine test cases */
static const SDLTest_TestCaseReference joystickTest1 = {
    joystick_testVirtual, "joystick_testVirtual", "Test virtual joystick functionality", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestInitialState = {
    joystick_twoAxisInitialState, "joystick_twoAxisInitialState", "Two-axis virtual joysticks start centered", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestVirtualGUIDFields = {
    joystick_virtualGUIDFields, "joystick_virtualGUIDFields", "Virtual joystick GUID retains its encoded fields", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestTypeClassification = {
    joystick_typeClassification, "joystick_typeClassification", "Known and hinted wheel VID/PID classification", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTest2 = {
    joystick_testMappings, "joystick_testMappings", "Test gamepad mapping functionality", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestMappingElementParsing = {
    joystick_mappingElementParsing, "joystick_mappingElementParsing", "Parse half axes, button, and hat bindings", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestSInputMapping = {
    joystick_sinputMapping, "joystick_sinputMapping", "Decode SInput styles into a gamepad mapping", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestMappingLabelHints = {
    joystick_mappingLabelHints, "joystick_mappingLabelHints", "Legacy label hints map buttons to positions", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestGamepadButtonLabelsForType = {
    joystick_gamepadButtonLabelsForType, "joystick_gamepadButtonLabelsForType", "Button labels follow the gamepad face style", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTestGamepadNameConversions = {
    joystick_gamepadNameConversions, "joystick_gamepadNameConversions", "Gamepad names and enums round-trip", TEST_ENABLED
};
static const SDLTest_TestCaseReference joystickTest3 = {
    joystick_testMissingAxes, "joystick_testMissingAxes", "Test mappings with missing joystick axes", TEST_ENABLED
};

/* Sequence of Joystick routine test cases */
static const SDLTest_TestCaseReference *joystickTests[] = {
    &joystickTest1,
    &joystickTestOpenReuse,
    &joystickTestRumbleExpiration,
    &joystickTestInitialState,
    &joystickTestVirtualGUIDFields,
    &joystickTestTypeClassification,
    &joystickTest2,
    &joystickTestMappingElementParsing,
    &joystickTestSInputMapping,
    &joystickTestGamepadNameConversions,
    &joystickTestMappingLabelHints,
    &joystickTestGamepadButtonLabelsForType,
    &joystickTest3,
    NULL
};

/* Joystick routine test suite (global) */
SDLTest_TestSuiteReference joystickTestSuite = {
    "Joystick",
    joystickSetUp,
    joystickTests,
    joystickTearDown
};
