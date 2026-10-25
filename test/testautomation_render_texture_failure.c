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

#include "../src/dynapi/SDL_dynapi.h"
#ifdef SDL_DYNAMIC_API
#undef SDL_DYNAMIC_API
#endif
#define SDL_DYNAMIC_API 0
#include "../src/SDL_internal.h"
#include "../src/render/SDL_sysrender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>

static int render_textureUploadFailureCount;

static bool SDLCALL render_failTextureUpload(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    (void)renderer;
    (void)texture;
    (void)rect;
    (void)pixels;
    (void)pitch;
    ++render_textureUploadFailureCount;
    return SDL_SetError("Controlled texture upload failure");
}

static int SDLCALL render_testTextureFromSurfaceUploadFailure(void *arg)
{
    SDL_Surface *target = NULL;
    SDL_Surface *surface = NULL;
    SDL_Surface *destination = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    bool (*update_texture)(SDL_Renderer *, SDL_Texture *, const SDL_Rect *, const void *, int) = NULL;
    const char *expected_error = "Controlled texture upload failure";
    bool completed = false;
    int upload_count;

    target = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_ARGB8888);
    if (!SDLTest_AssertCheck(target != NULL, "Create software renderer target: %s", SDL_GetError())) {
        goto done;
    }
    renderer = SDL_CreateSoftwareRenderer(target);
    if (!SDLTest_AssertCheck(renderer != NULL, "Create software renderer: %s", SDL_GetError())) {
        goto done;
    }
    update_texture = renderer->UpdateTexture;
    renderer->UpdateTexture = render_failTextureUpload;

    surface = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_ARGB8888);
    if (!SDLTest_AssertCheck(surface != NULL, "Create direct source surface: %s", SDL_GetError())) {
        goto done;
    }
    render_textureUploadFailureCount = 0;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    upload_count = render_textureUploadFailureCount;
    SDLTest_AssertCheck(texture == NULL && upload_count == 1, "Propagate direct upload failure");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), expected_error) == 0,
                        "Preserve direct upload error; expected '%s', got '%s'", expected_error, SDL_GetError());
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_DestroySurface(surface);
    surface = NULL;

    surface = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_ARGB8888);
    destination = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_ARGB8888);
    if (!SDLTest_AssertCheck(surface && destination, "Create locked source and destination surfaces: %s", SDL_GetError())) {
        goto done;
    }
    if (!SDLTest_AssertCheck(SDL_SetSurfaceRLE(surface, true), "Enable RLE for locked source: %s", SDL_GetError()) ||
        !SDLTest_AssertCheck(SDL_BlitSurface(surface, NULL, destination, NULL), "RLE encode source surface: %s", SDL_GetError())) {
        goto done;
    }
    if (!SDLTest_AssertCheck(SDL_MUSTLOCK(surface), "Prepare RLE source surface for locked upload")) {
        goto done;
    }
    render_textureUploadFailureCount = 0;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    upload_count = render_textureUploadFailureCount;
    SDLTest_AssertCheck(texture == NULL && upload_count == 1, "Propagate locked upload failure");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), expected_error) == 0,
                        "Preserve locked upload error; expected '%s', got '%s'", expected_error, SDL_GetError());
    SDLTest_AssertCheck((surface->flags & SDL_SURFACE_LOCKED) == 0, "Unlock source surface after failed upload");
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_DestroySurface(surface);
    surface = NULL;
    SDL_DestroySurface(destination);
    destination = NULL;

    surface = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_ARGB8888);
    if (!SDLTest_AssertCheck(surface != NULL, "Create converted source surface: %s", SDL_GetError())) {
        goto done;
    }
    if (!SDLTest_AssertCheck(SDL_SetSurfaceColorKey(surface, true, 0), "Set source color key: %s", SDL_GetError())) {
        goto done;
    }
    render_textureUploadFailureCount = 0;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    upload_count = render_textureUploadFailureCount;
    SDLTest_AssertCheck(texture == NULL && upload_count == 1, "Propagate converted upload failure");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), expected_error) == 0,
                        "Preserve converted upload error; expected '%s', got '%s'", expected_error, SDL_GetError());
    completed = true;

done:
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
    SDL_DestroySurface(destination);
    if (renderer) {
        if (update_texture) {
            renderer->UpdateTexture = update_texture;
        }
        SDL_DestroyRenderer(renderer);
    }
    SDL_DestroySurface(target);
    return completed ? TEST_COMPLETED : TEST_ABORTED;
}

const SDLTest_TestCaseReference renderTestTextureFromSurfaceUploadFailure = {
    render_testTextureFromSurfaceUploadFailure,
    "render_testTextureFromSurfaceUploadFailure",
    "Propagate texture upload failures from direct, locked, and converted surfaces",
    TEST_ENABLED
};
