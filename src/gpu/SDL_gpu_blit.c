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
#include "SDL_internal.h"
#include "SDL_sysgpu.h"

// Internal Utility Functions

SDL_GPUGraphicsPipeline *SDL_GPU_FetchBlitPipeline(
    SDL_GPUDevice *device,
    SDL_GPUTextureType source_texture_type,
    SDL_GPUTextureFormat destination_format,
    SDL_GPUShader *blit_vertex_shader,
    SDL_GPUShader *blit_from_2d_shader,
    SDL_GPUShader *blit_from_2d_array_shader,
    SDL_GPUShader *blit_from_3d_shader,
    SDL_GPUShader *blit_from_cube_shader,
    SDL_GPUShader *blit_from_cube_array_shader,
    BlitPipelineCacheEntry **blit_pipelines,
    Uint32 *blit_pipeline_count,
    Uint32 *blit_pipeline_capacity)
{
    SDL_GPUGraphicsPipelineCreateInfo blit_pipeline_create_info;
    SDL_GPUColorTargetDescription color_target_desc;
    SDL_GPUGraphicsPipeline *pipeline;

    if (blit_pipeline_count == NULL) {
        // use pre-created, format-agnostic pipelines
        return (*blit_pipelines)[source_texture_type].pipeline;
    }

    for (Uint32 i = 0; i < *blit_pipeline_count; i += 1) {
        if ((*blit_pipelines)[i].type == source_texture_type && (*blit_pipelines)[i].format == destination_format) {
            return (*blit_pipelines)[i].pipeline;
        }
    }

    // No pipeline found, we'll need to make one!
    SDL_zero(blit_pipeline_create_info);

    SDL_zero(color_target_desc);
    color_target_desc.blend_state.color_write_mask = 0xF;
    color_target_desc.format = destination_format;

    blit_pipeline_create_info.target_info.color_target_descriptions = &color_target_desc;
    blit_pipeline_create_info.target_info.num_color_targets = 1;
    blit_pipeline_create_info.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM; // arbitrary
    blit_pipeline_create_info.target_info.has_depth_stencil_target = false;

    blit_pipeline_create_info.vertex_shader = blit_vertex_shader;
    if (source_texture_type == SDL_GPU_TEXTURETYPE_CUBE) {
        blit_pipeline_create_info.fragment_shader = blit_from_cube_shader;
    } else if (source_texture_type == SDL_GPU_TEXTURETYPE_CUBE_ARRAY) {
        blit_pipeline_create_info.fragment_shader = blit_from_cube_array_shader;
    }  else if (source_texture_type == SDL_GPU_TEXTURETYPE_2D_ARRAY) {
        blit_pipeline_create_info.fragment_shader = blit_from_2d_array_shader;
    } else if (source_texture_type == SDL_GPU_TEXTURETYPE_3D) {
        blit_pipeline_create_info.fragment_shader = blit_from_3d_shader;
    } else {
        blit_pipeline_create_info.fragment_shader = blit_from_2d_shader;
    }
    blit_pipeline_create_info.rasterizer_state.enable_depth_clip = device->default_enable_depth_clip;

    blit_pipeline_create_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    blit_pipeline_create_info.multisample_state.enable_mask = false;

    blit_pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    pipeline = SDL_CreateGPUGraphicsPipeline(
        device,
        &blit_pipeline_create_info);

    if (pipeline == NULL) {
        SDL_SetError("Failed to create GPU pipeline for blit");
        return NULL;
    }

    // Cache the new pipeline
    if (*blit_pipeline_count >= *blit_pipeline_capacity) {
        const Uint32 new_capacity = *blit_pipeline_capacity ? *blit_pipeline_capacity * 2 : 2;
        BlitPipelineCacheEntry *new_blit_pipelines = SDL_realloc(
            *blit_pipelines,
            sizeof(**blit_pipelines) * new_capacity);
        if (new_blit_pipelines == NULL) {
            SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
            SDL_SetError("Out of memory");
            return NULL;
        }
        *blit_pipelines = new_blit_pipelines;
        *blit_pipeline_capacity = new_capacity;
    }

    (*blit_pipelines)[*blit_pipeline_count].pipeline = pipeline;
    (*blit_pipelines)[*blit_pipeline_count].type = source_texture_type;
    (*blit_pipelines)[*blit_pipeline_count].format = destination_format;
    *blit_pipeline_count += 1;

    return pipeline;
}

void SDL_GPU_BlitCommon(
    SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUBlitInfo *info,
    SDL_GPUSampler *blit_linear_sampler,
    SDL_GPUSampler *blit_nearest_sampler,
    SDL_GPUShader *blit_vertex_shader,
    SDL_GPUShader *blit_from_2d_shader,
    SDL_GPUShader *blit_from_2d_array_shader,
    SDL_GPUShader *blit_from_3d_shader,
    SDL_GPUShader *blit_from_cube_shader,
    SDL_GPUShader *blit_from_cube_array_shader,
    BlitPipelineCacheEntry **blit_pipelines,
    Uint32 *blit_pipeline_count,
    Uint32 *blit_pipeline_capacity)
{
    CommandBufferCommonHeader *cmdbufHeader = (CommandBufferCommonHeader *)command_buffer;
    SDL_GPURenderPass *render_pass;
    TextureCommonHeader *src_header = (TextureCommonHeader *)info->source.texture;
    TextureCommonHeader *dst_header = (TextureCommonHeader *)info->destination.texture;
    SDL_GPUGraphicsPipeline *blit_pipeline;
    SDL_GPUColorTargetInfo color_target_info;
    SDL_GPUViewport viewport;
    SDL_GPUTextureSamplerBinding texture_sampler_binding;
    BlitFragmentUniforms blit_fragment_uniforms;
    Uint32 layer_divisor;

    blit_pipeline = SDL_GPU_FetchBlitPipeline(
        cmdbufHeader->device,
        src_header->info.type,
        dst_header->info.format,
        blit_vertex_shader,
        blit_from_2d_shader,
        blit_from_2d_array_shader,
        blit_from_3d_shader,
        blit_from_cube_shader,
        blit_from_cube_array_shader,
        blit_pipelines,
        blit_pipeline_count,
        blit_pipeline_capacity);

    if (blit_pipeline == NULL) {
        return;
    }

    color_target_info.load_op = info->load_op;
    color_target_info.clear_color = info->clear_color;
    color_target_info.store_op = SDL_GPU_STOREOP_STORE;

    color_target_info.texture = info->destination.texture;
    color_target_info.mip_level = info->destination.mip_level;
    color_target_info.layer_or_depth_plane = info->destination.layer_or_depth_plane;
    color_target_info.cycle = info->cycle;

    render_pass = SDL_BeginGPURenderPass(
        command_buffer,
        &color_target_info,
        1,
        NULL);

    viewport.x = (float)info->destination.x;
    viewport.y = (float)info->destination.y;
    viewport.w = (float)info->destination.w;
    viewport.h = (float)info->destination.h;
    viewport.min_depth = 0;
    viewport.max_depth = 1;

    SDL_SetGPUViewport(
        render_pass,
        &viewport);

    SDL_BindGPUGraphicsPipeline(
        render_pass,
        blit_pipeline);

    texture_sampler_binding.texture = info->source.texture;
    texture_sampler_binding.sampler =
        info->filter == SDL_GPU_FILTER_NEAREST ? blit_nearest_sampler : blit_linear_sampler;

    SDL_BindGPUFragmentSamplers(
        render_pass,
        0,
        &texture_sampler_binding,
        1);

    blit_fragment_uniforms.left = (float)info->source.x / (src_header->info.width >> info->source.mip_level);
    blit_fragment_uniforms.top = (float)info->source.y / (src_header->info.height >> info->source.mip_level);
    blit_fragment_uniforms.width = (float)info->source.w / (src_header->info.width >> info->source.mip_level);
    blit_fragment_uniforms.height = (float)info->source.h / (src_header->info.height >> info->source.mip_level);
    blit_fragment_uniforms.mip_level = info->source.mip_level;

    layer_divisor = (src_header->info.type == SDL_GPU_TEXTURETYPE_3D) ? src_header->info.layer_count_or_depth : 1;
    blit_fragment_uniforms.layer_or_depth = (float)info->source.layer_or_depth_plane / layer_divisor;

    if (info->flip_mode & SDL_FLIP_HORIZONTAL) {
        blit_fragment_uniforms.left += blit_fragment_uniforms.width;
        blit_fragment_uniforms.width *= -1;
    }

    if (info->flip_mode & SDL_FLIP_VERTICAL) {
        blit_fragment_uniforms.top += blit_fragment_uniforms.height;
        blit_fragment_uniforms.height *= -1;
    }

    SDL_PushGPUFragmentUniformData(
        command_buffer,
        0,
        &blit_fragment_uniforms,
        sizeof(blit_fragment_uniforms));

    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
    SDL_EndGPURenderPass(render_pass);
}
