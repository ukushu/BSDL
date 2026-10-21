#include <SDL3/SDL.h>
#include <stdio.h>

static SDL_malloc_func original_malloc;
static SDL_calloc_func original_calloc;
static SDL_realloc_func original_realloc;
static SDL_free_func original_free;

static bool tracking_allocations;
static bool allocation_failure_triggered;
static bool allocation_tracking_overflowed;
static void *tracked_allocations[128];
static size_t num_tracked_allocations;
static size_t calloc_failure_call;
static size_t calloc_calls;
static size_t malloc_failure_size;
static size_t malloc_failure_occurrence;
static size_t matching_malloc_calls;

static void TrackAllocation(void *ptr)
{
    if (!tracking_allocations || !ptr) {
        return;
    }
    if (num_tracked_allocations < SDL_arraysize(tracked_allocations)) {
        tracked_allocations[num_tracked_allocations++] = ptr;
    } else {
        allocation_tracking_overflowed = true;
    }
}

static bool UntrackAllocation(void *ptr)
{
    for (size_t i = 0; i < num_tracked_allocations; ++i) {
        if (tracked_allocations[i] == ptr) {
            tracked_allocations[i] = tracked_allocations[--num_tracked_allocations];
            return true;
        }
    }
    return false;
}

static void BeginAllocationTracking(size_t fail_calloc, size_t fail_malloc, size_t fail_malloc_nth)
{
    num_tracked_allocations = 0;
    calloc_calls = 0;
    matching_malloc_calls = 0;
    calloc_failure_call = fail_calloc;
    malloc_failure_size = fail_malloc;
    malloc_failure_occurrence = fail_malloc_nth;
    allocation_failure_triggered = false;
    allocation_tracking_overflowed = false;
    tracking_allocations = true;
}

static void *SDLCALL test_malloc(size_t size)
{
    if (tracking_allocations && size == malloc_failure_size && malloc_failure_occurrence > 0 &&
        ++matching_malloc_calls == malloc_failure_occurrence) {
        allocation_failure_triggered = true;
        return NULL;
    }

    void *ptr = original_malloc(size);
    TrackAllocation(ptr);
    return ptr;
}

static void *SDLCALL test_calloc(size_t count, size_t size)
{
    if (tracking_allocations && calloc_failure_call > 0 && ++calloc_calls == calloc_failure_call) {
        allocation_failure_triggered = true;
        return NULL;
    }

    void *ptr = original_calloc(count, size);
    TrackAllocation(ptr);
    return ptr;
}

static void *SDLCALL test_realloc(void *ptr, size_t size)
{
    const bool was_tracked = tracking_allocations && UntrackAllocation(ptr);
    void *new_ptr = original_realloc(ptr, size);
    if (new_ptr && was_tracked) {
        TrackAllocation(new_ptr);
    } else if (new_ptr && tracking_allocations && !ptr) {
        TrackAllocation(new_ptr);
    } else if (!new_ptr && was_tracked) {
        TrackAllocation(ptr);
    }
    return new_ptr;
}

static void SDLCALL test_free(void *ptr)
{
    if (tracking_allocations) {
        UntrackAllocation(ptr);
    }
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
    static const struct {
        const char *name;
        size_t occurrence;
    } property_keys[] = {
        { SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, 1 },
        { SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, 1 },
        { SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, 2 },
        { SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, 1 },
        { SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, 3 }
    };
    SDL_Surface *surface;
    SDL_Renderer *renderer;
    SDL_PropertiesID props;
    SDL_Texture *texture;

    (void)argc;
    (void)argv;

    SDL_GetOriginalMemoryFunctions(&original_malloc, &original_calloc, &original_realloc, &original_free);
    CHECK(SDL_Init(0));
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

    // Initialize renderer and object-property internals before fault injection.
    texture = SDL_CreateTextureWithProperties(renderer, props);
    CHECK(texture != NULL);
    SDL_DestroyTexture(texture);

    CHECK(SDL_SetMemoryFunctions(test_malloc, test_calloc, test_realloc, test_free));

    CHECK(SDL_SetRenderLogicalPresentation(renderer, 1, 1, SDL_LOGICAL_PRESENTATION_LETTERBOX));
    {
        const SDL_FPoint points[100] = { { 0.0f, 0.0f } };
        const size_t xy_size = sizeof(float) * 4 * 2 * SDL_arraysize(points);
        const size_t indices_size = sizeof(int) * ((4 * 3 * (SDL_arraysize(points) - 1)) + (2 * 3 * SDL_arraysize(points)));
        const size_t failure_sizes[] = { xy_size, indices_size };

        for (size_t i = 0; i < SDL_arraysize(failure_sizes); ++i) {
            BeginAllocationTracking(0, failure_sizes[i], 1);
            SDL_ClearError();
            CHECK(!SDL_RenderLines(renderer, points, SDL_arraysize(points)));
            tracking_allocations = false;
            CHECK(allocation_failure_triggered);
            CHECK(!allocation_tracking_overflowed && num_tracked_allocations == 0);
            CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
        }
    }

    // The first calloc creates the texture; the second creates fallback properties.
    BeginAllocationTracking(2, 0, 0);
    SDL_ClearError();
    texture = SDL_CreateTextureWithProperties(renderer, props);
    tracking_allocations = false;
    CHECK(texture == NULL);
    CHECK(allocation_failure_triggered);
    CHECK(!allocation_tracking_overflowed && num_tracked_allocations == 0);
    CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);

    // SDL_strdup allocates each key by length; shared lengths use setter order.
    for (size_t i = 0; i < SDL_arraysize(property_keys); ++i) {
        const size_t key_size = SDL_strlen(property_keys[i].name) + 1;

        BeginAllocationTracking(0, key_size, property_keys[i].occurrence);
        SDL_ClearError();
        texture = SDL_CreateTextureWithProperties(renderer, props);
        tracking_allocations = false;
        CHECK(texture == NULL);
        CHECK(allocation_failure_triggered);
        CHECK(!allocation_tracking_overflowed && num_tracked_allocations == 0);
        CHECK(SDL_strcmp(SDL_GetError(), "Out of memory") == 0);
    }

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
