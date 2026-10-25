/* Regression test for propagating fullscreen desktop-mode restore failures. */
#include <SDL3/SDL.h>

#define CHECK(condition) do { if (!(condition)) { SDL_Log("Check failed at line %d: %s (%s)", __LINE__, #condition, SDL_GetError()); goto done; } } while (0)

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_DisplayMode **modes = NULL;
    const SDL_DisplayMode *desktop_mode;
    const SDL_DisplayMode *current_mode;
    int mode_count = 0;
    bool initialized = false;
    bool found_mode = false;
    int result = 1;

    (void)argc;
    (void)argv;

    CHECK(SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "offscreen", SDL_HINT_OVERRIDE));
    CHECK(SDL_Init(SDL_INIT_VIDEO));
    initialized = true;

    window = SDL_CreateWindow("Fullscreen restore failure", 640, 480, 0);
    CHECK(window != NULL);

    SDL_DisplayID display = SDL_GetDisplayForWindow(window);
    desktop_mode = SDL_GetDesktopDisplayMode(display);
    modes = SDL_GetFullscreenDisplayModes(display, &mode_count);
    CHECK(desktop_mode != NULL && modes != NULL);
    for (int i = 0; i < mode_count; ++i) {
        if (modes[i]->w != desktop_mode->w || modes[i]->h != desktop_mode->h) {
            CHECK(SDL_SetWindowFullscreenMode(window, modes[i]));
            found_mode = true;
            break;
        }
    }
    CHECK(found_mode);
    CHECK(SDL_SetWindowFullscreen(window, true));

    CHECK(SDL_SetHint("SDL_VIDEO_OFFSCREEN_FAIL_DESKTOP_MODE_RESTORE", "1"));
    CHECK(!SDL_SetWindowFullscreen(window, false));
    CHECK(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
    current_mode = SDL_GetCurrentDisplayMode(display);
    CHECK(current_mode && current_mode->w != desktop_mode->w);

    CHECK(SDL_SetHint("SDL_VIDEO_OFFSCREEN_FAIL_DESKTOP_MODE_RESTORE", "0"));
    CHECK(SDL_SetWindowFullscreen(window, false));
    CHECK(!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
    current_mode = SDL_GetCurrentDisplayMode(display);
    CHECK(current_mode && current_mode->w == desktop_mode->w && current_mode->h == desktop_mode->h);
    result = 0;

done:
    SDL_SetHint("SDL_VIDEO_OFFSCREEN_FAIL_DESKTOP_MODE_RESTORE", "0");
    SDL_free(modes);
    SDL_DestroyWindow(window);
    if (initialized) {
        SDL_Quit();
    }
    return result;
}
