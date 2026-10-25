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
#include "SDL_gamepad_c.h"
#include "controller_type.h"
#include "hidapi/SDL_hidapijoystick_c.h"
#include "../SDL_hints_c.h"

static Uint32 initial_old_xboxone_controllers[] = {
    MAKE_VIDPID(0x0000, 0x6686),
    MAKE_VIDPID(0x0079, 0x18a1),
    MAKE_VIDPID(0x0079, 0x18c2),
    MAKE_VIDPID(0x0079, 0x18c8),
    MAKE_VIDPID(0x0079, 0x18cf),
    MAKE_VIDPID(0x03f0, 0x0495),
    MAKE_VIDPID(0x045e, 0x02d1),
    MAKE_VIDPID(0x045e, 0x02dd),
    MAKE_VIDPID(0x045e, 0x02e0),
    MAKE_VIDPID(0x045e, 0x02e3),
    MAKE_VIDPID(0x045e, 0x02ea),
    MAKE_VIDPID(0x045e, 0x02fd),
    MAKE_VIDPID(0x045e, 0x02ff),
    MAKE_VIDPID(0x045e, 0x0867),
    MAKE_VIDPID(0x045e, 0x0b00),
    MAKE_VIDPID(0x045e, 0x0b05),
    MAKE_VIDPID(0x045e, 0x0b0a),
    MAKE_VIDPID(0x045e, 0x0b0c),
    MAKE_VIDPID(0x045e, 0x0b20),
    MAKE_VIDPID(0x045e, 0x0b21),
    MAKE_VIDPID(0x045e, 0x0b22),
    MAKE_VIDPID(0x046d, 0x0000),
    MAKE_VIDPID(0x046d, 0x1004),
    MAKE_VIDPID(0x046d, 0x1007),
    MAKE_VIDPID(0x046d, 0x1008),
    MAKE_VIDPID(0x046d, 0xf301),
    MAKE_VIDPID(0x0738, 0x02a0),
    MAKE_VIDPID(0x0738, 0x4a01),
    MAKE_VIDPID(0x0738, 0x7263),
    MAKE_VIDPID(0x0738, 0xb738),
    MAKE_VIDPID(0x0738, 0xcb29),
    MAKE_VIDPID(0x0738, 0xf401),
    MAKE_VIDPID(0x0c12, 0x0e17),
    MAKE_VIDPID(0x0c12, 0x0e1c),
    MAKE_VIDPID(0x0c12, 0x0e22),
    MAKE_VIDPID(0x0c12, 0x0e30),
    MAKE_VIDPID(0x0d62, 0x9a1a),
    MAKE_VIDPID(0x0d62, 0x9a1b),
    MAKE_VIDPID(0x0e00, 0x0e00),
    MAKE_VIDPID(0x0e6f, 0x012a),
    MAKE_VIDPID(0x0e6f, 0x0139),
    MAKE_VIDPID(0x0e6f, 0x013B),
    MAKE_VIDPID(0x0e6f, 0x013a),
    MAKE_VIDPID(0x0e6f, 0x0145),
    MAKE_VIDPID(0x0e6f, 0x0146),
    MAKE_VIDPID(0x0e6f, 0x0152),
    MAKE_VIDPID(0x0e6f, 0x015b),
    MAKE_VIDPID(0x0e6f, 0x015c),
    MAKE_VIDPID(0x0e6f, 0x015d),
    MAKE_VIDPID(0x0e6f, 0x015f),
    MAKE_VIDPID(0x0e6f, 0x0160),
    MAKE_VIDPID(0x0e6f, 0x0161),
    MAKE_VIDPID(0x0e6f, 0x0162),
    MAKE_VIDPID(0x0e6f, 0x0163),
    MAKE_VIDPID(0x0e6f, 0x0164),
    MAKE_VIDPID(0x0e6f, 0x0165),
    MAKE_VIDPID(0x0e6f, 0x0166),
    MAKE_VIDPID(0x0e6f, 0x0167),
    MAKE_VIDPID(0x0e6f, 0x0205),
    MAKE_VIDPID(0x0e6f, 0x0206),
    MAKE_VIDPID(0x0e6f, 0x0246),
    MAKE_VIDPID(0x0e6f, 0x0261),
    MAKE_VIDPID(0x0e6f, 0x0262),
    MAKE_VIDPID(0x0e6f, 0x02a0),
    MAKE_VIDPID(0x0e6f, 0x02a1),
    MAKE_VIDPID(0x0e6f, 0x02a2),
    MAKE_VIDPID(0x0e6f, 0x02a3),
    MAKE_VIDPID(0x0e6f, 0x02a4),
    MAKE_VIDPID(0x0e6f, 0x02a5),
    MAKE_VIDPID(0x0e6f, 0x02a6),
    MAKE_VIDPID(0x0e6f, 0x02a7),
    MAKE_VIDPID(0x0e6f, 0x02a8),
    MAKE_VIDPID(0x0e6f, 0x02a9),
    MAKE_VIDPID(0x0e6f, 0x02aa),
    MAKE_VIDPID(0x0e6f, 0x02ab),
    MAKE_VIDPID(0x0e6f, 0x02ac),
    MAKE_VIDPID(0x0e6f, 0x02ad),
    MAKE_VIDPID(0x0e6f, 0x02ae),
    MAKE_VIDPID(0x0e6f, 0x02af),
    MAKE_VIDPID(0x0e6f, 0x02b0),
    MAKE_VIDPID(0x0e6f, 0x02b1),
    MAKE_VIDPID(0x0e6f, 0x02b2),
    MAKE_VIDPID(0x0e6f, 0x02b3),
    MAKE_VIDPID(0x0e6f, 0x02b5),
    MAKE_VIDPID(0x0e6f, 0x02b6),
    MAKE_VIDPID(0x0e6f, 0x02b8),
    MAKE_VIDPID(0x0e6f, 0x02bd),
    MAKE_VIDPID(0x0e6f, 0x02be),
    MAKE_VIDPID(0x0e6f, 0x02bf),
    MAKE_VIDPID(0x0e6f, 0x02c0),
    MAKE_VIDPID(0x0e6f, 0x02c1),
    MAKE_VIDPID(0x0e6f, 0x02c2),
    MAKE_VIDPID(0x0e6f, 0x02c3),
    MAKE_VIDPID(0x0e6f, 0x02c4),
    MAKE_VIDPID(0x0e6f, 0x02c5),
    MAKE_VIDPID(0x0e6f, 0x02c6),
    MAKE_VIDPID(0x0e6f, 0x02c7),
    MAKE_VIDPID(0x0e6f, 0x02c8),
    MAKE_VIDPID(0x0e6f, 0x02c9),
    MAKE_VIDPID(0x0e6f, 0x02ca),
    MAKE_VIDPID(0x0e6f, 0x02cb),
    MAKE_VIDPID(0x0e6f, 0x02cd),
    MAKE_VIDPID(0x0e6f, 0x02ce),
    MAKE_VIDPID(0x0e6f, 0x02cf),
    MAKE_VIDPID(0x0e6f, 0x02d5),
    MAKE_VIDPID(0x0e6f, 0x0346),
    MAKE_VIDPID(0x0e6f, 0x0446),
    MAKE_VIDPID(0x0e6f, 0xf501),
    MAKE_VIDPID(0x0f0d, 0x0063),
    MAKE_VIDPID(0x0f0d, 0x0067),
    MAKE_VIDPID(0x0f0d, 0x0078),
    MAKE_VIDPID(0x0f0d, 0x0097),
    MAKE_VIDPID(0x0f0d, 0x00ba),
    MAKE_VIDPID(0x0f0d, 0x00c0),
    MAKE_VIDPID(0x0f0d, 0x00c5),
    MAKE_VIDPID(0x0f0d, 0x00d8),
    MAKE_VIDPID(0x0f0d, 0x00ed),
    MAKE_VIDPID(0x0fff, 0x02a1),
    MAKE_VIDPID(0x12ab, 0x0304),
    MAKE_VIDPID(0x1430, 0x0291),
    MAKE_VIDPID(0x1430, 0x02a9),
    MAKE_VIDPID(0x1430, 0x070b),
    MAKE_VIDPID(0x1430, 0x0719),
    MAKE_VIDPID(0x146b, 0x0611),
    MAKE_VIDPID(0x1532, 0x0a00),
    MAKE_VIDPID(0x1532, 0x0a03),
    MAKE_VIDPID(0x1532, 0x0a14),
    MAKE_VIDPID(0x1532, 0x0a15),
    MAKE_VIDPID(0x16d0, 0x0f3f),
    MAKE_VIDPID(0x1bad, 0x028e),
    MAKE_VIDPID(0x1bad, 0x02a0),
    MAKE_VIDPID(0x1bad, 0x5500),
    MAKE_VIDPID(0x20ab, 0x55ef),
    MAKE_VIDPID(0x24c6, 0x541a),
    MAKE_VIDPID(0x24c6, 0x542a),
    MAKE_VIDPID(0x24c6, 0x543a),
    MAKE_VIDPID(0x24c6, 0x5509),
    MAKE_VIDPID(0x24c6, 0x551a),
    MAKE_VIDPID(0x24c6, 0x561a),
    MAKE_VIDPID(0x24c6, 0x581a),
    MAKE_VIDPID(0x24c6, 0x591a),
    MAKE_VIDPID(0x24c6, 0x592a),
    MAKE_VIDPID(0x24c6, 0x791a),
    MAKE_VIDPID(0x2516, 0x0069),
    MAKE_VIDPID(0x25b1, 0x0360),
    MAKE_VIDPID(0x2c22, 0x2203),
    MAKE_VIDPID(0x2e24, 0x0652),
    MAKE_VIDPID(0x2e24, 0x1618),
    MAKE_VIDPID(0x2e24, 0x1688),
    MAKE_VIDPID(0x2f24, 0x0011),
    MAKE_VIDPID(0x2f24, 0x002e),
    MAKE_VIDPID(0x2f24, 0x0050),
    MAKE_VIDPID(0x2f24, 0x0053),
    MAKE_VIDPID(0x2f24, 0x008f),
    MAKE_VIDPID(0x2f24, 0x0091),
    MAKE_VIDPID(0x2f24, 0x00b7),
    MAKE_VIDPID(0xd2d2, 0xd2d2),
};
static SDL_vidpid_list old_xboxone_controllers = {
    "SDL_JOYSTICK_OLD_XBOXONE_CONTROLLERS", 0, 0, NULL,
    "SDL_JOYSTICK_OLD_XBOXONE_CONTROLLERS_EXCLUDED", 0, 0, NULL,
    SDL_arraysize(initial_old_xboxone_controllers), initial_old_xboxone_controllers,
    false
};

static Uint32 initial_arcadestick_devices[] = {
    MAKE_VIDPID(0x0079, 0x181a), // Venom Arcade Stick
    MAKE_VIDPID(0x0079, 0x181b), // Venom Arcade Stick
    MAKE_VIDPID(0x0c12, 0x0ef6), // Hitbox Arcade Stick
    MAKE_VIDPID(0x0e6f, 0x0109), // PDP Versus Fighting Pad
    MAKE_VIDPID(0x0f0d, 0x0016), // Hori Real Arcade Pro.EX
    MAKE_VIDPID(0x0f0d, 0x001b), // Hori Real Arcade Pro VX
    MAKE_VIDPID(0x0f0d, 0x0063), // Hori Real Arcade Pro Hayabusa (USA) Xbox One
    MAKE_VIDPID(0x0f0d, 0x006a), // Real Arcade Pro 4
    MAKE_VIDPID(0x0f0d, 0x0078), // Hori Real Arcade Pro V Kai Xbox One
    MAKE_VIDPID(0x0f0d, 0x008a), // HORI Real Arcade Pro 4
    MAKE_VIDPID(0x0f0d, 0x008c), // Hori Real Arcade Pro 4
    MAKE_VIDPID(0x0f0d, 0x00aa), // HORI Real Arcade Pro V Hayabusa in Switch Mode
    MAKE_VIDPID(0x0f0d, 0x00ed), // Hori Fighting Stick mini 4 kai
    MAKE_VIDPID(0x0f0d, 0x011c), // Hori Fighting Stick Alpha in PS4 Mode
    MAKE_VIDPID(0x0f0d, 0x011e), // Hori Fighting Stick Alpha in PC Mode
    MAKE_VIDPID(0x0f0d, 0x0184), // Hori Fighting Stick Alpha in PS5 Mode
    MAKE_VIDPID(0x146b, 0x0604), // NACON Daija Arcade Stick
    MAKE_VIDPID(0x1532, 0x0a00), // Razer Atrox Arcade Stick
    MAKE_VIDPID(0x1bad, 0xf03d), // Street Fighter IV Arcade Stick TE - Chun Li
    MAKE_VIDPID(0x1bad, 0xf502), // Hori Real Arcade Pro.VX SA
    MAKE_VIDPID(0x1bad, 0xf504), // Hori Real Arcade Pro. EX
    MAKE_VIDPID(0x1bad, 0xf506), // Hori Real Arcade Pro.EX Premium VLX
    MAKE_VIDPID(0x20d6, 0xa715), // PowerA Nintendo Switch Fusion Arcade Stick
    MAKE_VIDPID(0x24c6, 0x5000), // Razer Atrox Arcade Stick
    MAKE_VIDPID(0x24c6, 0x5501), // Hori Real Arcade Pro VX-SA
    MAKE_VIDPID(0x24c6, 0x550e), // Hori Real Arcade Pro V Kai 360
    MAKE_VIDPID(0x2c22, 0x2300), // Qanba Obsidian Arcade Joystick in PS4 Mode
    MAKE_VIDPID(0x2c22, 0x2302), // Qanba Obsidian Arcade Joystick in PS3 Mode
    MAKE_VIDPID(0x2c22, 0x2303), // Qanba Obsidian Arcade Joystick in PC Mode
    MAKE_VIDPID(0x2c22, 0x2500), // Qanba Dragon Arcade Joystick in PS4 Mode
    MAKE_VIDPID(0x2c22, 0x2502), // Qanba Dragon Arcade Joystick in PS3 Mode
    MAKE_VIDPID(0x2c22, 0x2503), // Qanba Dragon Arcade Joystick in PC Mode
};
static SDL_vidpid_list arcadestick_devices = {
    SDL_HINT_JOYSTICK_ARCADESTICK_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_ARCADESTICK_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_arcadestick_devices), initial_arcadestick_devices,
    false
};

/* This list is taken from:
   https://raw.githubusercontent.com/denilsonsa/udev-joystick-blacklist/master/generate_rules.py
 */
static Uint32 initial_blacklist_devices[] = {
    // Microsoft Microsoft Wireless Optical Desktop 2.10
    // Microsoft Wireless Desktop - Comfort Edition
    MAKE_VIDPID(0x045e, 0x009d),

    // Microsoft Microsoft Digital Media Pro Keyboard
    // Microsoft Corp. Digital Media Pro Keyboard
    MAKE_VIDPID(0x045e, 0x00b0),

    // Microsoft Microsoft Digital Media Keyboard
    // Microsoft Corp. Digital Media Keyboard 1.0A
    MAKE_VIDPID(0x045e, 0x00b4),

    // Microsoft Microsoft Digital Media Keyboard 3000
    MAKE_VIDPID(0x045e, 0x0730),

    // Microsoft Microsoft 2.4GHz Transceiver v6.0
    // Microsoft Microsoft 2.4GHz Transceiver v8.0
    // Microsoft Corp. Nano Transceiver v1.0 for Bluetooth
    // Microsoft Wireless Mobile Mouse 1000
    // Microsoft Wireless Desktop 3000
    MAKE_VIDPID(0x045e, 0x0745),

    // Microsoft SideWinder(TM) 2.4GHz Transceiver
    MAKE_VIDPID(0x045e, 0x0748),

    // Microsoft Corp. Wired Keyboard 600
    MAKE_VIDPID(0x045e, 0x0750),

    // Microsoft Corp. Sidewinder X4 keyboard
    MAKE_VIDPID(0x045e, 0x0768),

    // Microsoft Corp. Arc Touch Mouse Transceiver
    MAKE_VIDPID(0x045e, 0x0773),

    // Microsoft 2.4GHz Transceiver v9.0
    // Microsoft Nano Transceiver v2.1
    // Microsoft Sculpt Ergonomic Keyboard (5KV-00001)
    MAKE_VIDPID(0x045e, 0x07a5),

    // Microsoft Nano Transceiver v1.0
    // Microsoft Wireless Keyboard 800
    MAKE_VIDPID(0x045e, 0x07b2),

    // Microsoft Nano Transceiver v2.0
    MAKE_VIDPID(0x045e, 0x0800),

    MAKE_VIDPID(0x046d, 0xc30a), // Logitech, Inc. iTouch Composite keyboard

    MAKE_VIDPID(0x04d9, 0xa0df), // Tek Syndicate Mouse (E-Signal USB Gaming Mouse)

    // List of Wacom devices at: http://linuxwacom.sourceforge.net/wiki/index.php/Device_IDs
    MAKE_VIDPID(0x056a, 0x0010), // Wacom ET-0405 Graphire
    MAKE_VIDPID(0x056a, 0x0011), // Wacom ET-0405A Graphire2 (4x5)
    MAKE_VIDPID(0x056a, 0x0012), // Wacom ET-0507A Graphire2 (5x7)
    MAKE_VIDPID(0x056a, 0x0013), // Wacom CTE-430 Graphire3 (4x5)
    MAKE_VIDPID(0x056a, 0x0014), // Wacom CTE-630 Graphire3 (6x8)
    MAKE_VIDPID(0x056a, 0x0015), // Wacom CTE-440 Graphire4 (4x5)
    MAKE_VIDPID(0x056a, 0x0016), // Wacom CTE-640 Graphire4 (6x8)
    MAKE_VIDPID(0x056a, 0x0017), // Wacom CTE-450 Bamboo Fun (4x5)
    MAKE_VIDPID(0x056a, 0x0018), // Wacom CTE-650 Bamboo Fun 6x8
    MAKE_VIDPID(0x056a, 0x0019), // Wacom CTE-631 Bamboo One
    MAKE_VIDPID(0x056a, 0x00d1), // Wacom Bamboo Pen and Touch CTH-460
    MAKE_VIDPID(0x056a, 0x030e), // Wacom Intuos Pen (S) CTL-480

    MAKE_VIDPID(0x09da, 0x054f), // A4 Tech Co., G7 750 mouse
    MAKE_VIDPID(0x09da, 0x1410), // A4 Tech Co., Ltd Bloody AL9 mouse
    MAKE_VIDPID(0x09da, 0x3043), // A4 Tech Co., Ltd Bloody R8A Gaming Mouse
    MAKE_VIDPID(0x09da, 0x31b5), // A4 Tech Co., Ltd Bloody TL80 Terminator Laser Gaming Mouse
    MAKE_VIDPID(0x09da, 0x3997), // A4 Tech Co., Ltd Bloody RT7 Terminator Wireless
    MAKE_VIDPID(0x09da, 0x3f8b), // A4 Tech Co., Ltd Bloody V8 mouse
    MAKE_VIDPID(0x09da, 0x51f4), // Modecom MC-5006 Keyboard
    MAKE_VIDPID(0x09da, 0x5589), // A4 Tech Co., Ltd Terminator TL9 Laser Gaming Mouse
    MAKE_VIDPID(0x09da, 0x7b22), // A4 Tech Co., Ltd Bloody V5
    MAKE_VIDPID(0x09da, 0x7f2d), // A4 Tech Co., Ltd Bloody R3 mouse
    MAKE_VIDPID(0x09da, 0x8090), // A4 Tech Co., Ltd X-718BK Oscar Optical Gaming Mouse
    MAKE_VIDPID(0x09da, 0x9033), // A4 Tech Co., X7 X-705K
    MAKE_VIDPID(0x09da, 0x9066), // A4 Tech Co., Sharkoon Fireglider Optical
    MAKE_VIDPID(0x09da, 0x9090), // A4 Tech Co., Ltd XL-730K / XL-750BK / XL-755BK Laser Mouse
    MAKE_VIDPID(0x09da, 0x90c0), // A4 Tech Co., Ltd X7 G800V keyboard
    MAKE_VIDPID(0x09da, 0xf012), // A4 Tech Co., Ltd Bloody V7 mouse
    MAKE_VIDPID(0x09da, 0xf32a), // A4 Tech Co., Ltd Bloody B540 keyboard
    MAKE_VIDPID(0x09da, 0xf613), // A4 Tech Co., Ltd Bloody V2 mouse
    MAKE_VIDPID(0x09da, 0xf624), // A4 Tech Co., Ltd Bloody B120 Keyboard

    MAKE_VIDPID(0x1b1c, 0x1b3c), // Corsair Harpoon RGB gaming mouse

    MAKE_VIDPID(0x1d57, 0xad03), // [T3] 2.4GHz and IR Air Mouse Remote Control

    MAKE_VIDPID(0x1e7d, 0x2e4a), // Roccat Tyon Mouse

    MAKE_VIDPID(0x20a0, 0x422d), // Winkeyless.kr Keyboards

    MAKE_VIDPID(0x2516, 0x001f), // Cooler Master Storm Mizar Mouse
    MAKE_VIDPID(0x2516, 0x0028), // Cooler Master Storm Alcor Mouse

    /*****************************************************************/
    // Additional entries
    /*****************************************************************/

    MAKE_VIDPID(0x04d9, 0x8008), // OBINLB USB-HID Keyboard (Anne Pro II)
    MAKE_VIDPID(0x04d9, 0x8009), // OBINLB USB-HID Keyboard (Anne Pro II)
    MAKE_VIDPID(0x04d9, 0xa292), // OBINLB USB-HID Keyboard (Anne Pro II)
    MAKE_VIDPID(0x04d9, 0xa293), // OBINLB USB-HID Keyboard (Anne Pro II)
    MAKE_VIDPID(0x04f2, 0xa13c), // HP Deluxe Webcam KQ246AA
    MAKE_VIDPID(0x0e6f, 0x018a), // PDP REALMz Wireless Controller for Switch, USB charging
    MAKE_VIDPID(0x1532, 0x0266), // Razer Huntsman V2 Analog, non-functional DInput device
    MAKE_VIDPID(0x1532, 0x0282), // Razer Huntsman Mini Analog, non-functional DInput device
    MAKE_VIDPID(0x20d6, 0x0002), // PowerA Enhanced Wireless Controller for Nintendo Switch (charging port only)
    MAKE_VIDPID(0x256c, 0x006d), // Huion Tablet_GS1331, Huion Tablet_GS1331 Touch Strip
    MAKE_VIDPID(0x256c, 0x006e), // Huion Tablet Kamvas Pro 22, Huion Tablet Kamvas Pro 22 Touch Strip
    MAKE_VIDPID(0x26ce, 0x01a2), // ASRock LED Controller
    MAKE_VIDPID(0x3297, 0x1969), // Moonlander MK1 Keyboard
    MAKE_VIDPID(0x3434, 0x0121), // Keychron Q3 System Control
    MAKE_VIDPID(0x3434, 0x0163), // Keychron Q6 System Control
    MAKE_VIDPID(0x3434, 0x0211), // Keychron K1 Pro System Control
    MAKE_VIDPID(0x3434, 0x02a0), // Keychron K10 Pro System Control
    MAKE_VIDPID(0x3434, 0x0353), // Keychron V5 System Control
    MAKE_VIDPID(0x3434, 0xd030), // Keychron Link
};
static SDL_vidpid_list blacklist_devices = {
    SDL_HINT_JOYSTICK_BLACKLIST_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_BLACKLIST_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_blacklist_devices), initial_blacklist_devices,
    false
};

static Uint32 initial_flightstick_devices[] = {
    MAKE_VIDPID(0x044f, 0x0402), // HOTAS Warthog Joystick
    MAKE_VIDPID(0x044f, 0xb10a), // ThrustMaster, Inc. T.16000M Joystick
    MAKE_VIDPID(0x046d, 0xc215), // Logitech Extreme 3D
    MAKE_VIDPID(0x0583, 0x6258), // Padix USB joystick with viewfinder
    MAKE_VIDPID(0x0583, 0x688f), // Padix QF-688uv Windstorm Pro
    MAKE_VIDPID(0x0583, 0x7070), // Padix QF-707u Bazooka
    MAKE_VIDPID(0x0583, 0xa019), // Padix USB vibration joystick with viewfinder
    MAKE_VIDPID(0x0583, 0xa131), // Padix USB Wireless 2.4GHz
    MAKE_VIDPID(0x0583, 0xa209), // Padix MetalStrike ForceFeedback
    MAKE_VIDPID(0x0583, 0xb010), // Padix MetalStrike Pro
    MAKE_VIDPID(0x0583, 0xb012), // Padix Wireless MetalStrike
    MAKE_VIDPID(0x0583, 0xb013), // Padix USB Wireless 2.4GHZ
    MAKE_VIDPID(0x0738, 0x2221), // Saitek Pro Flight X-56 Rhino Stick
    MAKE_VIDPID(0x10f5, 0x7084), // Turtle Beach VelocityOne
    MAKE_VIDPID(0x231d, 0x0126), // Gunfighter Mk.III 'Space Combat Edition' (right)
    MAKE_VIDPID(0x231d, 0x0127), // Gunfighter Mk.III 'Space Combat Edition' (left)
    MAKE_VIDPID(0x231d, 0x0200), // VKB Gladiator NXT Evo (grip)
    MAKE_VIDPID(0x3344, 0x4391), // VIRPIL Controls R-VPC Stick MT-50CM3
    MAKE_VIDPID(0x3344, 0x8390), // VIRPIL Controls L-VPC Stick MT-50CM3
    MAKE_VIDPID(0x362c, 0x0001), // Yawman Arrow
};
static SDL_vidpid_list flightstick_devices = {
    SDL_HINT_JOYSTICK_FLIGHTSTICK_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_FLIGHTSTICK_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_flightstick_devices), initial_flightstick_devices,
    false
};

static Uint32 initial_gamecube_devices[] = {
    MAKE_VIDPID(0x0079, 0x1843), // DragonRise GameCube Controller Adapter
    MAKE_VIDPID(0x0079, 0x1844), // DragonRise GameCube Controller Adapter
    MAKE_VIDPID(0x0079, 0x1846), // DragonRise GameCube Controller Adapter
    MAKE_VIDPID(0x057e, 0x0337), // Nintendo Wii U GameCube Controller Adapter
    MAKE_VIDPID(0x057e, 0x2073), // Nintendo Switch 2 NSO GameCube Controller
    MAKE_VIDPID(0x05e3, 0x0681), // Austgame GameCube to USB convertor
    MAKE_VIDPID(0x0926, 0x8888), // Cyber Gadget GameCube Controller
    MAKE_VIDPID(0x0e6f, 0x0185), // PDP Wired Fight Pad Pro for Nintendo Switch
    MAKE_VIDPID(0x1a34, 0xf705), // GameCube {HuiJia USB box}
    MAKE_VIDPID(0x20d6, 0xa711), // PowerA Wired Controller Nintendo GameCube Style
};
static SDL_vidpid_list gamecube_devices = {
    SDL_HINT_JOYSTICK_GAMECUBE_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_GAMECUBE_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_gamecube_devices), initial_gamecube_devices,
    false
};

static Uint32 initial_rog_gamepad_mice[] = {
    MAKE_VIDPID(0x0b05, 0x18e3), // ROG Chakram (wired) Mouse
    MAKE_VIDPID(0x0b05, 0x18e5), // ROG Chakram (wireless) Mouse
    MAKE_VIDPID(0x0b05, 0x1906), // ROG Pugio II
    MAKE_VIDPID(0x0b05, 0x1958), // ROG Chakram Core Mouse
    MAKE_VIDPID(0x0b05, 0x1a18), // ROG Chakram X (wired) Mouse
    MAKE_VIDPID(0x0b05, 0x1a1a), // ROG Chakram X (wireless) Mouse
    MAKE_VIDPID(0x0b05, 0x1a1c), // ROG Chakram X (Bluetooth) Mouse
};
static SDL_vidpid_list rog_gamepad_mice = {
    SDL_HINT_ROG_GAMEPAD_MICE, 0, 0, NULL,
    SDL_HINT_ROG_GAMEPAD_MICE_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_rog_gamepad_mice), initial_rog_gamepad_mice,
    false
};

static Uint32 initial_throttle_devices[] = {
    MAKE_VIDPID(0x044f, 0x0404), // HOTAS Warthog Throttle
    MAKE_VIDPID(0x0738, 0xa221), // Saitek Pro Flight X-56 Rhino Throttle
    MAKE_VIDPID(0x10f5, 0x7085), // Turtle Beach VelocityOne Throttle
    MAKE_VIDPID(0x3344, 0x0196), // VIRPIL Controls VPC VMAX Prime Throttle
};
static SDL_vidpid_list throttle_devices = {
    SDL_HINT_JOYSTICK_THROTTLE_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_THROTTLE_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_throttle_devices), initial_throttle_devices,
    false
};

static Uint32 initial_wheel_devices[] = {
    MAKE_VIDPID(0x0079, 0x1864), // DragonRise Inc. Wired Wheel (active mode) (also known as PXN V900 (PS3), Superdrive SV-750, or a Genesis Seaborg 400)
    MAKE_VIDPID(0x044f, 0xb65d), // Thrustmaster Wheel FFB
    MAKE_VIDPID(0x044f, 0xb65e), // Thrustmaster T500RS
    MAKE_VIDPID(0x044f, 0xb664), // Thrustmaster TX (initial mode)
    MAKE_VIDPID(0x044f, 0xb669), // Thrustmaster TX (active mode)
    MAKE_VIDPID(0x044f, 0xb66d), // Thrustmaster T300RS (PS4 mode)
    MAKE_VIDPID(0x044f, 0xb66d), // Thrustmaster Wheel FFB
    MAKE_VIDPID(0x044f, 0xb66e), // Thrustmaster T300RS (normal mode)
    MAKE_VIDPID(0x044f, 0xb66f), // Thrustmaster T300RS (advanced mode)
    MAKE_VIDPID(0x044f, 0xb677), // Thrustmaster T150
    MAKE_VIDPID(0x044f, 0xb67f), // Thrustmaster TMX
    MAKE_VIDPID(0x044f, 0xb691), // Thrustmaster TS-XW (initial mode)
    MAKE_VIDPID(0x044f, 0xb692), // Thrustmaster TS-XW (active mode)
    MAKE_VIDPID(0x044f, 0xb696), // Thrustmaster T248
    MAKE_VIDPID(0x046d, 0xc24f), // Logitech G29 (PS3)
    MAKE_VIDPID(0x046d, 0xc260), // Logitech G29 (PS4)
    MAKE_VIDPID(0x046d, 0xc261), // Logitech G920 (initial mode)
    MAKE_VIDPID(0x046d, 0xc262), // Logitech G920 (active mode)
    MAKE_VIDPID(0x046d, 0xc266), // Logitech G923 for Playstation 4 and PC (PC mode)
    MAKE_VIDPID(0x046d, 0xc267), // Logitech G923 for Playstation 4 and PC (PS4 mode)
    MAKE_VIDPID(0x046d, 0xc268), // Logitech PRO Racing Wheel (PC mode)
    MAKE_VIDPID(0x046d, 0xc269), // Logitech PRO Racing Wheel (PS4/PS5 mode)
    MAKE_VIDPID(0x046d, 0xc26d), // Logitech G923 (Xbox)
    MAKE_VIDPID(0x046d, 0xc26e), // Logitech G923
    MAKE_VIDPID(0x046d, 0xc272), // Logitech PRO Racing Wheel for Xbox (PC mode)
    MAKE_VIDPID(0x046d, 0xc294), // Logitech generic wheel
    MAKE_VIDPID(0x046d, 0xc295), // Logitech Momo Force
    MAKE_VIDPID(0x046d, 0xc298), // Logitech Driving Force Pro
    MAKE_VIDPID(0x046d, 0xc299), // Logitech G25
    MAKE_VIDPID(0x046d, 0xc29a), // Logitech Driving Force GT
    MAKE_VIDPID(0x046d, 0xc29b), // Logitech G27
    MAKE_VIDPID(0x046d, 0xca03), // Logitech Momo Racing
    MAKE_VIDPID(0x045e, 0x02a2), // Xbox 360 Wireless Racing Wheel
    MAKE_VIDPID(0x0483, 0x0522), // Simagic Wheelbase (including M10, Alpha Mini, Alpha, Alpha U)
    MAKE_VIDPID(0x0483, 0xa355), // VRS DirectForce Pro Wheel Base
    MAKE_VIDPID(0x0583, 0xa132), // Padix USB Wireless 2.4GHz Wheelpad
    MAKE_VIDPID(0x0583, 0xa133), // Padix USB Wireless 2.4GHz Wheel
    MAKE_VIDPID(0x0583, 0xa202), // Padix Force Feedback Wheel
    MAKE_VIDPID(0x0583, 0xb002), // Padix Vibration USB Wheel
    MAKE_VIDPID(0x0583, 0xb005), // Padix USB Wheel
    MAKE_VIDPID(0x0583, 0xb008), // Padix USB Wireless 2.4GHz Wheel
    MAKE_VIDPID(0x0583, 0xb009), // Padix USB Wheel
    MAKE_VIDPID(0x0583, 0xb018), // Padix TW6 Wheel
    MAKE_VIDPID(0x0eb7, 0x0001), // Fanatec ClubSport Wheel Base V2
    MAKE_VIDPID(0x0eb7, 0x0004), // Fanatec ClubSport Wheel Base V2.5
    MAKE_VIDPID(0x0eb7, 0x0005), // Fanatec CSL Elite Wheel Base+ (PS4)
    MAKE_VIDPID(0x0eb7, 0x0006), // Fanatec Podium Wheel Base DD1
    MAKE_VIDPID(0x0eb7, 0x0007), // Fanatec Podium Wheel Base DD2
    MAKE_VIDPID(0x0eb7, 0x0011), // Fanatec Forza Motorsport (CSR Wheel / CSR Elite Wheel)
    MAKE_VIDPID(0x0eb7, 0x0020), // Fanatec generic wheel / CSL DD / GT DD Pro
    MAKE_VIDPID(0x0eb7, 0x0197), // Fanatec Porsche Wheel (Turbo / GT3 RS / Turbo S / GT3 V2 / GT2)
    MAKE_VIDPID(0x0eb7, 0x038e), // Fanatec ClubSport Wheel Base V1
    MAKE_VIDPID(0x0eb7, 0x0e03), // Fanatec CSL Elite Wheel Base
    MAKE_VIDPID(0x11ff, 0x0511), // DragonRise Inc. Wired Wheel (initial mode) (also known as PXN V900 (PS3), Superdrive SV-750, or a Genesis Seaborg 400)
    MAKE_VIDPID(0x1209, 0xffb0), // Generic FFBoard OpenFFBoard universal forcefeedback wheel
    MAKE_VIDPID(0x16d0, 0x0d5a), // Simucube 1 Wheelbase
    MAKE_VIDPID(0x16d0, 0x0d5f), // Simucube 2 Ultimate Wheelbase
    MAKE_VIDPID(0x16d0, 0x0d60), // Simucube 2 Pro Wheelbase
    MAKE_VIDPID(0x16d0, 0x0d61), // Simucube 2 Sport Wheelbase
    MAKE_VIDPID(0x2433, 0xf300), // Asetek SimSports Invicta Wheelbase
    MAKE_VIDPID(0x2433, 0xf301), // Asetek SimSports Forte Wheelbase
    MAKE_VIDPID(0x2433, 0xf303), // Asetek SimSports La Prima Wheelbase
    MAKE_VIDPID(0x2433, 0xf306), // Asetek SimSports Tony Kannan Wheelbase
    MAKE_VIDPID(0x3416, 0x0301), // Cammus C5 Wheelbase
    MAKE_VIDPID(0x3416, 0x0302), // Cammus C12 Wheelbase
    MAKE_VIDPID(0x346e, 0x0000), // Moza R16/R21 Wheelbase
    MAKE_VIDPID(0x346e, 0x0002), // Moza R9 Wheelbase
    MAKE_VIDPID(0x346e, 0x0004), // Moza R5 Wheelbase
    MAKE_VIDPID(0x346e, 0x0005), // Moza R3 Wheelbase
    MAKE_VIDPID(0x346e, 0x0006), // Moza R12 Wheelbase
    MAKE_VIDPID(0x36e6, 0x400f), // PXN VD6 Wheelbase
};
static SDL_vidpid_list wheel_devices = {
    SDL_HINT_JOYSTICK_WHEEL_DEVICES, 0, 0, NULL,
    SDL_HINT_JOYSTICK_WHEEL_DEVICES_EXCLUDED, 0, 0, NULL,
    SDL_arraysize(initial_wheel_devices), initial_wheel_devices,
    false
};

static Uint32 initial_guitar_devices[] = {
    MAKE_VIDPID(0x12ba, 0x0100), // PS3 Guitar Hero Guitar
    MAKE_VIDPID(0x12ba, 0x0200), // PS3 Rock Band Guitar
    MAKE_VIDPID(0x12ba, 0x074b), // PS3 / Wii U Guitar Hero Live Guitar
    MAKE_VIDPID(0x1BAD, 0x0004), // Wii RB1 Guitar (Uses PS3 protocol)
    MAKE_VIDPID(0x1BAD, 0x3010), // Wii RB2 Guitar (Uses PS3 protocol)
    MAKE_VIDPID(0x0351, 0x1000), // CRKD Guitar
    MAKE_VIDPID(0x0351, 0x2000), // CRKD Guitar
    MAKE_VIDPID(0x0738, 0x02A6), // Mad Catz Wireless Rock Band Guitar
    MAKE_VIDPID(0x0738, 0x02AB), // Mad Catz Wireless Precision Bass Guitar
    MAKE_VIDPID(0x0738, 0x9806), // Mad Catz Precision Bass Guitar
    MAKE_VIDPID(0x1430, 0x02a7), // Guitar Hero Wireless Guitar (Linux)
    MAKE_VIDPID(0x1430, 0x0705), // Guitar Hero 5 Guitar
    MAKE_VIDPID(0x1430, 0x070B), // Guitar Hero Live Guitar
    MAKE_VIDPID(0x1430, 0x4734), // Guitar Hero World Tour Kiosk
    MAKE_VIDPID(0x1430, 0x4748), // RedOctane Guitar Hero X-plorer
    MAKE_VIDPID(0x1bad, 0x02a6), // Rock Band 2 Wireless Guitar (Linux)
    MAKE_VIDPID(0x1bad, 0x02ab), // Rock Band Wireless Bass Guitar (Linux)
    MAKE_VIDPID(0x2068, 0x0001), // Power Gig Guitar
    MAKE_VIDPID(0x3651, 0x1000), // CRKD Guitar
    MAKE_VIDPID(0x3651, 0x6000), // CRKD Guitar
};
static SDL_vidpid_list guitar_devices = {
    SDL_HINT_JOYSTICK_GUITAR_DEVICES, 0, 0, NULL,
    NULL, 0, 0, NULL,
    SDL_arraysize(initial_guitar_devices), initial_guitar_devices,
    false
};

static Uint32 initial_drum_devices[] = {
    MAKE_VIDPID(0x12ba, 0x0120), // PS3 Guitar Hero Drums
    MAKE_VIDPID(0x12ba, 0x0210), // PS3 Rock Band Drums
    MAKE_VIDPID(0x12ba, 0x0218), // PS3 Midi Pro Adapter - Drums Mode
    MAKE_VIDPID(0x1BAD, 0x0005), // Wii RB1 Drums (Uses PS3 protocol)
    MAKE_VIDPID(0x1BAD, 0x3110), // Wii RB2 Drums (Uses PS3 protocol)
    MAKE_VIDPID(0x1BAD, 0x3138), // Wii RB3 Midi Pro Adapter - Drums Mode (Uses PS3 protocol)
    MAKE_VIDPID(0x1430, 0x02a8), // Guitar Hero Wireless Drum Kit (Linux)
    MAKE_VIDPID(0x1430, 0x0805), // Band Hero Wireless Drum Kit
    MAKE_VIDPID(0x1bad, 0x0003), // Harmonix Rock Band Drumkit
    MAKE_VIDPID(0x1bad, 0x0130), // ION Drum Rocker
    MAKE_VIDPID(0x2068, 0x0002), // Power Gig Drums
};
static SDL_vidpid_list drum_devices = {
    SDL_HINT_JOYSTICK_DRUM_DEVICES, 0, 0, NULL,
    NULL, 0, 0, NULL,
    SDL_arraysize(initial_drum_devices), initial_drum_devices,
    false
};

void SDL_InitJoystickClassificationLists(void)
{
    SDL_LoadVIDPIDList(&old_xboxone_controllers);
    SDL_LoadVIDPIDList(&arcadestick_devices);
    SDL_LoadVIDPIDList(&blacklist_devices);
    SDL_LoadVIDPIDList(&drum_devices);
    SDL_LoadVIDPIDList(&flightstick_devices);
    SDL_LoadVIDPIDList(&gamecube_devices);
    SDL_LoadVIDPIDList(&guitar_devices);
    SDL_LoadVIDPIDList(&rog_gamepad_mice);
    SDL_LoadVIDPIDList(&throttle_devices);
    SDL_LoadVIDPIDList(&wheel_devices);
}

void SDL_QuitJoystickClassificationLists(void)
{
    SDL_FreeVIDPIDList(&old_xboxone_controllers);
    SDL_FreeVIDPIDList(&arcadestick_devices);
    SDL_FreeVIDPIDList(&blacklist_devices);
    SDL_FreeVIDPIDList(&drum_devices);
    SDL_FreeVIDPIDList(&flightstick_devices);
    SDL_FreeVIDPIDList(&gamecube_devices);
    SDL_FreeVIDPIDList(&guitar_devices);
    SDL_FreeVIDPIDList(&rog_gamepad_mice);
    SDL_FreeVIDPIDList(&throttle_devices);
    SDL_FreeVIDPIDList(&wheel_devices);
}

char *SDL_CreateJoystickName(Uint16 vendor, Uint16 product, const char *vendor_name, const char *product_name)
{
    const char *custom_name = GuessControllerName(vendor, product);
    if (custom_name) {
        return SDL_strdup(custom_name);
    }

    return SDL_CreateDeviceName(vendor, product, vendor_name, product_name, "Controller");
}

SDL_GamepadType SDL_GetGamepadTypeFromVIDPID(Uint16 vendor, Uint16 product, const char *name, bool forUI)
{
    SDL_GamepadType type = SDL_GAMEPAD_TYPE_STANDARD;

    if (vendor == 0x0000 && product == 0x0000) {
        // Some devices are only identifiable by their name
        if (name &&
            (SDL_strcmp(name, "Lic Pro Controller") == 0 ||
             SDL_strcmp(name, "Nintendo Wireless Gamepad") == 0 ||
             SDL_strcmp(name, "Wireless Gamepad") == 0)) {
            // HORI or PowerA Switch Pro Controller clone
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
        }

    } else if (vendor == 0x0001 && product == 0x0001) {
        type = SDL_GAMEPAD_TYPE_STANDARD;

    } else if (vendor == USB_VENDOR_NINTENDO &&
               (product == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_LEFT ||
                product == USB_PRODUCT_NINTENDO_SWITCH2_JOYCON_LEFT)) {
        type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT;

    } else if (vendor == USB_VENDOR_NINTENDO &&
               (product == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_RIGHT ||
                product == USB_PRODUCT_NINTENDO_SWITCH2_JOYCON_RIGHT)) {
        if (name && SDL_strstr(name, "NES Controller") != NULL) {
            // We don't have a type for the Nintendo Online NES Controller
            type = SDL_GAMEPAD_TYPE_STANDARD;
        } else {
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT;
        }

    } else if (vendor == USB_VENDOR_NINTENDO && product == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_GRIP) {
        if (name && SDL_strstr(name, "(L)") != NULL) {
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT;
        } else if (name && SDL_strstr(name, "L+R") != NULL) {
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR;
        } else {
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT;
        }

    } else if (vendor == USB_VENDOR_NINTENDO &&
               (product == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_PAIR ||
                product == USB_PRODUCT_NINTENDO_SWITCH2_JOYCON_PAIR)) {
        type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR;

    } else if (forUI && SDL_IsJoystickGameCube(vendor, product)) {
        type = SDL_GAMEPAD_TYPE_GAMECUBE;

    } else {
        switch (GuessControllerType(vendor, product)) {
        case k_eControllerType_XBox360Controller:
            type = SDL_GAMEPAD_TYPE_XBOX360;
            break;
        case k_eControllerType_XBoxOneController:
        case k_eControllerType_XBoxEliteController:
            type = SDL_GAMEPAD_TYPE_XBOXONE;
            break;
        case k_eControllerType_PS3Controller:
            type = SDL_GAMEPAD_TYPE_PS3;
            break;
        case k_eControllerType_PS4Controller:
            type = SDL_GAMEPAD_TYPE_PS4;
            break;
        case k_eControllerType_PS5Controller:
        case k_eControllerType_PS5EdgeController:
            type = SDL_GAMEPAD_TYPE_PS5;
            break;
        case k_eControllerType_XInputPS4Controller:
            if (forUI) {
                type = SDL_GAMEPAD_TYPE_PS4;
            } else {
                type = SDL_GAMEPAD_TYPE_STANDARD;
            }
            break;
        case k_eControllerType_SwitchProController:
        case k_eControllerType_Switch2ProController:
        case k_eControllerType_SwitchInputOnlyController:
        case k_eControllerType_Switch2InputOnlyController:
            type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
            break;
        case k_eControllerType_XInputSwitchController:
            if (forUI) {
                type = SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO;
            } else {
                type = SDL_GAMEPAD_TYPE_XBOX360;
            }
            break;
        case k_eControllerType_SteamController:
        case k_eControllerType_SteamControllerV2:
        case k_eControllerType_SteamControllerNeptune:
        case k_eControllerType_SteamControllerTriton:
        case k_eControllerType_HoriSteamController:
        case k_eControllerType_UnknownSteamController:
            type = SDL_GAMEPAD_TYPE_STEAM;
            break;
        default:
            break;
        }
    }
    return type;
}

SDL_GamepadType SDL_GetGamepadTypeFromGUID(SDL_GUID guid, const char *name)
{
    SDL_GamepadType type;
    Uint16 vendor, product;

    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, NULL, NULL);
    type = SDL_GetGamepadTypeFromVIDPID(vendor, product, name, true);
    if (type == SDL_GAMEPAD_TYPE_STANDARD) {
        if (SDL_IsJoystickXInput(guid)) {
            // This is probably an Xbox One controller
            return SDL_GAMEPAD_TYPE_XBOXONE;
        }
#ifdef SDL_JOYSTICK_HIDAPI
        if (SDL_IsJoystickHIDAPI(guid)) {
            return HIDAPI_GetGamepadTypeFromGUID(guid);
        }
#endif // SDL_JOYSTICK_HIDAPI
    }
    return type;
}

bool SDL_IsJoystickXboxOne(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_XBoxOneController ||
           eType == k_eControllerType_XBoxEliteController;
}

bool SDL_IsJoystickXboxOneElite(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_XBoxEliteController;
}

bool SDL_IsJoystickXboxSeriesX(Uint16 vendor_id, Uint16 product_id)
{
    // Most new controllers have the share button, so we'll default to true and
    // have a list of older XBox One controllers that are known not to have it.
    if (SDL_VIDPIDInList(vendor_id, product_id, &old_xboxone_controllers)) {
        return false;
    }
    return true;
}

bool SDL_IsJoystickBluetoothXboxOne(Uint16 vendor_id, Uint16 product_id)
{
    if (vendor_id == USB_VENDOR_MICROSOFT) {
        if (product_id == USB_PRODUCT_XBOX_ONE_ADAPTIVE_BLUETOOTH ||
            product_id == USB_PRODUCT_XBOX_ONE_ADAPTIVE_BLE ||
            product_id == USB_PRODUCT_XBOX_ONE_S_REV1_BLUETOOTH ||
            product_id == USB_PRODUCT_XBOX_ONE_S_REV2_BLUETOOTH ||
            product_id == USB_PRODUCT_XBOX_ONE_S_REV2_BLE ||
            product_id == USB_PRODUCT_XBOX_ONE_ELITE_SERIES_2_BLUETOOTH ||
            product_id == USB_PRODUCT_XBOX_ONE_ELITE_SERIES_2_BLE ||
            product_id == USB_PRODUCT_XBOX_SERIES_X_BLE) {
            return true;
        }
    }
    return false;
}

bool SDL_IsJoystickPS4(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_PS4Controller;
}

bool SDL_IsJoystickPS5(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_PS5Controller ||
           eType == k_eControllerType_PS5EdgeController;
}

bool SDL_IsJoystickDualSenseEdge(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_PS5EdgeController;
}

bool SDL_IsJoystickNintendoSwitchPro(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SwitchProController ||
           eType == k_eControllerType_SwitchInputOnlyController;
}

bool SDL_IsJoystickNintendoSwitch2Pro(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_Switch2ProController ||
           eType == k_eControllerType_Switch2InputOnlyController;
}

bool SDL_IsJoystickNintendoSwitchProInputOnly(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SwitchInputOnlyController;
}

bool SDL_IsJoystickNintendoSwitch2ProInputOnly(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_Switch2InputOnlyController;
}

bool SDL_IsJoystickNintendoSwitchJoyCon(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SwitchJoyConLeft || eType == k_eControllerType_SwitchJoyConRight;
}

bool SDL_IsJoystickNintendoSwitchJoyConLeft(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SwitchJoyConLeft;
}

bool SDL_IsJoystickNintendoSwitchJoyConRight(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SwitchJoyConRight;
}

bool SDL_IsJoystickNintendoSwitchJoyConGrip(Uint16 vendor_id, Uint16 product_id)
{
    return vendor_id == USB_VENDOR_NINTENDO && product_id == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_GRIP;
}

bool SDL_IsJoystickNintendoSwitchJoyConPair(Uint16 vendor_id, Uint16 product_id)
{
    return vendor_id == USB_VENDOR_NINTENDO &&
           (product_id == USB_PRODUCT_NINTENDO_SWITCH_JOYCON_PAIR ||
            product_id == USB_PRODUCT_NINTENDO_SWITCH2_JOYCON_PAIR);
}

bool SDL_IsJoystickGameCube(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &gamecube_devices);
}

bool SDL_IsJoystickAmazonLunaController(Uint16 vendor_id, Uint16 product_id)
{
    return ((vendor_id == USB_VENDOR_AMAZON && product_id == USB_PRODUCT_AMAZON_LUNA_CONTROLLER) ||
            (vendor_id == BLUETOOTH_VENDOR_AMAZON && product_id == BLUETOOTH_PRODUCT_LUNA_CONTROLLER));
}

bool SDL_IsJoystickGoogleStadiaController(Uint16 vendor_id, Uint16 product_id)
{
    return vendor_id == USB_VENDOR_GOOGLE && product_id == USB_PRODUCT_GOOGLE_STADIA_CONTROLLER;
}

bool SDL_IsJoystickNVIDIASHIELDController(Uint16 vendor_id, Uint16 product_id)
{
    return (vendor_id == USB_VENDOR_NVIDIA &&
            (product_id == USB_PRODUCT_NVIDIA_SHIELD_CONTROLLER_V103 ||
             product_id == USB_PRODUCT_NVIDIA_SHIELD_CONTROLLER_V104));
}

bool SDL_IsJoystickSteamVirtualGamepad(Uint16 vendor_id, Uint16 product_id, Uint16 version)
{
#ifdef SDL_PLATFORM_MACOS
    return (vendor_id == USB_VENDOR_MICROSOFT && product_id == USB_PRODUCT_XBOX360_WIRED_CONTROLLER && version == 0);
#else
    return (vendor_id == USB_VENDOR_VALVE && product_id == USB_PRODUCT_STEAM_VIRTUAL_GAMEPAD);
#endif
}

bool SDL_IsJoystickSteamController(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SteamController || eType == k_eControllerType_SteamControllerV2;
}

bool SDL_IsJoystickHoriSteamController(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_HoriSteamController;
}

bool SDL_IsJoystickSInputController(Uint16 vendor_id, Uint16 product_id)
{
    if (vendor_id == USB_VENDOR_RASPBERRYPI) {
        if (product_id == USB_PRODUCT_HANDHELDLEGEND_SINPUT_GENERIC ||
            product_id == USB_PRODUCT_HANDHELDLEGEND_PROGCC ||
            product_id == USB_PRODUCT_HANDHELDLEGEND_GCULTIMATE ||
            product_id == USB_PRODUCT_BONZIRICHANNEL_FIREBIRD ||
            product_id == USB_PRODUCT_VOIDGAMING_PS4FIREBIRD) {
            return true;
        }
    }
    if (vendor_id == USB_VENDOR_ANDGAMER) {
        if (product_id == USB_PRODUCT_VOIDGAMING_GENESIS_SINPUT) {
            return true;
        }
    }
    return false;
}

bool SDL_IsJoystickFlydigiController(Uint16 vendor_id, Uint16 product_id)
{
    if (vendor_id == USB_VENDOR_FLYDIGI_V1) {
        if (product_id == USB_PRODUCT_FLYDIGI_V1_GAMEPAD) {
            return true;
        }
    }
    if (vendor_id == USB_VENDOR_FLYDIGI_V2) {
        if (product_id == USB_PRODUCT_FLYDIGI_V2_APEX || product_id == USB_PRODUCT_FLYDIGI_V2_VADER || product_id == USB_PRODUCT_FLYDIGI_V2_APEX6) {
            return true;
        }
    }
    return false;
}

bool SDL_IsJoystickGameSirController(Uint16 vendor_id, Uint16 product_id)
{
    if (vendor_id != USB_VENDOR_GAMESIR) {
        return false;
    }

    return (product_id == USB_PRODUCT_GAMESIR_GAMEPAD_G7_PRO_8K ||
            product_id == USB_PRODUCT_GAMESIR_GAMEPAD_TARANTULA_8K);
}

bool SDL_IsJoystickSteamDeck(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SteamControllerNeptune;
}

bool SDL_IsJoystickSteamTriton(Uint16 vendor_id, Uint16 product_id)
{
    EControllerType eType = GuessControllerType(vendor_id, product_id);
    return eType == k_eControllerType_SteamControllerTriton;
}

bool SDL_IsJoystickWheel(Uint16 vendor_id, Uint16 product_id, Uint16 crc)
{
    if (vendor_id == 0x11FF && product_id == 0x3331 && (crc == 0xFAF6 || crc == 0x2004)) {
        // Oklick W-2 racing wheel controller (on Windows via DirectInput).
        return true;
    }
    return SDL_VIDPIDInList(vendor_id, product_id, &wheel_devices);
}

static bool SDL_IsJoystickArcadeStick(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &arcadestick_devices);
}

static bool SDL_IsJoystickFlightStick(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &flightstick_devices);
}

static bool SDL_IsJoystickThrottle(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &throttle_devices);
}

static bool SDL_IsJoystickGuitar(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &guitar_devices);
}

static bool SDL_IsJoystickDrumKit(Uint16 vendor_id, Uint16 product_id)
{
    return SDL_VIDPIDInList(vendor_id, product_id, &drum_devices);
}

SDL_JoystickType SDL_GetJoystickGUIDType(SDL_GUID guid)
{
    Uint16 vendor;
    Uint16 product;
    Uint16 crc;

    SDL_GetJoystickGUIDInfo(guid, &vendor, &product, NULL, &crc);

    if (SDL_IsJoystickWheel(vendor, product, crc)) {
        return SDL_JOYSTICK_TYPE_WHEEL;
    }

    if (SDL_IsJoystickArcadeStick(vendor, product)) {
        return SDL_JOYSTICK_TYPE_ARCADE_STICK;
    }

    if (SDL_IsJoystickFlightStick(vendor, product)) {
        return SDL_JOYSTICK_TYPE_FLIGHT_STICK;
    }

    if (SDL_IsJoystickThrottle(vendor, product)) {
        return SDL_JOYSTICK_TYPE_THROTTLE;
    }

    if (SDL_IsJoystickGuitar(vendor, product)) {
        return SDL_JOYSTICK_TYPE_GUITAR;
    }

    if (SDL_IsJoystickDrumKit(vendor, product)) {
        return SDL_JOYSTICK_TYPE_DRUM_KIT;
    }

    if (SDL_IsJoystickXInput(guid)) {
        // XInput GUID, get the type based on the XInput device subtype
        switch (guid.data[15]) {
        case 0x01: // XINPUT_DEVSUBTYPE_GAMEPAD
            return SDL_JOYSTICK_TYPE_GAMEPAD;
        case 0x02: // XINPUT_DEVSUBTYPE_WHEEL
            return SDL_JOYSTICK_TYPE_WHEEL;
        case 0x03: // XINPUT_DEVSUBTYPE_ARCADE_STICK
            return SDL_JOYSTICK_TYPE_ARCADE_STICK;
        case 0x04: // XINPUT_DEVSUBTYPE_FLIGHT_STICK
            return SDL_JOYSTICK_TYPE_FLIGHT_STICK;
        case 0x05: // XINPUT_DEVSUBTYPE_DANCE_PAD
            return SDL_JOYSTICK_TYPE_DANCE_PAD;
        case 0x06: // XINPUT_DEVSUBTYPE_GUITAR
        case 0x07: // XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE
        case 0x0B: // XINPUT_DEVSUBTYPE_GUITAR_BASS
            return SDL_JOYSTICK_TYPE_GUITAR;
        case 0x08: // XINPUT_DEVSUBTYPE_DRUM_KIT
            return SDL_JOYSTICK_TYPE_DRUM_KIT;
        case 0x13: // XINPUT_DEVSUBTYPE_ARCADE_PAD
            return SDL_JOYSTICK_TYPE_ARCADE_PAD;
        default:
            return SDL_JOYSTICK_TYPE_UNKNOWN;
        }
    }

    if (SDL_IsJoystickWGI(guid)) {
        return (SDL_JoystickType)guid.data[15];
    }

    if (SDL_IsJoystickGameInput(guid)) {
        return (SDL_JoystickType)guid.data[15];
    }

    if (SDL_IsJoystickVIRTUAL(guid)) {
        return (SDL_JoystickType)guid.data[15];
    }

#ifdef SDL_JOYSTICK_HIDAPI
    if (SDL_IsJoystickHIDAPI(guid)) {
        return HIDAPI_GetJoystickTypeFromGUID(guid);
    }
#endif // SDL_JOYSTICK_HIDAPI

    if (GuessControllerType(vendor, product) != k_eControllerType_UnknownNonSteamController) {
        return SDL_JOYSTICK_TYPE_GAMEPAD;
    }

    return SDL_JOYSTICK_TYPE_UNKNOWN;
}

bool SDL_ShouldIgnoreJoystick(Uint16 vendor_id, Uint16 product_id, Uint16 version, const char *name)
{
    // Check the joystick blacklist
    if (SDL_VIDPIDInList(vendor_id, product_id, &blacklist_devices)) {
        return true;
    }
    if (!SDL_GetHintBoolean(SDL_HINT_JOYSTICK_ROG_CHAKRAM, false)) {
        if (SDL_VIDPIDInList(vendor_id, product_id, &rog_gamepad_mice)) {
            return true;
        }
    }

    if (SDL_ShouldIgnoreGamepad(vendor_id, product_id, version, name)) {
        return true;
    }

    return false;
}

