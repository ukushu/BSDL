/* Verify tray click context lifetime without requiring a desktop or tray host. */
#include "../src/tray/SDL_tray_utils.h"
#undef NDEBUG
#include <assert.h>

static bool SDLCALL nested_click(void *userdata, SDL_Tray *tray)
{
    int x = 0, y = 0;
    assert(SDL_GetTrayClickPosition(tray, &x, &y));
    assert(x == -30 && y == 40);
    assert(SDL_strcmp(SDL_GetTrayActivationToken(), "nested-token") == 0);
    assert(SDL_strcmp(SDL_GetTrayClickActivationToken(tray), "nested-token") == 0);
    return false;
}

static bool SDLCALL outer_click(void *userdata, SDL_Tray *tray)
{
    SDL_Tray *nested = (SDL_Tray *)SDL_malloc(1);
    SDL_Point position = { -30, 40 };
    int x = 0, y = 0;
    assert(nested);
    SDL_RegisterTray(nested);
    assert(!SDL_InvokeTrayClick(nested, nested_click, NULL, &position, "nested-token"));
    SDL_UnregisterTray(nested);
    SDL_free(nested);
    assert(SDL_GetTrayClickPosition(tray, &x, &y));
    assert(x == 10 && y == -20);
    assert(SDL_strcmp(SDL_GetTrayActivationToken(), "outer-token") == 0);
    assert(SDL_strcmp(SDL_GetTrayClickActivationToken(tray), "outer-token") == 0);
    return true;
}

static bool SDLCALL destroy_in_callback(void *userdata, SDL_Tray *tray)
{
    int x = 7, y = 8;
    assert(!SDL_GetTrayClickPosition(tray, &x, &y));
    assert(x == 7 && y == 8);
    SDL_UnregisterTray(tray);
    SDL_free(tray);
    return true;
}

int main(int argc, char **argv)
{
    SDL_Tray *tray;
    SDL_Point position = { 10, -20 };
    int x = 7, y = 8;
    SDL_SetMainReady();
    assert(SDL_Init(0));
    SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0");
    tray = (SDL_Tray *)SDL_malloc(1);
    assert(tray);
    SDL_RegisterTray(tray);
    assert(!SDL_GetTrayClickPosition(tray, &x, &y));
    assert(x == 7 && y == 8);
    assert(SDL_InvokeTrayClick(tray, outer_click, NULL, &position, "outer-token"));
    assert(!SDL_GetTrayClickPosition(tray, &x, &y));
    assert(!SDL_GetTrayActivationToken());
    assert(SDL_InvokeTrayClick(tray, destroy_in_callback, NULL, NULL, NULL));
    assert(!SDL_GetTrayActivationToken());
    assert(SDL_GetActiveTrayCount() == 0);
    SDL_Quit();
    return 0;
}
