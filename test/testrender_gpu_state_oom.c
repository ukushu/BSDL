#include "../src/dynapi/SDL_dynapi.h"
#ifdef SDL_DYNAMIC_API
#undef SDL_DYNAMIC_API
#endif
#define SDL_DYNAMIC_API 0
#include "../src/SDL_internal.h"
#include "../src/render/SDL_sysrender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>

#define SDL_CreateGPURenderState test_CreateGPURenderState
#define SDL_SetGPURenderStateSamplerBindings test_SetGPURenderStateSamplerBindings
#define SDL_SetGPURenderStateStorageTextures test_SetGPURenderStateStorageTextures
#define SDL_SetGPURenderStateStorageBuffers test_SetGPURenderStateStorageBuffers
#define SDL_SetGPURenderStateFragmentUniforms test_SetGPURenderStateFragmentUniforms
#define SDL_SetGPURenderState test_SetGPURenderState
#define SDL_DestroyGPURenderState test_DestroyGPURenderState
static void test_DestroyGPURenderState(SDL_GPURenderState *state);
#include "../src/render/SDL_render_gpu_state.c"
#undef SDL_CreateGPURenderState
#undef SDL_SetGPURenderStateSamplerBindings
#undef SDL_SetGPURenderStateStorageTextures
#undef SDL_SetGPURenderStateStorageBuffers
#undef SDL_SetGPURenderStateFragmentUniforms
#undef SDL_SetGPURenderState
#undef SDL_DestroyGPURenderState

bool SDL_object_validation = false;
bool SDL_FindObject(void *object, SDL_ObjectType type)
{
    (void)object;
    (void)type;
    return true;
}

bool SDL_FlushRenderCommandsIfGPURenderStateNeeded(SDL_GPURenderState *state)
{
    (void)state;
    return true;
}

static SDL_malloc_func test_original_malloc;
static SDL_calloc_func test_original_calloc;
static SDL_realloc_func test_original_realloc;
static SDL_free_func test_original_free;
static bool test_fail_malloc;
static bool test_malloc_failed;
static bool test_fail_realloc;
static bool test_realloc_failed;
static size_t test_realloc_fail_size;
static void *test_allocated_data;
static bool test_data_freed;

static void *SDLCALL test_malloc(size_t size)
{
    if (test_fail_malloc && size == sizeof(Uint32)) {
        test_fail_malloc = false;
        test_malloc_failed = true;
        return NULL;
    }
    void *ptr = test_original_malloc(size);
    if (size == sizeof(Uint32)) {
        test_allocated_data = ptr;
    }
    return ptr;
}

static void *SDLCALL test_realloc(void *ptr, size_t size)
{
    if (test_fail_realloc && size == test_realloc_fail_size) {
        test_fail_realloc = false;
        test_realloc_failed = true;
        return NULL;
    }
    void *new_ptr = test_original_realloc(ptr, size);
    if (new_ptr && ptr == test_allocated_data) {
        test_allocated_data = new_ptr;
    }
    return new_ptr;
}

static void SDLCALL test_free(void *ptr)
{
    if (ptr == test_allocated_data) {
        test_allocated_data = NULL;
        test_data_freed = true;
    }
    test_original_free(ptr);
}

static SDL_GPURenderState *test_create_render_state(void)
{
    SDL_GPURenderState *state = (SDL_GPURenderState *)test_original_malloc(sizeof(*state));
    if (state) {
        SDL_zero(*state);
    }
    return state;
}

int main(int argc, char **argv)
{
    SDL_GPURenderState *state;
    Uint32 value = 42;
    Uint32 second_value = 84;
    SDL_GPURenderStateUniformBuffer *original_cache;
    (void)argc;
    (void)argv;

    SDL_GetOriginalMemoryFunctions(&test_original_malloc, &test_original_calloc, &test_original_realloc, &test_original_free);
    state = test_create_render_state();
    if (!SDLTest_AssertCheck(state != NULL, "Allocate test render state")) {
        return 1;
    }
    SDL_zero(*state);
    if (!SDL_SetMemoryFunctions(test_malloc, test_original_calloc, test_realloc, test_free)) {
        test_original_free(state);
        return 1;
    }

    test_fail_malloc = true;
    SDLTest_AssertCheck(!test_SetGPURenderStateFragmentUniforms(state, 0, &value, sizeof(value)), "OOM is reported");
    SDLTest_AssertCheck(test_malloc_failed, "Inject OOM while allocating uniform data");
    SDLTest_AssertCheck(state->uniform_buffers == NULL && state->num_uniform_buffers == 0, "State remains unchanged after OOM");
    test_DestroyGPURenderState(state);

    state = test_create_render_state();
    if (!SDLTest_AssertCheck(state != NULL, "Allocate render state for existing-cache malloc failure")) {
        return 1;
    }
    SDLTest_AssertCheck(test_SetGPURenderStateFragmentUniforms(state, 0, &value, sizeof(value)), "Add uniform before existing-cache malloc failure");
    original_cache = state->uniform_buffers;
    test_fail_malloc = true;
    test_malloc_failed = false;
    SDLTest_AssertCheck(!test_SetGPURenderStateFragmentUniforms(state, 1, &second_value, sizeof(second_value)), "OOM adding a second uniform is reported");
    SDLTest_AssertCheck(test_malloc_failed, "Inject data allocation OOM with a populated cache");
    SDLTest_AssertCheck(state->uniform_buffers == original_cache && state->num_uniform_buffers == 1, "Populated cache remains unchanged after data allocation OOM");
    SDLTest_AssertCheck(state->uniform_buffers[0].slot_index == 0 && state->uniform_buffers[0].length == sizeof(value) && SDL_memcmp(state->uniform_buffers[0].data, &value, sizeof(value)) == 0, "Cached uniform survives data allocation OOM");
    test_data_freed = false;
    test_DestroyGPURenderState(state);
    SDLTest_AssertCheck(test_data_freed, "Destroy frees populated cache after data allocation OOM");

    state = test_create_render_state();
    if (!SDLTest_AssertCheck(state != NULL, "Allocate render state for cache realloc failure")) {
        return 1;
    }
    SDLTest_AssertCheck(test_SetGPURenderStateFragmentUniforms(state, 0, &value, sizeof(value)), "Add uniform before cache realloc failure");
    original_cache = state->uniform_buffers;
    test_fail_realloc = true;
    test_realloc_failed = false;
    test_realloc_fail_size = 2 * sizeof(SDL_GPURenderStateUniformBuffer);
    test_data_freed = false;
    SDLTest_AssertCheck(!test_SetGPURenderStateFragmentUniforms(state, 1, &second_value, sizeof(second_value)), "Cache realloc OOM is reported");
    SDLTest_AssertCheck(test_realloc_failed, "Inject OOM while expanding uniform buffer cache");
    SDLTest_AssertCheck(test_data_freed, "Temporary uniform data is freed after cache realloc fails");
    SDLTest_AssertCheck(state->uniform_buffers == original_cache && state->num_uniform_buffers == 1, "Populated cache remains unchanged after cache realloc OOM");
    SDLTest_AssertCheck(state->uniform_buffers[0].slot_index == 0 && state->uniform_buffers[0].length == sizeof(value) && SDL_memcmp(state->uniform_buffers[0].data, &value, sizeof(value)) == 0, "Cached uniform survives cache realloc OOM");
    SDLTest_AssertCheck(test_SetGPURenderStateFragmentUniforms(state, 1, &second_value, sizeof(second_value)), "Second uniform can be added after cache realloc OOM");
    SDLTest_AssertCheck(state->num_uniform_buffers == 2 && state->uniform_buffers[0].slot_index == 0 && state->uniform_buffers[1].slot_index == 1, "Successful retry preserves both uniform slots");
    SDLTest_AssertCheck(SDL_memcmp(state->uniform_buffers[0].data, &value, sizeof(value)) == 0 && SDL_memcmp(state->uniform_buffers[1].data, &second_value, sizeof(second_value)) == 0, "Successful retry preserves both uniform values");
    test_data_freed = false;
    test_DestroyGPURenderState(state);
    SDLTest_AssertCheck(test_data_freed, "Destroy frees retried uniform data after cache realloc OOM");

    state = test_create_render_state();
    if (!SDLTest_AssertCheck(state != NULL, "Allocate render state for uniform updates")) {
        return 1;
    }
    test_data_freed = false;
    SDLTest_AssertCheck(test_SetGPURenderStateFragmentUniforms(state, 0, &value, sizeof(value)), "Uniforms can be set after OOM");
    Uint64 updated_value = 0x123456789ABCDEF0;
    test_fail_realloc = true;
    test_realloc_failed = false;
    test_realloc_fail_size = sizeof(updated_value);
    SDLTest_AssertCheck(!test_SetGPURenderStateFragmentUniforms(state, 0, &updated_value, sizeof(updated_value)), "OOM while updating an existing uniform is reported");
    SDLTest_AssertCheck(test_realloc_failed, "Inject OOM while growing existing uniform data");
    SDLTest_AssertCheck(state->num_uniform_buffers == 1 && state->uniform_buffers[0].length == sizeof(value), "Existing uniform metadata survives OOM");
    SDLTest_AssertCheck(SDL_memcmp(state->uniform_buffers[0].data, &value, sizeof(value)) == 0, "Existing uniform data survives OOM");
    SDLTest_AssertCheck(test_SetGPURenderStateFragmentUniforms(state, 0, &updated_value, sizeof(updated_value)), "Existing uniform can be updated after OOM");
    SDLTest_AssertCheck(state->uniform_buffers[0].length == sizeof(updated_value) && SDL_memcmp(state->uniform_buffers[0].data, &updated_value, sizeof(updated_value)) == 0, "Successful retry stores updated uniform data");
    test_DestroyGPURenderState(state);
    SDLTest_AssertCheck(test_data_freed, "Destroy frees uniform data");
    SDLTest_AssertCheck(SDL_SetMemoryFunctions(test_original_malloc, test_original_calloc, test_original_realloc, test_original_free), "Restore original memory functions");
    return 0;
}
