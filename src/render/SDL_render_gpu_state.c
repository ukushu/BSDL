/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_sysrender.h"
#include "SDL_render_internal.h"

SDL_GPURenderState *SDL_CreateGPURenderState(SDL_Renderer *renderer, const SDL_GPURenderStateCreateInfo *createinfo)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    CHECK_PARAM(!createinfo) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    CHECK_PARAM(!createinfo->fragment_shader) {
        SDL_SetError("A fragment_shader is required");
        return NULL;
    }

    SDL_GPUDevice *device = (SDL_GPUDevice *)SDL_GetPointerProperty(renderer->props, SDL_PROP_RENDERER_GPU_DEVICE_POINTER, NULL);
    if (!device) {
        SDL_SetError("Renderer isn't associated with a GPU device");
        return NULL;
    }

    SDL_GPURenderState *state = (SDL_GPURenderState *)SDL_calloc(1, sizeof(*state));
    if (!state) {
        return NULL;
    }

    state->renderer = renderer;
    state->fragment_shader = createinfo->fragment_shader;

    if (createinfo->num_sampler_bindings > 0) {
        state->sampler_bindings = (SDL_GPUTextureSamplerBinding *)SDL_calloc(createinfo->num_sampler_bindings, sizeof(*state->sampler_bindings));
        if (!state->sampler_bindings) {
            SDL_DestroyGPURenderState(state);
            return NULL;
        }
        SDL_memcpy(state->sampler_bindings, createinfo->sampler_bindings, createinfo->num_sampler_bindings * sizeof(*state->sampler_bindings));
        state->num_sampler_bindings = createinfo->num_sampler_bindings;
    }

    if (createinfo->num_storage_textures > 0) {
        state->storage_textures = (SDL_GPUTexture **)SDL_calloc(createinfo->num_storage_textures, sizeof(*state->storage_textures));
        if (!state->storage_textures) {
            SDL_DestroyGPURenderState(state);
            return NULL;
        }
        SDL_memcpy(state->storage_textures, createinfo->storage_textures, createinfo->num_storage_textures * sizeof(*state->storage_textures));
        state->num_storage_textures = createinfo->num_storage_textures;
    }

    if (createinfo->num_storage_buffers > 0) {
        state->storage_buffers = (SDL_GPUBuffer **)SDL_calloc(createinfo->num_storage_buffers, sizeof(*state->storage_buffers));
        if (!state->storage_buffers) {
            SDL_DestroyGPURenderState(state);
            return NULL;
        }
        SDL_memcpy(state->storage_buffers, createinfo->storage_buffers, createinfo->num_storage_buffers * sizeof(*state->storage_buffers));
        state->num_storage_buffers = createinfo->num_storage_buffers;
    }

    return state;
}

bool SDL_SetGPURenderStateSamplerBindings(SDL_GPURenderState *state, int num_sampler_bindings, const SDL_GPUTextureSamplerBinding *sampler_bindings)
{
    if (!state) {
        return SDL_InvalidParamError("state");
    }

    if (!SDL_FlushRenderCommandsIfGPURenderStateNeeded(state)) {
        return false;
    }

    Sint32 length = sizeof(SDL_GPUTextureSamplerBinding) * num_sampler_bindings;
    SDL_GPUTextureSamplerBinding *new_sampler_bindings = (SDL_GPUTextureSamplerBinding *)SDL_realloc(state->sampler_bindings, length);
    if (!new_sampler_bindings) {
        return false;
    }
    SDL_memcpy(new_sampler_bindings, sampler_bindings, length);
    state->num_sampler_bindings = num_sampler_bindings;
    state->sampler_bindings = new_sampler_bindings;

    return true;
}

bool SDL_SetGPURenderStateStorageTextures(SDL_GPURenderState *state, int num_storage_textures, SDL_GPUTexture *const *storage_textures)
{
    if (!state) {
        return SDL_InvalidParamError("state");
    }

    if (!SDL_FlushRenderCommandsIfGPURenderStateNeeded(state)) {
        return false;
    }

    Sint32 length = sizeof(SDL_GPUTexture *) * num_storage_textures;
    SDL_GPUTexture **new_storage_textures = (SDL_GPUTexture **)SDL_realloc(state->storage_textures, length);
    if (!new_storage_textures) {
        return false;
    }
    SDL_memcpy(new_storage_textures, storage_textures, length);
    state->num_storage_textures = num_storage_textures;
    state->storage_textures = new_storage_textures;

    return true;
}

bool SDL_SetGPURenderStateStorageBuffers(SDL_GPURenderState *state, int num_storage_buffers, SDL_GPUBuffer *const *storage_buffers)
{
    if (!state) {
        return SDL_InvalidParamError("state");
    }

    if (!SDL_FlushRenderCommandsIfGPURenderStateNeeded(state)) {
        return false;
    }

    Sint32 length = sizeof(SDL_GPUBuffer *) * num_storage_buffers;
    SDL_GPUBuffer **new_storage_buffers = (SDL_GPUBuffer **)SDL_realloc(state->storage_buffers, length);
    if (!new_storage_buffers) {
        return false;
    }
    SDL_memcpy(new_storage_buffers, storage_buffers, length);
    state->num_storage_buffers = num_storage_buffers;
    state->storage_buffers = new_storage_buffers;

    return true;
}

bool SDL_SetGPURenderStateFragmentUniforms(SDL_GPURenderState *state, Uint32 slot_index, const void *data, Uint32 length)
{
    if (!state) {
        return SDL_InvalidParamError("state");
    }

    if (!SDL_FlushRenderCommandsIfGPURenderStateNeeded(state)) {
        return false;
    }

    for (int i = 0; i < state->num_uniform_buffers; i++) {
        SDL_GPURenderStateUniformBuffer *buffer = &state->uniform_buffers[i];
        if (buffer->slot_index == slot_index) {
            void *new_data = SDL_realloc(buffer->data, length);
            if (!new_data) {
                return false;
            }
            SDL_memcpy(new_data, data, length);
            buffer->data = new_data;
            buffer->length = length;
            return true;
        }
    }

    SDL_GPURenderStateUniformBuffer *buffers = (SDL_GPURenderStateUniformBuffer *)SDL_realloc(state->uniform_buffers, (state->num_uniform_buffers + 1) * sizeof(*state->uniform_buffers));
    if (!buffers) {
        return false;
    }
    state->uniform_buffers = buffers;

    SDL_GPURenderStateUniformBuffer *buffer = &buffers[state->num_uniform_buffers];
    buffer->slot_index = slot_index;
    buffer->length = length;
    buffer->data = SDL_malloc(length);
    if (!buffer->data) {
        return false;
    }
    SDL_memcpy(buffer->data, data, length);

    ++state->num_uniform_buffers;
    return true;
}

bool SDL_SetGPURenderState(SDL_Renderer *renderer, SDL_GPURenderState *state)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->gpu_render_state = state;
    return true;
}

void SDL_DestroyGPURenderState(SDL_GPURenderState *state)
{
    if (!state) {
        return;
    }

    SDL_FlushRenderCommandsIfGPURenderStateNeeded(state);

    if (state->num_uniform_buffers > 0) {
        for (int i = 0; i < state->num_uniform_buffers; i++) {
            SDL_free(state->uniform_buffers[i].data);
        }
        SDL_free(state->uniform_buffers);
    }
    SDL_free(state->sampler_bindings);
    SDL_free(state->storage_textures);
    SDL_free(state->storage_buffers);
    SDL_free(state);
}
