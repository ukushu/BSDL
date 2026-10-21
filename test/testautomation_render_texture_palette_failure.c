/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>
*/

#include "../src/dynapi/SDL_dynapi.h"
#ifdef SDL_DYNAMIC_API
#undef SDL_DYNAMIC_API
#endif
#define SDL_DYNAMIC_API 0
#include "../src/SDL_internal.h"
#include "../src/render/SDL_sysrender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>

static bool SDLCALL render_failCreatePalette(SDL_Renderer *renderer, SDL_TexturePalette *palette)
{
    (void)renderer;
    (void)palette;
    return SDL_SetError("Controlled palette creation failure");
}

static int SDLCALL render_testTexturePaletteFailure(void *arg)
{
    SDL_Surface *surface = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Palette *palette = NULL;
    SDL_PropertiesID props = 0;
    SDL_Texture *texture = NULL;
    bool (*create_palette)(SDL_Renderer *, SDL_TexturePalette *) = NULL;
    int initial_palette_refcount;
    bool completed = false;

    surface = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_ARGB8888);
    if (!SDLTest_AssertCheck(surface != NULL, "Create renderer target: %s", SDL_GetError())) {
        goto done;
    }
    renderer = SDL_CreateSoftwareRenderer(surface);
    if (!SDLTest_AssertCheck(renderer != NULL, "Create software renderer: %s", SDL_GetError())) {
        goto done;
    }
    palette = SDL_CreatePalette(256);
    if (!SDLTest_AssertCheck(palette != NULL, "Create texture palette: %s", SDL_GetError())) {
        goto done;
    }
    initial_palette_refcount = palette->refcount;
    create_palette = renderer->CreatePalette;
    renderer->CreatePalette = render_failCreatePalette;

    props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_INDEX8);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, 2);
    SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, 2);
    SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, palette);
    texture = SDL_CreateTextureWithProperties(renderer, props);
    SDLTest_AssertCheck(texture == NULL, "Reject palette creation failure");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), "Controlled palette creation failure") == 0,
                        "Preserve palette creation error: %s", SDL_GetError());
    SDLTest_AssertCheck(palette->refcount == initial_palette_refcount, "Release palette reference after backend failure");
    SDLTest_AssertCheck(renderer->textures == NULL, "Clean up texture after palette failure");
    completed = true;

done:
    SDL_DestroyTexture(texture);
    SDL_DestroyProperties(props);
    if (renderer) {
        if (create_palette) {
            renderer->CreatePalette = create_palette;
        }
        SDL_DestroyRenderer(renderer);
    }
    SDL_DestroyPalette(palette);
    SDL_DestroySurface(surface);
    return completed ? TEST_COMPLETED : TEST_ABORTED;
}

const SDLTest_TestCaseReference renderTestTexturePaletteFailure = {
    render_testTexturePaletteFailure,
    "render_testTexturePaletteFailure",
    "Propagate texture palette creation failures",
    TEST_ENABLED
};
