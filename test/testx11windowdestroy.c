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

#include <SDL3/SDL.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

static Display *native_display;
static struct
{
    Window window;
    Window marker;
    Window survivor;
    int sequence;
    int sibling_event;
    int pending_event;
    int destroy_event;
} ordering;
static Window expected_bad_window_resource;
static int expected_bad_window;
static int unexpected_errors;
static Display *unexpected_display;
static XErrorEvent unexpected_error;
static Display *racer_display;
static Window racer_window;
static SDL_AtomicInt racer_errors;

typedef struct
{
    SDL_AtomicInt ready;
} ErrorThreadData;

typedef struct
{
    Window window;
    Atom property;
    Atom value;
} DestroyWindowData;

static bool SDLCALL X11EventHook(void *userdata, XEvent *event)
{
    (void)userdata;

    if (event->type == DestroyNotify && event->xdestroywindow.window == ordering.marker) {
        ordering.sibling_event = ++ordering.sequence;
    } else if (event->type == ClientMessage && event->xclient.window == ordering.survivor && event->xclient.data.l[0] == 0x53444c) {
        ordering.pending_event = ++ordering.sequence;
    } else if (event->type == DestroyNotify && event->xdestroywindow.window == ordering.window) {
        ordering.destroy_event = ++ordering.sequence;
    }
    return true;
}

static int DestroyWindowThread(void *userdata)
{
    DestroyWindowData *data = userdata;
    Display *display;

    SDL_Delay(100);
    display = XOpenDisplay(NULL);
    if (display) {
        XChangeProperty(display, data->window, data->property, XA_ATOM, 32, PropModeReplace, (unsigned char *)&data->value, 1);
        XDestroyWindow(display, data->window);
        XSync(display, False);
        XCloseDisplay(display);
    }
    return display ? 0 : -1;
}

static int X11_TestErrorHandler(Display *display, XErrorEvent *event)
{
    if (event->error_code == BadWindow && display == racer_display && event->resourceid == racer_window) {
        SDL_AddAtomicInt(&racer_errors, 1);
    } else if (event->error_code == BadWindow && display == native_display && event->resourceid == expected_bad_window_resource) {
        ++expected_bad_window;
    } else {
        ++unexpected_errors;
        unexpected_display = display;
        unexpected_error = *event;
    }
    return 0;
}

static int X11ErrorThread(void *userdata)
{
    ErrorThreadData *data = userdata;
    Display *display = XOpenDisplay(NULL);
    Window window;

    if (!display) {
        SDL_SetAtomicInt(&data->ready, -1);
        return -1;
    }

    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    XDestroyWindow(display, window);
    XSync(display, False);
    racer_display = display;
    racer_window = window;
    SDL_SetAtomicInt(&data->ready, 1);
    for (int i = 0; i < 1000; ++i) {
        XMapWindow(display, window);
        XSync(display, False);
    }
    XCloseDisplay(display);
    return 0;
}

static bool DestroyWindowBeforeNotify(SDL_Window *window)
{
    Display *display;
    Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);

    if (!xwindow) {
        return false;
    }
    display = XOpenDisplay(NULL);
    if (!display) {
        return false;
    }
    XDestroyWindow(display, xwindow);
    XSync(display, False);
    XCloseDisplay(display);
    SDL_DestroyWindow(window);
    XSync(native_display, False);
    return true;
}

static bool DestroyWindowExternally(SDL_Window *window, SDL_Window *survivor)
{
    DestroyWindowData destroy_data;
    SDL_Event event;
    XEvent pending_event;
    SDL_Thread *thread;
    int status;
    bool close_requested = false;
    Uint64 deadline;

    destroy_data.window = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (!destroy_data.window) {
        return false;
    }
    ordering.window = destroy_data.window;
    destroy_data.property = XInternAtom(native_display, "_NET_WM_STATE", False);
    destroy_data.value = XInternAtom(native_display, "_NET_WM_STATE_HIDDEN", False);
    ordering.survivor = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(survivor), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    if (!ordering.survivor) {
        return false;
    }
    ordering.sequence = 0;
    ordering.sibling_event = 0;
    ordering.pending_event = 0;
    ordering.destroy_event = 0;
    ordering.marker = XCreateSimpleWindow(native_display, DefaultRootWindow(native_display), 0, 0, 1, 1, 0, 0, 0);
    XSelectInput(native_display, ordering.marker, StructureNotifyMask);
    XDestroyWindow(native_display, ordering.marker);
    XSync(native_display, False);
    SDL_zero(pending_event);
    pending_event.xclient.type = ClientMessage;
    pending_event.xclient.display = native_display;
    pending_event.xclient.window = ordering.survivor;
    pending_event.xclient.format = 32;
    pending_event.xclient.data.l[0] = 0x53444c;
    if (!XSendEvent(native_display, ordering.survivor, False, StructureNotifyMask, &pending_event)) {
        return false;
    }
    XSync(native_display, False);
    thread = SDL_CreateThread(DestroyWindowThread, "X11DestroyWindow", &destroy_data);
    if (!thread) {
        return false;
    }

    deadline = SDL_GetTicksNS() + 5000000000ULL;
    while (!close_requested && SDL_GetTicksNS() < deadline) {
        if (!SDL_WaitEventTimeout(&event, 1000)) {
            continue;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
            close_requested = true;
        }
    }
    SDL_WaitThread(thread, &status);
    if (!close_requested || !ordering.sibling_event || !ordering.pending_event || !ordering.destroy_event || ordering.sibling_event >= ordering.pending_event || ordering.pending_event >= ordering.destroy_event || status != 0) {
        SDL_Log("external destroy: close=%d, sibling event order=%d, pending event order=%d, destroy event order=%d, thread status=%d", close_requested, ordering.sibling_event, ordering.pending_event, ordering.destroy_event, status);
    }
    return close_requested && ordering.sibling_event && ordering.pending_event && ordering.destroy_event && ordering.sibling_event < ordering.pending_event && ordering.pending_event < ordering.destroy_event && status == 0;
}

int main(int argc, char **argv)
{
    XErrorHandler previous_error_handler;
    XErrorHandler restored_error_handler;
    XErrorHandler default_error_handler;
    ErrorThreadData error_thread_data;
    SDL_Thread *error_thread;
    SDL_Window *window;
    SDL_Window *default_window;
    SDL_Window *immediate_window;
    SDL_Window *survivor_window;
    bool handler_restored;
    bool default_handler_restored;
    bool immediate_destroy_ok;
    bool immediate_handler_restored;
    bool close_requested;
    bool default_close_requested;
    bool survivor_shown = false;
    Uint64 deadline;
    SDL_Event event;

    (void)argc;
    (void)argv;

    SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "x11", SDL_HINT_OVERRIDE);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 77;
    }

    window = SDL_CreateWindow("X11 external destroy test", 100, 100, 0);
    if (!window) {
        SDL_Quit();
        return 1;
    }

    previous_error_handler = XSetErrorHandler(X11_TestErrorHandler);
    native_display = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
    if (!SDL_SetWindowMouseGrab(window, true)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    if (!native_display) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }

    survivor_window = SDL_CreateWindow("X11 surviving window test", 100, 100, 0);
    if (!survivor_window || !SDL_ShowWindow(survivor_window)) {
        SDL_DestroyWindow(survivor_window);
        SDL_DestroyWindow(window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    deadline = SDL_GetTicksNS() + 5000000000ULL;
    while (!survivor_shown && SDL_GetTicksNS() < deadline) {
        if (SDL_WaitEventTimeout(&event, 1000) && event.type == SDL_EVENT_WINDOW_SHOWN && event.window.windowID == SDL_GetWindowID(survivor_window)) {
            survivor_shown = true;
        }
    }
    while (SDL_PollEvent(&event)) {
    }
    if (!survivor_shown) {
        SDL_DestroyWindow(survivor_window);
        SDL_DestroyWindow(window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    SDL_SetX11EventHook(X11EventHook, NULL);

    expected_bad_window_resource = XCreateSimpleWindow(native_display, DefaultRootWindow(native_display), 0, 0, 1, 1, 0, 0, 0);
    XDestroyWindow(native_display, expected_bad_window_resource);
    XSync(native_display, False);
    XMapWindow(native_display, expected_bad_window_resource);
    XSync(native_display, False);

    XSetErrorHandler(NULL);
    default_error_handler = XSetErrorHandler(X11_TestErrorHandler);

    close_requested = DestroyWindowExternally(window, survivor_window);

    SDL_SetAtomicInt(&error_thread_data.ready, 0);
    error_thread = SDL_CreateThread(X11ErrorThread, "X11ErrorRacer", &error_thread_data);
    if (!error_thread) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    while (SDL_GetAtomicInt(&error_thread_data.ready) == 0) {
        SDL_Delay(1);
    }
    SDL_DestroyWindow(window);
    int error_thread_status;
    SDL_WaitThread(error_thread, &error_thread_status);
    restored_error_handler = XSetErrorHandler(previous_error_handler);
    handler_restored = restored_error_handler == X11_TestErrorHandler;

    if (XSetErrorHandler(X11_TestErrorHandler) != previous_error_handler) {
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }

    immediate_window = SDL_CreateWindow("X11 destroy before notify test", 100, 100, 0);
    if (!immediate_window) {
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    XSetErrorHandler(NULL);
    immediate_destroy_ok = DestroyWindowBeforeNotify(immediate_window);
    immediate_handler_restored = XSetErrorHandler(X11_TestErrorHandler) == default_error_handler;
    SDL_PumpEvents();

    default_window = SDL_CreateWindow("X11 external destroy default handler test", 100, 100, 0);
    if (!default_window) {
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    if (!SDL_SetWindowMouseGrab(default_window, true)) {
        SDL_DestroyWindow(survivor_window);
        SDL_DestroyWindow(default_window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }

    default_close_requested = DestroyWindowExternally(default_window, survivor_window);
    SDL_SetX11EventHook(NULL, NULL);

    if (!SDL_SetWindowTitle(survivor_window, "X11 surviving window test") || !SDL_GetWindowID(survivor_window)) {
        SDL_DestroyWindow(default_window);
        SDL_DestroyWindow(survivor_window);
        SDL_Quit();
        XSetErrorHandler(previous_error_handler);
        return 1;
    }
    XSync(native_display, False);

    XSetErrorHandler(NULL);
    SDL_DestroyWindow(default_window);
    SDL_DestroyWindow(survivor_window);
    restored_error_handler = XSetErrorHandler(X11_TestErrorHandler);
    default_handler_restored = (restored_error_handler == default_error_handler);
    SDL_Quit();
    XSetErrorHandler(previous_error_handler);

    if (!close_requested || expected_bad_window != 1 || unexpected_errors != 0 || SDL_GetAtomicInt(&racer_errors) != 1000 || !handler_restored || !immediate_destroy_ok || !immediate_handler_restored || !default_close_requested || !default_handler_restored || error_thread_status != 0) {
        SDL_Log("close=%d, expected BadWindow=%d, unexpected errors=%d, error=%u, restored handler=%d, immediate destroy=%d, immediate handler restored=%d, default close=%d, default restored handler=%d, display=%p (native=%p), resource=0x%lx, serial=%lu, request=%u.%u",
                close_requested, expected_bad_window, unexpected_errors, (unsigned int)unexpected_error.error_code, handler_restored,
                immediate_destroy_ok, immediate_handler_restored, default_close_requested, default_handler_restored,
                (void *)unexpected_display, (void *)native_display,
                unexpected_error.resourceid, unexpected_error.serial,
                (unsigned int)unexpected_error.request_code, (unsigned int)unexpected_error.minor_code);
        return 1;
    }
    return 0;
}
