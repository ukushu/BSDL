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
#include <stdio.h>

static SDL_malloc_func palette_oom_original_malloc;
static SDL_calloc_func palette_oom_original_calloc;
static SDL_realloc_func palette_oom_original_realloc;
static SDL_free_func palette_oom_original_free;
static bool palette_oom_fail_texture_palette;
static bool palette_oom_failure_triggered;
static bool palette_oom_fail_next_calloc;
static bool palette_oom_hash_failure_triggered;
static bool (*palette_oom_original_create_palette)(SDL_Renderer *, SDL_TexturePalette *);

static void *SDLCALL palette_oom_malloc(size_t size)
{
    return palette_oom_original_malloc(size);
}

static void *SDLCALL palette_oom_calloc(size_t nmemb, size_t size)
{
    if (palette_oom_fail_next_calloc) {
        palette_oom_fail_next_calloc = false;
        palette_oom_hash_failure_triggered = true;
        return NULL;
    }
    if (palette_oom_fail_texture_palette && nmemb == 1 && size == sizeof(SDL_TexturePalette)) {
        palette_oom_fail_texture_palette = false;
        palette_oom_failure_triggered = true;
        return NULL;
    }
    return palette_oom_original_calloc(nmemb, size);
}

static bool SDLCALL palette_oom_create_palette_then_fail_hash_insert(SDL_Renderer *renderer, SDL_TexturePalette *palette)
{
    if (!palette_oom_original_create_palette(renderer, palette)) {
        return false;
    }
    palette_oom_fail_next_calloc = true;
    return true;
}

static void *SDLCALL palette_oom_realloc(void *ptr, size_t size)
{
    return palette_oom_original_realloc(ptr, size);
}

static void SDLCALL palette_oom_free(void *ptr)
{
    palette_oom_original_free(ptr);
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "failed at line %d: %s (%s)\n", __LINE__, #condition, SDL_GetError()); \
        result = 1; \
        goto done; \
    } \
} while (0)

int main(int argc, char **argv)
{
    SDL_Surface *surface = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Palette *palette = NULL;
    SDL_PropertiesID props = 0;
    SDL_Texture *texture = NULL;
    SDL_Palette *hash_palettes[4] = { NULL, NULL, NULL, NULL };
    SDL_Texture *hash_textures[3] = { NULL, NULL, NULL };
    int initial_palette_refcount;
    int result = 0;

    (void)argc;
    (void)argv;

    SDL_GetOriginalMemoryFunctions(&palette_oom_original_malloc, &palette_oom_original_calloc,
                                   &palette_oom_original_realloc, &palette_oom_original_free);
    CHECK(SDL_Init(0));

    surface = SDL_CreateSurface(16, 16, SDL_PIXELFORMAT_ARGB8888);
    CHECK(surface != NULL);
    renderer = SDL_CreateSoftwareRenderer(surface);
    CHECK(renderer != NULL);
    palette = SDL_CreatePalette(256);
    CHECK(palette != NULL);
    initial_palette_refcount = palette->refcount;
    props = SDL_CreateProperties();
    CHECK(props != 0);
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_INDEX8));
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, 2));
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, 2));
    CHECK(SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, palette));

    CHECK(SDL_SetMemoryFunctions(palette_oom_malloc, palette_oom_calloc,
                                 palette_oom_realloc, palette_oom_free));
    palette_oom_fail_texture_palette = true;
    SDL_ClearError();
    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(SDL_SetMemoryFunctions(palette_oom_original_malloc, palette_oom_original_calloc,
                                 palette_oom_original_realloc, palette_oom_original_free));

    CHECK(palette_oom_failure_triggered);
    CHECK(texture == NULL);
    CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
    CHECK(palette->refcount == initial_palette_refcount);
    CHECK(renderer->textures == NULL);

    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(texture != NULL);
    CHECK(SDL_GetTexturePalette(texture) == palette);
    CHECK(palette->refcount == initial_palette_refcount + 1);
    SDL_DestroyTexture(texture);
    texture = NULL;
    CHECK(palette->refcount == initial_palette_refcount);
    CHECK(renderer->textures == NULL);

    // Fill the renderer's initial four-entry palette table to its resize threshold.
    for (size_t i = 0; i < SDL_arraysize(hash_textures); ++i) {
        hash_palettes[i] = SDL_CreatePalette(256);
        CHECK(hash_palettes[i] != NULL);
        CHECK(SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, hash_palettes[i]));
        hash_textures[i] = SDL_CreateTextureWithProperties(renderer, props);
        CHECK(hash_textures[i] != NULL);
    }
    hash_palettes[3] = SDL_CreatePalette(256);
    CHECK(hash_palettes[3] != NULL);
    CHECK(SDL_SetPointerProperty(props, SDL_PROP_TEXTURE_CREATE_PALETTE_POINTER, hash_palettes[3]));

    palette_oom_original_create_palette = renderer->CreatePalette;
    renderer->CreatePalette = palette_oom_create_palette_then_fail_hash_insert;
    palette_oom_hash_failure_triggered = false;
    CHECK(SDL_SetMemoryFunctions(palette_oom_malloc, palette_oom_calloc,
                                 palette_oom_realloc, palette_oom_free));
    SDL_ClearError();
    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(SDL_SetMemoryFunctions(palette_oom_original_malloc, palette_oom_original_calloc,
                                 palette_oom_original_realloc, palette_oom_original_free));
    renderer->CreatePalette = palette_oom_original_create_palette;

    CHECK(palette_oom_hash_failure_triggered);
    CHECK(texture == NULL);
    CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
    CHECK(hash_palettes[3]->refcount == 1);
    CHECK(renderer->textures == hash_textures[2]);

    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(texture != NULL);
    CHECK(SDL_GetTexturePalette(texture) == hash_palettes[3]);
    SDL_DestroyTexture(texture);
    texture = NULL;
    CHECK(hash_palettes[3]->refcount == 1);
    CHECK(renderer->textures == hash_textures[2]);

done:
    SDL_SetMemoryFunctions(palette_oom_original_malloc, palette_oom_original_calloc,
                           palette_oom_original_realloc, palette_oom_original_free);
    SDL_DestroyTexture(texture);
    SDL_DestroyProperties(props);
    if (renderer && palette_oom_original_create_palette) {
        renderer->CreatePalette = palette_oom_original_create_palette;
    }
    for (size_t i = 0; i < SDL_arraysize(hash_textures); ++i) {
        SDL_DestroyTexture(hash_textures[i]);
    }
    SDL_DestroyRenderer(renderer);
    for (size_t i = 0; i < SDL_arraysize(hash_palettes); ++i) {
        SDL_DestroyPalette(hash_palettes[i]);
    }
    SDL_DestroyPalette(palette);
    SDL_DestroySurface(surface);
    SDL_Quit();
    return result;
}
