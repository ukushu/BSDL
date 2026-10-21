#include "../src/dynapi/SDL_dynapi.h"
#ifdef SDL_DYNAMIC_API
#undef SDL_DYNAMIC_API
#endif
#define SDL_DYNAMIC_API 0
#include "../src/SDL_internal.h"
#include "../src/gpu/SDL_sysgpu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_test.h>
#include "../src/gpu/SDL_gpu_blit.c"

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

int SDLCALL video_gpuBlitPipelineCacheOOM(void *arg)
{
    SDL_GPUDevice device;
    BlitPipelineCacheEntry *pipelines;
    BlitPipelineCacheEntry *original_pipelines;
    Uint32 pipeline_count = 0;
    Uint32 pipeline_capacity = 2;
    SDL_realloc_func original_realloc;
    SDL_malloc_func original_malloc;
    SDL_calloc_func original_calloc;
    SDL_free_func original_free;
    SDL_GPUGraphicsPipeline *pipeline;

    (void)arg;
    SDL_zero(device);
    device.CreateGraphicsPipeline = video_create_test_graphics_pipeline;
    device.ReleaseGraphicsPipeline = video_release_test_graphics_pipeline;
    device.default_enable_depth_clip = true;
    pipelines = (BlitPipelineCacheEntry *)SDL_malloc(sizeof(*pipelines) * pipeline_capacity);
    if (!SDLTest_AssertCheck(pipelines != NULL, "Allocate blit pipeline cache: %s", SDL_GetError())) {
        return TEST_ABORTED;
    }

    if (!SDLTest_AssertCheck(SDL_GPU_FetchBlitPipeline(
            &device, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity) != NULL,
            "Cache first blit pipeline")) {
        SDL_free(pipelines);
        return TEST_ABORTED;
    }
    if (!SDLTest_AssertCheck(SDL_GPU_FetchBlitPipeline(
            &device, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
            NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity) != NULL,
            "Cache second blit pipeline")) {
        SDL_free(pipelines);
        return TEST_ABORTED;
    }
    original_pipelines = pipelines;

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

    pipeline = SDL_GPU_FetchBlitPipeline(
        &device, SDL_GPU_TEXTURETYPE_2D_ARRAY, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        NULL, NULL, NULL, NULL, NULL, NULL, &pipelines, &pipeline_count, &pipeline_capacity);

    SDLTest_AssertCheck(SDL_SetMemoryFunctions(original_malloc, original_calloc, original_realloc, original_free),
                        "Restore SDL memory functions after injected OOM");
    video_fail_realloc = false;
    SDLTest_AssertCheck(video_realloc_failed, "Fail blit pipeline cache expansion realloc");
    SDLTest_AssertCheck(pipeline == NULL, "Return NULL when blit pipeline cache expansion fails");
    SDLTest_AssertCheck(SDL_strcmp(SDL_GetError(), "Out of memory") == 0,
                        "Preserve the allocation error after releasing the uncached pipeline");
    SDLTest_AssertCheck(pipelines == original_pipelines && pipeline_count == 2 && pipeline_capacity == 2,
                        "Preserve cache allocation and capacity after expansion failure");
    SDLTest_AssertCheck(pipelines[0].pipeline == (SDL_GPUGraphicsPipeline *)&video_test_pipeline_storage &&
                        pipelines[0].type == SDL_GPU_TEXTURETYPE_2D &&
                        pipelines[0].format == SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM &&
                        pipelines[1].pipeline == (SDL_GPUGraphicsPipeline *)&video_test_pipeline_storage &&
                        pipelines[1].type == SDL_GPU_TEXTURETYPE_2D &&
                        pipelines[1].format == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
                        "Preserve cached pipeline entries after expansion failure");
    SDLTest_AssertCheck(video_released_pipeline_count == 1, "Release newly created pipeline after cache expansion failure");
    SDL_free(pipelines);
    return TEST_COMPLETED;
}
