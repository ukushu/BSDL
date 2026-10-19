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

#define CHECK_DISPLAY_MAGIC(display, result)                            \
    CHECK_PARAM(!display) {                                             \
        return result;                                                  \
    }                                                                   \

#if defined(SDL_PLATFORM_MACOS) && defined(SDL_VIDEO_DRIVER_COCOA)
// Support for macOS fullscreen spaces, etc.
extern bool Cocoa_IsWindowInFullscreenSpace(SDL_Window *window);
extern bool Cocoa_SetWindowFullscreenSpace(SDL_Window *window, bool state, bool blocking);
extern bool Cocoa_IsShowingModalDialog(SDL_Window *window);
#endif

#ifdef SDL_VIDEO_DRIVER_UIKIT
extern void SDL_UpdateLifecycleObserver(void);
#endif


// Convenience functions for reading driver flags
static bool SDL_ModeSwitchingEmulated(SDL_VideoDevice *_this)
{
    if (_this->device_caps & VIDEO_DEVICE_CAPS_MODE_SWITCHING_EMULATED) {
        return true;
    }
    return false;
}

static bool IsFullscreenOnly(SDL_VideoDevice *_this)
{
    return !!(_this->device_caps & VIDEO_DEVICE_CAPS_FULLSCREEN_ONLY);
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

static SDL_VideoDevice *SDL_CreateVideoDevice(const char **driver_name, VideoBootStrap **selected_bootstrap)
{
    SDL_VideoDevice *video;
    int i;

    // Select the proper video driver
    video = NULL;
#ifdef SDL_PLATFORM_LINUX
    // https://github.com/libsdl-org/SDL/issues/12247
    if (SDL_IsUbuntuTouch()) {
        *driver_name = "wayland";
    }
#endif
    if (!*driver_name) {
        *driver_name = SDL_GetHint(SDL_HINT_VIDEO_DRIVER);
    }
    if (*driver_name && **driver_name != 0) {
        const char *driver_attempt = *driver_name;
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
                        *selected_bootstrap = bootstrap[i];
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
                *selected_bootstrap = bootstrap[i];
                break;
            }
        }
    }
    return video;
}

/*
 * Initialize the video and event subsystems -- determine native pixel format
 */
bool SDL_InitVideo(const char *driver_name)
{
    SDL_VideoDevice *video;
    VideoBootStrap *video_bootstrap = NULL;
    bool init_events = false;
    bool init_keyboard = false;
    bool init_mouse = false;
    bool init_touch = false;
    bool init_pen = false;

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

    video = SDL_CreateVideoDevice(&driver_name, &video_bootstrap);
    if (video) {
        SDL_DebugLogBackend("video", video_bootstrap->name);
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
    _this->name = video_bootstrap->name;
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

    SDL_InitVideoSyncHint();

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

    displayID = SDL_GetDisplayForWindowInternal(window);
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

static void SDL_InitializeWindowState(SDL_Window *window, SDL_PropertiesID props, SDL_Window *parent, SDL_WindowFlags *flags, SDL_DisplayID displayID, bool external_graphics_context)
{
    SDL_SetObjectValid(window, SDL_OBJECT_TYPE_WINDOW, true);
    window->id = SDL_GetNextObjectID();
    window->floating.x = window->windowed.x = window->x;
    window->floating.y = window->windowed.y = window->y;
    window->floating.w = window->windowed.w = window->w;
    window->floating.h = window->windowed.h = window->h;
    window->pending_displayID = displayID;

    SDL_VideoDisplay *display = SDL_GetVideoDisplayForWindow(window);
    if (display) {
        SDL_SetWindowHDRProperties(window, &display->HDR, false);
    }

    if ((*flags & SDL_WINDOW_FULLSCREEN) || IsFullscreenOnly(_this)) {
        SDL_Rect bounds;

        SDL_GetDisplayBounds(display ? display->id : SDL_GetPrimaryDisplay(), &bounds);
        window->x = bounds.x;
        window->y = bounds.y;
        window->w = bounds.w;
        window->h = bounds.h;
        window->pending_flags |= SDL_WINDOW_FULLSCREEN;
        *flags |= SDL_WINDOW_FULLSCREEN;
    }

    window->flags = ((*flags & CREATE_FLAGS) | SDL_WINDOW_HIDDEN);
    window->display_scale = 1.0f;
    window->opacity = 1.0f;
    window->next = _this->windows;
    window->is_destroying = false;
    window->displayID = SDL_GetDisplayForWindowInternal(window);
    window->external_graphics_context = external_graphics_context;
    window->constrain_popup = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_CONSTRAIN_POPUP_BOOLEAN, true);

    if (!_this->SetWindowFillDocument) {
        window->flags &= ~SDL_WINDOW_FILL_DOCUMENT;  // not an error, just unsupported here, so remove the flag.
    }

    if (_this->windows) {
        _this->windows->prev = window;
    }
    _this->windows = window;

    SDL_UpdateWindowHierarchy(window, parent);
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
    window->x = x;
    window->y = y;
    window->w = w;
    window->h = h;
    window->undefined_x = undefined_x;
    window->undefined_y = undefined_y;
    SDL_InitializeWindowState(window, props, parent, &flags, displayID, external_graphics_context);

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

static void SDL_ApplyRecreatedWindowSettings(SDL_Window *window)
{
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
}

static bool SDL_RecreateWindowGraphics(SDL_Window *window, SDL_WindowFlags flags, bool *loaded_opengl, bool *loaded_vulkan)
{
    bool need_gl_unload = false;
    bool need_gl_load = false;
    bool need_vulkan_unload = false;
    bool need_vulkan_load = false;

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
        *loaded_opengl = true;
    }

    if (need_vulkan_load) {
        if (!SDL_Vulkan_LoadLibrary(NULL)) {
            return false;
        }
        *loaded_vulkan = true;
    }

    return true;
}

bool SDL_RecreateWindow(SDL_Window *window, SDL_WindowFlags flags)
{
    bool loaded_opengl = false;
    bool loaded_vulkan = false;
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

    if (!SDL_RecreateWindowGraphics(window, flags, &loaded_opengl, &loaded_vulkan)) {
        return false;
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

    SDL_ApplyRecreatedWindowSettings(window);

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

bool SDL_ShowMessageBoxWithFallback(const SDL_MessageBoxData *messageboxdata, int *buttonID)
{
    bool result = false;

    if (_this && _this->ShowMessageBox) {
        return _this->ShowMessageBox(_this, messageboxdata, buttonID);
    }

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
    return result;
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
