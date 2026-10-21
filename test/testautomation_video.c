/**
 * Video test suite
 */
#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>
#include <SDL3/SDL_vulkan.h>
#include "../src/gpu/SDL_sysgpu.h"
#include "testautomation_suites.h"

static SDL_realloc_func video_original_realloc;
static bool video_fail_realloc;
static bool video_realloc_failed;
static size_t video_fail_realloc_size;
static char video_test_pipeline_storage;
static int video_released_pipeline_count;

static void *SDLCALL video_test_realloc(void *ptr, size_t size)
{
    if (video_fail_realloc && size == video_fail_realloc_size) {
        video_fail_realloc = false;
        video_realloc_failed = true;
        return NULL;
    }
    return video_original_realloc(ptr, size);
}

static SDL_GPUGraphicsPipeline *SDLCALL video_create_test_graphics_pipeline(
    SDL_GPURenderer *driverData,
    const SDL_GPUGraphicsPipelineCreateInfo *createinfo)
{
    (void)driverData;
    (void)createinfo;
    return (SDL_GPUGraphicsPipeline *)&video_test_pipeline_storage;
}

static void SDLCALL video_release_test_graphics_pipeline(
    SDL_GPURenderer *driverData,
    SDL_GPUGraphicsPipeline *pipeline)
{
    (void)driverData;
    (void)pipeline;
    ++video_released_pipeline_count;
}

/* Private helpers */

static bool VideoSupportsWindowPositioning(void)
{
    const char *video_driver = SDL_GetCurrentVideoDriver();

	if (SDL_strcmp(video_driver, "android") == 0) {
		return false;
	}
	if (SDL_strcmp(video_driver, "emscripten") == 0) {
		return false;
	}
	if (SDL_strcmp(video_driver, "uikit") == 0) {
		return false;
	}
	if (SDL_strcmp(video_driver, "wayland") == 0) {
		return false;
	}
	return true;
}

static bool VideoSupportsWindowResizing(void)
{
    const char *video_driver = SDL_GetCurrentVideoDriver();

	if (SDL_strcmp(video_driver, "android") == 0) {
		return false;
	}
	if (SDL_strcmp(video_driver, "emscripten") == 0) {
		return false;
	}
	if (SDL_strcmp(video_driver, "uikit") == 0) {
		return false;
	}
	return true;
}

static bool VideoSupportsWindowMinimizing(void)
{
    const char *video_driver = SDL_GetCurrentVideoDriver();

	if (SDL_strcmp(video_driver, "android") == 0) {
		// Technically it's supported, but minimizing backgrounds the application and stops processing
		return false;
	}
	return true;
}

/**
 * Create a test window
 */
static SDL_Window *createVideoSuiteTestWindow(const char *title)
{
    SDL_Window *window;
    SDL_Window **windows;
    SDL_Event event;
    int w, h;
    int count;
    SDL_WindowFlags flags;
    bool needs_renderer = false;
    bool needs_events_pumped = false;

    /* Standard window */
    w = SDLTest_RandomIntegerInRange(320, 1024);
    h = SDLTest_RandomIntegerInRange(320, 768);
    flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS;

    window = SDL_CreateWindow(title, w, h, flags);
    SDLTest_AssertPass("Call to SDL_CreateWindow('Title',%d,%d,%" SDL_PRIu64 ")", w, h, flags);
    SDLTest_AssertCheck(window != NULL, "Validate that returned window is not NULL");

    /* Check the window is available in the window list */
    windows = SDL_GetWindows(&count);
    SDLTest_AssertCheck(windows != NULL, "Validate that returned window list is not NULL");
    SDLTest_AssertCheck(windows[0] == window, "Validate that the window is first in the window list");
    SDL_free(windows);

    /* Wayland and XWayland windows require that a frame be presented before they are fully mapped and visible onscreen.
     * This is required for the mouse/keyboard grab tests to pass.
     */
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        needs_renderer = true;
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        /* Try to detect if the x11 driver is running under XWayland */
        const char *session_type = SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "XDG_SESSION_TYPE");
        if (session_type && SDL_strcasecmp(session_type, "wayland") == 0) {
            needs_renderer = true;
        }

        /* X11 needs the initial events pumped, or it can erroneously deliver old configuration events at a later time. */
        needs_events_pumped = true;
    }

    if (needs_renderer) {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            /* Some desktops don't display the window immediately after presentation,
             * so delay to give the window time to actually appear on the desktop.
             */
            SDL_Delay(100);
        } else {
            SDLTest_Log("Unable to create a renderer, some tests may fail on Wayland/XWayland");
        }
    }

    if (needs_events_pumped) {
        /* Pump out the event queue */
        while (SDL_PollEvent(&event)) {
        }
    }

    return window;
}

/**
 * Destroy test window
 */
static void destroyVideoSuiteTestWindow(SDL_Window *window)
{
    if (window != NULL) {
        SDL_Renderer *renderer = SDL_GetRenderer(window);
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        SDL_DestroyWindow(window);
        window = NULL;
        SDLTest_AssertPass("Call to SDL_DestroyWindow()");
    }
}

/* Test case functions */

/**
 * Enable and disable screensaver while checking state
 */
static int SDLCALL video_enableDisableScreensaver(void *arg)
{
    bool initialResult;
    bool result;

    /* Get current state and proceed according to current state */
    initialResult = SDL_ScreenSaverEnabled();
    SDLTest_AssertPass("Call to SDL_ScreenSaverEnabled()");
    if (initialResult == true) {

        /* Currently enabled: disable first, then enable again */

        /* Disable screensaver and check */
        SDL_DisableScreenSaver();
        SDLTest_AssertPass("Call to SDL_DisableScreenSaver()");
        result = SDL_ScreenSaverEnabled();
        SDLTest_AssertPass("Call to SDL_ScreenSaverEnabled()");
        SDLTest_AssertCheck(result == false, "Verify result from SDL_ScreenSaverEnabled, expected: %i, got: %i", false, result);

        /* Enable screensaver and check */
        SDL_EnableScreenSaver();
        SDLTest_AssertPass("Call to SDL_EnableScreenSaver()");
        result = SDL_ScreenSaverEnabled();
        SDLTest_AssertPass("Call to SDL_ScreenSaverEnabled()");
        SDLTest_AssertCheck(result == true, "Verify result from SDL_ScreenSaverEnabled, expected: %i, got: %i", true, result);

    } else {

        /* Currently disabled: enable first, then disable again */

        /* Enable screensaver and check */
        SDL_EnableScreenSaver();
        SDLTest_AssertPass("Call to SDL_EnableScreenSaver()");
        result = SDL_ScreenSaverEnabled();
        SDLTest_AssertPass("Call to SDL_ScreenSaverEnabled()");
        SDLTest_AssertCheck(result == true, "Verify result from SDL_ScreenSaverEnabled, expected: %i, got: %i", true, result);

        /* Disable screensaver and check */
        SDL_DisableScreenSaver();
        SDLTest_AssertPass("Call to SDL_DisableScreenSaver()");
        result = SDL_ScreenSaverEnabled();
        SDLTest_AssertPass("Call to SDL_ScreenSaverEnabled()");
        SDLTest_AssertCheck(result == false, "Verify result from SDL_ScreenSaverEnabled, expected: %i, got: %i", false, result);
    }

    return TEST_COMPLETED;
}

/**
 * Tests the functionality of the SDL_CreateWindow function using different sizes
 */
static int SDLCALL video_createWindowVariousSizes(void *arg)
{
    SDL_Window *window;
    const char *title = "video_createWindowVariousSizes Test Window";
    int w = 0, h = 0;
    int wVariation, hVariation;

    for (wVariation = 0; wVariation < 3; wVariation++) {
        for (hVariation = 0; hVariation < 3; hVariation++) {
            switch (wVariation) {
            case 0:
                /* Width of 1 */
                w = 1;
                break;
            case 1:
                /* Random "normal" width */
                w = SDLTest_RandomIntegerInRange(320, 1920);
                break;
            case 2:
                /* Random "large" width */
                w = SDLTest_RandomIntegerInRange(2048, 4095);
                break;
            }

            switch (hVariation) {
            case 0:
                /* Height of 1 */
                h = 1;
                break;
            case 1:
                /* Random "normal" height */
                h = SDLTest_RandomIntegerInRange(320, 1080);
                break;
            case 2:
                /* Random "large" height */
                h = SDLTest_RandomIntegerInRange(2048, 4095);
                break;
            }

            window = SDL_CreateWindow(title, w, h, 0);
            SDLTest_AssertPass("Call to SDL_CreateWindow('Title',%d,%d,SHOWN)", w, h);
            SDLTest_AssertCheck(window != NULL, "Validate that returned window is not NULL");

            /* Clean up */
            destroyVideoSuiteTestWindow(window);
        }
    }

    return TEST_COMPLETED;
}

/**
 * Tests the functionality of the SDL_CreateWindow function using different flags
 */
static int SDLCALL video_createWindowVariousFlags(void *arg)
{
    SDL_Window *window;
    const char *title = "video_createWindowVariousFlags Test Window";
    int w, h;
    int fVariation;
    SDL_WindowFlags flags;

    /* Standard window */
    w = SDLTest_RandomIntegerInRange(320, 1024);
    h = SDLTest_RandomIntegerInRange(320, 768);

    for (fVariation = 1; fVariation < 14; fVariation++) {
        switch (fVariation) {
        default:
        case 1:
            flags = SDL_WINDOW_FULLSCREEN;
            /* Skip - blanks screen; comment out next line to run test */
            continue;
        case 2:
            flags = SDL_WINDOW_OPENGL;
            /* Skip - not every video driver supports OpenGL; comment out next line to run test */
            continue;
        case 3:
            flags = 0;
            break;
        case 4:
            flags = SDL_WINDOW_HIDDEN;
            break;
        case 5:
            flags = SDL_WINDOW_BORDERLESS;
            break;
        case 6:
            flags = SDL_WINDOW_RESIZABLE;
            break;
        case 7:
            flags = SDL_WINDOW_MINIMIZED;
            break;
        case 8:
            flags = SDL_WINDOW_MAXIMIZED;
            break;
        case 9:
            flags = SDL_WINDOW_MOUSE_GRABBED;
            break;
        case 10:
            flags = SDL_WINDOW_INPUT_FOCUS;
            break;
        case 11:
            flags = SDL_WINDOW_MOUSE_FOCUS;
            break;
        case 12:
            flags = SDL_WINDOW_EXTERNAL;
            break;
        case 13:
            flags = SDL_WINDOW_KEYBOARD_GRABBED;
            break;
        }

        window = SDL_CreateWindow(title, w, h, flags);
        SDLTest_AssertPass("Call to SDL_CreateWindow('Title',%d,%d,%" SDL_PRIu64 ")", w, h, flags);
        SDLTest_AssertCheck(window != NULL, "Validate that returned window is not NULL");

        /* Clean up */
        destroyVideoSuiteTestWindow(window);
    }

    return TEST_COMPLETED;
}

static int SDLCALL video_rejectsModalWithoutParent(void *arg)
{
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_Window *window;

    if (!SDLTest_AssertCheck(props != 0, "Create window properties: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }
    if (!SDLTest_AssertCheck(SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MODAL_BOOLEAN, true),
                             "Set modal property: %s", SDL_GetError())) {
        SDL_DestroyProperties(props);
        return TEST_ABORTED;
    }
    window = SDL_CreateWindowWithProperties(props);
    SDLTest_AssertCheck(window == NULL, "Modal window without a parent is rejected");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), "Modal windows must specify a parent window") == 0,
                        "Modal window error is preserved: %s", SDL_GetError());

    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_DestroyProperties(props);
    return TEST_COMPLETED;
}

static int SDLCALL video_windowHierarchy(void *arg)
{
    SDL_Window *parent = SDL_CreateWindow("hierarchy parent", 64, 64, SDL_WINDOW_HIDDEN);
    SDL_Window *child = SDL_CreateWindow("hierarchy child", 64, 64, SDL_WINDOW_HIDDEN);
    bool parent_set;

    if (!SDLTest_AssertCheck(parent && child, "Create hierarchy windows: %s", SDL_GetError())) {
        destroyVideoSuiteTestWindow(child);
        destroyVideoSuiteTestWindow(parent);
        return TEST_ABORTED;
    }

    SDLTest_AssertCheck(SDL_GetWindowParent(child) == NULL, "New window has no parent");
    SDLTest_AssertCheck(!SDL_SetWindowParent(child, child), "A window cannot parent itself");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), "Cannot set the parent of a window to itself.") == 0,
                        "Self-parenting error is preserved: %s", SDL_GetError());

    parent_set = SDL_SetWindowParent(child, parent);
    SDLTest_AssertCheck(SDL_GetWindowParent(child) == (parent_set ? parent : NULL),
                        "Parent relationship matches backend result");

    if (parent_set && SDL_SetWindowModal(child, true)) {
        SDLTest_AssertCheck((SDL_GetWindowFlags(child) & SDL_WINDOW_MODAL) != 0, "Modal flag is set");
        SDLTest_AssertCheck(!SDL_SetWindowParent(child, NULL), "Modal window cannot be reparented");
        SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), "Modal windows cannot change parents; call SDL_SetWindowModal() to clear modal status first.") == 0,
                            "Modal reparent error is preserved: %s", SDL_GetError());
        SDLTest_AssertCheck(SDL_GetWindowParent(child) == parent, "Failed reparent keeps parent relationship");
        SDL_ClearError();
        SDLTest_AssertCheck(SDL_SetWindowModal(child, false), "Clear modal state: %s", SDL_GetError());
        SDLTest_AssertCheck((SDL_GetWindowFlags(child) & SDL_WINDOW_MODAL) == 0, "Modal flag is cleared");
    }

    if (parent_set && SDL_GetWindowParent(child) == parent && SDL_SetWindowParent(child, NULL)) {
        SDLTest_AssertCheck(SDL_GetWindowParent(child) == NULL, "Unparented window has no parent");
    }

    destroyVideoSuiteTestWindow(child);
    destroyVideoSuiteTestWindow(parent);
    return TEST_COMPLETED;
}

static int SDLCALL video_rejectsConflictingGraphicsFlags(void *arg)
{
    const SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN;
    SDL_Window *window = SDL_CreateWindow("video_rejectsConflictingGraphicsFlags", 64, 64, flags);
    char expected_error[128];

    SDL_snprintf(expected_error, sizeof(expected_error),
                 "Conflicting window graphics flags specified: 0x%.8x", (unsigned int)flags);
    SDLTest_AssertCheck(window == NULL, "Conflicting graphics flags are rejected");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), expected_error) == 0,
                        "Conflicting graphics flags error is preserved: %s", SDL_GetError());
    if (window) {
        SDL_DestroyWindow(window);
    }
    return TEST_COMPLETED;
}

static int SDLCALL video_testFullscreenEnterLeave(void *arg)
{
    SDL_Window *window;
    SDL_WindowFlags flags;
    SDL_DisplayID display;
    SDL_DisplayMode original_mode;
    const SDL_DisplayMode *current_mode;

    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") != 0) {
        return TEST_SKIPPED;
    }

    window = createVideoSuiteTestWindow("video_testFullscreenEnterLeave");
    if (!window) {
        return TEST_ABORTED;
    }
    display = SDL_GetDisplayForWindow(window);
    current_mode = SDL_GetCurrentDisplayMode(display);
    if (!SDLTest_AssertCheck(display != 0 && current_mode != NULL, "Get window display and mode: %s", SDL_GetError())) {
        destroyVideoSuiteTestWindow(window);
        return TEST_ABORTED;
    }
    original_mode = *current_mode;

    if (!SDL_SetWindowFullscreen(window, true)) {
        SDLTest_AssertCheck(false, "Enter fullscreen: %s", SDL_GetError());
        destroyVideoSuiteTestWindow(window);
        return TEST_ABORTED;
    }
    if (!SDLTest_AssertCheck(SDL_SyncWindow(window), "Sync fullscreen window: %s", SDL_GetError())) {
        destroyVideoSuiteTestWindow(window);
        return TEST_ABORTED;
    }
    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertCheck((flags & SDL_WINDOW_FULLSCREEN) != 0, "Fullscreen flag is set after entering");
    SDLTest_AssertCheck(SDL_GetDisplayForWindow(window) == display, "Fullscreen window remains on selected display");

    if (!SDL_SetWindowFullscreen(window, false)) {
        SDLTest_AssertCheck(false, "Leave fullscreen: %s", SDL_GetError());
        destroyVideoSuiteTestWindow(window);
        return TEST_ABORTED;
    }
    if (!SDLTest_AssertCheck(SDL_SyncWindow(window), "Sync window after leaving fullscreen: %s", SDL_GetError())) {
        destroyVideoSuiteTestWindow(window);
        return TEST_ABORTED;
    }
    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertCheck((flags & SDL_WINDOW_FULLSCREEN) == 0, "Fullscreen flag is cleared after leaving");
    SDLTest_AssertCheck(SDL_GetDisplayForWindow(window) == display, "Window remains on selected display after leaving fullscreen");
    current_mode = SDL_GetCurrentDisplayMode(display);
    SDLTest_AssertCheck(current_mode != NULL && current_mode->displayID == original_mode.displayID &&
                        current_mode->w == original_mode.w && current_mode->h == original_mode.h &&
                        current_mode->format == original_mode.format &&
                        current_mode->pixel_density == original_mode.pixel_density &&
                        current_mode->refresh_rate == original_mode.refresh_rate &&
                        current_mode->refresh_rate_numerator == original_mode.refresh_rate_numerator &&
                        current_mode->refresh_rate_denominator == original_mode.refresh_rate_denominator,
                        "Display mode is restored after leaving fullscreen");

    destroyVideoSuiteTestWindow(window);
    return TEST_COMPLETED;
}

/**
 * Tests the functionality of the SDL_GetWindowFlags function
 */
static int SDLCALL video_getWindowFlags(void *arg)
{
    SDL_Window *window;
    const char *title = "video_getWindowFlags Test Window";
    SDL_WindowFlags flags;
    SDL_WindowFlags actualFlags;

    /* Reliable flag set always set in test window */
    flags = 0;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (window != NULL) {
        actualFlags = SDL_GetWindowFlags(window);
        SDLTest_AssertPass("Call to SDL_GetWindowFlags()");
        SDLTest_AssertCheck((flags & actualFlags) == flags, "Verify returned value has flags %" SDL_PRIu64 " set, got: %" SDL_PRIu64, flags, actualFlags);
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    return TEST_COMPLETED;
}

/**
 * Tests the functionality of the SDL_GetFullscreenDisplayModes function
 */
static int SDLCALL video_getFullscreenDisplayModes(void *arg)
{
    SDL_DisplayID *displays;
    SDL_DisplayMode **modes;
    int count;
    int i;

    /* Get number of displays */
    displays = SDL_GetDisplays(NULL);
    if (displays) {
        SDLTest_AssertPass("Call to SDL_GetDisplays()");

        /* Make call for each display */
        for (i = 0; displays[i]; ++i) {
            modes = SDL_GetFullscreenDisplayModes(displays[i], &count);
            SDLTest_AssertPass("Call to SDL_GetFullscreenDisplayModes(%" SDL_PRIu32 ")", displays[i]);
            SDLTest_AssertCheck(modes != NULL, "Validate returned value from function; expected != NULL; got: %p", modes);
            SDLTest_AssertCheck(count >= 0, "Validate number of modes; expected: >= 0; got: %d", count);
            SDL_free(modes);
        }
        SDL_free(displays);
    }

    return TEST_COMPLETED;
}

static int SDLCALL video_displayModeOrderingAndMatch(void *arg)
{
    int display_count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&display_count);

    if (!SDLTest_AssertCheck(displays != NULL && display_count > 0, "Enumerate displays: %s", SDL_GetError())) {
        SDL_free(displays);
        return TEST_ABORTED;
    }

    for (int d = 0; d < display_count; ++d) {
        int mode_count = 0;
        SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(displays[d], &mode_count);
        if (!SDLTest_AssertCheck(modes != NULL, "Enumerate modes for display %" SDL_PRIu32 ": %s", displays[d], SDL_GetError())) {
            SDL_free(displays);
            return TEST_ABORTED;
        }

        for (int i = 0; i < mode_count; ++i) {
            SDL_DisplayMode closest;
            SDLTest_AssertCheck(modes[i]->displayID == displays[d] && modes[i]->pixel_density > 0.0f,
                                "Mode %d belongs to its display and has positive density", i);
            if (i > 0) {
                SDLTest_AssertCheck(modes[i - 1]->w > modes[i]->w ||
                                    (modes[i - 1]->w == modes[i]->w && modes[i - 1]->h >= modes[i]->h),
                                    "Modes %d and %d are ordered by size", i - 1, i);
            }
            if (SDLTest_AssertCheck(SDL_GetClosestFullscreenDisplayMode(displays[d], modes[i]->w, modes[i]->h,
                                                                        modes[i]->refresh_rate, true, &closest),
                                    "Find a mode for listed size %dx%d", modes[i]->w, modes[i]->h)) {
                SDLTest_AssertCheck(closest.displayID == displays[d] && closest.w == modes[i]->w && closest.h == modes[i]->h,
                                    "Exact listed size maps to the same display and dimensions");
            }
        }
        SDL_free(modes);
    }
    SDL_free(displays);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestDisplayModeOrderingAndMatch = {
    video_displayModeOrderingAndMatch, "video_displayModeOrderingAndMatch", "Check sorted display modes and exact size matching", TEST_ENABLED
};

static int SDLCALL video_displayRegistry(void *arg)
{
    int count = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&count);

    if (!SDLTest_AssertCheck(displays != NULL && count > 0, "Enumerate video displays: %s", SDL_GetError())) {
        SDL_free(displays);
        return TEST_ABORTED;
    }

    SDLTest_AssertCheck(SDL_GetPrimaryDisplay() == displays[0], "Primary display is first in the display list");
    SDLTest_AssertCheck(displays[count] == 0, "Display list ends with a zero ID");

    for (int i = 0; i < count; ++i) {
        SDL_Rect bounds;
        const char *name = SDL_GetDisplayName(displays[i]);
        SDLTest_AssertCheck(name != NULL && *name != '\0', "Display %d has a name", i);
        SDLTest_AssertCheck(SDL_GetDisplayProperties(displays[i]) != 0, "Display %d has properties", i);
        if (SDLTest_AssertCheck(SDL_GetDisplayBounds(displays[i], &bounds), "Get display %d bounds: %s", i, SDL_GetError())) {
            SDLTest_AssertCheck(bounds.w > 0 && bounds.h > 0, "Display %d has positive bounds", i);
        }
        SDLTest_AssertCheck(SDL_GetDisplayContentScale(displays[i]) > 0.0f, "Display %d has positive content scale", i);
    }

    SDL_ClearError();
    SDLTest_AssertCheck(SDL_GetDisplayName(0) == NULL && *SDL_GetError() != '\0',
                        "Invalid display lookup fails with an error");
    SDL_free(displays);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestDisplayRegistry = {
    video_displayRegistry, "video_displayRegistry", "Check display registry queries and invalid display error", TEST_ENABLED
};

/**
 * Tests the functionality of the SDL_GetClosestFullscreenDisplayMode function against current resolution
 */
static int SDLCALL video_getClosestDisplayModeCurrentResolution(void *arg)
{
    SDL_DisplayID *displays;
    SDL_DisplayMode **modes;
    SDL_DisplayMode current;
    SDL_DisplayMode closest;
    int i, result, num_modes;

    /* Get number of displays */
    displays = SDL_GetDisplays(NULL);
    if (displays) {
        SDLTest_AssertPass("Call to SDL_GetDisplays()");

        /* Make calls for each display */
        for (i = 0; displays[i]; ++i) {
            SDLTest_Log("Testing against display: %" SDL_PRIu32, displays[i]);

            /* Get first display mode to get a sane resolution; this should always work */
            modes = SDL_GetFullscreenDisplayModes(displays[i], &num_modes);
            SDLTest_AssertPass("Call to SDL_GetDisplayModes()");
            SDLTest_Assert(modes != NULL, "Verify returned value is not NULL");
            if (num_modes > 0) {
                SDL_memcpy(&current, modes[0], sizeof(current));

                /* Make call */
                result = SDL_GetClosestFullscreenDisplayMode(displays[i], current.w, current.h, current.refresh_rate, (current.pixel_density > 1.0f), &closest);
                SDLTest_AssertPass("Call to SDL_GetClosestFullscreenDisplayMode(target=current)");
                SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                /* Check that one gets the current resolution back again */
                if (result == 0) {
                    SDLTest_AssertCheck(closest.w == current.w,
                                        "Verify returned width matches current width; expected: %d, got: %d",
                                        current.w, closest.w);
                    SDLTest_AssertCheck(closest.h == current.h,
                                        "Verify returned height matches current height; expected: %d, got: %d",
                                        current.h, closest.h);
                }
            }
            SDL_free(modes);
        }
        SDL_free(displays);
    }

    return TEST_COMPLETED;
}

/**
 * Tests the functionality of the SDL_GetClosestFullscreenDisplayMode function against random resolution
 */
static int SDLCALL video_getClosestDisplayModeRandomResolution(void *arg)
{
    SDL_DisplayID *displays;
    SDL_DisplayMode target;
    SDL_DisplayMode closest;
    int i;
    int variation;

    /* Get number of displays */
    displays = SDL_GetDisplays(NULL);
    if (displays) {
        SDLTest_AssertPass("Call to SDL_GetDisplays()");

        /* Make calls for each display */
        for (i = 0; displays[i]; ++i) {
            SDLTest_Log("Testing against display: %" SDL_PRIu32, displays[i]);

            for (variation = 0; variation < 16; variation++) {

                /* Set random constraints */
                SDL_zero(target);
                target.w = (variation & 1) ? SDLTest_RandomIntegerInRange(1, 4096) : 0;
                target.h = (variation & 2) ? SDLTest_RandomIntegerInRange(1, 4096) : 0;
                target.refresh_rate = (variation & 8) ? (float)SDLTest_RandomIntegerInRange(25, 120) : 0.0f;

                /* Make call; may or may not find anything, so don't validate any further */
                SDL_GetClosestFullscreenDisplayMode(displays[i], target.w, target.h, target.refresh_rate, false, &closest);
                SDLTest_AssertPass("Call to SDL_GetClosestFullscreenDisplayMode(target=random/variation%d)", variation);
            }
        }
        SDL_free(displays);
    }

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_GetWindowFullscreenMode
 *
 * \sa SDL_GetWindowFullscreenMode
 */
static int SDLCALL video_getWindowDisplayMode(void *arg)
{
    SDL_Window *window;
    const char *title = "video_getWindowDisplayMode Test Window";
    const SDL_DisplayMode *mode;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (window != NULL) {
        mode = SDL_GetWindowFullscreenMode(window);
        SDLTest_AssertPass("Call to SDL_GetWindowFullscreenMode()");
        SDLTest_AssertCheck(mode == NULL, "Validate result value; expected: NULL, got: %p", mode);
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    return TEST_COMPLETED;
}

/* Helper function that checks for an 'Invalid window' error */
static void checkInvalidWindowError(void)
{
    const char *invalidWindowError = "Invalid window";
    const char *lastError;

    lastError = SDL_GetError();
    SDLTest_AssertPass("SDL_GetError()");
    SDLTest_AssertCheck(lastError != NULL, "Verify error message is not NULL");
    if (lastError != NULL) {
        SDLTest_AssertCheck(SDL_strcmp(lastError, invalidWindowError) == 0,
                            "SDL_GetError(): expected message '%s', was message: '%s'",
                            invalidWindowError,
                            lastError);
        SDL_ClearError();
        SDLTest_AssertPass("Call to SDL_ClearError()");
    }
}

/**
 * Tests call to SDL_GetWindowFullscreenMode with invalid input
 *
 * \sa SDL_GetWindowFullscreenMode
 */
static int SDLCALL video_getWindowDisplayModeNegative(void *arg)
{
    const SDL_DisplayMode *mode;

    /* Call against invalid window */
    mode = SDL_GetWindowFullscreenMode(NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowFullscreenMode(window=NULL)");
    SDLTest_AssertCheck(mode == NULL, "Validate result value; expected: NULL, got: %p", mode);
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/* Helper for setting and checking the window mouse grab state */
static void setAndCheckWindowMouseGrabState(SDL_Window *window, bool desiredState)
{
    bool currentState;

    /* Set state */
    SDL_SetWindowMouseGrab(window, desiredState);
    SDLTest_AssertPass("Call to SDL_SetWindowMouseGrab(%s)", (desiredState == false) ? "false" : "true");

    /* Get and check state */
    currentState = SDL_GetWindowMouseGrab(window);
    SDLTest_AssertPass("Call to SDL_GetWindowMouseGrab()");
    SDLTest_AssertCheck(
        currentState == desiredState,
        "Validate returned state; expected: %s, got: %s",
        (desiredState == false) ? "false" : "true",
        (currentState == false) ? "false" : "true");

    if (desiredState) {
        SDLTest_AssertCheck(
            SDL_GetGrabbedWindow() == window,
            "Grabbed window should be to our window");
        SDLTest_AssertCheck(
            SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_GRABBED,
            "SDL_WINDOW_MOUSE_GRABBED should be set");
    } else {
        SDLTest_AssertCheck(
            !(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_GRABBED),
            "SDL_WINDOW_MOUSE_GRABBED should be unset");
    }
}

/* Helper for setting and checking the window keyboard grab state */
static void setAndCheckWindowKeyboardGrabState(SDL_Window *window, bool desiredState)
{
    bool currentState;

    /* Set state */
    SDL_SetWindowKeyboardGrab(window, desiredState);
    SDLTest_AssertPass("Call to SDL_SetWindowKeyboardGrab(%s)", (desiredState == false) ? "false" : "true");

    /* Get and check state */
    currentState = SDL_GetWindowKeyboardGrab(window);
    SDLTest_AssertPass("Call to SDL_GetWindowKeyboardGrab()");
    SDLTest_AssertCheck(
        currentState == desiredState,
        "Validate returned state; expected: %s, got: %s",
        (desiredState == false) ? "false" : "true",
        (currentState == false) ? "false" : "true");

    if (desiredState) {
        SDLTest_AssertCheck(
            SDL_GetGrabbedWindow() == window,
            "Grabbed window should be set to our window");
        SDLTest_AssertCheck(
            SDL_GetWindowFlags(window) & SDL_WINDOW_KEYBOARD_GRABBED,
            "SDL_WINDOW_KEYBOARD_GRABBED should be set");
    } else {
        SDLTest_AssertCheck(
            !(SDL_GetWindowFlags(window) & SDL_WINDOW_KEYBOARD_GRABBED),
            "SDL_WINDOW_KEYBOARD_GRABBED should be unset");
    }
}

/**
 * Tests keyboard and mouse grab support
 *
 * \sa SDL_GetWindowMouseGrab
 * \sa SDL_GetWindowKeyboardGrab
 * \sa SDL_SetWindowMouseGrab
 * \sa SDL_SetWindowKeyboardGrab
 */
static int SDLCALL video_getSetWindowGrab(void *arg)
{
    const char *title = "video_getSetWindowGrab Test Window";
    SDL_Window *window;
    bool originalMouseState, originalKeyboardState;
    bool hasFocusGained = false;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    /* Need to raise the window to have and SDL_EVENT_WINDOW_FOCUS_GAINED,
     * so that the window gets the flags SDL_WINDOW_INPUT_FOCUS,
     * so that it can be "grabbed" */
    SDL_RaiseWindow(window);

    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)) {
        int count = 0;
        SDL_Event evt;
        SDL_zero(evt);
        while (!hasFocusGained && count++ < 3) {
            while (SDL_PollEvent(&evt)) {
                if (evt.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
                    hasFocusGained = true;
                }
            }
        }
    } else {
        hasFocusGained = true;
    }

    SDLTest_AssertCheck(hasFocusGained == true, "Expectded window with focus");

    /* Get state */
    originalMouseState = SDL_GetWindowMouseGrab(window);
    SDLTest_AssertPass("Call to SDL_GetWindowMouseGrab()");
    originalKeyboardState = SDL_GetWindowKeyboardGrab(window);
    SDLTest_AssertPass("Call to SDL_GetWindowKeyboardGrab()");

    /* F */
    setAndCheckWindowKeyboardGrabState(window, false);
    setAndCheckWindowMouseGrabState(window, false);
    SDLTest_AssertCheck(SDL_GetGrabbedWindow() == NULL,
                        "Expected NULL grabbed window");

    /* F --> F */
    setAndCheckWindowMouseGrabState(window, false);
    setAndCheckWindowKeyboardGrabState(window, false);
    SDLTest_AssertCheck(SDL_GetGrabbedWindow() == NULL,
                        "Expected NULL grabbed window");

    /* F --> T */
    setAndCheckWindowMouseGrabState(window, true);
    setAndCheckWindowKeyboardGrabState(window, true);

    /* T --> T */
    setAndCheckWindowKeyboardGrabState(window, true);
    setAndCheckWindowMouseGrabState(window, true);

    /* M: T --> F */
    /* K: T --> T */
    setAndCheckWindowKeyboardGrabState(window, true);
    setAndCheckWindowMouseGrabState(window, false);

    /* M: F --> T */
    /* K: T --> F */
    setAndCheckWindowMouseGrabState(window, true);
    setAndCheckWindowKeyboardGrabState(window, false);

    /* M: T --> F */
    /* K: F --> F */
    setAndCheckWindowMouseGrabState(window, false);
    setAndCheckWindowKeyboardGrabState(window, false);
    SDLTest_AssertCheck(SDL_GetGrabbedWindow() == NULL,
                        "Expected NULL grabbed window");

    /* Negative tests */
    SDL_GetWindowMouseGrab(NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowMouseGrab(window=NULL)");
    checkInvalidWindowError();

    SDL_GetWindowKeyboardGrab(NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowKeyboardGrab(window=NULL)");
    checkInvalidWindowError();

    SDL_SetWindowMouseGrab(NULL, false);
    SDLTest_AssertPass("Call to SDL_SetWindowMouseGrab(window=NULL,false)");
    checkInvalidWindowError();

    SDL_SetWindowKeyboardGrab(NULL, false);
    SDLTest_AssertPass("Call to SDL_SetWindowKeyboardGrab(window=NULL,false)");
    checkInvalidWindowError();

    SDL_SetWindowMouseGrab(NULL, true);
    SDLTest_AssertPass("Call to SDL_SetWindowMouseGrab(window=NULL,true)");
    checkInvalidWindowError();

    SDL_SetWindowKeyboardGrab(NULL, true);
    SDLTest_AssertPass("Call to SDL_SetWindowKeyboardGrab(window=NULL,true)");
    checkInvalidWindowError();

    /* Restore state */
    setAndCheckWindowMouseGrabState(window, originalMouseState);
    setAndCheckWindowKeyboardGrabState(window, originalKeyboardState);

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_GetWindowID and SDL_GetWindowFromID
 *
 * \sa SDL_GetWindowID
 * \sa SDL_SetWindowFromID
 */
static int SDLCALL video_getWindowId(void *arg)
{
    const char *title = "video_getWindowId Test Window";
    SDL_Window *window;
    SDL_Window *result;
    Uint32 id, randomId;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    /* Get ID */
    id = SDL_GetWindowID(window);
    SDLTest_AssertPass("Call to SDL_GetWindowID()");

    /* Get window from ID */
    result = SDL_GetWindowFromID(id);
    SDLTest_AssertPass("Call to SDL_GetWindowID(%" SDL_PRIu32 ")", id);
    SDLTest_AssertCheck(result == window, "Verify result matches window pointer");

    /* Get window from random large ID, no result check */
    randomId = SDLTest_RandomIntegerInRange(UINT8_MAX, UINT16_MAX);
    result = SDL_GetWindowFromID(randomId);
    SDLTest_AssertPass("Call to SDL_GetWindowID(%" SDL_PRIu32 "/random_large)", randomId);

    /* Get window from 0 and Uint32 max ID, no result check */
    result = SDL_GetWindowFromID(0);
    SDLTest_AssertPass("Call to SDL_GetWindowID(0)");
    result = SDL_GetWindowFromID(UINT32_MAX);
    SDLTest_AssertPass("Call to SDL_GetWindowID(UINT32_MAX)");

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Get window from ID for closed window */
    result = SDL_GetWindowFromID(id);
    SDLTest_AssertPass("Call to SDL_GetWindowID(%" SDL_PRIu32 "/closed_window)", id);
    SDLTest_AssertCheck(result == NULL, "Verify result is NULL");

    /* Negative test */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    id = SDL_GetWindowID(NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowID(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_GetWindowPixelFormat
 *
 * \sa SDL_GetWindowPixelFormat
 */
static int SDLCALL video_getWindowPixelFormat(void *arg)
{
    const char *title = "video_getWindowPixelFormat Test Window";
    SDL_Window *window;
    SDL_PixelFormat format;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    /* Get format */
    format = SDL_GetWindowPixelFormat(window);
    SDLTest_AssertPass("Call to SDL_GetWindowPixelFormat()");
    SDLTest_AssertCheck(format != SDL_PIXELFORMAT_UNKNOWN, "Verify that returned format is valid; expected: != SDL_PIXELFORMAT_UNKNOWN, got: SDL_PIXELFORMAT_UNKNOWN");

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Negative test */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    format = SDL_GetWindowPixelFormat(NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowPixelFormat(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}


static bool getPositionFromEvent(int *x, int *y)
{
    bool ret = false;
    SDL_Event evt;
    SDL_zero(evt);
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_EVENT_WINDOW_MOVED) {
            *x = evt.window.data1;
            *y = evt.window.data2;
            ret = true;
        }
    }
    return ret;
}

static bool getSizeFromEvent(int *w, int *h)
{
    bool ret = false;
    SDL_Event evt;
    SDL_zero(evt);
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_EVENT_WINDOW_RESIZED) {
            *w = evt.window.data1;
            *h = evt.window.data2;
            ret = true;
        }
    }
    return ret;
}

/**
 * Tests call to SDL_GetWindowPosition and SDL_SetWindowPosition
 *
 * \sa SDL_GetWindowPosition
 * \sa SDL_SetWindowPosition
 */
static int SDLCALL video_getSetWindowPosition(void *arg)
{
    const char *title = "video_getSetWindowPosition Test Window";
    SDL_Window *window;
    SDL_WindowFlags flags;
    int result;
    int maxxVariation, maxyVariation;
    int xVariation, yVariation;
    int referenceX, referenceY;
    int currentX, currentY;
    int desiredX, desiredY;
    SDL_Rect display_bounds;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    /* Sanity check */
    SDL_GetWindowPosition(window, &currentX, &currentY);
    if (!SDL_SetWindowPosition(window, currentX, currentY)) {
        SDLTest_Log("Skipping window positioning tests: %s reports window positioning as unsupported", SDL_GetCurrentVideoDriver());
        goto null_tests;
    }

    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        /* The X11 server allows arbitrary window placement, but compositing
         * window managers such as GNOME and KDE force windows to be within
         * desktop bounds.
         */
        maxxVariation = 2;
        maxyVariation = 2;

        SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &display_bounds);
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "cocoa") == 0) {
        /* Platform doesn't allow windows with negative Y desktop bounds */
        maxxVariation = 4;
        maxyVariation = 3;

        SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &display_bounds);
    } else {
        /* Platform allows windows to be placed out of bounds */
        maxxVariation = 4;
        maxyVariation = 4;

        SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display_bounds);
    }

    for (xVariation = 0; xVariation < maxxVariation; xVariation++) {
        for (yVariation = 0; yVariation < maxyVariation; yVariation++) {
            switch (xVariation) {
            default:
            case 0:
                /* Zero X Position */
                desiredX = display_bounds.x > 0 ? display_bounds.x : 0;
                break;
            case 1:
                /* Random X position inside screen */
                desiredX = SDLTest_RandomIntegerInRange(display_bounds.x + 1, display_bounds.x + 100);
                break;
            case 2:
                /* Random X position outside screen (positive) */
                desiredX = SDLTest_RandomIntegerInRange(10000, 11000);
                break;
            case 3:
                /* Random X position outside screen (negative) */
                desiredX = SDLTest_RandomIntegerInRange(-1000, -100);
                break;
            }

            switch (yVariation) {
            default:
            case 0:
                /* Zero Y Position */
                desiredY = display_bounds.y > 0 ? display_bounds.y : 0;
                break;
            case 1:
                /* Random Y position inside screen */
                desiredY = SDLTest_RandomIntegerInRange(display_bounds.y + 1, display_bounds.y + 100);
                break;
            case 2:
                /* Random Y position outside screen (positive) */
                desiredY = SDLTest_RandomIntegerInRange(10000, 11000);
                break;
            case 3:
                /* Random Y position outside screen (negative) */
                desiredY = SDLTest_RandomIntegerInRange(-1000, -100);
                break;
            }

            /* Set position */
            SDL_SetWindowPosition(window, desiredX, desiredY);
            SDLTest_AssertPass("Call to SDL_SetWindowPosition(...,%d,%d)", desiredX, desiredY);

            result = SDL_SyncWindow(window);
            SDLTest_AssertPass("SDL_SyncWindow()");
            SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

            /* Get position */
            currentX = desiredX + 1;
            currentY = desiredY + 1;
            SDL_GetWindowPosition(window, &currentX, &currentY);
            SDLTest_AssertPass("Call to SDL_GetWindowPosition()");

            if (desiredX == currentX && desiredY == currentY) {
                SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position; expected: %d, got: %d", desiredX, currentX);
                SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position; expected: %d, got: %d", desiredY, currentY);
            } else {
                bool hasEvent;
                /* SDL_SetWindowPosition() and SDL_SetWindowSize() will make requests of the window manager and set the internal position and size,
                 * and then we get events signaling what actually happened, and they get passed on to the application if they're not what we expect. */
                currentX = desiredX + 1;
                currentY = desiredY + 1;
                hasEvent = getPositionFromEvent(&currentX, &currentY);
                SDLTest_AssertCheck(hasEvent == true, "Changing position was not honored by WM, checking present of SDL_EVENT_WINDOW_MOVED");
                if (hasEvent) {
                    SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position is the position from SDL event; expected: %d, got: %d", desiredX, currentX);
                    SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position is the position from SDL event; expected: %d, got: %d", desiredY, currentY);
                }
            }

            /* Get position X */
            currentX = desiredX + 1;
            SDL_GetWindowPosition(window, &currentX, NULL);
            SDLTest_AssertPass("Call to SDL_GetWindowPosition(&y=NULL)");
            SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position; expected: %d, got: %d", desiredX, currentX);

            /* Get position Y */
            currentY = desiredY + 1;
            SDL_GetWindowPosition(window, NULL, &currentY);
            SDLTest_AssertPass("Call to SDL_GetWindowPosition(&x=NULL)");
            SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position; expected: %d, got: %d", desiredY, currentY);
        }
    }

    /* Fullscreen test */
    desiredX = 100;
    desiredY = 100;
    SDL_SetWindowPosition(window, desiredX, desiredY);
    SDLTest_AssertPass("Call to SDL_SetWindowPosition(...,%d,%d)", desiredX, desiredY);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    /* Get position */
    currentX = desiredX + 1;
    currentY = desiredY + 1;
    SDL_GetWindowPosition(window, &currentX, &currentY);
    SDLTest_AssertPass("Call to SDL_GetWindowPosition()");

    if (desiredX == currentX && desiredY == currentY) {
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position; expected: %d, got: %d", desiredX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position; expected: %d, got: %d", desiredY, currentY);
    } else {
        bool hasEvent;
        /* SDL_SetWindowPosition() and SDL_SetWindowSize() will make requests of the window manager and set the internal position and size,
         * and then we get events signaling what actually happened, and they get passed on to the application if they're not what we expect. */
        currentX = desiredX + 1;
        currentY = desiredY + 1;
        hasEvent = getPositionFromEvent(&currentX, &currentY);
        SDLTest_AssertCheck(hasEvent == true, "Changing position was not honored by WM, checking present of SDL_EVENT_WINDOW_MOVED");
        if (hasEvent) {
            SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position is the position from SDL event; expected: %d, got: %d", desiredX, currentX);
            SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position is the position from SDL event; expected: %d, got: %d", desiredY, currentY);
        }
    }

    /* Test setting position while fullscreen */
    result = SDL_SetWindowFullscreen(window, true);
    SDLTest_AssertPass("SDL_SetWindowFullscreen()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    /* Verify that window is in fullscreen */
    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(flags & SDL_WINDOW_FULLSCREEN, "Verify the `SDL_WINDOW_FULLSCREEN` flag is set: %s", (flags & SDL_WINDOW_FULLSCREEN) ? "true" : "false");

    /* Set the fullscreen window position */
    desiredX = desiredX + 10;
    desiredY = desiredY + 10;
    SDL_SetWindowPosition(window, desiredX, desiredY);
    SDLTest_AssertPass("Call to SDL_SetWindowPosition(...,%d,%d)", desiredX, desiredY);

    result = SDL_SetWindowFullscreen(window, false);
    SDLTest_AssertPass("SDL_SetWindowFullscreen()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    /* Verify that window left fullscreen */
    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_FULLSCREEN), "Verify the `SDL_WINDOW_FULLSCREEN` flag is not set: %s", !(flags & SDL_WINDOW_FULLSCREEN) ? "true" : "false");

    /* Get position */
    currentX = desiredX + 1;
    currentY = desiredY + 1;
    SDL_GetWindowPosition(window, &currentX, &currentY);
    SDLTest_AssertPass("Call to SDL_GetWindowPosition()");

    if (desiredX == currentX && desiredY == currentY) {
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position; expected: %d, got: %d", desiredX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position; expected: %d, got: %d", desiredY, currentY);
    } else {
        bool hasEvent;
        /* SDL_SetWindowPosition() and SDL_SetWindowSize() will make requests of the window manager and set the internal position and size,
         * and then we get events signaling what actually happened, and they get passed on to the application if they're not what we expect. */
        currentX = desiredX + 1;
        currentY = desiredY + 1;
        hasEvent = getPositionFromEvent(&currentX, &currentY);
        SDLTest_AssertCheck(hasEvent == true, "Changing position was not honored by WM, checking present of SDL_EVENT_WINDOW_MOVED");
        if (hasEvent) {
            SDLTest_AssertCheck(desiredX == currentX, "Verify returned X position is the position from SDL event; expected: %d, got: %d", desiredX, currentX);
            SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y position is the position from SDL event; expected: %d, got: %d", desiredY, currentY);
        }
    }

null_tests:

    /* Dummy call with both pointers NULL */
    SDL_GetWindowPosition(window, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowPosition(&x=NULL,&y=NULL)");

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Set some 'magic' value for later check that nothing was changed */
    referenceX = SDLTest_RandomSint32();
    referenceY = SDLTest_RandomSint32();
    currentX = referenceX;
    currentY = referenceY;
    desiredX = SDLTest_RandomSint32();
    desiredY = SDLTest_RandomSint32();

    /* Negative tests */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    SDL_GetWindowPosition(NULL, &currentX, &currentY);
    SDLTest_AssertPass("Call to SDL_GetWindowPosition(window=NULL)");
    SDLTest_AssertCheck(
        currentX == referenceX && currentY == referenceY,
        "Verify that content of X and Y pointers has not been modified; expected: %d,%d; got: %d,%d",
        referenceX, referenceY,
        currentX, currentY);
    checkInvalidWindowError();

    SDL_GetWindowPosition(NULL, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowPosition(NULL, NULL, NULL)");
    checkInvalidWindowError();

    SDL_SetWindowPosition(NULL, desiredX, desiredY);
    SDLTest_AssertPass("Call to SDL_SetWindowPosition(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/* Helper function that checks for an 'Invalid parameter' error */
static void checkInvalidParameterError(void)
{
    const char *invalidParameterError = "Parameter";
    const char *lastError;

    lastError = SDL_GetError();
    SDLTest_AssertPass("SDL_GetError()");
    SDLTest_AssertCheck(lastError != NULL, "Verify error message is not NULL");
    if (lastError != NULL) {
        SDLTest_AssertCheck(SDL_strncmp(lastError, invalidParameterError, SDL_strlen(invalidParameterError)) == 0,
                            "SDL_GetError(): expected message starts with '%s', was message: '%s'",
                            invalidParameterError,
                            lastError);
        SDL_ClearError();
        SDLTest_AssertPass("Call to SDL_ClearError()");
    }
}

/**
 * Tests call to SDL_GetWindowSize and SDL_SetWindowSize
 *
 * \sa SDL_GetWindowSize
 * \sa SDL_SetWindowSize
 */
static int SDLCALL video_getSetWindowSize(void *arg)
{
    const char *title = "video_getSetWindowSize Test Window";
    SDL_Window *window;
    int result;
    SDL_Rect display;
    int maxwVariation, maxhVariation;
    int wVariation, hVariation;
    int referenceW, referenceH;
    int currentW, currentH;
    int desiredW, desiredH;

    /* Get display bounds for size range */
    result = SDL_GetDisplayUsableBounds(SDL_GetPrimaryDisplay(), &display);
    SDLTest_AssertPass("SDL_GetDisplayUsableBounds()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    if (!result) {
        return TEST_ABORTED;
    }

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    SDL_GetWindowSize(window, &currentW, &currentH);
    if (!SDL_SetWindowSize(window, currentW, currentH)) {
        SDLTest_Log("Skipping window resize tests: %s reports window resizing as unsupported", SDL_GetCurrentVideoDriver());
        goto null_tests;
    }

    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "windows") == 0 ||
        SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        /* Platform clips window size to screen size */
        maxwVariation = 4;
        maxhVariation = 4;
    } else {
        /* Platform allows window size >= screen size */
        maxwVariation = 5;
        maxhVariation = 5;
    }

    for (wVariation = 0; wVariation < maxwVariation; wVariation++) {
        for (hVariation = 0; hVariation < maxhVariation; hVariation++) {
            switch (wVariation) {
            default:
            case 0:
                /* 1 Pixel Wide */
                desiredW = 1;
                break;
            case 1:
                /* Random width inside screen */
                desiredW = SDLTest_RandomIntegerInRange(1, 100);
                break;
            case 2:
                /* Width 1 pixel smaller than screen */
                desiredW = display.w - 1;
                break;
            case 3:
                /* Width at screen size */
                desiredW = display.w;
                break;
            case 4:
                /* Width 1 pixel larger than screen */
                desiredW = display.w + 1;
                break;
            }

            switch (hVariation) {
            default:
            case 0:
                /* 1 Pixel High */
                desiredH = 1;
                break;
            case 1:
                /* Random height inside screen */
                desiredH = SDLTest_RandomIntegerInRange(1, 100);
                break;
            case 2:
                /* Height 1 pixel smaller than screen */
                desiredH = display.h - 1;
                break;
            case 3:
                /* Height at screen size */
                desiredH = display.h;
                break;
            case 4:
                /* Height 1 pixel larger than screen */
                desiredH = display.h + 1;
                break;
            }

            /* Set size */
            SDL_SetWindowSize(window, desiredW, desiredH);
            SDLTest_AssertPass("Call to SDL_SetWindowSize(...,%d,%d)", desiredW, desiredH);

            result = SDL_SyncWindow(window);
            SDLTest_AssertPass("SDL_SyncWindow()");
            SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

            /* Get size */
            currentW = desiredW + 1;
            currentH = desiredH + 1;
            SDL_GetWindowSize(window, &currentW, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowSize()");

            if (desiredW == currentW && desiredH == currentH) {
                SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
                SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);
            } else {
                bool hasEvent;
                /* SDL_SetWindowPosition() and SDL_SetWindowSize() will make requests of the window manager and set the internal position and size,
                 * and then we get events signaling what actually happened, and they get passed on to the application if they're not what we expect. */
                currentW = desiredW + 1;
                currentH = desiredH + 1;
                hasEvent = getSizeFromEvent(&currentW, &currentH);
                SDLTest_AssertCheck(hasEvent == true, "Changing size was not honored by WM, checking presence of SDL_EVENT_WINDOW_RESIZED");
                if (hasEvent) {
                    SDLTest_AssertCheck(desiredW == currentW, "Verify returned width is the one from SDL event; expected: %d, got: %d", desiredW, currentW);
                    SDLTest_AssertCheck(desiredH == currentH, "Verify returned height is the one from SDL event; expected: %d, got: %d", desiredH, currentH);
                }
            }


            /* Get just width */
            currentW = desiredW + 1;
            SDL_GetWindowSize(window, &currentW, NULL);
            SDLTest_AssertPass("Call to SDL_GetWindowSize(&h=NULL)");
            SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);

            /* Get just height */
            currentH = desiredH + 1;
            SDL_GetWindowSize(window, NULL, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowSize(&w=NULL)");
            SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);
        }
    }

null_tests:

    /* Dummy call with both pointers NULL */
    SDL_GetWindowSize(window, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowSize(&w=NULL,&h=NULL)");

    /* Negative tests for parameter input */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    for (desiredH = -2; desiredH < 2; desiredH++) {
        for (desiredW = -2; desiredW < 2; desiredW++) {
            if (desiredW <= 0 || desiredH <= 0) {
                SDL_SetWindowSize(window, desiredW, desiredH);
                SDLTest_AssertPass("Call to SDL_SetWindowSize(...,%d,%d)", desiredW, desiredH);
                checkInvalidParameterError();
            }
        }
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Set some 'magic' value for later check that nothing was changed */
    referenceW = SDLTest_RandomSint32();
    referenceH = SDLTest_RandomSint32();
    currentW = referenceW;
    currentH = referenceH;
    desiredW = SDLTest_RandomSint32();
    desiredH = SDLTest_RandomSint32();

    /* Negative tests for window input */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    SDL_GetWindowSize(NULL, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize(window=NULL)");
    SDLTest_AssertCheck(
        currentW == referenceW && currentH == referenceH,
        "Verify that content of W and H pointers has not been modified; expected: %d,%d; got: %d,%d",
        referenceW, referenceH,
        currentW, currentH);
    checkInvalidWindowError();

    SDL_GetWindowSize(NULL, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowSize(NULL, NULL, NULL)");
    checkInvalidWindowError();

    SDL_SetWindowSize(NULL, desiredW, desiredH);
    SDLTest_AssertPass("Call to SDL_SetWindowSize(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_GetWindowMinimumSize and SDL_SetWindowMinimumSize
 *
 */
static int SDLCALL video_getSetWindowMinimumSize(void *arg)
{
    const char *title = "video_getSetWindowMinimumSize Test Window";
    SDL_Window *window;
    int result;
    SDL_Rect display;
    int wVariation, hVariation;
    int referenceW, referenceH;
    int currentW, currentH;
    int desiredW = 1;
    int desiredH = 1;

    /* Get display bounds for size range */
    result = SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display);
    SDLTest_AssertPass("SDL_GetDisplayBounds()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    if (!result) {
        return TEST_ABORTED;
    }

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    for (wVariation = 0; wVariation < 5; wVariation++) {
        for (hVariation = 0; hVariation < 5; hVariation++) {
            switch (wVariation) {
            case 0:
                /* 1 Pixel Wide */
                desiredW = 1;
                break;
            case 1:
                /* Random width inside screen */
                desiredW = SDLTest_RandomIntegerInRange(2, display.w - 1);
                break;
            case 2:
                /* Width at screen size */
                desiredW = display.w;
                break;
            }

            switch (hVariation) {
            case 0:
                /* 1 Pixel High */
                desiredH = 1;
                break;
            case 1:
                /* Random height inside screen */
                desiredH = SDLTest_RandomIntegerInRange(2, display.h - 1);
                break;
            case 2:
                /* Height at screen size */
                desiredH = display.h;
                break;
            case 4:
                /* Height 1 pixel larger than screen */
                desiredH = display.h + 1;
                break;
            }

            /* Set size */
            SDL_SetWindowMinimumSize(window, desiredW, desiredH);
            SDLTest_AssertPass("Call to SDL_SetWindowMinimumSize(...,%d,%d)", desiredW, desiredH);

            /* Get size */
            currentW = desiredW + 1;
            currentH = desiredH + 1;
            SDL_GetWindowMinimumSize(window, &currentW, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize()");
            SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
            SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);

            /* Get just width */
            currentW = desiredW + 1;
            SDL_GetWindowMinimumSize(window, &currentW, NULL);
            SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize(&h=NULL)");
            SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentH);

            /* Get just height */
            currentH = desiredH + 1;
            SDL_GetWindowMinimumSize(window, NULL, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize(&w=NULL)");
            SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredW, currentH);
        }
    }

    /* Dummy call with both pointers NULL */
    SDL_GetWindowMinimumSize(window, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize(&w=NULL,&h=NULL)");

    /* Negative tests for parameter input */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    for (desiredH = -2; desiredH < 2; desiredH++) {
        for (desiredW = -2; desiredW < 2; desiredW++) {
            if (desiredW < 0 || desiredH < 0) {
                SDL_SetWindowMinimumSize(window, desiredW, desiredH);
                SDLTest_AssertPass("Call to SDL_SetWindowMinimumSize(...,%d,%d)", desiredW, desiredH);
                checkInvalidParameterError();
            }
        }
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Set some 'magic' value for later check that nothing was changed */
    referenceW = SDLTest_RandomSint32();
    referenceH = SDLTest_RandomSint32();
    currentW = referenceW;
    currentH = referenceH;
    desiredW = SDLTest_RandomSint32();
    desiredH = SDLTest_RandomSint32();

    /* Negative tests for window input */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    SDL_GetWindowMinimumSize(NULL, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize(window=NULL)");
    SDLTest_AssertCheck(
        currentW == referenceW && currentH == referenceH,
        "Verify that content of W and H pointers has not been modified; expected: %d,%d; got: %d,%d",
        referenceW, referenceH,
        currentW, currentH);
    checkInvalidWindowError();

    SDL_GetWindowMinimumSize(NULL, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowMinimumSize(NULL, NULL, NULL)");
    checkInvalidWindowError();

    SDL_SetWindowMinimumSize(NULL, desiredW, desiredH);
    SDLTest_AssertPass("Call to SDL_SetWindowMinimumSize(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_GetWindowMaximumSize and SDL_SetWindowMaximumSize
 *
 */
static int SDLCALL video_getSetWindowMaximumSize(void *arg)
{
    const char *title = "video_getSetWindowMaximumSize Test Window";
    SDL_Window *window;
    int result;
    SDL_Rect display;
    int wVariation, hVariation;
    int referenceW, referenceH;
    int currentW, currentH;
    int desiredW = 0, desiredH = 0;

    /* Get display bounds for size range */
    result = SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &display);
    SDLTest_AssertPass("SDL_GetDisplayBounds()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    if (!result) {
        return TEST_ABORTED;
    }

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    for (wVariation = 0; wVariation < 3; wVariation++) {
        for (hVariation = 0; hVariation < 3; hVariation++) {
            switch (wVariation) {
            case 0:
                /* 1 Pixel Wide */
                desiredW = 1;
                break;
            case 1:
                /* Random width inside screen */
                desiredW = SDLTest_RandomIntegerInRange(2, display.w - 1);
                break;
            case 2:
                /* Width at screen size */
                desiredW = display.w;
                break;
            }

            switch (hVariation) {
            case 0:
                /* 1 Pixel High */
                desiredH = 1;
                break;
            case 1:
                /* Random height inside screen */
                desiredH = SDLTest_RandomIntegerInRange(2, display.h - 1);
                break;
            case 2:
                /* Height at screen size */
                desiredH = display.h;
                break;
            }

            /* Set size */
            SDL_SetWindowMaximumSize(window, desiredW, desiredH);
            SDLTest_AssertPass("Call to SDL_SetWindowMaximumSize(...,%d,%d)", desiredW, desiredH);

            /* Get size */
            currentW = desiredW + 1;
            currentH = desiredH + 1;
            SDL_GetWindowMaximumSize(window, &currentW, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize()");
            SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
            SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);

            /* Get just width */
            currentW = desiredW + 1;
            SDL_GetWindowMaximumSize(window, &currentW, NULL);
            SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize(&h=NULL)");
            SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentH);

            /* Get just height */
            currentH = desiredH + 1;
            SDL_GetWindowMaximumSize(window, NULL, &currentH);
            SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize(&w=NULL)");
            SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredW, currentH);
        }
    }

    /* Dummy call with both pointers NULL */
    SDL_GetWindowMaximumSize(window, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize(&w=NULL,&h=NULL)");

    /* Negative tests for parameter input */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    for (desiredH = -2; desiredH < 2; desiredH++) {
        for (desiredW = -2; desiredW < 2; desiredW++) {
            if (desiredW < 0 || desiredH < 0) {
                SDL_SetWindowMaximumSize(window, desiredW, desiredH);
                SDLTest_AssertPass("Call to SDL_SetWindowMaximumSize(...,%d,%d)", desiredW, desiredH);
                checkInvalidParameterError();
            }
        }
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Set some 'magic' value for later check that nothing was changed */
    referenceW = SDLTest_RandomSint32();
    referenceH = SDLTest_RandomSint32();
    currentW = referenceW;
    currentH = referenceH;
    desiredW = SDLTest_RandomSint32();
    desiredH = SDLTest_RandomSint32();

    /* Negative tests */
    SDL_ClearError();
    SDLTest_AssertPass("Call to SDL_ClearError()");
    SDL_GetWindowMaximumSize(NULL, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize(window=NULL)");
    SDLTest_AssertCheck(
        currentW == referenceW && currentH == referenceH,
        "Verify that content of W and H pointers has not been modified; expected: %d,%d; got: %d,%d",
        referenceW, referenceH,
        currentW, currentH);
    checkInvalidWindowError();

    SDL_GetWindowMaximumSize(NULL, NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowMaximumSize(NULL, NULL, NULL)");
    checkInvalidWindowError();

    SDL_SetWindowMaximumSize(NULL, desiredW, desiredH);
    SDLTest_AssertPass("Call to SDL_SetWindowMaximumSize(window=NULL)");
    checkInvalidWindowError();

    return TEST_COMPLETED;
}

/**
 * Tests call to SDL_SetWindowData and SDL_GetWindowData
 *
 * \sa SDL_SetWindowData
 * \sa SDL_GetWindowData
 */
static int SDLCALL video_getSetWindowData(void *arg)
{
    int returnValue = TEST_COMPLETED;
    const char *title = "video_setGetWindowData Test Window";
    SDL_Window *window;
    const char *referenceName = "TestName";
    const char *name = "TestName";
    const char *referenceName2 = "TestName2";
    const char *name2 = "TestName2";
    int datasize;
    char *referenceUserdata = NULL;
    char *userdata = NULL;
    char *referenceUserdata2 = NULL;
    char *userdata2 = NULL;
    char *result;
    int iteration;

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    /* Create testdata */
    datasize = SDLTest_RandomIntegerInRange(1, 32);
    referenceUserdata = SDLTest_RandomAsciiStringOfSize(datasize);
    if (!referenceUserdata) {
        returnValue = TEST_ABORTED;
        goto cleanup;
    }
    userdata = SDL_strdup(referenceUserdata);
    if (!userdata) {
        returnValue = TEST_ABORTED;
        goto cleanup;
    }
    datasize = SDLTest_RandomIntegerInRange(1, 32);
    referenceUserdata2 = SDLTest_RandomAsciiStringOfSize(datasize);
    if (!referenceUserdata2) {
        returnValue = TEST_ABORTED;
        goto cleanup;
    }
    userdata2 = SDL_strdup(referenceUserdata2);
    if (!userdata2) {
        returnValue = TEST_ABORTED;
        goto cleanup;
    }

    /* Get non-existent data */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s)", name);
    SDLTest_AssertCheck(result == NULL, "Validate that result is NULL");
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);

    /* Set data */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, userdata);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,%s)", name, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);

    /* Get data (twice) */
    for (iteration = 1; iteration <= 2; iteration++) {
        result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
        SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s) [iteration %d]", name, iteration);
        SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, result) == 0, "Validate that correct result was returned; expected: %s, got: %s", referenceUserdata, result);
        SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    }

    /* Set data again twice */
    for (iteration = 1; iteration <= 2; iteration++) {
        SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, userdata);
        SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,%s) [iteration %d]", name, userdata, iteration);
        SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
        SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);
    }

    /* Get data again */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s) [again]", name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, result) == 0, "Validate that correct result was returned; expected: %s, got: %s", referenceUserdata, result);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);

    /* Set data with new data */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, userdata2);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,%s) [new userdata]", name, userdata2);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata2, userdata2) == 0, "Validate that userdata2 was not changed, expected: %s, got: %s", referenceUserdata2, userdata2);

    /* Set data with new data again */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, userdata2);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,%s) [new userdata again]", name, userdata2);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata2, userdata2) == 0, "Validate that userdata2 was not changed, expected: %s, got: %s", referenceUserdata2, userdata2);

    /* Get new data */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s)", name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata2, result) == 0, "Validate that correct result was returned; expected: %s, got: %s", referenceUserdata2, result);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);

    /* Set data with NULL to clear */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,NULL)", name);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata2, userdata2) == 0, "Validate that userdata2 was not changed, expected: %s, got: %s", referenceUserdata2, userdata2);

    /* Set data with NULL to clear again */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,NULL) [again]", name);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata2, userdata2) == 0, "Validate that userdata2 was not changed, expected: %s, got: %s", referenceUserdata2, userdata2);

    /* Get non-existent data */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s)", name);
    SDLTest_AssertCheck(result == NULL, "Validate that result is NULL");
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);

    /* Get non-existent data new name */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name2, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s)", name2);
    SDLTest_AssertCheck(result == NULL, "Validate that result is NULL");
    SDLTest_AssertCheck(SDL_strcmp(referenceName2, name2) == 0, "Validate that name2 was not changed, expected: %s, got: %s", referenceName2, name2);

    /* Set data (again) */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), name, userdata);
    SDLTest_AssertPass("Call to SDL_SetWindowData(...%s,%s) [again, after clear]", name, userdata);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, userdata) == 0, "Validate that userdata was not changed, expected: %s, got: %s", referenceUserdata, userdata);

    /* Get data (again) */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), name, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(..,%s) [again, after clear]", name);
    SDLTest_AssertCheck(SDL_strcmp(referenceUserdata, result) == 0, "Validate that correct result was returned; expected: %s, got: %s", referenceUserdata, result);
    SDLTest_AssertCheck(SDL_strcmp(referenceName, name) == 0, "Validate that name was not changed, expected: %s, got: %s", referenceName, name);

    /* Set data with NULL name, valid userdata */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), NULL, userdata);
    SDLTest_AssertPass("Call to SDL_SetWindowData(name=NULL)");
    checkInvalidParameterError();

    /* Set data with empty name, valid userdata */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), "", userdata);
    SDLTest_AssertPass("Call to SDL_SetWindowData(name='')");
    checkInvalidParameterError();

    /* Set data with NULL name, NULL userdata */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), NULL, NULL);
    SDLTest_AssertPass("Call to SDL_SetWindowData(name=NULL,userdata=NULL)");
    checkInvalidParameterError();

    /* Set data with empty name, NULL userdata */
    SDL_SetPointerProperty(SDL_GetWindowProperties(window), "", NULL);
    SDLTest_AssertPass("Call to SDL_SetWindowData(name='',userdata=NULL)");
    checkInvalidParameterError();

    /* Get data with NULL name */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), NULL, NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(name=NULL)");
    SDLTest_AssertCheck(result == NULL, "Validate that result is NULL");

    /* Get data with empty name */
    result = (char *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), "", NULL);
    SDLTest_AssertPass("Call to SDL_GetWindowData(name='')");
    SDLTest_AssertCheck(result == NULL, "Validate that result is NULL");

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

cleanup:
    SDL_free(referenceUserdata);
    SDL_free(referenceUserdata2);
    SDL_free(userdata);
    SDL_free(userdata2);

    return returnValue;
}

/**
 * Tests the functionality of the SDL_WINDOWPOS_CENTERED_DISPLAY along with SDL_WINDOW_FULLSCREEN.
 *
 * Especially useful when run on a multi-monitor system with different DPI scales per monitor,
 * to test that the window size is maintained when moving between monitors.
 *
 * As the Wayland windowing protocol does not allow application windows to control their position in the
 * desktop space, coupled with the general asynchronous nature of Wayland compositors, the positioning
 * tests don't work in windowed mode and are unreliable in fullscreen mode, thus are disabled when using
 * the Wayland video driver. All that can be done is check that the windows are the expected size.
 */
static int SDLCALL video_setWindowCenteredOnDisplay(void *arg)
{
    SDL_DisplayID *displays;
    SDL_Window *window;
    const char *title = "video_setWindowCenteredOnDisplay Test Window";
    int x, y, w, h;
    int xVariation, yVariation;
    int displayNum;
    int result;
    SDL_Rect display0, display1;
    const char *video_driver = SDL_GetCurrentVideoDriver();
    const bool video_driver_is_wayland = SDL_strcmp(video_driver, "wayland") == 0;
    const bool video_driver_is_emscripten = SDL_strcmp(video_driver, "emscripten") == 0;

    displays = SDL_GetDisplays(&displayNum);
    if (displays) {

        /* Get display bounds */
        result = SDL_GetDisplayUsableBounds(displays[0 % displayNum], &display0);
        SDLTest_AssertPass("SDL_GetDisplayUsableBounds()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
        if (!result) {
            return TEST_ABORTED;
        }

        result = SDL_GetDisplayUsableBounds(displays[1 % displayNum], &display1);
        SDLTest_AssertPass("SDL_GetDisplayUsableBounds()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
        if (!result) {
            return TEST_ABORTED;
        }

        for (xVariation = 0; xVariation < 2; xVariation++) {
            for (yVariation = 0; yVariation < 2; yVariation++) {
                int currentX = 0, currentY = 0;
                int currentW = 0, currentH = 0;
                int expectedX = 0, expectedY = 0;
                int currentDisplay;
                int expectedDisplay;
                SDL_Rect expectedDisplayRect, expectedFullscreenRect;
                SDL_PropertiesID props;

                /* xVariation is the display we start on */
                expectedDisplay = displays[xVariation % displayNum];
                x = SDL_WINDOWPOS_CENTERED_DISPLAY(expectedDisplay);
                y = SDL_WINDOWPOS_CENTERED_DISPLAY(expectedDisplay);
                w = SDLTest_RandomIntegerInRange(640, 800);
                h = SDLTest_RandomIntegerInRange(400, 600);
                expectedDisplayRect = (xVariation == 0) ? display0 : display1;
                expectedX = (expectedDisplayRect.x + ((expectedDisplayRect.w - w) / 2));
                expectedY = (expectedDisplayRect.y + ((expectedDisplayRect.h - h) / 2));

                props = SDL_CreateProperties();
                SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, w);
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, h);
                SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true);
                window = SDL_CreateWindowWithProperties(props);
                SDL_DestroyProperties(props);
                SDLTest_AssertPass("Call to SDL_CreateWindow('Title',%d,%d,%d,%d,SHOWN)", x, y, w, h);
                SDLTest_AssertCheck(window != NULL, "Validate that returned window is not NULL");

                /* Wayland windows require that a frame be presented before they are fully mapped and visible onscreen. */
                if (video_driver_is_wayland) {
                    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

                    if (renderer) {
                        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                        SDL_RenderClear(renderer);
                        SDL_RenderPresent(renderer);

                        /* Some desktops don't display the window immediately after presentation,
                         * so delay to give the window time to actually appear on the desktop.
                         */
                        SDL_Delay(100);
                    } else {
                        SDLTest_Log("Unable to create a renderer, tests may fail under Wayland");
                    }
                }

                /* Check the window is centered on the requested display */
                currentDisplay = SDL_GetDisplayForWindow(window);
                SDL_GetWindowSize(window, &currentW, &currentH);
                SDL_GetWindowPosition(window, &currentX, &currentY);

                if (video_driver_is_wayland) {
                    SDLTest_Log("Skipping display ID validation: %s driver does not support window positioning", video_driver);
                } else {
                    SDLTest_AssertCheck(currentDisplay == expectedDisplay, "Validate display ID (current: %d, expected: %d)", currentDisplay, expectedDisplay);
                }
                if (VideoSupportsWindowResizing()) {
                    SDLTest_AssertCheck(currentW == w, "Validate width (current: %d, expected: %d)", currentW, w);
                    SDLTest_AssertCheck(currentH == h, "Validate height (current: %d, expected: %d)", currentH, h);
                } else {
                    SDLTest_Log("Skipping window size validation: %s driver does not support window resizing", video_driver);
                }
                if (VideoSupportsWindowPositioning()) {
                    SDLTest_AssertCheck(currentX == expectedX, "Validate x (current: %d, expected: %d)", currentX, expectedX);
                    SDLTest_AssertCheck(currentY == expectedY, "Validate y (current: %d, expected: %d)", currentY, expectedY);
                } else {
                    SDLTest_Log("Skipping window position validation: %s driver does not support window positioning", video_driver);
                }

                if (video_driver_is_emscripten) {
                    SDLTest_Log("Skipping fullscreen checks on Emscripten: can't toggle fullscreen without returning to mainloop.");
                } else {
                    /* Enter fullscreen desktop */
                    SDL_SetWindowPosition(window, x, y);
                    result = SDL_SetWindowFullscreen(window, true);
                    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                    result = SDL_SyncWindow(window);
                    SDLTest_AssertPass("SDL_SyncWindow()");
                    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                    /* Check we are filling the full display */
                    currentDisplay = SDL_GetDisplayForWindow(window);
                    SDL_GetWindowSize(window, &currentW, &currentH);
                    SDL_GetWindowPosition(window, &currentX, &currentY);

                    /* Get the expected fullscreen rect.
                     * This needs to be queried after window creation and positioning as some drivers can alter the
                     * usable bounds based on the window scaling mode.
                     */
                    result = SDL_GetDisplayBounds(expectedDisplay, &expectedFullscreenRect);
                    SDLTest_AssertPass("SDL_GetDisplayBounds()");
                    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                    if (video_driver_is_wayland) {
                        SDLTest_Log("Skipping display ID validation: Wayland driver does not support window positioning");
                    } else {
                        SDLTest_AssertCheck(currentDisplay == expectedDisplay, "Validate display ID (current: %d, expected: %d)", currentDisplay, expectedDisplay);
                    }

                    if (VideoSupportsWindowResizing()) {
                        SDLTest_AssertCheck(currentW == expectedFullscreenRect.w, "Validate width (current: %d, expected: %d)", currentW, expectedFullscreenRect.w);
                        SDLTest_AssertCheck(currentH == expectedFullscreenRect.h, "Validate height (current: %d, expected: %d)", currentH, expectedFullscreenRect.h);
                    } else {
                        SDLTest_Log("Skipping window size validation: %s driver does not support window resizing", video_driver);
                    }
                    if (VideoSupportsWindowPositioning()) {
                        SDLTest_AssertCheck(currentX == expectedFullscreenRect.x, "Validate x (current: %d, expected: %d)", currentX, expectedFullscreenRect.x);
                        SDLTest_AssertCheck(currentY == expectedFullscreenRect.y, "Validate y (current: %d, expected: %d)", currentY, expectedFullscreenRect.y);
                    } else {
                        SDLTest_Log("Skipping window position validation: %s driver does not support window positioning", video_driver);
                    }

                    /* Leave fullscreen desktop */

                    result = SDL_SetWindowFullscreen(window, false);
                    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                    result = SDL_SyncWindow(window);
                    SDLTest_AssertPass("SDL_SyncWindow()");
                    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                    /* Check window was restored correctly */
                    currentDisplay = SDL_GetDisplayForWindow(window);
                    SDL_GetWindowSize(window, &currentW, &currentH);
                    SDL_GetWindowPosition(window, &currentX, &currentY);

                    if (video_driver_is_wayland) {
                        SDLTest_Log("Skipping display ID validation: %s driver does not support window positioning", video_driver);
                    } else {
                        SDLTest_AssertCheck(currentDisplay == expectedDisplay, "Validate display index (current: %d, expected: %d)", currentDisplay, expectedDisplay);
                    }
                    if (VideoSupportsWindowResizing()) {
                        SDLTest_AssertCheck(currentW == w, "Validate width (current: %d, expected: %d)", currentW, w);
                        SDLTest_AssertCheck(currentH == h, "Validate height (current: %d, expected: %d)", currentH, h);
                    } else {
                        SDLTest_Log("Skipping window size validation: %s driver does not support window resizing", video_driver);
                    }
                    if (VideoSupportsWindowPositioning()) {
                        SDLTest_AssertCheck(currentX == expectedX, "Validate x (current: %d, expected: %d)", currentX, expectedX);
                        SDLTest_AssertCheck(currentY == expectedY, "Validate y (current: %d, expected: %d)", currentY, expectedY);
                    } else {
                        SDLTest_Log("Skipping window position validation: %s driver does not support window positioning", video_driver);
                    }
                }

                /* Center on display yVariation, and check window properties */

                expectedDisplay = displays[yVariation % displayNum];
                x = SDL_WINDOWPOS_CENTERED_DISPLAY(expectedDisplay);
                y = SDL_WINDOWPOS_CENTERED_DISPLAY(expectedDisplay);
                expectedDisplayRect = (yVariation == 0) ? display0 : display1;
                expectedX = (expectedDisplayRect.x + ((expectedDisplayRect.w - w) / 2));
                expectedY = (expectedDisplayRect.y + ((expectedDisplayRect.h - h) / 2));
                SDL_SetWindowPosition(window, x, y);

                result = SDL_SyncWindow(window);
                SDLTest_AssertPass("SDL_SyncWindow()");
                SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

                currentDisplay = SDL_GetDisplayForWindow(window);
                SDL_GetWindowSize(window, &currentW, &currentH);
                SDL_GetWindowPosition(window, &currentX, &currentY);

                if (video_driver_is_wayland) {
                    SDLTest_Log("Skipping display ID validation: %s driver does not support window positioning", video_driver);
                } else {
                    SDLTest_AssertCheck(currentDisplay == expectedDisplay, "Validate display ID (current: %d, expected: %d)", currentDisplay, expectedDisplay);
                }
                if (VideoSupportsWindowResizing()) {
                    SDLTest_AssertCheck(currentW == w, "Validate width (current: %d, expected: %d)", currentW, w);
                    SDLTest_AssertCheck(currentH == h, "Validate height (current: %d, expected: %d)", currentH, h);
                } else {
                    SDLTest_Log("Skipping window size validation: %s driver does not support window resizing", video_driver);
                }
                if (VideoSupportsWindowPositioning()) {
                    SDLTest_AssertCheck(currentX == expectedX, "Validate x (current: %d, expected: %d)", currentX, expectedX);
                    SDLTest_AssertCheck(currentY == expectedY, "Validate y (current: %d, expected: %d)", currentY, expectedY);
                } else {
                    SDLTest_Log("Skipping window position validation: %s driver does not support window positioning", video_driver);
                }

                /* Clean up */
                destroyVideoSuiteTestWindow(window);
            }
        }
        SDL_free(displays);
    }

    return TEST_COMPLETED;
}

/**
 * Tests calls to SDL_MaximizeWindow(), SDL_RestoreWindow(), and SDL_SetWindowFullscreen(),
 * interspersed with calls to set the window size and position, and verifies the flags,
 * sizes, and positions of maximized, fullscreen, and restored windows.
 *
 * NOTE: This test is good on Mac, Win32, GNOME, and KDE (Wayland and X11). Other *nix
 *       desktops, particularly tiling desktops, may not support the expected behavior,
 *       so don't be surprised if this fails.
 */
static int SDLCALL video_waylandShowHideLifecycle(void *arg)
{
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") != 0) {
        return TEST_SKIPPED;
    }

    SDL_Window *window = SDL_CreateWindow("Wayland show/hide lifecycle", 320, 240, SDL_WINDOW_HIDDEN);
    if (!SDLTest_AssertCheck(window != NULL, "Create hidden Wayland window: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    const SDL_PropertiesID props = SDL_GetWindowProperties(window);
    for (int i = 0; i < 2; ++i) {
        if (!SDLTest_AssertCheck(SDL_ShowWindow(window), "Show Wayland window %d: %s", i, SDL_GetError())) {
            break;
        }
        SDLTest_AssertCheck(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_XDG_SURFACE_POINTER, NULL) != NULL,
                            "Show %d creates an xdg surface", i);
        if (!SDLTest_AssertCheck(SDL_HideWindow(window), "Hide Wayland window %d: %s", i, SDL_GetError())) {
            break;
        }
        SDLTest_AssertCheck(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_XDG_SURFACE_POINTER, NULL) == NULL,
                            "Hide %d clears the xdg surface", i);
    }

    SDL_DestroyWindow(window);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestWaylandShowHideLifecycle = {
    video_waylandShowHideLifecycle, "video_waylandShowHideLifecycle", "Create and destroy Wayland shell surfaces across repeated show/hide", TEST_ENABLED
};

static int SDLCALL video_waylandPopupShowHideLifecycle(void *arg)
{
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") != 0) {
        return TEST_SKIPPED;
    }

    SDL_Window *parent = createVideoSuiteTestWindow("Wayland popup parent");
    if (!parent) {
        return TEST_ABORTED;
    }
    SDL_PumpEvents();
    SDL_Window *popup = SDL_CreatePopupWindow(parent, 8, 8, 64, 64, SDL_WINDOW_TOOLTIP | SDL_WINDOW_HIDDEN);
    if (!SDLTest_AssertCheck(popup != NULL, "Create hidden Wayland popup: %s", SDL_GetError())) {
        destroyVideoSuiteTestWindow(parent);
        return TEST_ABORTED;
    }

    const SDL_PropertiesID props = SDL_GetWindowProperties(popup);
    for (int i = 0; i < 2; ++i) {
        if (!SDLTest_AssertCheck(SDL_ShowWindow(popup), "Show Wayland popup %d: %s", i, SDL_GetError())) {
            break;
        }
        const Uint64 deadline = SDL_GetTicks() + 1000;
        while (SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_XDG_POPUP_POINTER, NULL) == NULL && SDL_GetTicks() < deadline) {
            SDL_PumpEvents();
            SDL_Delay(10);
        }
        SDLTest_AssertCheck(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_XDG_POPUP_POINTER, NULL) != NULL,
                            "Show %d creates an xdg popup", i);
        if (!SDLTest_AssertCheck(SDL_HideWindow(popup), "Hide Wayland popup %d: %s", i, SDL_GetError())) {
            break;
        }
        SDLTest_AssertCheck(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_XDG_POPUP_POINTER, NULL) == NULL,
                            "Hide %d clears the xdg popup", i);
    }

    SDL_DestroyWindow(popup);
    destroyVideoSuiteTestWindow(parent);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestWaylandPopupShowHideLifecycle = {
    video_waylandPopupShowHideLifecycle, "video_waylandPopupShowHideLifecycle", "Create and destroy Wayland popup roles across repeated show/hide", TEST_ENABLED
};

static int SDLCALL video_getSetWindowState(void *arg)
{
    const char *title = "video_getSetWindowState Test Window";
    SDL_Window *window;
    int result;
    SDL_Rect display;
    SDL_WindowFlags flags;
    int windowedX, windowedY;
    int currentX = 0, currentY = 0;
    int desiredX = 0, desiredY = 0;
    int windowedW, windowedH;
    int currentW, currentH;
    int desiredW = 0, desiredH = 0;
    SDL_WindowFlags skipFlags = 0;
    const bool restoreHint = SDL_GetHintBoolean("SDL_BORDERLESS_RESIZABLE_STYLE", true);
    const bool skipPos = SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0;

    /* This test is known to be good only on GNOME and KDE. At the time of writing, Weston seems to have maximize related bugs
     * that prevent it from running correctly (no configure events are received when unsetting maximize), and tiling window
     * managers such as Sway have fundamental behavioral differences that conflict with it.
     *
     * Other desktops can be enabled in the future as required.
     */
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0 || SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        const char *desktop = SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CURRENT_DESKTOP");
        if (SDL_strcmp(desktop, "GNOME") != 0 && SDL_strcmp(desktop, "KDE") != 0) {
            SDLTest_Log("Skipping test video_getSetWindowState: desktop environment %s not supported", desktop);
            return TEST_SKIPPED;
        }
    }

    /* Win32 borderless windows are not resizable by default and need this undocumented hint */
    SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");

    /* Call against new test window */
    window = createVideoSuiteTestWindow(title);
    if (!window) {
        return TEST_ABORTED;
    }

    SDL_GetWindowSize(window, &windowedW, &windowedH);
    SDLTest_AssertPass("SDL_GetWindowSize()");

    SDL_GetWindowPosition(window, &windowedX, &windowedY);
    SDLTest_AssertPass("SDL_GetWindowPosition()");

    if (skipPos) {
        SDLTest_Log("Skipping positioning tests: %s reports window positioning as unsupported", SDL_GetCurrentVideoDriver());
    }

    /* Maximize and check the dimensions */
    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    if (!result) {
        SDLTest_Log("Skipping state transition tests: %s reports window maximizing as unsupported", SDL_GetCurrentVideoDriver());
        skipFlags |= SDL_WINDOW_MAXIMIZED;
        goto minimize_test;
    }

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(flags & SDL_WINDOW_MAXIMIZED, "Verify the `SDL_WINDOW_MAXIMIZED` flag is set: %s", (flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    /* Check that the maximized window doesn't extend beyond the usable display bounds.
     * FIXME: Maximizing Win32 borderless windows is broken, so this always fails.
     *        Skip it for now.
     */
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "windows") != 0) {
        result = SDL_GetDisplayUsableBounds(SDL_GetDisplayForWindow(window), &display);
        SDLTest_AssertPass("SDL_GetDisplayUsableBounds()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

        desiredW = display.w;
        desiredH = display.h;
        currentW = windowedW + 1;
        currentH = windowedH + 1;
        SDL_GetWindowSize(window, &currentW, &currentH);
        SDLTest_AssertPass("Call to SDL_GetWindowSize()");
        SDLTest_AssertCheck(currentW <= desiredW, "Verify returned width; expected: <= %d, got: %d", desiredW,
                            currentW);
        SDLTest_AssertCheck(currentH <= desiredH, "Verify returned height; expected: <= %d, got: %d", desiredH,
                            currentH);
    }

    /* Restore and check the dimensions */
    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    if (!skipPos) {
        currentX = windowedX + 1;
        currentY = windowedY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(windowedX == currentX, "Verify returned X coordinate; expected: %d, got: %d", windowedX, currentX);
        SDLTest_AssertCheck(windowedY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", windowedY, currentY);
    }

    currentW = windowedW + 1;
    currentH = windowedH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(windowedW == currentW, "Verify returned width; expected: %d, got: %d", windowedW, currentW);
    SDLTest_AssertCheck(windowedH == currentH, "Verify returned height; expected: %d, got: %d", windowedH, currentH);

    /* Maximize, then immediately restore */
    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    /* Make sure the restored size and position matches the original windowed size and position. */
    if (!skipPos) {
        currentX = windowedX + 1;
        currentY = windowedY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(windowedX == currentX, "Verify returned X coordinate; expected: %d, got: %d", windowedX, currentX);
        SDLTest_AssertCheck(windowedY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", windowedY, currentY);
    }

    currentW = windowedW + 1;
    currentH = windowedH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(windowedW == currentW, "Verify returned width; expected: %d, got: %d", windowedW, currentW);
    SDLTest_AssertCheck(windowedH == currentH, "Verify returned height; expected: %d, got: %d", windowedH, currentH);

    /* Maximize, then enter fullscreen */
    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SetWindowFullscreen(window, true);
    SDLTest_AssertPass("SDL_SetWindowFullscreen(true)");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(flags & SDL_WINDOW_FULLSCREEN, "Verify the `SDL_WINDOW_FULLSCREEN` flag is set: %s", (flags & SDL_WINDOW_FULLSCREEN) ? "true" : "false");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    /* Verify the fullscreen size and position */
    result = SDL_GetDisplayBounds(SDL_GetDisplayForWindow(window), &display);
    SDLTest_AssertPass("SDL_GetDisplayBounds()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    if (!skipPos) {
        desiredX = display.x;
        desiredY = display.y;
        currentX = windowedX + 1;
        currentY = windowedY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X coordinate; expected: %d, got: %d", desiredX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", desiredY, currentY);
    }

    desiredW = display.w;
    desiredH = display.h;
    currentW = windowedW + 1;
    currentH = windowedH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(currentW == desiredW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
    SDLTest_AssertCheck(currentH == desiredH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);

    /* Leave fullscreen and restore the window */
    result = SDL_SetWindowFullscreen(window, false);
    SDLTest_AssertPass("SDL_SetWindowFullscreen(false)");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    /* Make sure the restored size and position matches the original windowed size and position. */
    if (!skipPos) {
        currentX = windowedX + 1;
        currentY = windowedY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(windowedX == currentX, "Verify returned X coordinate; expected: %d, got: %d", windowedX, currentX);
        SDLTest_AssertCheck(windowedY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", windowedY, currentY);
    }

    currentW = windowedW + 1;
    currentH = windowedH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(windowedW == currentW, "Verify returned width; expected: %d, got: %d", windowedW, currentW);
    SDLTest_AssertCheck(windowedH == currentH, "Verify returned height; expected: %d, got: %d", windowedH, currentH);

    /* Maximize, restore, and change size */
    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    desiredW = windowedW + 10;
    desiredH = windowedH + 10;
    result = SDL_SetWindowSize(window, desiredW, desiredH);
    SDLTest_AssertPass("SDL_SetWindowSize()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    if (!skipPos) {
        desiredX = windowedX + 10;
        desiredY = windowedY + 10;
        result = SDL_SetWindowPosition(window, desiredX, desiredY);
        SDLTest_AssertPass("SDL_SetWindowPosition()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    }

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    if (!skipPos) {
        currentX = desiredX + 1;
        currentY = desiredY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X coordinate; expected: %d, got: %d", desiredX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", desiredY, currentY);
    }

    currentW = desiredW + 1;
    currentH = desiredH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
    SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);

    /* Maximize, change size/position (should be ignored), and restore. */
    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    desiredW = windowedW + 10;
    desiredH = windowedH + 10;
    result = SDL_SetWindowSize(window, desiredW, desiredH);
    SDLTest_AssertPass("SDL_SetWindowSize()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    if (!skipPos) {
        desiredX = windowedX + 10;
        desiredY = windowedY + 10;
        result = SDL_SetWindowPosition(window, desiredX, desiredY);
        SDLTest_AssertPass("SDL_SetWindowPosition()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    }

    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    if (!skipPos) {
        int previousX = desiredX + 1;
        int previousY = desiredY + 1;
        SDL_GetWindowPosition(window, &previousX, &previousY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X coordinate; expected: %d, got: %d", previousX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", previousY, currentY);
    }

    int previousW = desiredW + 1;
    int previousH = desiredH + 1;
    SDL_GetWindowSize(window, &previousW, &previousH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", previousW, currentW);
    SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", previousH, currentH);

    /* Change size and position, maximize and restore */
    desiredW = windowedW - 5;
    desiredH = windowedH - 5;
    result = SDL_SetWindowSize(window, desiredW, desiredH);
    SDLTest_AssertPass("SDL_SetWindowSize()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    if (!skipPos) {
        desiredX = windowedX + 5;
        desiredY = windowedY + 5;
        result = SDL_SetWindowPosition(window, desiredX, desiredY);
        SDLTest_AssertPass("SDL_SetWindowPosition()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    }

    result = SDL_MaximizeWindow(window);
    SDLTest_AssertPass("SDL_MaximizeWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_RestoreWindow(window);
    SDLTest_AssertPass("SDL_RestoreWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    result = SDL_SyncWindow(window);
    SDLTest_AssertPass("SDL_SyncWindow()");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    flags = SDL_GetWindowFlags(window);
    SDLTest_AssertPass("SDL_GetWindowFlags()");
    SDLTest_AssertCheck(!(flags & SDL_WINDOW_MAXIMIZED), "Verify that the `SDL_WINDOW_MAXIMIZED` flag is cleared: %s", !(flags & SDL_WINDOW_MAXIMIZED) ? "true" : "false");

    if (!skipPos) {
        currentX = desiredX + 1;
        currentY = desiredY + 1;
        SDL_GetWindowPosition(window, &currentX, &currentY);
        SDLTest_AssertPass("Call to SDL_GetWindowPosition()");
        SDLTest_AssertCheck(desiredX == currentX, "Verify returned X coordinate; expected: %d, got: %d", desiredX, currentX);
        SDLTest_AssertCheck(desiredY == currentY, "Verify returned Y coordinate; expected: %d, got: %d", desiredY, currentY);
    }

    currentW = desiredW + 1;
    currentH = desiredH + 1;
    SDL_GetWindowSize(window, &currentW, &currentH);
    SDLTest_AssertPass("Call to SDL_GetWindowSize()");
    SDLTest_AssertCheck(desiredW == currentW, "Verify returned width; expected: %d, got: %d", desiredW, currentW);
    SDLTest_AssertCheck(desiredH == currentH, "Verify returned height; expected: %d, got: %d", desiredH, currentH);

minimize_test:

    /* Minimize */
	if (VideoSupportsWindowMinimizing() && SDL_MinimizeWindow(window)) {
        SDLTest_AssertPass("SDL_MinimizeWindow()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

        result = SDL_SyncWindow(window);
        SDLTest_AssertPass("SDL_SyncWindow()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

        flags = SDL_GetWindowFlags(window);
        SDLTest_AssertPass("SDL_GetWindowFlags()");
        SDLTest_AssertCheck(flags & SDL_WINDOW_MINIMIZED, "Verify that the `SDL_WINDOW_MINIMIZED` flag is set: %s", (flags & SDL_WINDOW_MINIMIZED) ? "true" : "false");
    } else {
        SDLTest_Log("Skipping minimize test: %s reports window minimizing as unsupported", SDL_GetCurrentVideoDriver());
        skipFlags |= SDL_WINDOW_MINIMIZED;
    }

    /* Clean up */
    destroyVideoSuiteTestWindow(window);

    /* Restore the hint to the previous value */
    SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", restoreHint ? "1" : "0");

    return skipFlags != (SDL_WINDOW_MAXIMIZED | SDL_WINDOW_MINIMIZED)  ? TEST_COMPLETED : TEST_SKIPPED;
}

static int SDLCALL video_createMinimized(void *arg)
{
    const char *title = "video_createMinimized Test Window";
    int result;
    SDL_Window *window;
    int windowedX, windowedY;
    int windowedW, windowedH;

	if (!VideoSupportsWindowMinimizing()) {
		SDLTest_Log("Skipping creating mimized window, %s reports window minimizing as unsupported", SDL_GetCurrentVideoDriver());
		return TEST_SKIPPED;
	}

    /* Call against new test window */
    window = SDL_CreateWindow(title, 320, 200, SDL_WINDOW_MINIMIZED);
    if (!window) {
        return TEST_ABORTED;
    }

    SDL_GetWindowSize(window, &windowedW, &windowedH);
    SDLTest_AssertPass("SDL_GetWindowSize()");
    SDLTest_AssertCheck(windowedW > 0 && windowedH > 0, "Verify return value; expected: 320x200, got: %dx%d", windowedW, windowedH);

    SDL_GetWindowSizeInPixels(window, &windowedW, &windowedH);
    SDLTest_AssertPass("SDL_GetWindowSizeInPixels()");
    SDLTest_AssertCheck(windowedW > 0 && windowedH > 0, "Verify return value; expected: > 0, got: %dx%d", windowedW, windowedH);

    SDL_GetWindowPosition(window, &windowedX, &windowedY);
    SDLTest_AssertPass("SDL_GetWindowPosition()");
    SDLTest_AssertCheck(windowedX >= 0 && windowedY >= 0, "Verify return value; expected: >= 0, got: %d,%d", windowedX, windowedY);

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
        result = SDL_RestoreWindow(window);
        SDLTest_AssertPass("SDL_RestoreWindow()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    } else {
        SDLTest_Log("Requested minimized window on creation, but that isn't supported on this platform.");
    }

    SDL_DestroyWindow(window);

    return TEST_COMPLETED;
}

static int SDLCALL video_createMaximized(void *arg)
{
    const char *title = "video_createMaximized Test Window";
    int result;
    SDL_Window *window;
    int windowedX, windowedY;
    int windowedW, windowedH;

    /* Call against new test window */
    window = SDL_CreateWindow(title, 320, 200, SDL_WINDOW_MAXIMIZED);
    if (!window) {
        return TEST_ABORTED;
    }

    SDL_GetWindowSize(window, &windowedW, &windowedH);
    SDLTest_AssertPass("SDL_GetWindowSize()");
    SDLTest_AssertCheck(windowedW > 0 && windowedH > 0, "Verify return value; expected: 320x200, got: %dx%d", windowedW, windowedH);

    SDL_GetWindowSizeInPixels(window, &windowedW, &windowedH);
    SDLTest_AssertPass("SDL_GetWindowSizeInPixels()");
    SDLTest_AssertCheck(windowedW > 0 && windowedH > 0, "Verify return value; expected: > 0, got: %dx%d", windowedW, windowedH);

    SDL_GetWindowPosition(window, &windowedX, &windowedY);
    SDLTest_AssertPass("SDL_GetWindowPosition()");
    SDLTest_AssertCheck(windowedX >= 0 && windowedY >= 0, "Verify return value; expected: >= 0, got: %d,%d", windowedX, windowedY);

    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED) {
        result = SDL_RestoreWindow(window);
        SDLTest_AssertPass("SDL_RestoreWindow()");
        SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    } else {
        SDLTest_Log("Requested maximized window on creation, but that isn't supported on this platform.");
    }

    SDL_DestroyWindow(window);

    return TEST_COMPLETED;
}

/**
 * Tests window surface functionality
 */
static int SDLCALL video_getWindowSurface(void *arg)
{
    const char *title = "video_getWindowSurface Test Window";
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
    const char *renderer_name = NULL;
    int result;

    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "dummy") == 0) {
        renderer_name = SDL_SOFTWARE_RENDERER;
    }

    /* Make sure we're testing interaction with an accelerated renderer */
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");

    window = SDL_CreateWindow(title, 320, 320, 0);
    SDLTest_AssertPass("Call to SDL_CreateWindow('Title',320,320,0)");
    SDLTest_AssertCheck(window != NULL, "Validate that returned window is not NULL");

    surface = SDL_GetWindowSurface(window);
    SDLTest_AssertPass("Call to SDL_GetWindowSurface(window)");
    SDLTest_AssertCheck(surface != NULL, "Validate that returned surface is not NULL");
    SDLTest_AssertCheck(SDL_WindowHasSurface(window), "Validate that window has a surface");

    result = SDL_UpdateWindowSurface(window);
    SDLTest_AssertPass("Call to SDL_UpdateWindowSurface(window)");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);

    if (surface) {
        SDL_Rect dirty = { 4, 5, 12, 10 };
        SDLTest_AssertCheck(SDL_FillSurfaceRect(surface, &dirty, 0), "Fill a dirty surface rectangle: %s", SDL_GetError());
        SDLTest_AssertCheck(SDL_UpdateWindowSurfaceRects(window, &dirty, 1), "Present a dirty surface rectangle: %s", SDL_GetError());
        SDLTest_AssertCheck(SDL_GetWindowSurface(window) == surface, "Partial update preserves the surface");
    }

    /* We shouldn't be able to create a renderer on a window with a surface */
    renderer = SDL_CreateRenderer(window, renderer_name);
    SDLTest_AssertPass("Call to SDL_CreateRenderer(window, %s)", renderer_name);
    SDLTest_AssertCheck(renderer == NULL, "Validate that returned renderer is NULL");

    result = SDL_DestroyWindowSurface(window);
    SDLTest_AssertPass("Call to SDL_DestroyWindowSurface(window)");
    SDLTest_AssertCheck(result == true, "Verify return value; expected: true, got: %d", result);
    SDLTest_AssertCheck(!SDL_WindowHasSurface(window), "Validate that window does not have a surface");
    SDLTest_AssertCheck(!SDL_UpdateWindowSurface(window), "Updating a destroyed window surface fails");

    /* We should be able to create a renderer on the window now */
    renderer = SDL_CreateRenderer(window, renderer_name);
    SDLTest_AssertPass("Call to SDL_CreateRenderer(window, %s)", renderer_name);
    SDLTest_AssertCheck(renderer != NULL, "Validate that returned renderer is not NULL");

    /* We should not be able to create a window surface now, unless it was created by the renderer */
    if (!SDL_WindowHasSurface(window)) {
        surface = SDL_GetWindowSurface(window);
        SDLTest_AssertPass("Call to SDL_GetWindowSurface(window)");
        SDLTest_AssertCheck(surface == NULL, "Validate that returned surface is NULL");
    }

    SDL_DestroyRenderer(renderer);
    SDLTest_AssertPass("Call to SDL_DestroyRenderer(renderer)");
    SDLTest_AssertCheck(!SDL_WindowHasSurface(window), "Validate that window does not have a surface");

    /* We should be able to create a window surface again */
    surface = SDL_GetWindowSurface(window);
    SDLTest_AssertPass("Call to SDL_GetWindowSurface(window)");
    SDLTest_AssertCheck(surface != NULL, "Validate that returned surface is not NULL");
    SDLTest_AssertCheck(SDL_WindowHasSurface(window), "Validate that window has a surface");

    /* Clean up */
    SDL_DestroyWindow(window);

    return TEST_COMPLETED;
}

/**
 * Tests SDL_RaiseWindow
 */
static int SDLCALL video_raiseWindow(void *arg)
{
    bool result;
    SDL_Window *window;

    SDLTest_AssertPass("SDL_SetWindowInputFocus is not supported on dummy and SDL2 wayland driver");
        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "dummy") == 0 || SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        return TEST_SKIPPED;
    }

    window = createVideoSuiteTestWindow("video_raiseWindow");
    if (!window) {
        return TEST_ABORTED;
    }

    SDLTest_AssertPass("About to call SDL_RaiseWindow(window)");
    result = SDL_RaiseWindow(window);
    SDLTest_AssertCheck(result, "Result is %d, expected 1 (%s)", result, SDL_GetError());

    destroyVideoSuiteTestWindow(window);

    return TEST_COMPLETED;
}

/* ================= Test References ================== */

/* Video test cases */
static const SDLTest_TestCaseReference videoTestEnableDisableScreensaver = {
    video_enableDisableScreensaver, "video_enableDisableScreensaver", "Enable and disable screenaver while checking state", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestCreateWindowVariousSizes = {
    video_createWindowVariousSizes, "video_createWindowVariousSizes", "Create windows with various sizes", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestCreateWindowVariousFlags = {
    video_createWindowVariousFlags, "video_createWindowVariousFlags", "Create windows using various flags", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestRejectsModalWithoutParent = {
    video_rejectsModalWithoutParent, "video_rejectsModalWithoutParent", "Reject modal windows without a parent", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestWindowHierarchy = {
    video_windowHierarchy, "video_windowHierarchy", "Keep parent and modal state consistent", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestRejectsConflictingGraphicsFlags = {
    video_rejectsConflictingGraphicsFlags, "video_rejectsConflictingGraphicsFlags", "Reject conflicting graphics flags", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestFullscreenEnterLeave = {
    video_testFullscreenEnterLeave, "video_testFullscreenEnterLeave", "Tests fullscreen display selection on X11", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowFlags = {
    video_getWindowFlags, "video_getWindowFlags", "Get window flags set during SDL_CreateWindow", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetFullscreenDisplayModes = {
    video_getFullscreenDisplayModes, "video_getFullscreenDisplayModes", "Use SDL_GetFullscreenDisplayModes function to get number of display modes", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetClosestDisplayModeCurrentResolution = {
    video_getClosestDisplayModeCurrentResolution, "video_getClosestDisplayModeCurrentResolution", "Use function to get closes match to requested display mode for current resolution", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetClosestDisplayModeRandomResolution = {
    video_getClosestDisplayModeRandomResolution, "video_getClosestDisplayModeRandomResolution", "Use function to get closes match to requested display mode for random resolution", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowDisplayMode = {
    video_getWindowDisplayMode, "video_getWindowDisplayMode", "Get window display mode", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowDisplayModeNegative = {
    video_getWindowDisplayModeNegative, "video_getWindowDisplayModeNegative", "Get window display mode with invalid input", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowGrab = {
    video_getSetWindowGrab, "video_getSetWindowGrab", "Checks input grab positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowID = {
    video_getWindowId, "video_getWindowId", "Checks SDL_GetWindowID and SDL_GetWindowFromID", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowPixelFormat = {
    video_getWindowPixelFormat, "video_getWindowPixelFormat", "Checks SDL_GetWindowPixelFormat", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowPosition = {
    video_getSetWindowPosition, "video_getSetWindowPosition", "Checks SDL_GetWindowPosition and SDL_SetWindowPosition positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowSize = {
    video_getSetWindowSize, "video_getSetWindowSize", "Checks SDL_GetWindowSize and SDL_SetWindowSize positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowMinimumSize = {
    video_getSetWindowMinimumSize, "video_getSetWindowMinimumSize", "Checks SDL_GetWindowMinimumSize and SDL_SetWindowMinimumSize positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowMaximumSize = {
    video_getSetWindowMaximumSize, "video_getSetWindowMaximumSize", "Checks SDL_GetWindowMaximumSize and SDL_SetWindowMaximumSize positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowData = {
    video_getSetWindowData, "video_getSetWindowData", "Checks SDL_SetWindowData and SDL_GetWindowData positive and negative cases", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestSetWindowCenteredOnDisplay = {
    video_setWindowCenteredOnDisplay, "video_setWindowCenteredOnDisplay", "Checks using SDL_WINDOWPOS_CENTERED_DISPLAY centers the window on a display", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetSetWindowState = {
    video_getSetWindowState, "video_getSetWindowState", "Checks transitioning between windowed, minimized, maximized, and fullscreen states", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestCreateMinimized = {
    video_createMinimized, "video_createMinimized", "Checks window state for windows created minimized", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestCreateMaximized = {
    video_createMaximized, "video_createMaximized", "Checks window state for windows created maximized", TEST_ENABLED
};

static const SDLTest_TestCaseReference videoTestGetWindowSurface = {
    video_getWindowSurface, "video_getWindowSurface", "Checks window surface functionality", TEST_ENABLED
};
static const SDLTest_TestCaseReference videoTestRaiseWindow = {
    video_raiseWindow, "video_raiseWindow", "Checks window focus", TEST_ENABLED
};

static int SDLCALL video_glContextLifecycle(void *arg)
{
    SDL_Window *gl_window;
    SDL_GLContext context;
    int red_size = 0;

    if (!SDL_GL_LoadLibrary(NULL)) {
        SDLTest_Log("OpenGL loader unavailable: %s", SDL_GetError());
        return TEST_SKIPPED;
    }

    SDLTest_AssertCheck(SDL_GL_GetProcAddress("glGetString") != NULL, "Loaded OpenGL library exposes glGetString");
    gl_window = SDL_CreateWindow("GL context lifecycle", 32, 32, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!SDLTest_AssertCheck(gl_window != NULL, "Create OpenGL window: %s", SDL_GetError())) {
        SDL_GL_UnloadLibrary();
        return TEST_ABORTED;
    }
    context = SDL_GL_CreateContext(gl_window);
    if (!SDLTest_AssertCheck(context != NULL, "Create OpenGL context: %s", SDL_GetError())) {
        SDL_DestroyWindow(gl_window);
        SDL_GL_UnloadLibrary();
        return TEST_ABORTED;
    }

    SDLTest_AssertCheck(SDL_GL_GetCurrentWindow() == gl_window, "New context makes its window current");
    SDLTest_AssertCheck(SDL_GL_GetCurrentContext() == context, "New context becomes current");
    SDLTest_AssertCheck(SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &red_size) && red_size > 0,
                        "Current context reports a red channel: %s", SDL_GetError());
    SDLTest_AssertCheck(SDL_GL_MakeCurrent(NULL, NULL), "Clear current OpenGL context: %s", SDL_GetError());
    SDLTest_AssertCheck(SDL_GL_GetCurrentContext() == NULL, "Current context is cleared");
    SDLTest_AssertCheck(SDL_GL_MakeCurrent(gl_window, context), "Restore current OpenGL context: %s", SDL_GetError());
    SDLTest_AssertCheck(SDL_GL_DestroyContext(context), "Destroy OpenGL context: %s", SDL_GetError());
    SDLTest_AssertCheck(SDL_GL_GetCurrentContext() == NULL, "Destroyed context is no longer current");

    SDL_DestroyWindow(gl_window);
    SDL_GL_UnloadLibrary();
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGLContextLifecycle = {
    video_glContextLifecycle, "video_glContextLifecycle", "Checks OpenGL context lifecycle", TEST_ENABLED
};

static int SDLCALL video_vulkanLoaderReferences(void *arg)
{
    SDL_FunctionPointer proc;

    if (!SDL_Vulkan_LoadLibrary(NULL)) {
        SDLTest_Log("Vulkan loader unavailable: %s", SDL_GetError());
        return TEST_SKIPPED;
    }

    proc = SDL_Vulkan_GetVkGetInstanceProcAddr();
    SDLTest_AssertCheck(proc != NULL, "Loaded Vulkan library exposes vkGetInstanceProcAddr");
    SDLTest_AssertCheck(SDL_Vulkan_LoadLibrary(NULL), "Second Vulkan loader reference succeeds: %s", SDL_GetError());
    SDL_ClearError();
    SDLTest_AssertCheck(!SDL_Vulkan_LoadLibrary("/nonexistent/SDL-vulkan-loader"), "Different loader path is rejected while loaded");
    SDLTest_AssertCheck(SDL_GetError()[0] != '\0', "Different loader path sets an error");
    SDL_Vulkan_UnloadLibrary();
    SDLTest_AssertCheck(SDL_Vulkan_GetVkGetInstanceProcAddr() == proc,
                        "One unload leaves the second loader reference active");
    SDL_Vulkan_UnloadLibrary();
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestVulkanLoaderReferences = {
    video_vulkanLoaderReferences, "video_vulkanLoaderReferences", "Checks Vulkan loader references and path validation", TEST_ENABLED
};

static int SDLCALL video_gpuDriverEnumeration(void *arg)
{
    const int count = SDL_GetNumGPUDrivers();
    SDLTest_AssertCheck(count >= 0, "GPU driver count is nonnegative: %d", count);
    for (int i = 0; i < count; i++) {
        const char *name = SDL_GetGPUDriver(i);
        SDLTest_AssertCheck(name != NULL && name[0] != '\0', "GPU driver %d has a name", i);
    }

    SDL_ClearError();
    SDLTest_AssertCheck(SDL_GetGPUDriver(-1) == NULL, "Negative GPU driver index is rejected");
    SDLTest_AssertCheck(SDL_GetError()[0] != '\0', "Negative GPU driver index sets an error");
    SDL_ClearError();
    SDLTest_AssertCheck(SDL_GetGPUDriver(count) == NULL, "Out-of-range GPU driver index is rejected");
    SDLTest_AssertCheck(SDL_GetError()[0] != '\0', "Out-of-range GPU driver index sets an error");
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGPUDriverEnumeration = {
    video_gpuDriverEnumeration, "video_gpuDriverEnumeration", "Enumerate GPU drivers and reject invalid indices", TEST_ENABLED
};

static int SDLCALL video_gpuDeviceLifecycle(void *arg)
{
    SDL_GPUDevice *device;
    const char *name;

    if (!SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_SPIRV, "vulkan")) {
        SDLTest_Log("Skipping Vulkan device lifecycle: %s", SDL_GetError());
        SDLTest_AssertPass("Vulkan backend is unavailable with this video driver");
        return TEST_COMPLETED;
    }

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
    if (!SDLTest_AssertCheck(device != NULL, "Create Vulkan GPU device: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    SDLTest_AssertCheck(SDL_strcmp(SDL_GetGPUDeviceDriver(device), "vulkan") == 0,
                        "Vulkan device reports its driver");
    SDLTest_AssertCheck((SDL_GetGPUShaderFormats(device) & SDL_GPU_SHADERFORMAT_SPIRV) != 0,
                        "Vulkan device supports SPIR-V");
    name = SDL_GetStringProperty(SDL_GetGPUDeviceProperties(device), SDL_PROP_GPU_DEVICE_NAME_STRING, NULL);
    SDLTest_AssertCheck(name != NULL && name[0] != '\0', "Vulkan device reports its name");
    name = SDL_GetStringProperty(SDL_GetGPUDeviceProperties(device), SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING, NULL);
    SDLTest_AssertCheck(name != NULL && name[0] != '\0', "Vulkan device reports its driver version");

    SDL_ClearError();
    for (int i = 0; i < 2; ++i) {
        SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUFence *fence;
        if (!SDLTest_AssertCheck(command_buffer != NULL, "Acquire Vulkan command buffer %d: %s", i, SDL_GetError())) {
            break;
        }
        fence = SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
        if (!SDLTest_AssertCheck(fence != NULL, "Submit Vulkan command buffer %d: %s", i, SDL_GetError())) {
            break;
        }
        SDLTest_AssertCheck(SDL_WaitForGPUFences(device, true, &fence, 1), "Wait for Vulkan fence %d: %s", i, SDL_GetError());
        SDL_ReleaseGPUFence(device, fence);
    }

    SDL_DestroyGPUDevice(device);

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
    if (!SDLTest_AssertCheck(device != NULL, "Recreate Vulkan GPU device: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }
    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
    if (SDLTest_AssertCheck(command_buffer != NULL, "Acquire command buffer after device recreation: %s", SDL_GetError())) {
        SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
        if (SDLTest_AssertCheck(fence != NULL, "Submit after device recreation: %s", SDL_GetError())) {
            SDLTest_AssertCheck(SDL_WaitForGPUFences(device, true, &fence, 1), "Wait after device recreation: %s", SDL_GetError());
            SDL_ReleaseGPUFence(device, fence);
        }
    }
    SDL_DestroyGPUDevice(device);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGPUDeviceLifecycle = {
    video_gpuDeviceLifecycle, "video_gpuDeviceLifecycle", "Create, inspect, and destroy a Vulkan GPU device", TEST_ENABLED
};

static int SDLCALL video_gpuBufferCopyPass(void *arg)
{
    const Uint8 expected[] = { 0x12, 0x34, 0x56, 0x78 };
    SDL_GPUDevice *device;
    SDL_GPUBuffer *buffer = NULL;
    SDL_GPUTransferBuffer *upload = NULL, *download = NULL;
    SDL_GPUBufferCreateInfo buffer_info = { SDL_GPU_BUFFERUSAGE_VERTEX, sizeof(expected), 0 };
    SDL_GPUTransferBufferCreateInfo transfer_info = { SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, sizeof(expected), 0 };
    SDL_GPUBufferRegion region;
    SDL_GPUTransferBufferLocation location;
    void *mapped;

    if (!SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_SPIRV, "vulkan")) {
        SDLTest_Log("Skipping Vulkan buffer copy: %s", SDL_GetError());
        SDLTest_AssertPass("Vulkan backend is unavailable with this video driver");
        return TEST_COMPLETED;
    }

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
    if (!SDLTest_AssertCheck(device != NULL, "Create Vulkan device for buffer copy: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    buffer = SDL_CreateGPUBuffer(device, &buffer_info);
    upload = SDL_CreateGPUTransferBuffer(device, &transfer_info);
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    download = SDL_CreateGPUTransferBuffer(device, &transfer_info);
    if (!SDLTest_AssertCheck(buffer && upload && download, "Create GPU copy buffers: %s", SDL_GetError())) {
        goto done;
    }

    mapped = SDL_MapGPUTransferBuffer(device, upload, false);
    if (!SDLTest_AssertCheck(mapped != NULL, "Map upload buffer: %s", SDL_GetError())) {
        goto done;
    }
    SDL_memcpy(mapped, expected, sizeof(expected));
    SDL_UnmapGPUTransferBuffer(device, upload);

    region.buffer = buffer;
    region.offset = 0;
    region.size = sizeof(expected);
    location.offset = 0;

    for (int i = 0; i < 2; ++i) {
        SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUCopyPass *copy_pass;
        SDL_GPUFence *fence;
        if (!SDLTest_AssertCheck(command_buffer != NULL, "Acquire copy command buffer %d: %s", i, SDL_GetError())) {
            break;
        }
        copy_pass = SDL_BeginGPUCopyPass(command_buffer);
        if (!SDLTest_AssertCheck(copy_pass != NULL, "Begin copy pass %d: %s", i, SDL_GetError())) {
            SDL_CancelGPUCommandBuffer(command_buffer);
            break;
        }
        location.transfer_buffer = i == 0 ? upload : download;
        if (i == 0) {
            SDL_UploadToGPUBuffer(copy_pass, &location, &region, false);
        } else {
            SDL_DownloadFromGPUBuffer(copy_pass, &region, &location);
        }
        SDL_EndGPUCopyPass(copy_pass);
        fence = SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
        if (!SDLTest_AssertCheck(fence != NULL, "Submit copy pass %d: %s", i, SDL_GetError())) {
            break;
        }
        const bool waited = SDL_WaitForGPUFences(device, true, &fence, 1);
        SDLTest_AssertCheck(waited, "Wait for copy pass %d: %s", i, SDL_GetError());
        SDL_ReleaseGPUFence(device, fence);
        if (!waited) {
            break;
        }
        if (i == 1) {
            mapped = SDL_MapGPUTransferBuffer(device, download, false);
            if (SDLTest_AssertCheck(mapped != NULL, "Map download buffer: %s", SDL_GetError())) {
                SDLTest_AssertCheck(SDL_memcmp(mapped, expected, sizeof(expected)) == 0, "GPU buffer copy preserves bytes");
                SDL_UnmapGPUTransferBuffer(device, download);
            }
        }
    }

done:
    if (download) SDL_ReleaseGPUTransferBuffer(device, download);
    if (upload) SDL_ReleaseGPUTransferBuffer(device, upload);
    if (buffer) SDL_ReleaseGPUBuffer(device, buffer);
    SDL_DestroyGPUDevice(device);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGPUBufferCopyPass = {
    video_gpuBufferCopyPass, "video_gpuBufferCopyPass", "Upload and download bytes through Vulkan copy passes", TEST_ENABLED
};

static int SDLCALL video_gpuBufferAllocationReuse(void *arg)
{
    SDL_GPUBuffer *buffers[24] = { NULL };
    SDL_GPUBufferCreateInfo info = { SDL_GPU_BUFFERUSAGE_VERTEX, 64, 0 };
    SDL_GPUDevice *device;
    const int count = (int)SDL_arraysize(buffers);

    if (!SDL_GPUSupportsShaderFormats(SDL_GPU_SHADERFORMAT_SPIRV, "vulkan")) {
        SDLTest_Log("Skipping Vulkan buffer allocation reuse: %s", SDL_GetError());
        return TEST_SKIPPED;
    }

    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, "vulkan");
    if (!SDLTest_AssertCheck(device != NULL, "Create Vulkan device for buffer reuse: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    for (int i = 0; i < count; ++i) {
        buffers[i] = SDL_CreateGPUBuffer(device, &info);
        if (!SDLTest_AssertCheck(buffers[i] != NULL, "Create GPU buffer %d: %s", i, SDL_GetError())) {
            goto done;
        }
    }
    for (int i = 0; i < count; i += 2) {
        SDL_ReleaseGPUBuffer(device, buffers[i]);
        buffers[i] = NULL;
    }
    for (int i = 0; i < count; i += 2) {
        buffers[i] = SDL_CreateGPUBuffer(device, &info);
        if (!SDLTest_AssertCheck(buffers[i] != NULL, "Recreate GPU buffer %d: %s", i, SDL_GetError())) {
            goto done;
        }
    }

done:
    for (int i = 0; i < count; ++i) {
        if (buffers[i]) {
            SDL_ReleaseGPUBuffer(device, buffers[i]);
        }
    }
    SDL_DestroyGPUDevice(device);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGPUBufferAllocationReuse = {
    video_gpuBufferAllocationReuse, "video_gpuBufferAllocationReuse", "Recreate small Vulkan buffers after release", TEST_ENABLED
};

static int SDLCALL video_gpuBlitPipelineCacheOOM(void *arg)
{
    SDL_GPUDevice device;
    BlitPipelineCacheEntry *pipelines;
    Uint32 pipeline_count = 0;
    Uint32 pipeline_capacity = 2;
    SDL_realloc_func original_realloc;
    SDL_malloc_func original_malloc;
    SDL_calloc_func original_calloc;
    SDL_free_func original_free;

    SDL_zero(device);
    device.CreateGraphicsPipeline = video_create_test_graphics_pipeline;
    device.ReleaseGraphicsPipeline = video_release_test_graphics_pipeline;
    device.default_enable_depth_clip = true;
    pipelines = (BlitPipelineCacheEntry *)SDL_malloc(sizeof(*pipelines) * pipeline_capacity);
    if (!SDLTest_AssertCheck(pipelines != NULL, "Allocate blit pipeline cache: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    SDL_GPU_FetchBlitPipeline(&device, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                              NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity);
    SDL_GPU_FetchBlitPipeline(&device, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
                              NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity);

    SDL_GetOriginalMemoryFunctions(&original_malloc, &original_calloc, &original_realloc, &original_free);
    video_original_realloc = original_realloc;
    video_fail_realloc = true;
    video_realloc_failed = false;
    video_fail_realloc_size = sizeof(*pipelines) * 4;
    video_released_pipeline_count = 0;
    if (!SDL_SetMemoryFunctions(original_malloc, original_calloc, video_test_realloc, original_free)) {
        video_fail_realloc = false;
        SDL_free(pipelines);
        return TEST_ABORTED;
    }

    SDL_GPUGraphicsPipeline *pipeline = SDL_GPU_FetchBlitPipeline(
        &device, SDL_GPU_TEXTURETYPE_2D_ARRAY, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity);

    SDL_SetMemoryFunctions(original_malloc, original_calloc, original_realloc, original_free);
    SDLTest_AssertCheck(video_realloc_failed, "Fail blit pipeline cache expansion realloc");
    SDLTest_AssertCheck(pipeline == NULL, "Return NULL when blit pipeline cache expansion fails");
    SDLTest_AssertCheck(pipeline_count == 2 && pipeline_capacity == 2, "Preserve blit pipeline cache after expansion failure");
    SDLTest_AssertCheck(video_released_pipeline_count == 1, "Release newly created pipeline after cache expansion failure");
    SDL_free(pipelines);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestGPUBlitPipelineCacheOOM = {
    video_gpuBlitPipelineCacheOOM, "video_gpuBlitPipelineCacheOOM", "Release a blit pipeline when cache expansion runs out of memory", TEST_ENABLED
};

static int SDLCALL video_recreateWindowSettings(void *arg)
{
    typedef int (*XFetchNameFn)(void *, unsigned long, char **);
    typedef int (*XFreeFn)(void *);
    const char *title = "Recreated window settings";
    void *x11;
    XFetchNameFn xfetchname;
    XFreeFn xfree;
    SDL_Window *window;
    SDL_Renderer *renderer = NULL;
    SDL_WindowID id;
    unsigned long old_xid, new_xid;
    char *native_title = NULL;
    int min_w = 0, min_h = 0, max_w = 0, max_h = 0;
    float min_aspect = 0.0f, max_aspect = 0.0f;

    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") != 0) {
        return TEST_SKIPPED;
    }

    x11 = SDL_LoadObject("libX11.so.6");
    if (!SDLTest_AssertCheck(x11 != NULL, "Load X11 to inspect native title: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }
    xfetchname = (XFetchNameFn)SDL_LoadFunction(x11, "XFetchName");
    xfree = (XFreeFn)SDL_LoadFunction(x11, "XFree");
    if (!SDLTest_AssertCheck(xfetchname && xfree, "Load X11 title functions: %s", SDL_GetError())) {
        SDL_UnloadObject(x11);
        return TEST_ABORTED;
    }

    window = SDL_CreateWindow(title, 180, 120, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    if (!SDLTest_AssertCheck(window != NULL, "Create window before OpenGL renderer: %s", SDL_GetError())) {
        SDL_UnloadObject(x11);
        return TEST_ABORTED;
    }
    id = SDL_GetWindowID(window);
    old_xid = (unsigned long)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (!SDLTest_AssertCheck(SDL_SetWindowMinimumSize(window, 80, 70) &&
                             SDL_SetWindowMaximumSize(window, 400, 300) &&
                             SDL_SetWindowAspectRatio(window, 1.0f, 2.0f),
                             "Set window constraints before recreation: %s", SDL_GetError())) {
        SDL_DestroyWindow(window);
        SDL_UnloadObject(x11);
        return TEST_ABORTED;
    }

    renderer = SDL_CreateRenderer(window, "opengl");
    if (SDLTest_AssertCheck(renderer != NULL, "Create OpenGL renderer and recreate window: %s", SDL_GetError())) {
        SDLTest_AssertCheck((SDL_GetWindowFlags(window) & SDL_WINDOW_OPENGL) != 0, "Recreated window has OpenGL flag");
        SDLTest_AssertCheck(SDL_GetWindowID(window) == id, "Recreated window keeps its ID");
        SDLTest_AssertCheck(SDL_strcmp(SDL_GetWindowTitle(window), title) == 0, "Recreated window keeps its title");
        new_xid = (unsigned long)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        SDLTest_AssertCheck(old_xid != 0 && new_xid != 0 && new_xid != old_xid, "OpenGL renderer recreated the native X11 window");
        if (new_xid != 0) {
            void *display = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
            SDLTest_AssertCheck(display && xfetchname(display, new_xid, &native_title) && native_title &&
                                SDL_strcmp(native_title, title) == 0, "Recreated X11 window has the native title");
            if (native_title) {
                xfree(native_title);
            }
        }
        SDL_GetWindowMinimumSize(window, &min_w, &min_h);
        SDL_GetWindowMaximumSize(window, &max_w, &max_h);
        SDL_GetWindowAspectRatio(window, &min_aspect, &max_aspect);
        SDLTest_AssertCheck(min_w == 80 && min_h == 70 && max_w == 400 && max_h == 300,
                            "Recreated window keeps size constraints");
        SDLTest_AssertCheck(min_aspect == 1.0f && max_aspect == 2.0f, "Recreated window keeps aspect limits");
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_UnloadObject(x11);
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestRecreateWindowSettings = {
    video_recreateWindowSettings, "video_recreateWindowSettings", "Preserve window settings when OpenGL renderer recreates a window", TEST_ENABLED
};

static int SDLCALL video_destroyWindowUnlinksList(void *arg)
{
    SDL_Window *first = SDL_CreateWindow("first", 32, 32, SDL_WINDOW_HIDDEN);
    SDL_Window *middle = SDL_CreateWindow("middle", 32, 32, SDL_WINDOW_HIDDEN);
    SDL_Window *last = SDL_CreateWindow("last", 32, 32, SDL_WINDOW_HIDDEN);
    SDL_WindowID first_id, middle_id, last_id;

    if (!SDLTest_AssertCheck(first && middle && last, "Create three windows for list removal: %s", SDL_GetError())) {
        if (last) SDL_DestroyWindow(last);
        if (middle) SDL_DestroyWindow(middle);
        if (first) SDL_DestroyWindow(first);
        return TEST_ABORTED;
    }

    first_id = SDL_GetWindowID(first);
    middle_id = SDL_GetWindowID(middle);
    last_id = SDL_GetWindowID(last);
    SDL_DestroyWindow(middle);
    SDLTest_AssertCheck(SDL_GetWindowFromID(middle_id) == NULL, "Removed middle window is absent");
    SDLTest_AssertCheck(SDL_GetWindowFromID(first_id) == first && SDL_GetWindowFromID(last_id) == last,
                        "Windows on both sides remain reachable");

    SDL_DestroyWindow(last);
    SDLTest_AssertCheck(SDL_GetWindowFromID(last_id) == NULL && SDL_GetWindowFromID(first_id) == first,
                        "Removing list head preserves remaining window");
    SDL_DestroyWindow(first);
    SDLTest_AssertCheck(SDL_GetWindowFromID(first_id) == NULL, "Final window is absent after destruction");
    return TEST_COMPLETED;
}

static const SDLTest_TestCaseReference videoTestDestroyWindowUnlinksList = {
    video_destroyWindowUnlinksList, "video_destroyWindowUnlinksList", "Remove middle and head windows without losing neighbors", TEST_ENABLED
};

/* Sequence of Video test cases */
static const SDLTest_TestCaseReference *videoTests[] = {
    &videoTestEnableDisableScreensaver,
    &videoTestCreateWindowVariousSizes,
    &videoTestCreateWindowVariousFlags,
    &videoTestRejectsModalWithoutParent,
    &videoTestWindowHierarchy,
    &videoTestRejectsConflictingGraphicsFlags,
    &videoTestFullscreenEnterLeave,
    &videoTestGetWindowFlags,
    &videoTestGetFullscreenDisplayModes,
    &videoTestDisplayModeOrderingAndMatch,
    &videoTestDisplayRegistry,
    &videoTestGetClosestDisplayModeCurrentResolution,
    &videoTestGetClosestDisplayModeRandomResolution,
    &videoTestGetWindowDisplayMode,
    &videoTestGetWindowDisplayModeNegative,
    &videoTestGetSetWindowGrab,
    &videoTestGetWindowID,
    &videoTestGetWindowPixelFormat,
    &videoTestGetSetWindowPosition,
    &videoTestGetSetWindowSize,
    &videoTestGetSetWindowMinimumSize,
    &videoTestGetSetWindowMaximumSize,
    &videoTestGetSetWindowData,
    &videoTestSetWindowCenteredOnDisplay,
    &videoTestGetSetWindowState,
    &videoTestWaylandShowHideLifecycle,
    &videoTestWaylandPopupShowHideLifecycle,
    &videoTestCreateMinimized,
    &videoTestCreateMaximized,
    &videoTestGetWindowSurface,
    &videoTestRaiseWindow,
    &videoTestGLContextLifecycle,
    &videoTestVulkanLoaderReferences,
    &videoTestGPUDriverEnumeration,
    &videoTestGPUDeviceLifecycle,
    &videoTestGPUBufferCopyPass,
    &videoTestGPUBufferAllocationReuse,
    &videoTestGPUBlitPipelineCacheOOM,
    &videoTestRecreateWindowSettings,
    &videoTestDestroyWindowUnlinksList,
    NULL
};

/* Video test suite (global) */
SDLTest_TestSuiteReference videoTestSuite = {
    "Video",
    NULL,
    videoTests,
    NULL
};
