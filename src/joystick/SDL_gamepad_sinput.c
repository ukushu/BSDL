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
#include "hidapi/SDL_hidapi_sinput.h"

// Helper function to add button mapping
#define SDL_ADD_BUTTON_MAPPING(sdl_name, button_id, maxlen)                     \
    do {                                                                        \
        char temp[32];                                                          \
        (void)SDL_snprintf(temp, sizeof(temp), "%s:b%d,", sdl_name, button_id); \
        SDL_strlcat(mapping_string, temp, maxlen);                              \
    } while (0)

// Helper function to add axis mapping
#define SDL_ADD_AXIS_MAPPING(sdl_name, axis_id, maxlen)                       \
    do {                                                                      \
        char temp[32];                                                        \
        (void)SDL_snprintf(temp, sizeof(temp), "%s:a%d,", sdl_name, axis_id); \
        SDL_strlcat(mapping_string, temp, maxlen);                            \
    } while (0)

/*
* Helper function to apply SInput decoded styles to the mapping string
*/
static inline void SDL_SInputStylesMapExtraction(SDL_SInputStyles_t* styles, char* mapping_string, size_t mapping_string_len)
{
    int current_button = 0;
    int current_axis = 0;
    int misc_buttons = 0;
    int misc_button, misc_end;
    bool digital_triggers = false;
    bool dualstage_triggers = false;
    int bumpers = 0;
    bool left_stick = false;
    bool right_stick = false;
    int paddle_pairs = 0;
    char misc_label[32];

    // Determine how many misc buttons are used
    switch (styles->misc_style) {
    case SINPUT_MISCSTYLE_1:
        misc_buttons = 1;
        break;

    case SINPUT_MISCSTYLE_2:
        misc_buttons = 2;
        break;

    case SINPUT_MISCSTYLE_3:
        misc_buttons = 3;
        break;

    case SINPUT_MISCSTYLE_4:
        misc_buttons = 4;
        break;

    default:
        break;
    }
    // The share button is reserved as misc1, additional buttons start at misc2
    misc_button = 2;
    misc_end = misc_button + misc_buttons;

    // Analog joysticks (always come first in axis mapping)
    switch (styles->analog_style) {
    case SINPUT_ANALOGSTYLE_LEFTONLY:
        SDL_ADD_AXIS_MAPPING("leftx", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("lefty", current_axis++, mapping_string_len);
        left_stick = true;
        break;

    case SINPUT_ANALOGSTYLE_LEFTRIGHT:
        SDL_ADD_AXIS_MAPPING("leftx", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("lefty", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("rightx", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("righty", current_axis++, mapping_string_len);
        left_stick = true;
        right_stick = true;
        break;

    case SINPUT_ANALOGSTYLE_RIGHTONLY:
        SDL_ADD_AXIS_MAPPING("rightx", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("righty", current_axis++, mapping_string_len);
        right_stick = true;
        break;

    default:
        break;
    }

    // Bumpers
    switch (styles->bumper_style) {
    case SINPUT_BUMPERSTYLE_ONE:
        bumpers = 1;
        break;

    case SINPUT_BUMPERSTYLE_TWO:
        bumpers = 2;
        break;

    default:
        break;
    }

    // Analog triggers
    switch (styles->trigger_style) {
    // Analog triggers
    case SINPUT_TRIGGERSTYLE_ANALOG:
        SDL_ADD_AXIS_MAPPING("lefttrigger", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("righttrigger", current_axis++, mapping_string_len);
        break;

    // Digital triggers
    case SINPUT_TRIGGERSTYLE_DIGITAL:
        digital_triggers = true;
        break;

    // Analog triggers with digital press
    case SINPUT_TRIGGERSTYLE_DUALSTAGE:
        SDL_ADD_AXIS_MAPPING("lefttrigger", current_axis++, mapping_string_len);
        SDL_ADD_AXIS_MAPPING("righttrigger", current_axis++, mapping_string_len);
        dualstage_triggers = true;
        break;

    default:
        break;
    }

    switch (styles->paddle_style) {
    case SINPUT_PADDLESTYLE_TWO:
        paddle_pairs = 1;
        break;

    case SINPUT_PADDLESTYLE_FOUR:
        paddle_pairs = 2;
        break;

    default:
        break;
    }

    // Digital button mappings
    // ABXY buttons (always applied as South, East, West, North)
    SDL_ADD_BUTTON_MAPPING("a", current_button++, mapping_string_len); // South (typically A on Xbox, X on PlayStation)
    SDL_ADD_BUTTON_MAPPING("b", current_button++, mapping_string_len); // East  (typically B on Xbox, Circle on PlayStation)
    SDL_ADD_BUTTON_MAPPING("x", current_button++, mapping_string_len); // West  (typically X on Xbox, Square on PlayStation)
    SDL_ADD_BUTTON_MAPPING("y", current_button++, mapping_string_len); // North (typically Y on Xbox, Triangle on PlayStation)

    // D-Pad (always applied)
    SDL_strlcat(mapping_string, "dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,", mapping_string_len);

    // Left and Right stick buttons
    if (left_stick) {
        SDL_ADD_BUTTON_MAPPING("leftstick", current_button++, mapping_string_len);
    }
    if (right_stick) {
        SDL_ADD_BUTTON_MAPPING("rightstick", current_button++, mapping_string_len);
    }

    // Digital shoulder buttons (L/R Shoulder)
    if (bumpers > 0) {
        SDL_ADD_BUTTON_MAPPING("leftshoulder", current_button++, mapping_string_len);
    }
    if (bumpers > 1) {
        SDL_ADD_BUTTON_MAPPING("rightshoulder", current_button++, mapping_string_len);
    }

    // Digital trigger buttons (capability overrides analog)
    if (digital_triggers) {
        SDL_ADD_BUTTON_MAPPING("lefttrigger", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("righttrigger", current_button++, mapping_string_len);
    } else if (dualstage_triggers) {
        // Dual-stage trigger buttons are appended as MISC buttons
        // By convention the trigger buttons are misc3 and misc4 for GameCube style controllers
        if (misc_end < 3) {
            misc_end = 3;
        }
        SDL_snprintf(misc_label, sizeof(misc_label), "misc%d", misc_end++);
        SDL_ADD_BUTTON_MAPPING(misc_label, current_button++, mapping_string_len);
        SDL_snprintf(misc_label, sizeof(misc_label), "misc%d", misc_end++);
        SDL_ADD_BUTTON_MAPPING(misc_label, current_button++, mapping_string_len);
    }

    // Paddle 1/2
    if (paddle_pairs > 0) {
        // Paddle 2 is first for left/right order of SInput
        SDL_ADD_BUTTON_MAPPING("paddle2", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("paddle1", current_button++, mapping_string_len);
    }

    // Start/Plus
    SDL_ADD_BUTTON_MAPPING("start", current_button++, mapping_string_len);

    // Back/Minus, Guide/Home, Share/Capture
    switch (styles->meta_style) {
    case SINPUT_METASTYLE_BACK:
        SDL_ADD_BUTTON_MAPPING("back", current_button++, mapping_string_len);
        break;

    case SINPUT_METASTYLE_BACKGUIDE:
        SDL_ADD_BUTTON_MAPPING("back", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("guide", current_button++, mapping_string_len);
        break;

    case SINPUT_METASTYLE_BACKGUIDESHARE:
        SDL_ADD_BUTTON_MAPPING("back", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("guide", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("misc1", current_button++, mapping_string_len);
        break;

    default:
        break;
    }

    // Paddle 3/4
    if (paddle_pairs > 1) {
        // Paddle 4 is first for left/right order of SInput
        SDL_ADD_BUTTON_MAPPING("paddle4", current_button++, mapping_string_len);
        SDL_ADD_BUTTON_MAPPING("paddle3", current_button++, mapping_string_len);
    }

    // Touchpad buttons
    switch (styles->touch_style) {
    case SINPUT_TOUCHSTYLE_SINGLE:
        SDL_ADD_BUTTON_MAPPING("touchpad", current_button++, mapping_string_len);
        break;

    case SINPUT_TOUCHSTYLE_DOUBLE:
        SDL_ADD_BUTTON_MAPPING("touchpad", current_button++, mapping_string_len);
        // Add the second touchpad button at the end of the misc buttons
        SDL_snprintf(misc_label, sizeof(misc_label), "misc%d", misc_end++);
        SDL_ADD_BUTTON_MAPPING(misc_label, current_button++, mapping_string_len);
        break;

    default:
        break;
    }

    for (int i = 0; i < misc_buttons; ++i) {
        SDL_snprintf(misc_label, sizeof(misc_label), "misc%d", misc_button++);
        SDL_ADD_BUTTON_MAPPING(misc_label, current_button++, mapping_string_len);
    }
}

/*
* Helper function to decode SInput features information packed into version
*/
void SDL_CreateMappingStringForSInputGamepad(Uint16 version, Uint8 face_style, char* mapping_string, size_t mapping_string_len)
{
    SDL_SInputStyles_t decoded = { 0 };

    switch (face_style) {
    default:
        SDL_strlcat(mapping_string, "face:abxy,", mapping_string_len);
        break;
    case 2:
        SDL_strlcat(mapping_string, "face:axby,", mapping_string_len);
        break;
    case 3:
        SDL_strlcat(mapping_string, "face:bayx,", mapping_string_len);
        break;
    case 4:
        SDL_strlcat(mapping_string, "face:sony,", mapping_string_len);
        break;
    }

    // Interpret the mapping string
    // dynamically based on the feature responses
    decoded.misc_style = (SInput_MiscStyleType)(version % SINPUT_MISCSTYLE_MAX);
    version /= SINPUT_MISCSTYLE_MAX;

    decoded.touch_style = (SInput_TouchStyleType)(version % SINPUT_TOUCHSTYLE_MAX);
    version /= SINPUT_TOUCHSTYLE_MAX;

    decoded.meta_style = (SInput_MetaStyleType)(version % SINPUT_METASTYLE_MAX);
    version /= SINPUT_METASTYLE_MAX;

    decoded.paddle_style = (SInput_PaddleStyleType)(version % SINPUT_PADDLESTYLE_MAX);
    version /= SINPUT_PADDLESTYLE_MAX;

    decoded.trigger_style = (SInput_TriggerStyleType)(version % SINPUT_TRIGGERSTYLE_MAX);
    version /= SINPUT_TRIGGERSTYLE_MAX;

    decoded.bumper_style = (SInput_BumperStyleType)(version % SINPUT_BUMPERSTYLE_MAX);
    version /= SINPUT_BUMPERSTYLE_MAX;

    decoded.analog_style = (SInput_AnalogStyleType)(version % SINPUT_ANALOGSTYLE_MAX);

    SDL_SInputStylesMapExtraction(&decoded, mapping_string, mapping_string_len);
}

