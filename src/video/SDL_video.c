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

// The high-level video driver subsystem

#include "SDL_sysvideo.h"
#include "SDL_clipboard_c.h"
#include "SDL_surface_c.h"
#include "SDL_pixels_c.h"
#include "SDL_rect_c.h"
#include "SDL_video_c.h"
#include "../events/SDL_events_c.h"
#include "../SDL_hints_c.h"
#include "../SDL_properties_c.h"
#include "../timer/SDL_timer_c.h"
#include "../camera/SDL_camera_c.h"
#include "../render/SDL_sysrender.h"
#include "../main/SDL_main_callbacks.h"

#ifdef SDL_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

#ifdef SDL_PLATFORM_3DS
#include <3ds.h>
#endif

// Available video drivers
static VideoBootStrap *bootstrap[] = {
#ifdef SDL_VIDEO_DRIVER_PRIVATE
    &PRIVATE_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_COCOA
    &COCOA_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_X11
#ifdef SDL_VIDEO_DRIVER_WAYLAND
    &Wayland_preferred_bootstrap,
#endif
    &X11_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_WAYLAND
    &Wayland_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_VIVANTE
    &VIVANTE_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_WINDOWS
    &WINDOWS_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_HAIKU
    &HAIKU_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_UIKIT
    &UIKIT_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_ANDROID
    &Android_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_OPENHARMONY
    &OPENHARMONY_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_PS2
    &PS2_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_PSP
    &PSP_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_VITA
    &VITA_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_N3DS
    &N3DS_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_NGAGE
    &NGAGE_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_KMSDRM
    &KMSDRM_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_RISCOS
    &RISCOS_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_RPI
    &RPI_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_EMSCRIPTEN
    &Emscripten_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_QNX
    &QNX_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_OFFSCREEN
    &OFFSCREEN_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_DUMMY
    &DUMMY_bootstrap,
#ifdef SDL_INPUT_LINUXEV
    &DUMMY_evdev_bootstrap,
#endif
#endif
#ifdef SDL_VIDEO_DRIVER_DOSVESA
    &DOSVESA_bootstrap,
#endif
#ifdef SDL_VIDEO_DRIVER_OPENVR
    &OPENVR_bootstrap,
#endif
    NULL
};

#define CHECK_WINDOW_MAGIC(window, result)                              \
    CHECK_PARAM(!_this) {                                               \
        SDL_UninitializedVideo();                                       \
        return result;                                                  \
    }                                                                   \
    CHECK_PARAM(!SDL_ObjectValid(window, SDL_OBJECT_TYPE_WINDOW)) {     \
        SDL_SetError("Invalid window");                                 \
        return result;                                                  \
    }

#define CHECK_DISPLAY_MAGIC(display, result)                            \
    CHECK_PARAM(!display) {                                             \
        return result;                                                  \
    }                                                                   \

#define CHECK_WINDOW_NOT_POPUP(window, result)                          \
    CHECK_PARAM(SDL_WINDOW_IS_POPUP(window)) {                          \
        SDL_SetError("Operation invalid on popup windows");             \
        return result;                                                  \
    }

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
// Support for macOS fullscreen spaces, etc.
extern bool Cocoa_IsWindowInFullscreenSpace(SDL_Window *window);
extern bool Cocoa_IsWindowInFullscreenSpaceTransition(SDL_Window *window);
extern bool Cocoa_SetWindowFullscreenSpace(SDL_Window *window, bool state, bool blocking);
extern bool Cocoa_IsShowingModalDialog(SDL_Window *window);
#endif

#ifdef SDL_VIDEO_DRIVER_UIKIT
extern void SDL_UpdateLifecycleObserver(void);
#endif

static void SDL_CheckWindowDisplayChanged(SDL_Window *window);
static void SDL_CheckWindowDisplayScaleChanged(SDL_Window *window);
static void SDL_CheckWindowSafeAreaChanged(SDL_Window *window);

// Convenience functions for reading driver flags
static bool SDL_ModeSwitchingEmulated(SDL_VideoDevice *_this)
{
    if (_this->device_caps & VIDEO_DEVICE_CAPS_MODE_SWITCHING_EMULATED) {
        return true;
    }
    return false;
}

static bool SDL_SendsFullscreenDimensions(SDL_VideoDevice *_this)
{
    return !!(_this->device_caps & VIDEO_DEVICE_CAPS_SENDS_FULLSCREEN_DIMENSIONS);
}

static bool IsFullscreenOnly(SDL_VideoDevice *_this)
{
    return !!(_this->device_caps & VIDEO_DEVICE_CAPS_FULLSCREEN_ONLY);
}

static bool SDL_SendsDisplayChanges(SDL_VideoDevice *_this)
{
    return !!(_this->device_caps & VIDEO_DEVICE_CAPS_SENDS_DISPLAY_CHANGES);
}

static bool SDL_DriverSendsHDRChanges(SDL_VideoDevice *_this)
{
    return !!(_this->device_caps & VIDEO_DEVICE_CAPS_SENDS_HDR_CHANGES);
}

// Hint to treat all window ops as synchronous
static bool syncHint;

static void SDL_SyncHintWatcher(void *userdata, const char *name, const char *oldValue, const char *newValue)
{
    syncHint = SDL_GetStringBoolean(newValue, false);
}

static void SDL_SyncIfRequired(SDL_Window *window)
{
    if (syncHint) {
        SDL_SyncWindow(window);
    }
}

static void SDL_UpdateWindowHierarchy(SDL_Window *window, SDL_Window *parent)
{
    // Unlink the window from the existing parent.
    if (window->parent) {
        if (window->next_sibling) {
            window->next_sibling->prev_sibling = window->prev_sibling;
        }
        if (window->prev_sibling) {
            window->prev_sibling->next_sibling = window->next_sibling;
        } else {
            window->parent->first_child = window->next_sibling;
        }

        window->parent = NULL;
    }

    if (parent) {
        window->parent = parent;

        window->next_sibling = parent->first_child;
        if (parent->first_child) {
            parent->first_child->prev_sibling = window;
        }
        parent->first_child = window;
    }
}

static Uint32 SDL_DefaultGraphicsBackends(SDL_VideoDevice *_this)
{
#if (defined(SDL_VIDEO_OPENGL) && defined(SDL_PLATFORM_MACOS)) || (defined(SDL_PLATFORM_IOS) && !TARGET_OS_MACCATALYST) || defined(SDL_PLATFORM_QNXNTO)
    if (_this->GL_CreateContext) {
        return SDL_WINDOW_OPENGL;
    }
#endif
#if defined(SDL_VIDEO_METAL) && (TARGET_OS_MACCATALYST || defined(SDL_PLATFORM_MACOS) || defined(SDL_PLATFORM_IOS))
    if (_this->Metal_CreateView) {
        return SDL_WINDOW_METAL;
    }
#endif
#if defined(SDL_VIDEO_OPENGL) && defined(SDL_VIDEO_DRIVER_OPENVR)
    if (SDL_strcmp(_this->name, "openvr") == 0) {
        return SDL_WINDOW_OPENGL;
    }
#endif
    return 0;
}

static SDL_VideoDevice *_this = NULL;
static SDL_AtomicInt SDL_messagebox_count;

bool SDL_UninitializedVideo(void)
{
    return SDL_SetError("Video subsystem has not been initialized");
}

// Deduplicated list of video bootstrap drivers.
static const VideoBootStrap *deduped_bootstrap[SDL_arraysize(bootstrap) - 1];

int SDL_GetNumVideoDrivers(void)
{
    static int num_drivers = -1;

    if (num_drivers >= 0) {
        return num_drivers;
    }

    num_drivers = 0;

    // Build a list of unique video drivers.
    for (int i = 0; bootstrap[i] != NULL; ++i) {
        bool duplicate = false;
        for (int j = 0; j < i; ++j) {
            if (SDL_strcmp(bootstrap[i]->name, bootstrap[j]->name) == 0) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            deduped_bootstrap[num_drivers++] = bootstrap[i];
        }
    }

    return num_drivers;
}

const char *SDL_GetVideoDriver(int index)
{
    CHECK_PARAM(index < 0 || index >= SDL_GetNumVideoDrivers()) {
        SDL_InvalidParamError("index");
        return NULL;
    }
    return deduped_bootstrap[index]->name;
}

/*
 * Initialize the video and event subsystems -- determine native pixel format
 */
bool SDL_InitVideo(const char *driver_name)
{
    SDL_VideoDevice *video;
    bool init_events = false;
    bool init_keyboard = false;
    bool init_mouse = false;
    bool init_touch = false;
    bool init_pen = false;
    int i = 0;

    // Check to make sure we don't overwrite '_this'
    if (_this) {
        SDL_QuitVideo();
    }

    SDL_InitTicks();

    // Start the event loop
    if (!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
        goto pre_driver_error;
    }
    init_events = true;
    if (!SDL_InitKeyboard()) {
        goto pre_driver_error;
    }
    init_keyboard = true;
    if (!SDL_PreInitMouse()) {
        goto pre_driver_error;
    }
    init_mouse = true;
    if (!SDL_InitTouch()) {
        goto pre_driver_error;
    }
    init_touch = true;
    if (!SDL_InitPen()) {
        goto pre_driver_error;
    }
    init_pen = true;

    // Select the proper video driver
    video = NULL;
#ifdef SDL_PLATFORM_LINUX
    // https://github.com/libsdl-org/SDL/issues/12247
    if (SDL_IsUbuntuTouch()) {
        driver_name = "wayland";
    }
#endif
    if (!driver_name) {
        driver_name = SDL_GetHint(SDL_HINT_VIDEO_DRIVER);
    }
    if (driver_name && *driver_name != 0) {
        const char *driver_attempt = driver_name;
        while (driver_attempt && *driver_attempt != 0 && !video) {
            const char *driver_attempt_end = SDL_strchr(driver_attempt, ',');
            size_t driver_attempt_len = (driver_attempt_end) ? (driver_attempt_end - driver_attempt)
                                                                     : SDL_strlen(driver_attempt);

            for (i = 0; bootstrap[i]; ++i) {
                if (!bootstrap[i]->is_preferred &&
                    (driver_attempt_len == SDL_strlen(bootstrap[i]->name)) &&
                    (SDL_strncasecmp(bootstrap[i]->name, driver_attempt, driver_attempt_len) == 0)) {
                    video = bootstrap[i]->create();
                    if (video) {
                        break;
                    }
                }
            }

            driver_attempt = (driver_attempt_end) ? (driver_attempt_end + 1) : NULL;
        }
    } else {
        for (i = 0; bootstrap[i]; ++i) {
            video = bootstrap[i]->create();
            if (video) {
                break;
            }
        }
    }
    if (video) {
        SDL_DebugLogBackend("video", bootstrap[i]->name);
    } else {
        if (driver_name) {
            SDL_SetError("%s not available", driver_name);
            goto pre_driver_error;
        }
        SDL_SetError("No available video device");
        goto pre_driver_error;
    }

    /* From this point on, use SDL_QuitVideo to cleanup on error, rather than
    pre_driver_error. */
    _this = video;
    _this->name = bootstrap[i]->name;
    _this->thread = SDL_GetCurrentThreadID();

    // Set some very sane GL defaults
    _this->gl_config.driver_loaded = 0;
    _this->gl_config.dll_handle = NULL;
    SDL_GL_ResetAttributes();

    // Initialize the video subsystem
    if (!_this->VideoInit(_this)) {
        SDL_QuitVideo();
        return false;
    }

    // Make sure some displays were added
    if (_this->num_displays == 0) {
        SDL_QuitVideo();
        return SDL_SetError("The video driver did not add any displays");
    }

    SDL_AddHintCallback(SDL_HINT_VIDEO_SYNC_WINDOW_OPERATIONS, SDL_SyncHintWatcher, NULL);

    /* Disable the screen saver by default. This is a change from <= 2.0.1,
       but most things using SDL are games or media players; you wouldn't
       want a screensaver to trigger if you're playing exclusively with a
       joystick, or passively watching a movie. Things that use SDL but
       function more like a normal desktop app should explicitly re-enable the
       screensaver. */
    if (!SDL_GetHintBoolean(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, false)) {
        SDL_DisableScreenSaver();
    }

    SDL_PostInitMouse();

    // We're ready to go!
    return true;

pre_driver_error:
    SDL_assert(_this == NULL);
    if (init_pen) {
        SDL_QuitPen();
    }
    if (init_touch) {
        SDL_QuitTouch();
    }
    if (init_mouse) {
        SDL_QuitMouse();
    }
    if (init_keyboard) {
        SDL_QuitKeyboard();
    }
    if (init_events) {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }
    return false;
}

const char *SDL_GetCurrentVideoDriver(void)
{
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    return _this->name;
}

SDL_VideoDevice *SDL_GetVideoDevice(void)
{
    return _this;
}

bool SDL_OnVideoThread(void)
{
    return (_this && SDL_GetCurrentThreadID() == _this->thread);
}

void SDL_SetSystemTheme(SDL_SystemTheme theme)
{
    if (_this && theme != _this->system_theme) {
        _this->system_theme = theme;
        SDL_SendSystemThemeChangedEvent();
    }
}

SDL_SystemTheme SDL_GetSystemTheme(void)
{
    if (_this) {
        return _this->system_theme;
    } else {
        return SDL_SYSTEM_THEME_UNKNOWN;
    }
}

void SDL_OnDisplayAdded(SDL_VideoDisplay *display)
{
    SDL_Window *window;

    // See if any windows have changed to the new display
    for (window = _this->windows; window; window = window->next) {
        SDL_CheckWindowDisplayChanged(window);
    }
}

static SDL_DisplayID GetDisplayForWindow(SDL_Window *window)
{
    SDL_DisplayID displayID = 0;

    // An explicit fullscreen display overrides all
    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        displayID = window->current_fullscreen_mode.displayID;
    }

    if (!displayID) {
        displayID = SDL_GetDisplayForWindowPosition(window);
    }
    return displayID;
}

SDL_VideoDisplay *SDL_GetVideoDisplayForWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return SDL_GetVideoDisplay(GetDisplayForWindow(window));
}

SDL_DisplayData *SDL_GetDisplayDriverDataForWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return SDL_GetDisplayDriverData(GetDisplayForWindow(window));
}

void SDL_SetDisplayContentScale(SDL_VideoDisplay *display, float scale)
{
    if (scale != display->content_scale) {
        SDL_Window *window;

        display->content_scale = scale;
        SDL_SendDisplayEvent(display, SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED, 0, 0);

        // Check the windows on this display
        for (window = _this->windows; window; window = window->next) {
            if (display->id == window->displayID) {
                SDL_CheckWindowDisplayScaleChanged(window);
            }
        }
    }
}

void SDL_SetWindowHDRProperties(SDL_Window *window, const SDL_HDROutputProperties *HDR, bool send_event)
{
    const SDL_HDROutputProperties HDR_clamped = {
        SDL_max(HDR->SDR_white_level, 1.0f),
        SDL_max(HDR->HDR_headroom, 1.0f)
    };

    if (window->HDR.HDR_headroom != HDR_clamped.HDR_headroom || window->HDR.SDR_white_level != HDR_clamped.SDR_white_level) {
        SDL_PropertiesID window_props = SDL_GetWindowProperties(window);

        SDL_SetFloatProperty(window_props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, HDR_clamped.HDR_headroom);
        SDL_SetFloatProperty(window_props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, HDR_clamped.SDR_white_level);
        SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, HDR_clamped.HDR_headroom > 1.0f);
        SDL_copyp(&window->HDR, &HDR_clamped);

        if (send_event) {
            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_HDR_STATE_CHANGED, HDR_clamped.HDR_headroom > 1.0f, 0);
        }
    }
}

void SDL_SetDisplayHDRProperties(SDL_VideoDisplay *display, const SDL_HDROutputProperties *HDR)
{
    const SDL_HDROutputProperties HDR_clamped = {
        SDL_max(HDR->SDR_white_level, 1.0f),
        SDL_max(HDR->HDR_headroom, 1.0f)
    };
    const bool changed = HDR_clamped.SDR_white_level != display->HDR.SDR_white_level ||
                         HDR_clamped.HDR_headroom != display->HDR.HDR_headroom;

    SDL_copyp(&display->HDR, &HDR_clamped);

    if (changed && !SDL_DriverSendsHDRChanges(_this)) {
        for (SDL_Window *w = display->device->windows; w; w = w->next) {
            if (GetDisplayForWindow(w) == display->id) {
                SDL_SetWindowHDRProperties(w, &display->HDR, true);
            }
        }
    }
}

/**
 * If x, y are outside of rect, snaps them to the closest point inside rect
 * (between rect->x, rect->y, inclusive, and rect->x + w, rect->y + h, exclusive)
 */
static void SDL_GetClosestPointOnRect(const SDL_Rect *rect, SDL_Point *point)
{
    const int right = rect->x + rect->w - 1;
    const int bottom = rect->y + rect->h - 1;

    if (point->x < rect->x) {
        point->x = rect->x;
    } else if (point->x > right) {
        point->x = right;
    }

    if (point->y < rect->y) {
        point->y = rect->y;
    } else if (point->y > bottom) {
        point->y = bottom;
    }
}

static SDL_DisplayID GetDisplayForRect(int x, int y, int w, int h)
{
    int i, dist;
    SDL_DisplayID closest = 0;
    int closest_dist = 0x7FFFFFFF;
    SDL_Point closest_point_on_display;
    SDL_Point delta;
    SDL_Point center;
    center.x = x + w / 2;
    center.y = y + h / 2;

    if (_this) {
        for (i = 0; i < _this->num_displays; ++i) {
            SDL_VideoDisplay *display = _this->displays[i];
            SDL_Rect display_rect;
            SDL_GetDisplayBounds(display->id, &display_rect);

            // Check if the window is fully enclosed
            if (SDL_GetRectEnclosingPoints(&center, 1, &display_rect, NULL)) {
                return display->id;
            }

            // Snap window center to the display rect
            closest_point_on_display = center;
            SDL_GetClosestPointOnRect(&display_rect, &closest_point_on_display);

            delta.x = center.x - closest_point_on_display.x;
            delta.y = center.y - closest_point_on_display.y;
            dist = (delta.x * delta.x + delta.y * delta.y);
            if (dist < closest_dist) {
                closest = display->id;
                closest_dist = dist;
            }
        }
    }

    if (closest == 0) {
        SDL_SetError("Couldn't find any displays");
    }

    return closest;
}

void SDL_RelativeToGlobalForWindow(SDL_Window *window, int rel_x, int rel_y, int *abs_x, int *abs_y)
{
    SDL_Window *w;

    if (SDL_WINDOW_IS_POPUP(window)) {
        // Calculate the total offset of the popup from the parents
        for (w = window->parent; w; w = w->parent) {
            rel_x += w->x;
            rel_y += w->y;

            if (!SDL_WINDOW_IS_POPUP(w)) {
                break;
            }
        }
    }

    if (abs_x) {
        *abs_x = rel_x;
    }
    if (abs_y) {
        *abs_y = rel_y;
    }
}

void SDL_GlobalToRelativeForWindow(SDL_Window *window, int abs_x, int abs_y, int *rel_x, int *rel_y)
{
    SDL_Window *w;

    if (SDL_WINDOW_IS_POPUP(window)) {
        // Convert absolute window coordinates to relative for a popup
        for (w = window->parent; w; w = w->parent) {
            abs_x -= w->x;
            abs_y -= w->y;

            if (!SDL_WINDOW_IS_POPUP(w)) {
                break;
            }
        }
    }

    if (rel_x) {
        *rel_x = abs_x;
    }
    if (rel_y) {
        *rel_y = abs_y;
    }
}

SDL_DisplayID SDL_GetDisplayForPoint(const SDL_Point *point)
{
    CHECK_PARAM(!point) {
        SDL_InvalidParamError("point");
        return 0;
    }

    return GetDisplayForRect(point->x, point->y, 1, 1);
}

SDL_DisplayID SDL_GetDisplayForRect(const SDL_Rect *rect)
{
    CHECK_PARAM(!rect) {
        SDL_InvalidParamError("rect");
        return 0;
    }

    return GetDisplayForRect(rect->x, rect->y, rect->w, rect->h);
}

static SDL_DisplayID GetDisplayAtOrigin(int x, int y)
{
    for (int i = 0; i < _this->num_displays; ++i) {
        SDL_Rect rect;
        const SDL_DisplayID cur_id = _this->displays[i]->id;
        if (SDL_GetDisplayBounds(cur_id, &rect)) {
            if (x == rect.x && y == rect.y) {
                return cur_id;
            }
        }
    }

    return 0;
}

SDL_DisplayID SDL_GetDisplayForWindowPosition(SDL_Window *window)
{
    int x, y;
    SDL_DisplayID displayID = 0;

    CHECK_WINDOW_MAGIC(window, 0);

    if (_this->GetDisplayForWindow) {
        displayID = _this->GetDisplayForWindow(_this, window);
    }

    /* A backend implementation may fail to get a display for the window
     * (for example if the window is off-screen), but other code may expect it
     * to succeed in that situation, so we fall back to a generic position-
     * based implementation in that case. */
    SDL_RelativeToGlobalForWindow(window, window->x, window->y, &x, &y);

    if (!displayID) {
        /* Fullscreen windows may be larger than the display if they were moved between differently sized
         * displays and the new position was received before the new size or vice versa. Using the center
         * of the window rect in this case can report the wrong display, so use the origin.
         */
        if (window->flags & SDL_WINDOW_FULLSCREEN) {
            displayID = GetDisplayForRect(x, y, 1, 1);
        } else {
            displayID = GetDisplayForRect(x, y, window->w, window->h);
        }
    }
    if (!displayID) {
        // Use the primary display for a window if we can't find it anywhere else
        displayID = SDL_GetPrimaryDisplay();
    }
    return displayID;
}

SDL_VideoDisplay *SDL_GetVideoDisplayForFullscreenWindow(SDL_Window *window)
{
    SDL_DisplayID displayID = 0;

    CHECK_WINDOW_MAGIC(window, NULL);

    // An explicit fullscreen display overrides all
    if (window->current_fullscreen_mode.displayID) {
        displayID = window->current_fullscreen_mode.displayID;
    }

    /* This is used to handle the very common pattern of SDL_SetWindowPosition()
     * followed immediately by SDL_SetWindowFullscreen() to make the window fullscreen
     * desktop on a specific display. If the backend doesn't support changing the
     * window position, or an async window manager hasn't yet actually moved the window,
     * the current position won't be updated at the time of the fullscreen call.
     */
    if (!displayID) {
        displayID = window->pending_displayID;
    }
    if (!displayID) {
        // Use the pending position and dimensions, if available, otherwise, use the current.
        const int x = window->last_position_pending ? window->pending.x : window->x;
        const int y = window->last_position_pending ? window->pending.y : window->y;
        const int w = window->last_size_pending ? window->pending.w : window->w;
        const int h = window->last_size_pending ? window->pending.h : window->h;

        // Check if the window is exactly at the origin of a display. Otherwise, fall back to the generic check.
        displayID = GetDisplayAtOrigin(x, y);
        if (!displayID) {
            displayID = GetDisplayForRect(x, y, w, h);
        }
    }
    if (!displayID) {
        // Use the primary display for a window if we can't find it anywhere else
        displayID = SDL_GetPrimaryDisplay();
    }
    return SDL_GetVideoDisplay(displayID);
}

#define SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY "sdl2-compat.window.preferred_fullscreen_display"

SDL_DisplayID SDL_GetDisplayForWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    /* sdl2-compat calls this function to get a display on which to make the window fullscreen,
     * so pass it the preferred fullscreen display ID in a property.
     */
    SDL_PropertiesID window_props = SDL_GetWindowProperties(window);
    SDL_VideoDisplay *fs_display = SDL_GetVideoDisplayForFullscreenWindow(window);
    if (fs_display) {
        SDL_SetNumberProperty(window_props, SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY, fs_display->id);
    } else {
        SDL_ClearProperty(window_props, SDL_PROP_SDL2_COMPAT_WINDOW_PREFERRED_FULLSCREEN_DISPLAY);
    }

    return GetDisplayForWindow(window);
}

static void SDL_CheckWindowDisplayChanged(SDL_Window *window)
{
    if (SDL_SendsDisplayChanges(_this)) {
        return;
    }

    SDL_DisplayID displayID = SDL_GetDisplayForWindowPosition(window);

    if (displayID != window->displayID) {
        // See if we are fully committed to the new display
        // 80% is about the right value, tested with 350% scale on the left monitor and 100% scale on the right
        SDL_Rect old_bounds, new_bounds;
        SDL_Rect window_rect;
        SDL_Rect old_overlap, new_overlap;

        if (SDL_GetDisplayBounds(window->displayID, &old_bounds) &&
            SDL_GetDisplayBounds(displayID, &new_bounds)) {
            window_rect.x = window->x;
            window_rect.y = window->y;
            window_rect.w = window->w;
            window_rect.h = window->h;

            if (SDL_GetRectIntersection(&old_bounds, &window_rect, &old_overlap) &&
                SDL_GetRectIntersection(&new_bounds, &window_rect, &new_overlap)) {
                int old_area = old_overlap.w * old_overlap.h;
                int new_area = new_overlap.w * new_overlap.h;
                float new_overlap_ratio = (new_area / ((float)old_area + new_area));
                if (new_overlap_ratio < 0.80) {
                    return;
                }
            }
        }
    }

    if (displayID != window->displayID) {
        int i, display_index;

        // Sanity check our fullscreen windows
        display_index = SDL_GetDisplayIndex(displayID);
        for (i = 0; i < _this->num_displays; ++i) {
            SDL_VideoDisplay *display = _this->displays[i];

            if (display->fullscreen_window == window) {
                if (display_index != i) {
                    if (display_index < 0) {
                        display_index = i;
                    } else {
                        SDL_VideoDisplay *new_display = _this->displays[display_index];

                        // The window was moved to a different display
                        if (new_display->fullscreen_window &&
                            new_display->fullscreen_window != window) {
                            // Uh oh, there's already a fullscreen window here; minimize it
                            SDL_MinimizeWindow(new_display->fullscreen_window);
                        }
                        new_display->fullscreen_window = window;
                        display->fullscreen_window = NULL;
                    }
                }
                break;
            }
        }

        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DISPLAY_CHANGED, (int)displayID, 0);
    }
}

float SDL_GetWindowPixelDensity(SDL_Window *window)
{
    int window_w, window_h, pixel_w, pixel_h;
    float pixel_density = 1.0f;

    CHECK_WINDOW_MAGIC(window, 0.0f);

    if (SDL_GetWindowSize(window, &window_w, &window_h) &&
        SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h)) {
        pixel_density = (float)pixel_w / window_w;
    }
    return pixel_density;
}

float SDL_GetWindowDisplayScale(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0.0f);

    return window->display_scale;
}

static void SDL_CheckWindowDisplayScaleChanged(SDL_Window *window)
{
    float display_scale;

    if (_this->GetWindowContentScale) {
        display_scale = _this->GetWindowContentScale(_this, window);
    } else {
        const float pixel_density = SDL_GetWindowPixelDensity(window);
        const float content_scale = SDL_GetDisplayContentScale(window->displayID);

        display_scale = pixel_density * content_scale;
    }

    if (display_scale != window->display_scale) {
        window->display_scale = display_scale;
        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED, 0, 0);
    }
}

static SDL_VideoDisplay *SDL_GetFullscreenDisplay(SDL_Window *window, bool entering)
{
    if (entering) {
        return SDL_GetVideoDisplayForFullscreenWindow(window);
    }

    for (int i = 0; i < _this->num_displays; ++i) {
        SDL_VideoDisplay *display = _this->displays[i];
        if (display->fullscreen_window == window) {
            return display;
        }
    }
    return NULL;
}

bool SDL_UpdateFullscreenMode(SDL_Window *window, SDL_FullscreenOp fullscreen, bool commit)
{
    SDL_VideoDisplay *display = NULL;
    SDL_DisplayMode *mode = NULL;
    int i;

    CHECK_WINDOW_MAGIC(window, false);

    window->fullscreen_exclusive = false;
    window->update_fullscreen_on_display_changed = false;

    // If we are in the process of hiding don't go back to fullscreen
    if (window->is_destroying || window->is_hiding) {
        fullscreen = SDL_FULLSCREEN_OP_LEAVE;
    }

    // Get the correct display for this operation
    display = SDL_GetFullscreenDisplay(window, fullscreen != SDL_FULLSCREEN_OP_LEAVE);
    if (fullscreen) {
        if (!display) {
            // This should never happen, but it did...
            goto done;
        }
    }

    if (fullscreen) {
        mode = (SDL_DisplayMode *)SDL_GetWindowFullscreenMode(window);
        if (mode) {
            window->fullscreen_exclusive = true;
        } else {
            // Make sure the current mode is zeroed for fullscreen desktop.
            SDL_zero(window->current_fullscreen_mode);
        }
    }

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
    /* if the window is going away and no resolution change is necessary,
       do nothing, or else we may trigger an ugly double-transition
     */
    if (SDL_strcmp(_this->name, "cocoa") == 0) { // don't do this for X11, etc
        if (window->is_destroying && !window->last_fullscreen_exclusive_display) {
            window->fullscreen_exclusive = false;
            if (display) {
                display->fullscreen_window = NULL;
            }
            goto done;
        }
        if (commit) {
            // If we're switching between a fullscreen Space and exclusive fullscreen, we need to get back to normal first.
            if (fullscreen && Cocoa_IsWindowInFullscreenSpace(window) && !window->last_fullscreen_exclusive_display && window->fullscreen_exclusive) {
                if (!Cocoa_SetWindowFullscreenSpace(window, false, true)) {
                    goto error;
                }
            } else if (fullscreen && window->last_fullscreen_exclusive_display && !window->fullscreen_exclusive) {
                for (i = 0; i < _this->num_displays; ++i) {
                    SDL_VideoDisplay *last_display = _this->displays[i];
                    if (last_display->fullscreen_window == window) {
                        SDL_SetDisplayModeForDisplay(last_display, NULL);
                        if (_this->SetWindowFullscreen) {
                            _this->SetWindowFullscreen(_this, window, last_display, false);
                        }
                        last_display->fullscreen_window = NULL;
                    }
                }
            }

            if (Cocoa_SetWindowFullscreenSpace(window, !!fullscreen, syncHint)) {
                goto done;
            }
        }
    }
#endif

    if (display) {
        // Restore the video mode on other displays if needed
        for (i = 0; i < _this->num_displays; ++i) {
            SDL_VideoDisplay *other = _this->displays[i];
            if (other != display && other->fullscreen_window == window) {
                SDL_SetDisplayModeForDisplay(other, NULL);
                other->fullscreen_window = NULL;
            }
        }
    }

    if (fullscreen) {
        int mode_w = 0, mode_h = 0;
        bool resized = false;

        // Hide any other fullscreen window on this display
        if (display->fullscreen_window &&
            display->fullscreen_window != window) {
            SDL_MinimizeWindow(display->fullscreen_window);
        }

        display->fullscreen_active = window->fullscreen_exclusive;

        if (!SDL_SetDisplayModeForDisplay(display, mode)) {
            goto error;
        }
        if (commit) {
            SDL_FullscreenResult ret = SDL_FULLSCREEN_SUCCEEDED;
            if (_this->SetWindowFullscreen) {
                ret = _this->SetWindowFullscreen(_this, window, display, fullscreen);
            } else {
                resized = true;
            }

            if (ret == SDL_FULLSCREEN_SUCCEEDED) {
                // Window is fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
                if (!(window->flags & SDL_WINDOW_FULLSCREEN)) {
                    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_ENTER_FULLSCREEN, 0, 0);
                }
            } else if (ret == SDL_FULLSCREEN_FAILED) {
                display->fullscreen_active = false;
                goto error;
            }
        }

        if (window->flags & SDL_WINDOW_FULLSCREEN) {
            display->fullscreen_window = window;

            /* Android may not resize the window to exactly what our fullscreen mode is,
             * especially on windowed Android environments like the Chromebook or Samsung DeX.
             * Given this, we shouldn't use the mode size. Android's SetWindowFullscreen
             * will generate the window event for us with the proper final size.
             *
             * This is also unnecessary on Cocoa, Wayland, Win32, and X11 (will send SDL_EVENT_WINDOW_RESIZED).
             */
            if (!SDL_SendsFullscreenDimensions(_this)) {
                SDL_Rect displayRect;

                if (mode) {
                    mode_w = mode->w;
                    mode_h = mode->h;
                    SDL_GetDisplayBounds(mode->displayID, &displayRect);
                } else {
                    mode_w = display->desktop_mode.w;
                    mode_h = display->desktop_mode.h;
                    SDL_GetDisplayBounds(display->id, &displayRect);
                }

                if (window->w != mode_w || window->h != mode_h) {
                    resized = true;
                }

                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MOVED, displayRect.x, displayRect.y);

                if (resized) {
                    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESIZED, mode_w, mode_h);
                } else {
                    SDL_OnWindowResized(window);
                }
            }
        }
    } else {
        bool resized = false;

        // Restore the desktop mode
        if (display) {
            display->fullscreen_active = false;

            SDL_SetDisplayModeForDisplay(display, NULL);
        }
        if (commit) {
            SDL_FullscreenResult ret = SDL_FULLSCREEN_SUCCEEDED;
            if (_this->SetWindowFullscreen) {
                SDL_VideoDisplay *full_screen_display = display ? display : SDL_GetVideoDisplayForFullscreenWindow(window);
                if (full_screen_display) {
                    ret = _this->SetWindowFullscreen(_this, window, full_screen_display, SDL_FULLSCREEN_OP_LEAVE);
                }
            } else {
                resized = true;
            }

            if (ret == SDL_FULLSCREEN_SUCCEEDED) {
                // Window left fullscreen immediately upon return. If the driver hasn't already sent the event, do so now.
                if (window->flags & SDL_WINDOW_FULLSCREEN) {
                    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_LEAVE_FULLSCREEN, 0, 0);
                }
            } else if (ret == SDL_FULLSCREEN_FAILED) {
                goto error;
            }
        }

        if (!(window->flags & SDL_WINDOW_FULLSCREEN)) {
            if (display) {
                display->fullscreen_window = NULL;
            }

            if (!SDL_SendsFullscreenDimensions(_this)) {
                SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MOVED, window->windowed.x, window->windowed.y);
                if (resized) {
                    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESIZED, window->windowed.w, window->windowed.h);
                } else {
                    SDL_OnWindowResized(window);
                }
            }
        }
    }

done:
    window->last_fullscreen_exclusive_display = display && (window->flags & SDL_WINDOW_FULLSCREEN) && window->fullscreen_exclusive ? display->id : 0;
    return true;

error:
    if (fullscreen) {
        // Something went wrong and the window is no longer fullscreen.
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, commit);
    }
    return false;
}

bool SDL_SetWindowFullscreenMode(SDL_Window *window, const SDL_DisplayMode *mode)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (mode) {
        if (!SDL_GetFullscreenModeMatch(mode)) {
            return SDL_SetError("Invalid fullscreen display mode");
        }

        // Save the mode so we can look up the closest match later
        SDL_copyp(&window->requested_fullscreen_mode, mode);
    } else {
        SDL_zero(window->requested_fullscreen_mode);
    }

    /* Copy to the current mode now, in case an asynchronous fullscreen window request
     * is in progress. It will be overwritten if a new request is made.
     */
    SDL_copyp(&window->current_fullscreen_mode, &window->requested_fullscreen_mode);

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
    /* If this is called while in the middle of a Cocoa fullscreen spaces transition,
     * wait until the transition has completed, or the window can wind up in a weird,
     * broken state if a mode switch occurs while in a fullscreen space.
     */
    if (SDL_strcmp(_this->name, "cocoa") == 0 && Cocoa_IsWindowInFullscreenSpaceTransition(window)) {
        SDL_SyncWindow(window);
    }
#endif
    if (SDL_WINDOW_FULLSCREEN_VISIBLE(window)) {
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_UPDATE, true);
        SDL_SyncIfRequired(window);
    }

    return true;
}

const SDL_DisplayMode *SDL_GetWindowFullscreenMode(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, NULL);
    CHECK_WINDOW_NOT_POPUP(window, NULL);

    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        return SDL_GetFullscreenModeMatch(&window->current_fullscreen_mode);
    } else {
        return SDL_GetFullscreenModeMatch(&window->requested_fullscreen_mode);
    }
}

void *SDL_GetWindowICCProfile(SDL_Window *window, size_t *size)
{
    if (!_this->GetWindowICCProfile) {
        SDL_Unsupported();
        return NULL;
    }
    return _this->GetWindowICCProfile(_this, window, size);
}

SDL_PixelFormat SDL_GetWindowPixelFormat(SDL_Window *window)
{
    SDL_DisplayID displayID;
    const SDL_DisplayMode *mode;

    CHECK_WINDOW_MAGIC(window, SDL_PIXELFORMAT_UNKNOWN);

    displayID = GetDisplayForWindow(window);
    mode = SDL_GetCurrentDisplayMode(displayID);
    if (mode) {
        return mode->format;
    } else {
        return SDL_PIXELFORMAT_UNKNOWN;
    }
}

#define CREATE_FLAGS \
    (SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_POPUP_MENU | SDL_WINDOW_UTILITY | SDL_WINDOW_TOOLTIP | SDL_WINDOW_VULKAN | SDL_WINDOW_MINIMIZED | SDL_WINDOW_METAL | SDL_WINDOW_TRANSPARENT | SDL_WINDOW_NOT_FOCUSABLE | SDL_WINDOW_FILL_DOCUMENT)

static SDL_INLINE bool IsAcceptingDragAndDrop(void)
{
    if (SDL_EventEnabled(SDL_EVENT_DROP_FILE) || SDL_EventEnabled(SDL_EVENT_DROP_TEXT)) {
        return true;
    }
    return false;
}

// prepare a newly-created window
static SDL_INLINE void PrepareDragAndDropSupport(SDL_Window *window)
{
    if (_this->AcceptDragAndDrop) {
        _this->AcceptDragAndDrop(window, IsAcceptingDragAndDrop());
    }
}

// toggle d'n'd for all existing windows.
void SDL_ToggleDragAndDropSupport(void)
{
    if (_this && _this->AcceptDragAndDrop) {
        const bool enable = IsAcceptingDragAndDrop();
        SDL_Window *window;
        for (window = _this->windows; window; window = window->next) {
            _this->AcceptDragAndDrop(window, enable);
        }
    }
}

SDL_Window ** SDLCALL SDL_GetWindows(int *count)
{
    if (count) {
        *count = 0;
    }

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }

    SDL_Window *window;
    int num_added = 0;
    int num_windows = 0;
    for (window = _this->windows; window; window = window->next) {
        ++num_windows;
    }

    SDL_Window **windows = (SDL_Window **)SDL_malloc((num_windows + 1) * sizeof(*windows));
    if (!windows) {
        return NULL;
    }

    for (window = _this->windows; window; window = window->next) {
        windows[num_added++] = window;
        if (num_added == num_windows) {
            // Race condition? Multi-threading not supported, ignore it
            break;
        }
    }
    windows[num_added] = NULL;

    if (count) {
        *count = num_added;
    }
    return windows;
}

static void ApplyWindowFlags(SDL_Window *window, SDL_WindowFlags flags)
{
    if (!SDL_WINDOW_IS_POPUP(window)) {
        if (!(flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_MAXIMIZED))) {
            SDL_RestoreWindow(window);
        }
        if (flags & SDL_WINDOW_MAXIMIZED) {
            SDL_MaximizeWindow(window);
        }

        SDL_SetWindowFullscreen(window, (flags & SDL_WINDOW_FULLSCREEN) != 0);

        if (flags & SDL_WINDOW_MINIMIZED) {
            SDL_MinimizeWindow(window);
        }

        if (flags & SDL_WINDOW_MODAL) {
            SDL_SetWindowModal(window, true);
        }

        if (flags & SDL_WINDOW_MOUSE_GRABBED) {
            SDL_SetWindowMouseGrab(window, true);
        }
        if (flags & SDL_WINDOW_KEYBOARD_GRABBED) {
            SDL_SetWindowKeyboardGrab(window, true);
        }
    }
}

static void SDL_FinishWindowCreation(SDL_Window *window, SDL_WindowFlags flags)
{
    PrepareDragAndDropSupport(window);

    if (window->flags & SDL_WINDOW_EXTERNAL) {
        // Whoever has created the window has already applied whatever flags are needed
    } else {
        ApplyWindowFlags(window, flags);
        if (!(flags & SDL_WINDOW_HIDDEN)) {
            SDL_ShowWindow(window);
        }
    }

#if defined(SDL_PLATFORM_LINUX)
    if (!SDL_WINDOW_IS_POPUP(window)) {
        // On Linux the progress state is persisted throughout multiple program runs, so reset state on window creation
        SDL_SetWindowProgressState(window, SDL_PROGRESS_STATE_NONE);
        SDL_SetWindowProgressValue(window, 0.0f);
    }
#endif
}

static bool SDL_ContextNotSupported(const char *name)
{
    return SDL_SetError("%s support is either not configured in SDL "
                        "or not available in current SDL video driver "
                        "(%s) or platform",
                        name,
                        _this->name);
}

static struct {
    const char *property_name;
    SDL_WindowFlags flag;
    bool invert_value;
} SDL_WindowFlagProperties[] = {
    { SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN,      SDL_WINDOW_ALWAYS_ON_TOP,       false },
    { SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN,         SDL_WINDOW_BORDERLESS,          false },
    { SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN,          SDL_WINDOW_NOT_FOCUSABLE,       true },
    { SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN,         SDL_WINDOW_FULLSCREEN,          false },
    { SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN,             SDL_WINDOW_HIDDEN,              false },
    { SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, SDL_WINDOW_HIGH_PIXEL_DENSITY,  false },
    { SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN,          SDL_WINDOW_MAXIMIZED,           false },
    { SDL_PROP_WINDOW_CREATE_MENU_BOOLEAN,               SDL_WINDOW_POPUP_MENU,          false },
    { SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN,              SDL_WINDOW_METAL,               false },
    { SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN,          SDL_WINDOW_MINIMIZED,           false },
    { SDL_PROP_WINDOW_CREATE_MODAL_BOOLEAN,              SDL_WINDOW_MODAL,               false },
    { SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN,      SDL_WINDOW_MOUSE_GRABBED,       false },
    { SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN,             SDL_WINDOW_OPENGL,              false },
    { SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN,          SDL_WINDOW_RESIZABLE,           false },
    { SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN,        SDL_WINDOW_TRANSPARENT,         false },
    { SDL_PROP_WINDOW_CREATE_TOOLTIP_BOOLEAN,            SDL_WINDOW_TOOLTIP,             false },
    { SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN,            SDL_WINDOW_UTILITY,             false },
    { SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN,             SDL_WINDOW_VULKAN,              false }
};

static SDL_WindowFlags SDL_GetWindowFlagProperties(SDL_PropertiesID props)
{
    unsigned i;
    SDL_WindowFlags flags = (SDL_WindowFlags)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, 0);

    for (i = 0; i < SDL_arraysize(SDL_WindowFlagProperties); ++i) {
        if (SDL_WindowFlagProperties[i].invert_value) {
            if (!SDL_GetBooleanProperty(props, SDL_WindowFlagProperties[i].property_name, true)) {
                flags |= SDL_WindowFlagProperties[i].flag;
            }
        } else {
            if (SDL_GetBooleanProperty(props, SDL_WindowFlagProperties[i].property_name, false)) {
                flags |= SDL_WindowFlagProperties[i].flag;
            }
        }
    }
    return flags;
}

static bool SDL_ValidateWindowType(SDL_VideoDevice *device, SDL_Window *parent, SDL_WindowFlags *flags)
{
    if ((*flags & SDL_WINDOW_MODAL) && !SDL_ObjectValid(parent, SDL_OBJECT_TYPE_WINDOW)) {
        return SDL_SetError("Modal windows must specify a parent window");
    }

    if ((*flags & (SDL_WINDOW_TOOLTIP | SDL_WINDOW_POPUP_MENU)) != 0) {
        if (!(device->device_caps & VIDEO_DEVICE_CAPS_HAS_POPUP_WINDOW_SUPPORT)) {
            return SDL_Unsupported();
        }
        if (!SDL_ObjectValid(parent, SDL_OBJECT_TYPE_WINDOW)) {
            return SDL_SetError("Tooltip and popup menu windows must specify a parent window");
        }
        *flags &= ~(SDL_WINDOW_MINIMIZED | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS);
    }

    SDL_WindowFlags type_flags = *flags & (SDL_WINDOW_UTILITY | SDL_WINDOW_TOOLTIP | SDL_WINDOW_POPUP_MENU | SDL_WINDOW_MODAL);
    if (type_flags & (type_flags - 1)) {
        return SDL_SetError("Conflicting window type flags specified: 0x%.8x", (unsigned int)type_flags);
    }
    return true;
}

static bool SDL_PrepareWindowGraphics(SDL_VideoDevice *device, SDL_WindowFlags *flags, bool external_graphics_context)
{
    SDL_WindowFlags graphics_flags = *flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_METAL | SDL_WINDOW_VULKAN);
    if (graphics_flags & (graphics_flags - 1)) {
        return SDL_SetError("Conflicting window graphics flags specified: 0x%.8x", (unsigned int)graphics_flags);
    }

    // Some platforms have certain graphics backends enabled by default
    if (!graphics_flags && !external_graphics_context) {
        *flags |= SDL_DefaultGraphicsBackends(device);
    }

    if (*flags & SDL_WINDOW_OPENGL) {
        if (!device->GL_CreateContext) {
            return SDL_ContextNotSupported("OpenGL");
        }
        if (!SDL_GL_LoadLibrary(NULL)) {
            return false;
        }
    }

    if (*flags & SDL_WINDOW_VULKAN) {
        if (!device->Vulkan_CreateSurface) {
            return SDL_ContextNotSupported("Vulkan");
        }
        if (!SDL_Vulkan_LoadLibrary(NULL)) {
            return false;
        }
    }

    if ((*flags & SDL_WINDOW_METAL) && !device->Metal_CreateView) {
        return SDL_ContextNotSupported("Metal");
    }
    return true;
}

static SDL_DisplayID SDL_ResolveWindowPosition(int *window_x, int *window_y, int w, int h, bool *undefined_x, bool *undefined_y)
{
    int x = *window_x;
    int y = *window_y;
    SDL_DisplayID displayID = 0;

    if (SDL_WINDOWPOS_ISUNDEFINED(x) || SDL_WINDOWPOS_ISUNDEFINED(y) ||
        SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISCENTERED(y)) {
        SDL_Rect bounds;

        if ((SDL_WINDOWPOS_ISUNDEFINED(x) || SDL_WINDOWPOS_ISCENTERED(x)) && (x & 0xFFFF)) {
            displayID = (x & 0xFFFF);
        } else if ((SDL_WINDOWPOS_ISUNDEFINED(y) || SDL_WINDOWPOS_ISCENTERED(y)) && (y & 0xFFFF)) {
            displayID = (y & 0xFFFF);
        }
        if (displayID == 0 || SDL_GetDisplayIndex(displayID) < 0) {
            displayID = SDL_GetPrimaryDisplay();
        }

        SDL_zero(bounds);
        SDL_GetDisplayUsableBounds(displayID, &bounds);
        if (w > bounds.w || h > bounds.h) {
            // This window is larger than the usable bounds, just center on the display
            SDL_GetDisplayBounds(displayID, &bounds);
        }
        if (SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISUNDEFINED(x)) {
            if (SDL_WINDOWPOS_ISUNDEFINED(x)) {
                *undefined_x = true;
            }
            x = bounds.x + (bounds.w - w) / 2;
        }
        if (SDL_WINDOWPOS_ISCENTERED(y) || SDL_WINDOWPOS_ISUNDEFINED(y)) {
            if (SDL_WINDOWPOS_ISUNDEFINED(y)) {
                *undefined_y = true;
            }
            y = bounds.y + (bounds.h - h) / 2;
        }
    }

    *window_x = x;
    *window_y = y;
    return displayID;
}

SDL_Window *SDL_CreateWindowWithProperties(SDL_PropertiesID props)
{
    SDL_Window *window;
    const char *title = SDL_GetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, NULL);
    int x = (int)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    int y = (int)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    int w = (int)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 0);
    int h = (int)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 0);
    SDL_Window *parent = (SDL_Window *)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, NULL);
    SDL_WindowFlags flags = SDL_GetWindowFlagProperties(props);
    SDL_DisplayID displayID;
    bool undefined_x = false;
    bool undefined_y = false;
    bool external_graphics_context = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN, false);

    if (!_this) {
        // Initialize the video system if needed
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            return NULL;
        }

        // Make clang-tidy happy
        if (!_this) {
            return NULL;
        }
    }

    if (!SDL_ValidateWindowType(_this, parent, &flags)) {
        return NULL;
    }

    // Make sure the display list is up to date for window placement
    if (_this->RefreshDisplays) {
        _this->RefreshDisplays(_this);
    }

    // Some platforms can't create zero-sized windows
    if (w < 1) {
        w = 1;
    }
    if (h < 1) {
        h = 1;
    }

    displayID = SDL_ResolveWindowPosition(&x, &y, w, h, &undefined_x, &undefined_y);

    if (!SDL_PrepareWindowGraphics(_this, &flags, external_graphics_context)) {
        return NULL;
    }

    window = (SDL_Window *)SDL_calloc(1, sizeof(*window));
    if (!window) {
        return NULL;
    }
    SDL_SetObjectValid(window, SDL_OBJECT_TYPE_WINDOW, true);
    window->id = SDL_GetNextObjectID();
    window->floating.x = window->windowed.x = window->x = x;
    window->floating.y = window->windowed.y = window->y = y;
    window->floating.w = window->windowed.w = window->w = w;
    window->floating.h = window->windowed.h = window->h = h;
    window->undefined_x = undefined_x;
    window->undefined_y = undefined_y;
    window->pending_displayID = displayID;

    SDL_VideoDisplay *display = SDL_GetVideoDisplayForWindow(window);
    if (display) {
        SDL_SetWindowHDRProperties(window, &display->HDR, false);
    }

    if (flags & SDL_WINDOW_FULLSCREEN || IsFullscreenOnly(_this)) {
        SDL_Rect bounds;

        SDL_GetDisplayBounds(display ? display->id : SDL_GetPrimaryDisplay(), &bounds);
        window->x = bounds.x;
        window->y = bounds.y;
        window->w = bounds.w;
        window->h = bounds.h;
        window->pending_flags |= SDL_WINDOW_FULLSCREEN;
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window->flags = ((flags & CREATE_FLAGS) | SDL_WINDOW_HIDDEN);
    window->display_scale = 1.0f;
    window->opacity = 1.0f;
    window->next = _this->windows;
    window->is_destroying = false;
    window->displayID = GetDisplayForWindow(window);
    window->external_graphics_context = external_graphics_context;
    window->constrain_popup = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_CONSTRAIN_POPUP_BOOLEAN, true);

    if (!_this->SetWindowFillDocument) {
        window->flags &= ~SDL_WINDOW_FILL_DOCUMENT;  // not an error, just unsupported here, so remove the flag.
    }

    if (_this->windows) {
        _this->windows->prev = window;
    }
    _this->windows = window;

    // Set the parent before creation.
    SDL_UpdateWindowHierarchy(window, parent);

    if (_this->CreateSDLWindow && !_this->CreateSDLWindow(_this, window, props)) {
        SDL_PushError();
        SDL_DestroyWindow(window);
        SDL_PopError();
        return NULL;
    }

    /* Clear minimized if not on windows, only windows handles it at create rather than FinishWindowCreation,
     * but it's important or window focus will get broken on windows!
     */
#if !defined(SDL_PLATFORM_WINDOWS)
    if (window->flags & SDL_WINDOW_MINIMIZED) {
        window->flags &= ~SDL_WINDOW_MINIMIZED;
    }
#endif

    if (title) {
        SDL_SetWindowTitle(window, title);
    }
    SDL_FinishWindowCreation(window, flags);

    // Make sure window pixel size is up to date
    SDL_CheckWindowPixelSizeChanged(window);

#ifdef SDL_VIDEO_DRIVER_UIKIT
    SDL_UpdateLifecycleObserver();
#endif

    SDL_ClearError();

    return window;
}

SDL_Window *SDL_CreateWindow(const char *title, int w, int h, SDL_WindowFlags flags)
{
    SDL_Window *window;
    SDL_PropertiesID props = SDL_CreateProperties();
    if (title && *title) {
        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
    }
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, w);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, h);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, flags);
    window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    return window;
}

SDL_Window *SDL_CreatePopupWindow(SDL_Window *parent, int offset_x, int offset_y, int w, int h, SDL_WindowFlags flags)
{
    SDL_Window *window;
    SDL_PropertiesID props = SDL_CreateProperties();

    // Popups must specify either the tooltip or popup menu window flags
    if (!(flags & (SDL_WINDOW_TOOLTIP | SDL_WINDOW_POPUP_MENU))) {
        SDL_SetError("Popup windows must specify either the 'SDL_WINDOW_TOOLTIP' or the 'SDL_WINDOW_POPUP_MENU' flag");
        return NULL;
    }

    SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, parent);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, offset_x);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, offset_y);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, w);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, h);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, flags);
    window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    return window;
}

static bool SDL_ValidateReconfiguredWindowGraphics(SDL_WindowFlags graphics_flags)
{
    if (graphics_flags & (graphics_flags - 1)) {
        return SDL_SetError("Conflicting window flags specified");
    }

    if ((graphics_flags & SDL_WINDOW_OPENGL) && !_this->GL_CreateContext) {
        return SDL_ContextNotSupported("OpenGL");
    }
    if ((graphics_flags & SDL_WINDOW_VULKAN) && !_this->Vulkan_CreateSurface) {
        return SDL_ContextNotSupported("Vulkan");
    }
    if ((graphics_flags & SDL_WINDOW_METAL) && !_this->Metal_CreateView) {
        return SDL_ContextNotSupported("Metal");
    }
    return true;
}

static bool SDL_ReconfigureWindowInternal(SDL_Window *window, SDL_WindowFlags flags)
{
    bool loaded_opengl = false;
    bool loaded_vulkan = false;

    if (!_this->ReconfigureWindow) {
        return false;
    }

    const SDL_WindowFlags graphics_flags = flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_METAL | SDL_WINDOW_VULKAN);
    if (!SDL_ValidateReconfiguredWindowGraphics(graphics_flags)) {
        return false;
    }

    if (!(window->flags & SDL_WINDOW_EXTERNAL)) {
        // Only attempt to reconfigure if the window has no existing graphics flags.
        if (window->flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_METAL | SDL_WINDOW_VULKAN)) {
            return false;
        }
    } else {
        // Can't destroy and recreate an external window, so try our best to reconfigure it.
        if (!_this->ReconfigureWindow(_this, window, 0)) {
            return false;
        }

        // Reload the GL/Vulkan libraries in case the profile changed.
        if (window->flags & SDL_WINDOW_OPENGL) {
            SDL_GL_UnloadLibrary();
        }
        if (window->flags & SDL_WINDOW_VULKAN) {
            SDL_Vulkan_UnloadLibrary();
        }

        window->flags &= ~(SDL_WINDOW_OPENGL | SDL_WINDOW_METAL | SDL_WINDOW_VULKAN);
    }

    SDL_DestroyWindowSurface(window);

    if (graphics_flags & SDL_WINDOW_OPENGL) {
        loaded_opengl = SDL_GL_LoadLibrary(NULL);
        if (!loaded_opengl) {
            return false;
        }
    } else if (graphics_flags & SDL_WINDOW_VULKAN) {
        loaded_vulkan = SDL_Vulkan_LoadLibrary(NULL);
        if (!loaded_vulkan) {
            return false;
        }
    }

    // Try to reconfigure the window for the requested graphics flags.
    if (!_this->ReconfigureWindow(_this, window, graphics_flags)) {
        if (loaded_opengl) {
            SDL_GL_UnloadLibrary();
        }
        if (loaded_vulkan) {
            SDL_Vulkan_UnloadLibrary();
        }

        return false;
    }

    window->flags |= graphics_flags;

    return true;
}

bool SDL_RecreateWindow(SDL_Window *window, SDL_WindowFlags flags)
{
    bool loaded_opengl = false;
    bool need_gl_unload = false;
    bool need_gl_load = false;
    bool loaded_vulkan = false;
    bool need_vulkan_unload = false;
    bool need_vulkan_load = false;
    SDL_WindowFlags graphics_flags;

    graphics_flags = flags & (SDL_WINDOW_OPENGL | SDL_WINDOW_METAL | SDL_WINDOW_VULKAN);
    if (!SDL_ValidateReconfiguredWindowGraphics(graphics_flags)) {
        return false;
    }

    if (window->flags & SDL_WINDOW_EXTERNAL) {
        // Can't destroy and re-create external windows, hrm
        flags |= SDL_WINDOW_EXTERNAL;
    } else {
        flags &= ~SDL_WINDOW_EXTERNAL;
    }

    // If this is a modal dialog, clear the modal status.
    if (window->flags & SDL_WINDOW_MODAL) {
        SDL_SetWindowModal(window, false);
    }

    // Restore video mode, etc.
    if (!(window->flags & SDL_WINDOW_EXTERNAL)) {
        const bool restore_on_show = window->restore_on_show;
        SDL_HideWindow(window);
        window->restore_on_show = restore_on_show;
    }

    // Tear down the old native window
    SDL_DestroyWindowSurface(window);

    if ((window->flags & SDL_WINDOW_OPENGL) != (flags & SDL_WINDOW_OPENGL)) {
        if (flags & SDL_WINDOW_OPENGL) {
            need_gl_load = true;
        } else {
            need_gl_unload = true;
        }
    } else if (window->flags & SDL_WINDOW_OPENGL) {
        need_gl_unload = true;
        need_gl_load = true;
    }

    if ((window->flags & SDL_WINDOW_VULKAN) != (flags & SDL_WINDOW_VULKAN)) {
        if (flags & SDL_WINDOW_VULKAN) {
            need_vulkan_load = true;
        } else {
            need_vulkan_unload = true;
        }
    } else if (window->flags & SDL_WINDOW_VULKAN) {
        need_vulkan_unload = true;
        need_vulkan_load = true;
    }

    if (need_gl_unload) {
        SDL_GL_UnloadLibrary();
    }

    if (need_vulkan_unload) {
        SDL_Vulkan_UnloadLibrary();
    }

    if (_this->DestroyWindow && !(flags & SDL_WINDOW_EXTERNAL)) {
        _this->DestroyWindow(_this, window);
    }

    if (need_gl_load) {
        if (!SDL_GL_LoadLibrary(NULL)) {
            return false;
        }
        loaded_opengl = true;
    }

    if (need_vulkan_load) {
        if (!SDL_Vulkan_LoadLibrary(NULL)) {
            return false;
        }
        loaded_vulkan = true;
    }

    window->flags = ((flags & CREATE_FLAGS) | SDL_WINDOW_HIDDEN);
    window->is_destroying = false;

    if (_this->CreateSDLWindow && !(flags & SDL_WINDOW_EXTERNAL)) {
        /* Reset the window size to the original floating value, so the
         * recreated window has the proper base size.
         */
        window->x = window->windowed.x = window->floating.x;
        window->y = window->windowed.y = window->floating.y;
        window->w = window->windowed.w = window->floating.w;
        window->h = window->windowed.h = window->floating.h;

        if (!_this->CreateSDLWindow(_this, window, 0)) {
            if (loaded_opengl) {
                SDL_GL_UnloadLibrary();
                window->flags &= ~SDL_WINDOW_OPENGL;
            }
            if (loaded_vulkan) {
                SDL_Vulkan_UnloadLibrary();
                window->flags &= ~SDL_WINDOW_VULKAN;
            }
            return false;
        }
    }

    if (flags & SDL_WINDOW_EXTERNAL) {
        window->flags |= SDL_WINDOW_EXTERNAL;
    }

    if (_this->SetWindowTitle && window->title) {
        _this->SetWindowTitle(_this, window);
    }

    if (_this->SetWindowIcon && window->icon) {
        _this->SetWindowIcon(_this, window, window->icon);
    }

    if (_this->SetWindowMinimumSize && (window->min_w || window->min_h)) {
        _this->SetWindowMinimumSize(_this, window);
    }

    if (_this->SetWindowMaximumSize && (window->max_w || window->max_h)) {
        _this->SetWindowMaximumSize(_this, window);
    }

    if (_this->SetWindowAspectRatio && (window->min_aspect > 0.0f || window->max_aspect > 0.0f)) {
        _this->SetWindowAspectRatio(_this, window);
    }

    if (window->hit_test) {
        _this->SetWindowHitTest(window, true);
    }

    SDL_FinishWindowCreation(window, flags);

    return true;
}

bool SDL_ReconfigureWindow(SDL_Window *window, SDL_WindowFlags flags)
{
    // Try to reconfigure the window for the desired flags first, before completely destroying and recreating it.
    if (!SDL_ReconfigureWindowInternal(window, flags)) {
        return SDL_RecreateWindow(window, flags);
    }

    return true;
}

bool SDL_HasWindows(void)
{
    return _this && _this->windows;
}

SDL_WindowID SDL_GetWindowID(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return window->id;
}

SDL_Window *SDL_GetWindowFromID(SDL_WindowID id)
{
    SDL_Window *window;

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    if (id) {
        for (window = _this->windows; window; window = window->next) {
            if (window->id == id) {
                return window;
            }
        }
    }
    SDL_SetError("Invalid window ID");
    return NULL;
}

SDL_Window *SDL_GetWindowParent(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, NULL);

    return window->parent;
}

SDL_PropertiesID SDL_GetWindowProperties(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    if (window->props == 0) {
        window->props = SDL_CreateProperties();
    }
    return window->props;
}

SDL_WindowFlags SDL_GetWindowFlags(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, 0);

    return window->flags | window->pending_flags;
}

bool SDL_SetWindowTitle(SDL_Window *window, const char *title)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (title == window->title) {
        return true;
    }
    if (!title) {
        title = "";
    }
    if (window->title && SDL_strcmp(title, window->title) == 0) {
        return true;
    }

    SDL_free(window->title);

    window->title = SDL_strdup(title);

    if (_this->SetWindowTitle) {
        _this->SetWindowTitle(_this, window);
    }
    return true;
}

const char *SDL_GetWindowTitle(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, "");

    return window->title ? window->title : "";
}

bool SDL_SetWindowIcon(SDL_Window *window, SDL_Surface *icon)
{
    CHECK_WINDOW_MAGIC(window, false);

    CHECK_PARAM(!icon) {
        return SDL_InvalidParamError("icon");
    }

    SDL_DestroySurface(window->icon);

    // Convert the icon into ARGB8888
    window->icon = SDL_ConvertSurface(icon, SDL_PIXELFORMAT_ARGB8888);
    if (!window->icon) {
        return false;
    }

    if (!_this->SetWindowIcon) {
        return SDL_Unsupported();
    }

    return _this->SetWindowIcon(_this, window, window->icon);
}

bool SDL_SetWindowPosition(SDL_Window *window, int x, int y)
{
    SDL_DisplayID original_displayID;

    CHECK_WINDOW_MAGIC(window, false);

    const int w = window->last_size_pending ? window->pending.w : window->windowed.w;
    const int h = window->last_size_pending ? window->pending.h : window->windowed.h;

    original_displayID = GetDisplayForWindow(window);
    window->pending_displayID = 0;

    if (SDL_WINDOWPOS_ISUNDEFINED(x)) {
        x = window->windowed.x;
    }
    if (SDL_WINDOWPOS_ISUNDEFINED(y)) {
        y = window->windowed.y;
    }
    if (SDL_WINDOWPOS_ISCENTERED(x) || SDL_WINDOWPOS_ISCENTERED(y)) {
        SDL_DisplayID displayID = original_displayID;
        SDL_Rect bounds;

        if (SDL_WINDOWPOS_ISCENTERED(x) && (x & 0xFFFF)) {
            displayID = (x & 0xFFFF);
        } else if (SDL_WINDOWPOS_ISCENTERED(y) && (y & 0xFFFF)) {
            displayID = (y & 0xFFFF);
        }
        if (displayID == 0 || SDL_GetDisplayIndex(displayID) < 0) {
            displayID = SDL_GetPrimaryDisplay();
        }

        window->pending_displayID = displayID;

        SDL_zero(bounds);
        if (!SDL_GetDisplayUsableBounds(displayID, &bounds) || w > bounds.w || h > bounds.h) {
            if (!SDL_GetDisplayBounds(displayID, &bounds)) {
                return false;
            }
        }
        if (SDL_WINDOWPOS_ISCENTERED(x)) {
            x = bounds.x + (bounds.w - w) / 2;
        }
        if (SDL_WINDOWPOS_ISCENTERED(y)) {
            y = bounds.y + (bounds.h - h) / 2;
        }
    } else {
        /* See if the requested window position matches the origin of any displays and set
         * the pending fullscreen display ID if it does. This needs to be set early in case
         * the window is prevented from moving to the exact origin due to struts.
         */
        window->pending_displayID = GetDisplayAtOrigin(x, y);
    }

    window->pending.x = x;
    window->pending.y = y;

    /* Windows are placed at the coordinates received while in fullscreen after leaving fullscreen.
     * Asynchronous backends need special handling in this case.
     */
    if (!_this->SyncWindow && (window->flags & SDL_WINDOW_FULLSCREEN)) {
        window->floating.x = window->windowed.x = x;
        window->floating.y = window->windowed.y = y;
    }
    window->undefined_x = false;
    window->undefined_y = false;
    window->last_position_pending = true;

    if (_this->SetWindowPosition) {
        const bool result = _this->SetWindowPosition(_this, window);
        if (result) {
            SDL_SyncIfRequired(window);
        }
        return result;
    }

    return SDL_Unsupported();
}

bool SDL_GetWindowPosition(SDL_Window *window, int *x, int *y)
{
    CHECK_WINDOW_MAGIC(window, false);

#ifdef SDL_PLATFORM_MACOS
    // On newer MacBooks, the fullscreen window might be placed below the camera notch, so use the actual window position
    bool use_display_origin = false;
#else
    // Fullscreen windows are always at their display's origin
    bool use_display_origin = ((window->flags & SDL_WINDOW_FULLSCREEN) != 0);
#endif
    if (use_display_origin) {
        SDL_DisplayID displayID;

        if (x) {
            *x = 0;
        }
        if (y) {
            *y = 0;
        }

        /* Find the window's monitor and update to the
           monitor offset. */
        displayID = GetDisplayForWindow(window);
        if (displayID != 0) {
            SDL_Rect bounds;

            SDL_zero(bounds);

            SDL_GetDisplayBounds(displayID, &bounds);
            if (x) {
                *x = bounds.x;
            }
            if (y) {
                *y = bounds.y;
            }
        }
    } else {
        const bool use_pending = (window->flags & SDL_WINDOW_HIDDEN) && window->last_position_pending;
        if (x) {
            *x = use_pending ? window->pending.x : window->x;
        }
        if (y) {
            *y = use_pending ? window->pending.y : window->y;
        }
    }
    return true;
}

bool SDL_SetWindowBordered(SDL_Window *window, bool bordered)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowBordered) {
        return SDL_Unsupported();
    }

    const bool want = bordered;
    const bool have = !(window->flags & SDL_WINDOW_BORDERLESS);
    if (want != have) {
        if (want) {
            window->flags &= ~SDL_WINDOW_BORDERLESS;
        } else {
            window->flags |= SDL_WINDOW_BORDERLESS;
        }
        _this->SetWindowBordered(_this, window, want);
    }

    return true;
}

bool SDL_SetWindowResizable(SDL_Window *window, bool resizable)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowResizable) {
        return SDL_Unsupported();
    }

    const bool want = resizable;
    const bool have = ((window->flags & SDL_WINDOW_RESIZABLE) != 0);
    if (want != have) {
        if (want) {
            window->flags |= SDL_WINDOW_RESIZABLE;
        } else {
            window->flags &= ~SDL_WINDOW_RESIZABLE;
            SDL_copyp(&window->windowed, &window->floating);
        }
        _this->SetWindowResizable(_this, window, want);
    }

    return true;
}

bool SDL_SetWindowAlwaysOnTop(SDL_Window *window, bool on_top)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowAlwaysOnTop) {
        return SDL_Unsupported();
    }

    const bool want = on_top;
    const bool have = ((window->flags & SDL_WINDOW_ALWAYS_ON_TOP) != 0);
    if (want != have) {
        if (want) {
            window->flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        } else {
            window->flags &= ~SDL_WINDOW_ALWAYS_ON_TOP;
        }
        _this->SetWindowAlwaysOnTop(_this, window, want);
    }

    return true;
}

bool SDL_SetWindowSize(SDL_Window *window, int w, int h)
{
    CHECK_WINDOW_MAGIC(window, false);

    CHECK_PARAM(w <= 0) {
        return SDL_InvalidParamError("w");
    }
    CHECK_PARAM(h <= 0) {
        return SDL_InvalidParamError("h");
    }

    // It is possible for the aspect ratio constraints to not satisfy the size constraints.
    // The size constraints will override the aspect ratio constraints so we will apply the
    // the aspect ratio constraints first
    float new_aspect = w / (float)h;
    if (window->max_aspect > 0.0f && new_aspect > window->max_aspect) {
        w = (int)SDL_roundf(h * window->max_aspect);
    } else if (window->min_aspect > 0.0f && new_aspect < window->min_aspect) {
        h = (int)SDL_roundf(w / window->min_aspect);
    }

    // Make sure we don't exceed any window size limits
    if (window->min_w && w < window->min_w) {
        w = window->min_w;
    }
    if (window->max_w && w > window->max_w) {
        w = window->max_w;
    }
    if (window->min_h && h < window->min_h) {
        h = window->min_h;
    }
    if (window->max_h && h > window->max_h) {
        h = window->max_h;
    }

    window->last_size_pending = true;
    window->pending.w = w;
    window->pending.h = h;

    if (_this->SetWindowSize) {
        _this->SetWindowSize(_this, window);
        SDL_SyncIfRequired(window);
    } else {
        return SDL_Unsupported();
    }
    return true;
}

bool SDL_GetWindowSize(SDL_Window *window, int *w, int *h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (w) {
        *w = window->w;
    }
    if (h) {
        *h = window->h;
    }
    return true;
}

bool SDL_SetWindowAspectRatio(SDL_Window *window, float min_aspect, float max_aspect)
{
    CHECK_WINDOW_MAGIC(window, false);

    window->min_aspect = min_aspect;
    window->max_aspect = max_aspect;

    if (_this->SetWindowAspectRatio) {
        _this->SetWindowAspectRatio(_this, window);
    }

    // Ensure that window has the correct aspect ratio
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowAspectRatio(SDL_Window *window, float *min_aspect, float *max_aspect)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (min_aspect) {
        *min_aspect = window->min_aspect;
    }
    if (max_aspect) {
        *max_aspect = window->max_aspect;
    }
    return true;
}

bool SDL_GetWindowBordersSize(SDL_Window *window, int *top, int *left, int *bottom, int *right)
{
    int dummy = 0;

    if (!top) {
        top = &dummy;
    }
    if (!left) {
        left = &dummy;
    }
    if (!right) {
        right = &dummy;
    }
    if (!bottom) {
        bottom = &dummy;
    }

    // Always initialize, so applications don't have to care
    *top = *left = *bottom = *right = 0;

    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->GetWindowBordersSize) {
        return SDL_Unsupported();
    }

    return _this->GetWindowBordersSize(_this, window, top, left, bottom, right);
}

bool SDL_GetWindowSizeInPixels(SDL_Window *window, int *w, int *h)
{
    int filter;

    CHECK_WINDOW_MAGIC(window, false);

    if (!w) {
        w = &filter;
    }

    if (!h) {
        h = &filter;
    }

    if (_this->GetWindowSizeInPixels) {
        _this->GetWindowSizeInPixels(_this, window, w, h);
    } else {
        SDL_DisplayID displayID = GetDisplayForWindow(window);
        const SDL_DisplayMode *mode;

        SDL_GetWindowSize(window, w, h);

        if ((window->flags & SDL_WINDOW_FULLSCREEN) && SDL_GetWindowFullscreenMode(window)) {
            mode = SDL_GetCurrentDisplayMode(displayID);
        } else {
            mode = SDL_GetDesktopDisplayMode(displayID);
        }
        if (mode) {
            *w = (int)SDL_ceilf(*w * mode->pixel_density);
            *h = (int)SDL_ceilf(*h * mode->pixel_density);
        }
    }
    return true;
}

bool SDL_SetWindowMinimumSize(SDL_Window *window, int min_w, int min_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_PARAM(min_w < 0) {
        return SDL_InvalidParamError("min_w");
    }
    CHECK_PARAM(min_h < 0) {
        return SDL_InvalidParamError("min_h");
    }

    CHECK_PARAM((window->max_w && min_w > window->max_w) || (window->max_h && min_h > window->max_h)) {
        return SDL_SetError("SDL_SetWindowMinimumSize(): Tried to set minimum size larger than maximum size");
    }

    window->min_w = min_w;
    window->min_h = min_h;

    if (_this->SetWindowMinimumSize) {
        _this->SetWindowMinimumSize(_this, window);
    }

    // Ensure that window is not smaller than minimal size
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowMinimumSize(SDL_Window *window, int *min_w, int *min_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (min_w) {
        *min_w = window->min_w;
    }
    if (min_h) {
        *min_h = window->min_h;
    }
    return true;
}

bool SDL_SetWindowMaximumSize(SDL_Window *window, int max_w, int max_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_PARAM(max_w < 0) {
        return SDL_InvalidParamError("max_w");
    }
    CHECK_PARAM(max_h < 0) {
        return SDL_InvalidParamError("max_h");
    }

    if ((max_w && max_w < window->min_w) ||
        (max_h && max_h < window->min_h)) {
        return SDL_SetError("SDL_SetWindowMaximumSize(): Tried to set maximum size smaller than minimum size");
    }

    window->max_w = max_w;
    window->max_h = max_h;

    if (_this->SetWindowMaximumSize) {
        _this->SetWindowMaximumSize(_this, window);
    }

    // Ensure that window is not larger than maximal size
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowMaximumSize(SDL_Window *window, int *max_w, int *max_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (max_w) {
        *max_w = window->max_w;
    }
    if (max_h) {
        *max_h = window->max_h;
    }
    return true;
}

bool SDL_ShowWindow(SDL_Window *window)
{
    SDL_Window *child;
    CHECK_WINDOW_MAGIC(window, false);

    if (!(window->flags & SDL_WINDOW_HIDDEN)) {
        return true;
    }

    // If the parent is hidden, set the flag to restore this when the parent is shown
    if (window->parent && (window->parent->flags & SDL_WINDOW_HIDDEN)) {
        window->restore_on_show = true;
        return true;
    }

    if (_this->ShowWindow) {
        _this->ShowWindow(_this, window);
    } else {
        SDL_SetMouseFocus(window);
        SDL_SetKeyboardFocus(window);
    }
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_SHOWN, 0, 0);

    // Restore child windows
    for (child = window->first_child; child; child = child->next_sibling) {
        if (!child->restore_on_show && (child->flags & SDL_WINDOW_HIDDEN)) {
            break;
        }
        SDL_ShowWindow(child);
        child->restore_on_show = false;
    }
    return true;
}

bool SDL_HideWindow(SDL_Window *window)
{
    SDL_Window *child;
    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->restore_on_show = false;
        return true;
    }

    // Hide all child windows
    for (child = window->first_child; child; child = child->next_sibling) {
        if (child->flags & SDL_WINDOW_HIDDEN) {
            break;
        }
        SDL_HideWindow(child);
        child->restore_on_show = true;
    }

    // Store the flags for restoration later.
    const SDL_WindowFlags pending_mask = (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_KEYBOARD_GRABBED | SDL_WINDOW_MOUSE_GRABBED);
    window->pending_flags = (window->flags & pending_mask);

    window->is_hiding = true;
    if (_this->HideWindow) {
        _this->HideWindow(_this, window);
    } else {
        SDL_SetMouseFocus(NULL);
        SDL_SetKeyboardFocus(NULL);
    }
    window->is_hiding = false;
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_HIDDEN, 0, 0);
    return true;
}

bool SDL_RaiseWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        return true;
    }
    if (_this->RaiseWindow) {
        _this->RaiseWindow(_this, window);
    }
    return true;
}

bool SDL_MaximizeWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->MaximizeWindow) {
        return SDL_Unsupported();
    }

    if (!(window->flags & SDL_WINDOW_RESIZABLE)) {
        return SDL_SetError("A window without the 'SDL_WINDOW_RESIZABLE' flag can't be maximized");
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags |= SDL_WINDOW_MAXIMIZED;
        return true;
    }

    _this->MaximizeWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

bool SDL_MinimizeWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->MinimizeWindow) {
        return SDL_Unsupported();
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags |= SDL_WINDOW_MINIMIZED;
        return true;
    }

    _this->MinimizeWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

bool SDL_RestoreWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->RestoreWindow) {
        return SDL_Unsupported();
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        window->pending_flags &= ~(SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED);
        return true;
    }

    _this->RestoreWindow(_this, window);
    SDL_SyncIfRequired(window);
    return true;
}

bool SDL_SetWindowFullscreen(SDL_Window *window, bool fullscreen)
{
    bool result;

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        if (fullscreen) {
            window->pending_flags |= SDL_WINDOW_FULLSCREEN;
        } else {
            window->pending_flags &= ~SDL_WINDOW_FULLSCREEN;
        }
        return true;
    }

    if (fullscreen) {
        // Set the current fullscreen mode to the desired mode
        SDL_copyp(&window->current_fullscreen_mode, &window->requested_fullscreen_mode);
    }

    result = SDL_UpdateFullscreenMode(window, fullscreen ? SDL_FULLSCREEN_OP_ENTER : SDL_FULLSCREEN_OP_LEAVE, true);

    if (!fullscreen || !result) {
        // Clear the current fullscreen mode.
        SDL_zero(window->current_fullscreen_mode);
    }

    if (result) {
        SDL_SyncIfRequired(window);
    }

    return result;
}

bool SDL_SyncWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (_this->SyncWindow) {
        return _this->SyncWindow(_this, window);
    } else {
        return true;
    }
}

bool SDL_SetWindowOpacity(SDL_Window *window, float opacity)
{
    bool result;

    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->SetWindowOpacity) {
        return SDL_Unsupported();
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    } else if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    result = _this->SetWindowOpacity(_this, window, opacity);
    if (result) {
        window->opacity = opacity;
    }

    return result;
}

float SDL_GetWindowOpacity(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, -1.0f);

    return window->opacity;
}

bool SDL_SetWindowParent(SDL_Window *window, SDL_Window *parent)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (parent) {
        CHECK_WINDOW_MAGIC(parent, false);
        CHECK_WINDOW_NOT_POPUP(parent, false);
    }

    if (window == parent) {
        return SDL_SetError("Cannot set the parent of a window to itself.");
    }

    if (!_this->SetWindowParent) {
        return SDL_Unsupported();
    }

    if (window->flags & SDL_WINDOW_MODAL) {
        return SDL_SetError("Modal windows cannot change parents; call SDL_SetWindowModal() to clear modal status first.");
    }

    if (window->parent == parent) {
        return true;
    }

    const bool ret = _this->SetWindowParent(_this, window, parent);
    SDL_UpdateWindowHierarchy(window, ret ? parent : NULL);

    return ret;
}

bool SDL_SetWindowModal(SDL_Window *window, bool modal)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (!_this->SetWindowModal) {
        return SDL_Unsupported();
    }

    if (modal) {
        if (!window->parent) {
            return SDL_SetError("Window must have a parent to enable the modal state; use SDL_SetWindowParent() to set the parent first.");
        }
        window->flags |= SDL_WINDOW_MODAL;
    } else if (window->flags & SDL_WINDOW_MODAL) {
        window->flags &= ~SDL_WINDOW_MODAL;
    } else {
        return true; // Already not modal, so nothing to do.
    }

    if (window->flags & SDL_WINDOW_HIDDEN) {
        return true;
    }

    return _this->SetWindowModal(_this, window, modal);
}

bool SDL_ShouldRelinquishPopupFocus(SDL_Window *window, SDL_Window **new_focus)
{
    SDL_Window *focus = window->parent;
    bool set_focus = !!(window->flags & SDL_WINDOW_INPUT_FOCUS);

    // Find the highest level window, up to the toplevel parent, that isn't being hidden or destroyed, and can grab the keyboard focus.
    while (SDL_WINDOW_IS_POPUP(focus) && ((focus->flags & SDL_WINDOW_NOT_FOCUSABLE) || focus->is_hiding || focus->is_destroying)) {
        focus = focus->parent;

        // If some window in the chain currently had focus, set it to the new lowest-level window.
        if (!set_focus) {
            set_focus = !!(focus->flags & SDL_WINDOW_INPUT_FOCUS);
        }
    }

    *new_focus = focus;
    return set_focus;
}

bool SDL_ShouldFocusPopup(SDL_Window *window)
{
    SDL_Window *toplevel_parent;
    for (toplevel_parent = window->parent; SDL_WINDOW_IS_POPUP(toplevel_parent); toplevel_parent = toplevel_parent->parent) {
    }

    SDL_Window *current_focus = toplevel_parent->keyboard_focus;
    bool found_higher_focus = false;

    /* Traverse the window tree from the currently focused window to the toplevel parent and see if we encounter
     * the new focus request. If the new window is found, a higher-level window already has focus.
     */
    SDL_Window *w;
    for (w = current_focus; w != toplevel_parent; w = w->parent) {
        if (w == window) {
            found_higher_focus = true;
            break;
        }
    }

    return !found_higher_focus || w == toplevel_parent;
}

bool SDL_SetWindowFocusable(SDL_Window *window, bool focusable)
{
    CHECK_WINDOW_MAGIC(window, false);

    const bool want = (focusable != false); // normalize the flag.
    const bool have = !(window->flags & SDL_WINDOW_NOT_FOCUSABLE);
    if ((want != have) && (_this->SetWindowFocusable)) {
        if (want) {
            window->flags &= ~SDL_WINDOW_NOT_FOCUSABLE;
        } else {
            window->flags |= SDL_WINDOW_NOT_FOCUSABLE;
        }
        if (!_this->SetWindowFocusable(_this, window, want)) {
            return false;
        }
    }

    return true;
}

bool SDL_SetWindowFillDocument(SDL_Window *window, bool fill)
{
    CHECK_WINDOW_MAGIC(window, false);

    const bool want = (fill != false); // normalize the flag.
    const bool have = ((window->flags & SDL_WINDOW_FILL_DOCUMENT) != 0);
    if ((want != have) && (_this->SetWindowFillDocument)) {
        if (!_this->SetWindowFillDocument(_this, window, want)) {
            return false;
        } else if (want) {
            window->flags |= SDL_WINDOW_FILL_DOCUMENT;
        } else {
            window->flags &= ~SDL_WINDOW_FILL_DOCUMENT;
        }
    }

    return true;
}

void SDL_UpdateWindowGrab(SDL_Window *window)
{
    bool keyboard_grabbed, mouse_grabbed;

    if (window->flags & SDL_WINDOW_INPUT_FOCUS) {
        if (SDL_GetMouse()->relative_mode || (window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
            mouse_grabbed = true;
        } else {
            mouse_grabbed = false;
        }

        if (window->flags & SDL_WINDOW_KEYBOARD_GRABBED) {
            keyboard_grabbed = true;
        } else {
            keyboard_grabbed = false;
        }
    } else {
        mouse_grabbed = false;
        keyboard_grabbed = false;
    }

    if (mouse_grabbed || keyboard_grabbed) {
        if (_this->grabbed_window && (_this->grabbed_window != window)) {
            // stealing a grab from another window!
            _this->grabbed_window->flags &= ~(SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED);
            if (_this->SetWindowMouseGrab) {
                _this->SetWindowMouseGrab(_this, _this->grabbed_window, false);
            }
            if (_this->SetWindowKeyboardGrab) {
                _this->SetWindowKeyboardGrab(_this, _this->grabbed_window, false);
            }
        }
        _this->grabbed_window = window;
    } else if (_this->grabbed_window == window) {
        _this->grabbed_window = NULL; // ungrabbing input.
    }

    if (_this->SetWindowMouseGrab) {
        if (!_this->SetWindowMouseGrab(_this, window, mouse_grabbed)) {
            window->flags &= ~SDL_WINDOW_MOUSE_GRABBED;
        }
    }
    if (_this->SetWindowKeyboardGrab) {
        if (!_this->SetWindowKeyboardGrab(_this, window, keyboard_grabbed)) {
            window->flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
        }
    }

    if (_this->grabbed_window && !(_this->grabbed_window->flags & (SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED))) {
        _this->grabbed_window = NULL;
    }
}

bool SDL_SetWindowKeyboardGrab(SDL_Window *window, bool grabbed)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        if (grabbed) {
            window->pending_flags |= SDL_WINDOW_KEYBOARD_GRABBED;
        } else {
            window->pending_flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
        }
        return true;
    }

    if (!!grabbed == !!(window->flags & SDL_WINDOW_KEYBOARD_GRABBED)) {
        return true;
    }
    if (grabbed) {
        window->flags |= SDL_WINDOW_KEYBOARD_GRABBED;
    } else {
        window->flags &= ~SDL_WINDOW_KEYBOARD_GRABBED;
    }
    SDL_UpdateWindowGrab(window);

    if (grabbed && !(window->flags & SDL_WINDOW_KEYBOARD_GRABBED)) {
        return false;
    }
    return true;
}

bool SDL_SetWindowMouseGrab(SDL_Window *window, bool grabbed)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (window->flags & SDL_WINDOW_HIDDEN) {
        if (grabbed) {
            window->pending_flags |= SDL_WINDOW_MOUSE_GRABBED;
        } else {
            window->pending_flags &= ~SDL_WINDOW_MOUSE_GRABBED;
        }
        return true;
    }

    if (!!grabbed == !!(window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
        return true;
    }
    if (grabbed) {
        window->flags |= SDL_WINDOW_MOUSE_GRABBED;
    } else {
        window->flags &= ~SDL_WINDOW_MOUSE_GRABBED;
    }
    SDL_UpdateWindowGrab(window);

    if (grabbed && !(window->flags & SDL_WINDOW_MOUSE_GRABBED)) {
        return false;
    }
    return true;
}

bool SDL_GetWindowKeyboardGrab(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);
    return window == _this->grabbed_window && (_this->grabbed_window->flags & SDL_WINDOW_KEYBOARD_GRABBED);
}

bool SDL_GetWindowMouseGrab(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);
    return window == _this->grabbed_window && (_this->grabbed_window->flags & SDL_WINDOW_MOUSE_GRABBED);
}

SDL_Window *SDL_GetGrabbedWindow(void)
{
    if (_this && _this->grabbed_window &&
        (_this->grabbed_window->flags & (SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_KEYBOARD_GRABBED)) != 0) {
        return _this->grabbed_window;
    } else {
        return NULL;
    }
}

bool SDL_SetWindowMouseRect(SDL_Window *window, const SDL_Rect *rect)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->SetWindowMouseRect) {
        return SDL_Unsupported();
    }

    SDL_Rect zero = { 0, 0, 0, 0 };
    if (!rect) {
        rect = &zero;
    }

    if (SDL_memcmp(&window->mouse_rect, rect, sizeof(*rect)) == 0) {
        return true;
    }
    SDL_memcpy(&window->mouse_rect, rect, sizeof(*rect));

    return _this->SetWindowMouseRect(_this, window);
}

const SDL_Rect *SDL_GetWindowMouseRect(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, NULL);

    if (SDL_RectEmpty(&window->mouse_rect)) {
        return NULL;
    } else {
        return &window->mouse_rect;
    }
}

bool SDL_SetWindowRelativeMouseMode(SDL_Window *window, bool enabled)
{
    CHECK_WINDOW_MAGIC(window, false);

    /* If the app toggles relative mode directly, it probably shouldn't
     * also be emulating it using repeated mouse warps, so disable
     * mouse warp emulation by default.
     */
    SDL_DisableMouseWarpEmulation();

    if (enabled == SDL_GetWindowRelativeMouseMode(window)) {
        return true;
    }

    if (enabled) {
        window->flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
    } else {
        window->flags &= ~SDL_WINDOW_MOUSE_RELATIVE_MODE;
    }

    if (!SDL_UpdateRelativeMouseMode()) {
        if (enabled) {
            window->flags &= ~SDL_WINDOW_MOUSE_RELATIVE_MODE;
        } else {
            window->flags |= SDL_WINDOW_MOUSE_RELATIVE_MODE;
        }
        return false;
    }
    return true;
}

bool SDL_GetWindowRelativeMouseMode(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (window->flags & SDL_WINDOW_MOUSE_RELATIVE_MODE) {
        return true;
    } else {
        return false;
    }
}

bool SDL_FlashWindow(SDL_Window *window, SDL_FlashOperation operation)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (_this->FlashWindow) {
        return _this->FlashWindow(_this, window, operation);
    }

    return SDL_Unsupported();
}

bool SDL_SetWindowProgressState(SDL_Window *window, SDL_ProgressState state)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    CHECK_PARAM(state < SDL_PROGRESS_STATE_NONE || state > SDL_PROGRESS_STATE_ERROR) {
        return SDL_InvalidParamError("state");
    }

    window->progress_state = state;

    if (_this->ApplyWindowProgress) {
        if (!_this->ApplyWindowProgress(_this, window)) {
            return false;
        }
    }

    return true;
}

SDL_ProgressState SDL_GetWindowProgressState(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, SDL_PROGRESS_STATE_INVALID);
    CHECK_WINDOW_NOT_POPUP(window, SDL_PROGRESS_STATE_INVALID);

    return window->progress_state;
}

bool SDL_SetWindowProgressValue(SDL_Window *window, float value)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    value = SDL_clamp(value, 0.0f, 1.f);

    window->progress_value = value;

    if (_this->ApplyWindowProgress) {
        if (!_this->ApplyWindowProgress(_this, window)) {
            return false;
        }
    }

    return true;
}

float SDL_GetWindowProgressValue(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, -1.0f);
    CHECK_WINDOW_NOT_POPUP(window, -1.0f);

    return window->progress_value;
}

void SDL_OnWindowShown(SDL_Window *window)
{
    // Set window state if we have pending window flags cached
    ApplyWindowFlags(window, window->pending_flags);
    window->pending_flags = 0;
}

void SDL_OnWindowHidden(SDL_Window *window)
{
    /* Store the maximized and fullscreen flags for restoration later, in case
     * this was initiated by the window manager due to the window being unmapped
     * when minimized.
     */
    window->pending_flags |= (window->flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_MAXIMIZED));

    // The window is already hidden at this point, so just change the mode back if necessary.
    SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, false);
}

void SDL_OnWindowDisplayChanged(SDL_Window *window)
{
    // Don't run this if a fullscreen change was made in an event watcher callback in response to a display changed event.
    if (window->update_fullscreen_on_display_changed && (window->flags & SDL_WINDOW_FULLSCREEN)) {
        const bool auto_mode_switch = SDL_GetHintBoolean(SDL_HINT_VIDEO_MATCH_EXCLUSIVE_MODE_ON_MOVE, true);

        if (auto_mode_switch && (window->requested_fullscreen_mode.w != 0 || window->requested_fullscreen_mode.h != 0)) {
            SDL_DisplayID displayID = SDL_GetDisplayForWindowPosition(window);
            bool include_high_density_modes = false;

            if (window->requested_fullscreen_mode.pixel_density > 1.0f) {
                include_high_density_modes = true;
            }
            const bool found_match = SDL_GetClosestFullscreenDisplayMode(displayID, window->requested_fullscreen_mode.w, window->requested_fullscreen_mode.h,
                                                                         window->requested_fullscreen_mode.refresh_rate, include_high_density_modes, &window->current_fullscreen_mode);

            // If a mode without matching dimensions was not found, just go to fullscreen desktop.
            if (!found_match ||
                window->requested_fullscreen_mode.w != window->current_fullscreen_mode.w ||
                window->requested_fullscreen_mode.h != window->current_fullscreen_mode.h) {
                SDL_zero(window->current_fullscreen_mode);
            }
        } else {
            SDL_zero(window->current_fullscreen_mode);
        }

        if (SDL_WINDOW_FULLSCREEN_VISIBLE(window)) {
            SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_UPDATE, true);
        }
    }

    SDL_CheckWindowPixelSizeChanged(window);
}

void SDL_OnWindowMoved(SDL_Window *window)
{
    SDL_CheckWindowDisplayChanged(window);
}

void SDL_OnWindowResized(SDL_Window *window)
{
    SDL_CheckWindowDisplayChanged(window);
    SDL_CheckWindowPixelSizeChanged(window);
    SDL_CheckWindowSafeAreaChanged(window);

    if ((window->flags & SDL_WINDOW_TRANSPARENT) && _this->UpdateWindowShape) {
        SDL_Surface *surface = (SDL_Surface *)SDL_GetPointerProperty(window->props, SDL_PROP_WINDOW_SHAPE_POINTER, NULL);
        if (surface) {
            _this->UpdateWindowShape(_this, window, surface);
        }
    }
}

void SDL_CheckWindowPixelSizeChanged(SDL_Window *window)
{
    int pixel_w = 0, pixel_h = 0;

    SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h);
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED, pixel_w, pixel_h);

    SDL_CheckWindowDisplayScaleChanged(window);
}

void SDL_OnWindowPixelSizeChanged(SDL_Window *window)
{
    window->surface_valid = false;
}

void SDL_OnWindowLiveResizeUpdate(SDL_Window *window)
{
    if (SDL_HasMainCallbacks()) {
        SDL_IterateMainCallbacks(false);
    } else {
        // Send an expose event so the application can redraw
        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_EXPOSED, 1, 0);
    }

    SDL_PumpEventMaintenance();
}

static void SDL_CheckWindowSafeAreaChanged(SDL_Window *window)
{
    SDL_Rect rect;

    rect.x = window->safe_inset_left;
    rect.y = window->safe_inset_top;
    rect.w = window->w - (window->safe_inset_right + window->safe_inset_left);
    rect.h = window->h - (window->safe_inset_top + window->safe_inset_bottom);
    if (SDL_memcmp(&rect, &window->safe_rect, sizeof(rect)) != 0) {
        SDL_copyp(&window->safe_rect, &rect);
        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_SAFE_AREA_CHANGED, 0, 0);
    }
}

void SDL_SetWindowSafeAreaInsets(SDL_Window *window, int left, int right, int top, int bottom)
{
    window->safe_inset_left = left;
    window->safe_inset_right = right;
    window->safe_inset_top = top;
    window->safe_inset_bottom = bottom;
    SDL_CheckWindowSafeAreaChanged(window);
}

bool SDL_GetWindowSafeArea(SDL_Window *window, SDL_Rect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_WINDOW_MAGIC(window, false);

    if (rect) {
        if (SDL_RectEmpty(&window->safe_rect)) {
            rect->w = window->w;
            rect->h = window->h;
        } else {
            SDL_copyp(rect, &window->safe_rect);
        }
    }
    return true;
}

void SDL_OnWindowMinimized(SDL_Window *window)
{
    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, false);
    }
}

void SDL_OnWindowMaximized(SDL_Window *window)
{
}

void SDL_OnWindowRestored(SDL_Window *window)
{
    /*
     * FIXME: Is this fine to just remove this, or should it be preserved just
     * for the fullscreen case? In principle it seems like just hiding/showing
     * windows shouldn't affect the stacking order; maybe the right fix is to
     * re-decouple OnWindowShown and OnWindowRestored.
     */
    // SDL_RaiseWindow(window);

    if (window->flags & SDL_WINDOW_FULLSCREEN) {
        SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_ENTER, false);
    }
}

void SDL_OnWindowEnter(SDL_Window *window)
{
    if (_this->OnWindowEnter) {
        _this->OnWindowEnter(_this, window);
    }
    SDL_UpdateRelativeMouseMode();
}

void SDL_OnWindowLeave(SDL_Window *window)
{
    SDL_UpdateRelativeMouseMode();
}

void SDL_OnWindowFocusGained(SDL_Window *window)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    if (mouse && mouse->relative_mode) {
        SDL_SetMouseFocus(window);
    }

    SDL_UpdateWindowGrab(window);
}

static bool SDL_ShouldMinimizeOnFocusLoss(SDL_Window *window)
{
    const char *hint;

    if (!(window->flags & SDL_WINDOW_FULLSCREEN) || window->is_destroying) {
        return false;
    }

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
    if (SDL_strcmp(_this->name, "cocoa") == 0) { // don't do this for X11, etc
        if (Cocoa_IsShowingModalDialog(window)) {
            return false;  // modal system dialogs can live over fullscreen windows, don't minimize.
        } else if (Cocoa_IsWindowInFullscreenSpace(window)) {
            return false;
        }
    }
#endif

#ifdef SDL_PLATFORM_ANDROID
    {
        extern bool Android_JNI_ShouldMinimizeOnFocusLoss(void);
        if (!Android_JNI_ShouldMinimizeOnFocusLoss()) {
            return false;
        }
    }
#endif

    // Real fullscreen windows should minimize on focus loss so the desktop video mode is restored
    hint = SDL_GetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS);
    if (!hint || !*hint || SDL_strcasecmp(hint, "auto") == 0) {
        if (window->fullscreen_exclusive && !SDL_ModeSwitchingEmulated(_this)) {
            return true;
        } else {
            return false;
        }
    }
    return SDL_GetHintBoolean(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, false);
}

void SDL_OnWindowFocusLost(SDL_Window *window)
{
    SDL_UpdateWindowGrab(window);

    if (SDL_ShouldMinimizeOnFocusLoss(window)) {
        SDL_MinimizeWindow(window);
    }
}

SDL_Window *SDL_GetToplevelForKeyboardFocus(void)
{
    SDL_Window *focus = SDL_GetKeyboardFocus();

    if (focus) {
        // Get the toplevel parent window.
        while (focus->parent) {
            focus = focus->parent;
        }
    }

    return focus;
}

bool SDL_AddWindowRenderer(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_Renderer **renderers = (SDL_Renderer **)SDL_realloc(window->renderers, (window->num_renderers + 1) * sizeof(*renderers));
    if (!renderers) {
        return false;
    }

    window->renderers = renderers;
    window->renderers[window->num_renderers++] = renderer;

    SDL_SetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_RENDERER_POINTER, renderer);
    return true;
}

void SDL_RemoveWindowRenderer(SDL_Window *window, SDL_Renderer *renderer)
{
    for (int i = 0; i < window->num_renderers; ++i) {
        if (window->renderers[i] == renderer) {
            if (i < (window->num_renderers - 1)) {
                SDL_memmove(&window->renderers[i], &window->renderers[i + 1], (window->num_renderers - i - 1) * sizeof(window->renderers[i]));
            }
            --window->num_renderers;
            break;
        }
    }

    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    if (SDL_GetPointerProperty(props, SDL_PROP_WINDOW_RENDERER_POINTER, NULL) == renderer) {
        SDL_ClearProperty(props, SDL_PROP_WINDOW_RENDERER_POINTER);
    }
}

void SDL_DestroyWindow(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window,);

    window->is_destroying = true;

    // Destroy any child windows of this window
    while (window->first_child) {
        SDL_DestroyWindow(window->first_child);
    }

    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_DESTROYED, 0, 0);

    SDL_DestroyWindowSurface(window);

    SDL_Renderer *renderer = SDL_GetRenderer(window);
    if (renderer) {
        SDL_DestroyRendererWithoutFreeing(renderer);
    }

    // Restore video mode, etc.
    SDL_UpdateFullscreenMode(window, SDL_FULLSCREEN_OP_LEAVE, true);
    if (!(window->flags & SDL_WINDOW_EXTERNAL)) {
        SDL_HideWindow(window);
    }

    SDL_DestroyProperties(window->text_input_props);

    SDL_DestroyWindowTexture(_this, window);
    SDL_DestroyProperties(window->props);

    /* Clear the modal status, but don't unset the parent just yet, as it
     * may be needed later in the destruction process if a backend needs
     * to update the input focus.
     */
    if (_this->SetWindowModal && (window->flags & SDL_WINDOW_MODAL)) {
        _this->SetWindowModal(_this, window, false);
    }

    // Make sure the destroyed window isn't referenced by any display as a fullscreen window.
    for (int i = 0; i < _this->num_displays; ++i) {
        if (_this->displays[i]->fullscreen_window == window) {
            _this->displays[i]->fullscreen_window = NULL;
        }
    }

    // Make sure this window no longer has focus
    if (SDL_GetKeyboardFocus() == window) {
        SDL_SetKeyboardFocus(NULL);
    }
    if ((window->flags & SDL_WINDOW_MOUSE_CAPTURE)) {
        SDL_UpdateMouseCapture(true);
    }
    if (SDL_GetMouseFocus() == window) {
        SDL_SetMouseFocus(NULL);
    }

    // Make no context current if this is the current context window
    if (window->flags & SDL_WINDOW_OPENGL) {
        if (_this->current_glwin == window) {
            SDL_GL_MakeCurrent(window, NULL);
        }
    }

    if (_this->DestroyWindow) {
        _this->DestroyWindow(_this, window);
    }

    // Unload the graphics libraries after the window is destroyed, which may clean up EGL surfaces
    if (window->flags & SDL_WINDOW_OPENGL) {
        SDL_GL_UnloadLibrary();
    }
    if (window->flags & SDL_WINDOW_VULKAN) {
        SDL_Vulkan_UnloadLibrary();
    }

    if (_this->grabbed_window == window) {
        _this->grabbed_window = NULL; // ungrabbing input.
    }

    if (_this->current_glwin == window) {
        _this->current_glwin = NULL;
    }

    SDL_CompareAndSwapAtomicPointer(&_this->wakeup_window, window, NULL);

    // Now invalidate magic
    SDL_SetObjectValid(window, SDL_OBJECT_TYPE_WINDOW, false);

    // Free memory associated with the window
    SDL_free(window->title);
    SDL_DestroySurface(window->icon);

    // Unlink the window from its siblings.
    SDL_UpdateWindowHierarchy(window, NULL);

    // Unlink the window from the global window list
    if (window->next) {
        window->next->prev = window->prev;
    }
    if (window->prev) {
        window->prev->next = window->next;
    } else {
        _this->windows = window->next;
    }

    SDL_free(window->renderers);
    SDL_free(window);

#ifdef SDL_VIDEO_DRIVER_UIKIT
    SDL_UpdateLifecycleObserver();
#endif
}

bool SDL_ScreenSaverEnabled(void)
{
    if (!_this) {
        return true;
    }
    return !_this->suspend_screensaver;
}

bool SDL_EnableScreenSaver(void)
{
    if (!_this) {
        return SDL_UninitializedVideo();
    }
    if (!_this->suspend_screensaver) {
        return true;
    }
    _this->suspend_screensaver = false;
    if (_this->SuspendScreenSaver) {
        return _this->SuspendScreenSaver(_this);
    }

    return SDL_Unsupported();
}

bool SDL_DisableScreenSaver(void)
{
    if (!_this) {
        return SDL_UninitializedVideo();
    }
    if (_this->suspend_screensaver) {
        return true;
    }
    _this->suspend_screensaver = true;
    if (_this->SuspendScreenSaver) {
        return _this->SuspendScreenSaver(_this);
    }

    return SDL_Unsupported();
}

void SDL_QuitVideo(void)
{
    int i;

    if (!_this) {
        return;
    }

    _this->is_quitting = true;

    // Halt event processing before doing anything else
    SDL_QuitTouch();
    SDL_QuitMouse();
    SDL_QuitKeyboard();
    SDL_QuitSubSystem(SDL_INIT_EVENTS);

    SDL_EnableScreenSaver();

    // Clean up the system video
    while (_this->windows) {
        SDL_DestroyWindow(_this->windows);
    }

    if (_this->gl_config.driver_loaded && _this->GL_UnloadLibrary) {
        _this->GL_UnloadLibrary(_this);
        _this->gl_config.driver_loaded = 0;
    }
    if (_this->vulkan_config.loader_loaded && _this->Vulkan_UnloadLibrary) {
        _this->Vulkan_UnloadLibrary(_this);
        _this->vulkan_config.loader_loaded = 0;
    }

    _this->VideoQuit(_this);

    for (i = _this->num_displays; i--; ) {
        SDL_VideoDisplay *display = _this->displays[i];
        SDL_DelVideoDisplay(display->id, false);
    }

    SDL_assert(_this->num_displays == 0);
    SDL_free(_this->displays);
    _this->displays = NULL;

    SDL_CancelClipboardData(0);

    if (_this->primary_selection_text) {
        SDL_free(_this->primary_selection_text);
        _this->primary_selection_text = NULL;
    }
    _this->free(_this);
    _this = NULL;

    // This needs to happen after the video subsystem has removed pen data
    SDL_QuitPen();
}

int SDL_GetMessageBoxCount(void)
{
    return SDL_GetAtomicInt(&SDL_messagebox_count);
}

bool SDL_ShowMessageBox(const SDL_MessageBoxData *messageboxdata, int *buttonID)
{
    int dummybutton;
    bool result = false;
    bool show_cursor_prev;
    SDL_Window *current_window;
    SDL_MessageBoxData mbdata;

    CHECK_PARAM(!messageboxdata) {
        return SDL_InvalidParamError("messageboxdata");
    }
    CHECK_PARAM(messageboxdata->numbuttons < 0) {
        return SDL_SetError("Invalid number of buttons");
    }

    // in case either the title or message was a pointer from SDL_GetError(), make a copy
    // now, as we'll likely overwrite error state in here.
    bool titleisstack = false, msgisstack = false;
    char *titlecpy = NULL;
    char *msgcpy = NULL;
    if (messageboxdata->title) {
        const size_t slen = SDL_strlen(messageboxdata->title) + 1;
        titlecpy = SDL_small_alloc(char, slen, &titleisstack);
        if (!titlecpy) {
            return false;
        }
        SDL_memcpy(titlecpy, messageboxdata->title, slen);
    }

    if (messageboxdata->message) {
        const size_t slen = SDL_strlen(messageboxdata->message) + 1;
        msgcpy = SDL_small_alloc(char, slen, &msgisstack);
        if (!msgcpy) {
            SDL_small_free(titlecpy, titleisstack);
            return false;
        }
        SDL_memcpy(msgcpy, messageboxdata->message, slen);
    }

    (void)SDL_AtomicIncRef(&SDL_messagebox_count);

    current_window = SDL_GetKeyboardFocus();
    SDL_UpdateMouseCapture(false);
    SDL_SetRelativeMouseMode(false);
    show_cursor_prev = SDL_CursorVisible();
    SDL_ShowCursor();
    SDL_ResetKeyboard();

    if (!buttonID) {
        buttonID = &dummybutton;
    }

    SDL_memcpy(&mbdata, messageboxdata, sizeof(*messageboxdata));
    mbdata.title = titlecpy;
    if (!mbdata.title) {
        mbdata.title = "";
    }
    mbdata.message = msgcpy;
    if (!mbdata.message) {
        mbdata.message = "";
    }
    messageboxdata = &mbdata;

    SDL_ClearError();

    if (_this && _this->ShowMessageBox) {
        result = _this->ShowMessageBox(_this, messageboxdata, buttonID);
    } else {
        // It's completely fine to call this function before video is initialized
        const char *driver_name = SDL_GetHint(SDL_HINT_VIDEO_DRIVER);
        if (driver_name && *driver_name != 0) {
            const char *driver_attempt = driver_name;
            while (driver_attempt && (*driver_attempt != 0) && !result) {
                const char *driver_attempt_end = SDL_strchr(driver_attempt, ',');
                size_t driver_attempt_len = (driver_attempt_end) ? (driver_attempt_end - driver_attempt)
                                                                     : SDL_strlen(driver_attempt);
                for (int i = 0; bootstrap[i]; ++i) {
                    if (bootstrap[i]->ShowMessageBox && (driver_attempt_len == SDL_strlen(bootstrap[i]->name)) &&
                        (SDL_strncasecmp(bootstrap[i]->name, driver_attempt, driver_attempt_len) == 0)) {
                        if (bootstrap[i]->ShowMessageBox(messageboxdata, buttonID)) {
                            result = true;
                        }
                        break;
                    }
                }

                driver_attempt = (driver_attempt_end) ? (driver_attempt_end + 1) : NULL;
            }
        } else {
            for (int i = 0; bootstrap[i]; ++i) {
                if (bootstrap[i]->ShowMessageBox && bootstrap[i]->ShowMessageBox(messageboxdata, buttonID)) {
                    result = true;
                    break;
                }
            }
        }
    }

    if (!result) {
        const char *error = SDL_GetError();

        if (!*error) {
            SDL_SetError("No message system available");
        }
    } else {
        SDL_ClearError();
    }

    (void)SDL_AtomicDecRef(&SDL_messagebox_count);

    if (current_window) {
        SDL_RaiseWindow(current_window);
    }

    if (!show_cursor_prev) {
        SDL_HideCursor();
    }
    SDL_UpdateRelativeMouseMode();
    SDL_UpdateMouseCapture(false);

    SDL_small_free(msgcpy, msgisstack);
    SDL_small_free(titlecpy, titleisstack);

    return result;
}

bool SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags flags, const char *title, const char *message, SDL_Window *window)
{
#if defined(SDL_PLATFORM_3DS)
    errorConf errCnf;
    bool hasGpuRight;

    // If the video subsystem has not been initialised, set up graphics temporarily
    hasGpuRight = gspHasGpuRight();
    if (!hasGpuRight)
        gfxInitDefault();

    errorInit(&errCnf, ERROR_TEXT_WORD_WRAP, CFG_LANGUAGE_EN);
    errorText(&errCnf, message);
    errorDisp(&errCnf);

    if (!hasGpuRight)
        gfxExit();

    return true;
#else
    SDL_MessageBoxData data;
    SDL_MessageBoxButtonData button;

    SDL_zero(data);
    data.flags = flags;
    data.title = title;
    data.message = message;
    data.numbuttons = 1;
    data.buttons = &button;
    data.window = window;

    SDL_zero(button);
    button.flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    button.flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    button.text = "OK";

    return SDL_ShowMessageBox(&data, NULL);
#endif
}

bool SDL_ShouldAllowTopmost(void)
{
    return SDL_GetHintBoolean(SDL_HINT_WINDOW_ALLOW_TOPMOST, true);
}

bool SDL_ShowWindowSystemMenu(SDL_Window *window, int x, int y)
{
    CHECK_WINDOW_MAGIC(window, false);
    CHECK_WINDOW_NOT_POPUP(window, false);

    if (_this->ShowWindowSystemMenu) {
        _this->ShowWindowSystemMenu(window, x, y);
        return true;
    }

    return SDL_Unsupported();
}

bool SDL_SetWindowHitTest(SDL_Window *window, SDL_HitTest callback, void *callback_data)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->SetWindowHitTest) {
        return SDL_Unsupported();
    }

    window->hit_test = callback;
    window->hit_test_data = callback_data;

    return _this->SetWindowHitTest(window, callback != NULL);
}

bool SDL_SetWindowShape(SDL_Window *window, SDL_Surface *shape)
{
    SDL_PropertiesID props;
    SDL_Surface *surface;

    CHECK_WINDOW_MAGIC(window, false);

    if (!(window->flags & SDL_WINDOW_TRANSPARENT)) {
        return SDL_SetError("Window must be created with SDL_WINDOW_TRANSPARENT");
    }

    props = SDL_GetWindowProperties(window);
    if (!props) {
        return false;
    }

    if (shape) {
        surface = SDL_ConvertSurface(shape, SDL_PIXELFORMAT_ARGB32);
        if (!surface) {
            return false;
        }
    } else {
        surface = NULL;
    }

    if (!SDL_SetSurfaceProperty(props, SDL_PROP_WINDOW_SHAPE_POINTER, surface)) {
        return false;
    }

    if (_this->UpdateWindowShape) {
        if (!_this->UpdateWindowShape(_this, window, surface)) {
            return false;
        }
    }
    return true;
}

/*
 * Functions used by iOS application delegates
 */
void SDL_OnApplicationWillTerminate(void)
{
    SDL_SendAppEvent(SDL_EVENT_TERMINATING);
}

void SDL_OnApplicationDidReceiveMemoryWarning(void)
{
    SDL_SendAppEvent(SDL_EVENT_LOW_MEMORY);
}

void SDL_OnApplicationWillEnterBackground(void)
{
    if (_this) {
        SDL_Window *window;
        for (window = _this->windows; window; window = window->next) {
            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MINIMIZED, 0, 0);
        }
        SDL_SetKeyboardFocus(NULL);
    }
    SDL_SendAppEvent(SDL_EVENT_WILL_ENTER_BACKGROUND);
}

void SDL_OnApplicationDidEnterBackground(void)
{
    SDL_SendAppEvent(SDL_EVENT_DID_ENTER_BACKGROUND);
}

void SDL_OnApplicationWillEnterForeground(void)
{
    SDL_SendAppEvent(SDL_EVENT_WILL_ENTER_FOREGROUND);
}

void SDL_OnApplicationDidEnterForeground(void)
{
    SDL_SendAppEvent(SDL_EVENT_DID_ENTER_FOREGROUND);

    if (_this) {
        SDL_Window *window;
        for (window = _this->windows; window; window = window->next) {
            SDL_SetKeyboardFocus(window);
            SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESTORED, 0, 0);
        }
    }
}

SDL_MetalView SDL_Metal_CreateView(SDL_Window *window)
{
    CHECK_WINDOW_MAGIC(window, NULL);

    if (!_this->Metal_CreateView) {
        SDL_Unsupported();
        return NULL;
    }

    if (!(window->flags & SDL_WINDOW_METAL)) {
        // No problem, we can convert to Metal
        if (window->flags & SDL_WINDOW_OPENGL) {
            window->flags &= ~SDL_WINDOW_OPENGL;
            SDL_GL_UnloadLibrary();
        }
        if (window->flags & SDL_WINDOW_VULKAN) {
            window->flags &= ~SDL_WINDOW_VULKAN;
            SDL_Vulkan_UnloadLibrary();
        }
        window->flags |= SDL_WINDOW_METAL;
    }

    return _this->Metal_CreateView(_this, window);
}

void SDL_Metal_DestroyView(SDL_MetalView view)
{
    if (_this && view && _this->Metal_DestroyView) {
        _this->Metal_DestroyView(_this, view);
    }
}

void *SDL_Metal_GetLayer(SDL_MetalView view)
{
    if (_this && _this->Metal_GetLayer) {
        CHECK_PARAM(!view) {
            SDL_InvalidParamError("view");
            return NULL;
        }
        return _this->Metal_GetLayer(_this, view);
    } else {
        SDL_SetError("Metal is not supported.");
        return NULL;
    }
}
