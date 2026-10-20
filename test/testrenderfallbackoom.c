#include <SDL3/SDL.h>
#include <stdio.h>

static SDL_malloc_func original_malloc;
static SDL_calloc_func original_calloc;
static SDL_realloc_func original_realloc;
static SDL_free_func original_free;
static bool fail_next_allocation;

static void *SDLCALL test_malloc(size_t size)
{
    if (fail_next_allocation) {
        fail_next_allocation = false;
        return NULL;
    }
    return original_malloc(size);
}

static void *SDLCALL test_calloc(size_t count, size_t size)
{
    if (fail_next_allocation) {
        fail_next_allocation = false;
        return NULL;
    }
    return original_calloc(count, size);
}

static void *SDLCALL test_realloc(void *ptr, size_t size)
{
    if (fail_next_allocation) {
        fail_next_allocation = false;
        return NULL;
    }
    return original_realloc(ptr, size);
}

static void SDLCALL test_free(void *ptr)
{
    original_free(ptr);
}

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "failed at line %d: %s (%s)\n", __LINE__, #condition, SDL_GetError()); \
        return 1; \
    } \
} while (0)

int main(int argc, char **argv)
{
    SDL_Surface *surface;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_PropertiesID props;

    (void)argc;
    (void)argv;

    SDL_GetOriginalMemoryFunctions(&original_malloc, &original_calloc, &original_realloc, &original_free);
    CHECK(SDL_Init(0));

    CHECK(SDL_SetMemoryFunctions(test_malloc, test_calloc, test_realloc, test_free));
    fail_next_allocation = true;
    SDL_ClearError();
    CHECK(SDL_CreateProperties() == 0);
    CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
    CHECK(SDL_SetMemoryFunctions(original_malloc, original_calloc, original_realloc, original_free));

    surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
    CHECK(surface != NULL);
    renderer = SDL_CreateSoftwareRenderer(surface);
    CHECK(renderer != NULL);
    props = SDL_CreateProperties();
    CHECK(props != 0);
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_RGB24));
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_STATIC));
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, 1));
    CHECK(SDL_SetNumberProperty(props, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, 1));

    CHECK(SDL_SetMemoryFunctions(test_malloc, test_calloc, test_realloc, test_free));
    fail_next_allocation = true;
    SDL_ClearError();
    CHECK(!SDL_SetNumberProperty(props, "test.property", 1));
    CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
    CHECK(SDL_SetMemoryFunctions(original_malloc, original_calloc, original_realloc, original_free));

    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(texture != NULL);
    SDL_DestroyTexture(texture);

    SDL_DestroyProperties(props);
    SDL_DestroyRenderer(renderer);
    SDL_DestroySurface(surface);
    SDL_Quit();
    return 0;
}
