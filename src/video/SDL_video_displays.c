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

#include "SDL_sysvideo.h"
#include "SDL_video_c.h"
#include "../events/SDL_events_c.h"

#define CHECK_DISPLAY_MAGIC(display, result)                            \
    CHECK_PARAM(!display) {                                             \
        return result;                                                  \
    }

void SDL_UpdateDesktopBounds(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Rect rect;
    SDL_zero(rect);

    SDL_DisplayID *displays = SDL_GetDisplays(NULL);
    if (displays) {
        for (int i = 0; displays[i]; ++i) {
            SDL_Rect bounds;
            if (SDL_GetDisplayBounds(displays[i], &bounds)) {
                if (i == 0) {
                    SDL_copyp(&rect, &bounds);
                } else {
                    SDL_GetRectUnion(&rect, &bounds, &rect);
                }
            }
        }
        SDL_free(displays);
    }
    SDL_copyp(&_this->desktop_bounds, &rect);
}

SDL_DisplayID SDL_AddBasicVideoDisplay(const SDL_DisplayMode *desktop_mode)
{
    SDL_VideoDisplay display;

    SDL_zero(display);
    if (desktop_mode) {
        SDL_memcpy(&display.desktop_mode, desktop_mode, sizeof(display.desktop_mode));
    }
    return SDL_AddVideoDisplay(&display, false);
}

SDL_DisplayID SDL_AddVideoDisplay(const SDL_VideoDisplay *display, bool send_event)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_VideoDisplay **displays, *new_display;
    SDL_DisplayID id;
    SDL_PropertiesID props;
    int i;

    new_display = (SDL_VideoDisplay *)SDL_malloc(sizeof(*new_display));
    if (!new_display) {
        return 0;
    }

    displays = (SDL_VideoDisplay **)SDL_realloc(_this->displays, (_this->num_displays + 1) * sizeof(*displays));
    if (!displays) {
        SDL_free(new_display);
        return 0;
    }
    _this->displays = displays;
    _this->displays[_this->num_displays++] = new_display;

    id = SDL_GetNextObjectID();
    SDL_copyp(new_display, display);
    new_display->id = id;
    new_display->device = _this;
    if (display->name) {
        new_display->name = SDL_strdup(display->name);
    } else {
        char name[32];

        SDL_itoa(id, name, 10);
        new_display->name = SDL_strdup(name);
    }
    if (new_display->content_scale == 0.0f) {
        new_display->content_scale = 1.0f;
    }

    new_display->desktop_mode.displayID = id;
    new_display->current_mode = &new_display->desktop_mode;
    SDL_FinalizeDisplayMode(&new_display->desktop_mode);

    for (i = 0; i < new_display->num_fullscreen_modes; ++i) {
        new_display->fullscreen_modes[i].displayID = id;
    }

    new_display->HDR.HDR_headroom = SDL_max(display->HDR.HDR_headroom, 1.0f);
    new_display->HDR.SDR_white_level = SDL_max(display->HDR.SDR_white_level, 1.0f);

    props = SDL_GetDisplayProperties(id);
    SDL_SetBooleanProperty(props, SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, new_display->HDR.HDR_headroom > 1.0f);

    SDL_UpdateDesktopBounds();

    if (send_event) {
        SDL_SendDisplayEvent(new_display, SDL_EVENT_DISPLAY_ADDED, 0, 0);
    }

    return id;
}

void SDL_OnDisplayMoved(SDL_VideoDisplay *display)
{
    SDL_UpdateDesktopBounds();
}

void SDL_DelVideoDisplay(SDL_DisplayID displayID, bool send_event)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_VideoDisplay *display;
    int display_index = SDL_GetDisplayIndex(displayID);
    if (display_index < 0) {
        return;
    }

    display = _this->displays[display_index];

    if (send_event) {
        SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_REMOVED, 0, 0);
    }

    SDL_DestroyProperties(display->props);
    SDL_free(display->name);
    SDL_ResetFullscreenDisplayModes(display);
    SDL_free(display->desktop_mode.internal);
    display->desktop_mode.internal = NULL;
    SDL_free(display->internal);
    display->internal = NULL;
    SDL_free(display);

    if (display_index < (_this->num_displays - 1)) {
        SDL_memmove(&_this->displays[display_index], &_this->displays[display_index + 1], (_this->num_displays - display_index - 1) * sizeof(_this->displays[display_index]));
    }
    --_this->num_displays;

    SDL_UpdateDesktopBounds();
}

SDL_DisplayID *SDL_GetDisplays(int *count)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int i;
    SDL_DisplayID *displays;

    if (!_this) {
        if (count) {
            *count = 0;
        }

        SDL_UninitializedVideo();
        return NULL;
    }

    displays = (SDL_DisplayID *)SDL_malloc((_this->num_displays + 1) * sizeof(*displays));
    if (displays) {
        if (count) {
            *count = _this->num_displays;
        }

        for (i = 0; i < _this->num_displays; ++i) {
            displays[i] = _this->displays[i]->id;
        }
        displays[i] = 0;
    } else {
        if (count) {
            *count = 0;
        }
    }
    return displays;
}

SDL_VideoDisplay *SDL_GetVideoDisplay(SDL_DisplayID displayID)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int display_index;

    display_index = SDL_GetDisplayIndex(displayID);
    if (display_index < 0) {
        return NULL;
    }
    return _this->displays[display_index];
}

SDL_DisplayID SDL_GetPrimaryDisplay(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this || _this->num_displays == 0) {
        SDL_UninitializedVideo();
        return 0;
    }
    return _this->displays[0]->id;
}

int SDL_GetDisplayIndex(SDL_DisplayID displayID)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int display_index;

    if (!_this) {
        SDL_UninitializedVideo();
        return -1;
    }

    for (display_index = 0; display_index < _this->num_displays; ++display_index) {
        if (displayID == _this->displays[display_index]->id) {
            return display_index;
        }
    }
    SDL_SetError("Invalid display");
    return -1;
}

SDL_DisplayData *SDL_GetDisplayDriverData(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, NULL);

    return display->internal;
}

SDL_PropertiesID SDL_GetDisplayProperties(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, 0);

    if (display->props == 0) {
        display->props = SDL_CreateProperties();
    }
    return display->props;
}

const char *SDL_GetDisplayName(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, NULL);

    return display->name;
}

bool SDL_GetDisplayBounds(SDL_DisplayID displayID, SDL_Rect *rect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, false);

    CHECK_PARAM(!rect) {
        return SDL_InvalidParamError("rect");
    }

    if (_this->GetDisplayBounds) {
        SDL_zerop(rect);
        if (_this->GetDisplayBounds(_this, display, rect)) {
            return true;
        }
    }

    // Assume that the displays are left to right
    if (displayID == SDL_GetPrimaryDisplay()) {
        rect->x = 0;
        rect->y = 0;
    } else {
        SDL_GetDisplayBounds(_this->displays[SDL_GetDisplayIndex(displayID) - 1]->id, rect);
        rect->x += rect->w;
    }
    rect->w = display->current_mode->w;
    rect->h = display->current_mode->h;
    return true;
}

static int ParseDisplayUsableBoundsHint(SDL_Rect *rect)
{
    const char *hint = SDL_GetHint(SDL_HINT_DISPLAY_USABLE_BOUNDS);
    return hint && (SDL_sscanf(hint, "%d,%d,%d,%d", &rect->x, &rect->y, &rect->w, &rect->h) == 4);
}

bool SDL_GetDisplayUsableBounds(SDL_DisplayID displayID, SDL_Rect *rect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, false);

    CHECK_PARAM(!rect) {
        return SDL_InvalidParamError("rect");
    }

    if (displayID == SDL_GetPrimaryDisplay() && ParseDisplayUsableBoundsHint(rect)) {
        return true;
    }

    if (_this->GetDisplayUsableBounds) {
        SDL_zerop(rect);
        if (_this->GetDisplayUsableBounds(_this, display, rect)) {
            return true;
        }
    }

    // Oh well, just give the entire display bounds.
    return SDL_GetDisplayBounds(displayID, rect);
}

SDL_DisplayOrientation SDL_GetNaturalDisplayOrientation(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, SDL_ORIENTATION_UNKNOWN);

    if (display->natural_orientation != SDL_ORIENTATION_UNKNOWN) {
        return display->natural_orientation;
    } else {
        // Default to landscape if the driver hasn't set it
        return SDL_ORIENTATION_LANDSCAPE;
    }
}

SDL_DisplayOrientation SDL_GetCurrentDisplayOrientation(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, SDL_ORIENTATION_UNKNOWN);

    if (display->current_orientation != SDL_ORIENTATION_UNKNOWN) {
        return display->current_orientation;
    } else {
        // Default to landscape if the driver hasn't set it
        return SDL_ORIENTATION_LANDSCAPE;
    }
}

float SDL_GetDisplayContentScale(SDL_DisplayID displayID)
{
    SDL_VideoDisplay *display = SDL_GetVideoDisplay(displayID);

    CHECK_DISPLAY_MAGIC(display, 0.0f);

    return display->content_scale;
}
