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

/* Normally this macro would use something like SDL_IsObjectValid, but in GPU's
 * case we can prioritize performance and be more trusting of application
 * behavior than, say, SDL_Render, so trust that applications will be careful
 * about disposing the device and its resources.
 * -flibit
 */
#define CHECK_DEVICE_MAGIC(device, retval)  \
    CHECK_PARAM(device == NULL) {           \
        SDL_SetError("Invalid GPU device"); \
        return retval;                      \
    }

#define CHECK_COMMAND_BUFFER                                        \
    if (((CommandBufferCommonHeader *)command_buffer)->submitted) { \
        SDL_assert_release(!"Command buffer already submitted!");   \
        return;                                                     \
    }

#define CHECK_COMMAND_BUFFER_RETURN_FALSE                           \
    if (((CommandBufferCommonHeader *)command_buffer)->submitted) { \
        SDL_assert_release(!"Command buffer already submitted!");   \
        return false;                                               \
    }

#define CHECK_COMMAND_BUFFER_RETURN_NULL                            \
    if (((CommandBufferCommonHeader *)command_buffer)->submitted) { \
        SDL_assert_release(!"Command buffer already submitted!");   \
        return NULL;                                                \
    }

#define CHECK_ANY_PASS_IN_PROGRESS(msg, retval)                                 \
    if (                                                                        \
        ((CommandBufferCommonHeader *)command_buffer)->render_pass.in_progress ||  \
        ((CommandBufferCommonHeader *)command_buffer)->compute_pass.in_progress || \
        ((CommandBufferCommonHeader *)command_buffer)->copy_pass.in_progress) {    \
        SDL_assert_release(!msg);                                               \
        return retval;                                                          \
    }

#define CHECK_RENDERPASS                                     \
    if (!((RenderPass *)render_pass)->in_progress) {                 \
        SDL_assert_release(!"Render pass not in progress!"); \
        return;                                              \
    }

#if 0
// The below validation is too aggressive, since there are advanced situations
// where this is legal. This is being temporarily disabled for further review.
// See: https://github.com/libsdl-org/SDL/issues/13871
#define CHECK_SAMPLER_TEXTURES                                                                                                          \
    RenderPass *rp = (RenderPass *)render_pass;                                                                                         \
    for (Uint32 color_target_index = 0; color_target_index < rp->num_color_targets; color_target_index += 1) {                          \
        for (Uint32 texture_sampler_index = 0; texture_sampler_index < num_bindings; texture_sampler_index += 1) {                      \
            if (rp->color_targets[color_target_index] == texture_sampler_bindings[texture_sampler_index].texture) {                     \
                SDL_assert_release(!"Texture cannot be simultaneously bound as a color target and a sampler!");                         \
            }                                                                                                                           \
        }                                                                                                                               \
    }                                                                                                                                   \
                                                                                                                                        \
    for (Uint32 texture_sampler_index = 0; texture_sampler_index < num_bindings; texture_sampler_index += 1) {                          \
        if (rp->depth_stencil_target != NULL && rp->depth_stencil_target == texture_sampler_bindings[texture_sampler_index].texture) {  \
            SDL_assert_release(!"Texture cannot be simultaneously bound as a depth stencil target and a sampler!");                     \
        }                                                                                                                               \
    }

#define CHECK_STORAGE_TEXTURES                                                                                              \
    RenderPass *rp = (RenderPass *)render_pass;                                                                             \
    for (Uint32 color_target_index = 0; color_target_index < rp->num_color_targets; color_target_index += 1) {              \
        for (Uint32 texture_sampler_index = 0; texture_sampler_index < num_bindings; texture_sampler_index += 1) {          \
            if (rp->color_targets[color_target_index] == storage_textures[texture_sampler_index]) {                         \
                SDL_assert_release(!"Texture cannot be simultaneously bound as a color target and a storage texture!");     \
            }                                                                                                               \
        }                                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    for (Uint32 texture_sampler_index = 0; texture_sampler_index < num_bindings; texture_sampler_index += 1) {              \
        if (rp->depth_stencil_target != NULL && rp->depth_stencil_target == storage_textures[texture_sampler_index]) {      \
            SDL_assert_release(!"Texture cannot be simultaneously bound as a depth stencil target and a storage texture!"); \
        }                                                                                                                   \
    }
#else
#define CHECK_SAMPLER_TEXTURES
#define CHECK_STORAGE_TEXTURES
#endif

#define CHECK_GRAPHICS_PIPELINE_BOUND                                                   \
    if (!((RenderPass *)render_pass)->graphics_pipeline) { \
        SDL_assert_release(!"Graphics pipeline not bound!");                            \
        return;                                                                         \
    }

#define CHECK_COMPUTEPASS                                     \
    if (!((Pass *)compute_pass)->in_progress) {                 \
        SDL_assert_release(!"Compute pass not in progress!"); \
        return;                                               \
    }

#define CHECK_COMPUTE_PIPELINE_BOUND                                                        \
    if (!((ComputePass *)compute_pass)->compute_pipeline) { \
        SDL_assert_release(!"Compute pipeline not bound!");                                 \
        return;                                                                             \
    }

#define CHECK_TEXTUREFORMAT_ENUM_INVALID(enumval, retval)     \
    if (enumval <= SDL_GPU_TEXTUREFORMAT_INVALID || enumval >= SDL_GPU_TEXTUREFORMAT_MAX_ENUM_VALUE) {               \
        SDL_assert_release(!"Invalid texture format enum!"); \
        return retval;                                       \
    }

#define CHECK_VERTEXELEMENTFORMAT_ENUM_INVALID(enumval, retval)       \
    if (enumval <= SDL_GPU_VERTEXELEMENTFORMAT_INVALID || enumval >= SDL_GPU_VERTEXELEMENTFORMAT_MAX_ENUM_VALUE) {  \
        SDL_assert_release(!"Invalid vertex format enum!");          \
        return retval;                                               \
    }

#define CHECK_COMPAREOP_ENUM_INVALID(enumval, retval)                              \
    if (enumval <= SDL_GPU_COMPAREOP_INVALID || enumval >= SDL_GPU_COMPAREOP_MAX_ENUM_VALUE) { \
        SDL_assert_release(!"Invalid compare op enum!");                          \
        return retval;                                                            \
    }

#define CHECK_STENCILOP_ENUM_INVALID(enumval, retval)                                \
    if (enumval <= SDL_GPU_STENCILOP_INVALID || enumval >= SDL_GPU_STENCILOP_MAX_ENUM_VALUE) { \
        SDL_assert_release(!"Invalid stencil op enum!");                            \
        return retval;                                                              \
    }

#define CHECK_BLENDOP_ENUM_INVALID(enumval, retval)                              \
    if (enumval <= SDL_GPU_BLENDOP_INVALID || enumval >= SDL_GPU_BLENDOP_MAX_ENUM_VALUE) { \
        SDL_assert_release(!"Invalid blend op enum!");                          \
        return retval;                                                          \
    }

#define CHECK_BLENDFACTOR_ENUM_INVALID(enumval, retval)                                  \
    if (enumval <= SDL_GPU_BLENDFACTOR_INVALID || enumval >= SDL_GPU_BLENDFACTOR_MAX_ENUM_VALUE) { \
        SDL_assert_release(!"Invalid blend factor enum!");                              \
        return retval;                                                                  \
    }

#define CHECK_SWAPCHAINCOMPOSITION_ENUM_INVALID(enumval, retval)    \
    if (enumval < 0 || enumval >= SDL_GPU_SWAPCHAINCOMPOSITION_MAX_ENUM_VALUE) {              \
        SDL_assert_release(!"Invalid swapchain composition enum!"); \
        return retval;                                              \
    }

#define CHECK_PRESENTMODE_ENUM_INVALID(enumval, retval)    \
    if (enumval < 0 || enumval >= SDL_GPU_PRESENTMODE_MAX_ENUM_VALUE) {              \
        SDL_assert_release(!"Invalid present mode enum!"); \
        return retval;                                     \
    }

#define COMMAND_BUFFER_DEVICE \
    ((CommandBufferCommonHeader *)command_buffer)->device

#define RENDERPASS_COMMAND_BUFFER \
    ((RenderPass *)render_pass)->command_buffer

#define RENDERPASS_DEVICE \
    ((CommandBufferCommonHeader *)RENDERPASS_COMMAND_BUFFER)->device

#define RENDERPASS_BOUND_PIPELINE \
    ((RenderPass *)render_pass)->graphics_pipeline

#define COMPUTEPASS_COMMAND_BUFFER \
    ((Pass *)compute_pass)->command_buffer

#define COMPUTEPASS_DEVICE \
    ((CommandBufferCommonHeader *)COMPUTEPASS_COMMAND_BUFFER)->device

#define COMPUTEPASS_BOUND_PIPELINE \
    ((ComputePass *)compute_pass)->compute_pipeline

// Internal Utility Functions

static void SDL_GPU_CheckGraphicsBindings(SDL_GPURenderPass *render_pass)
{
    RenderPass *rp = (RenderPass *)render_pass;
    GraphicsPipelineCommonHeader *pipeline = (GraphicsPipelineCommonHeader *)RENDERPASS_BOUND_PIPELINE;
    for (Uint32 i = 0; i < pipeline->num_vertex_samplers; i += 1) {
        if (!rp->vertex_sampler_bound[i]) {
            SDL_assert_release(!"Missing vertex sampler binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->num_vertex_storage_textures; i += 1) {
        if (!rp->vertex_storage_texture_bound[i]) {
            SDL_assert_release(!"Missing vertex storage texture binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->num_vertex_storage_buffers; i += 1) {
        if (!rp->vertex_storage_buffer_bound[i]) {
            SDL_assert_release(!"Missing vertex storage buffer binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->num_fragment_samplers; i += 1) {
        if (!rp->fragment_sampler_bound[i]) {
            SDL_assert_release(!"Missing fragment sampler binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->num_fragment_storage_textures; i += 1) {
        if (!rp->fragment_storage_texture_bound[i]) {
            SDL_assert_release(!"Missing fragment storage texture binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->num_fragment_storage_buffers; i += 1) {
        if (!rp->fragment_storage_buffer_bound[i]) {
            SDL_assert_release(!"Missing fragment storage buffer binding!");
        }
    }
}

static void SDL_GPU_CheckComputeBindings(SDL_GPUComputePass *compute_pass)
{
    ComputePass *cp = (ComputePass *)compute_pass;
    ComputePipelineCommonHeader *pipeline = (ComputePipelineCommonHeader *)COMPUTEPASS_BOUND_PIPELINE;
    for (Uint32 i = 0; i < pipeline->numSamplers; i += 1) {
        if (!cp->sampler_bound[i]) {
            SDL_assert_release(!"Missing compute sampler binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->numReadonlyStorageTextures; i += 1) {
        if (!cp->read_only_storage_texture_bound[i]) {
            SDL_assert_release(!"Missing compute readonly storage texture binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->numReadonlyStorageBuffers; i += 1) {
        if (!cp->read_only_storage_buffer_bound[i]) {
            SDL_assert_release(!"Missing compute readonly storage buffer binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->numReadWriteStorageTextures; i += 1) {
        if (!cp->read_write_storage_texture_bound[i]) {
            SDL_assert_release(!"Missing compute read-write storage texture binding!");
        }
    }
    for (Uint32 i = 0; i < pipeline->numReadWriteStorageBuffers; i += 1) {
        if (!cp->read_write_storage_buffer_bound[i]) {
            SDL_assert_release(!"Missing compute read-write storage buffer bbinding!");
        }
    }
}

// State Creation

SDL_GPUComputePipeline *SDL_CreateGPUComputePipeline(
    SDL_GPUDevice *device,
    const SDL_GPUComputePipelineCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    if (createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (createinfo->format == SDL_GPU_SHADERFORMAT_INVALID) {
            SDL_assert_release(!"Shader format cannot be INVALID!");
            return NULL;
        }
        if (!(createinfo->format & device->shader_formats)) {
            SDL_assert_release(!"Incompatible shader format for GPU backend");
            return NULL;
        }
        if (createinfo->num_readwrite_storage_textures > MAX_COMPUTE_WRITE_TEXTURES) {
            SDL_COMPILE_TIME_ASSERT(compute_write_textures, MAX_COMPUTE_WRITE_TEXTURES == 8);
            SDL_assert_release(!"Compute pipeline write-only texture count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_readwrite_storage_buffers > MAX_COMPUTE_WRITE_BUFFERS) {
            SDL_COMPILE_TIME_ASSERT(compute_write_buffers, MAX_COMPUTE_WRITE_BUFFERS == 8);
            SDL_assert_release(!"Compute pipeline write-only buffer count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_samplers > MAX_TEXTURE_SAMPLERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(compute_texture_samplers, MAX_TEXTURE_SAMPLERS_PER_STAGE == 16);
            SDL_assert_release(!"Compute pipeline sampler count cannot be higher than 16!");
            return NULL;
        }
        if (createinfo->num_readonly_storage_textures > MAX_STORAGE_TEXTURES_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(compute_storage_textures, MAX_STORAGE_TEXTURES_PER_STAGE == 8);
            SDL_assert_release(!"Compute pipeline readonly storage texture count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_readonly_storage_buffers > MAX_STORAGE_BUFFERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(compute_storage_buffers, MAX_STORAGE_BUFFERS_PER_STAGE == 8);
            SDL_assert_release(!"Compute pipeline readonly storage buffer count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_uniform_buffers > MAX_UNIFORM_BUFFERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(compute_uniform_buffers, MAX_UNIFORM_BUFFERS_PER_STAGE == 4);
            SDL_assert_release(!"Compute pipeline uniform buffer count cannot be higher than 4!");
            return NULL;
        }
        if (createinfo->threadcount_x == 0 ||
            createinfo->threadcount_y == 0 ||
            createinfo->threadcount_z == 0) {
            SDL_assert_release(!"Compute pipeline threadCount dimensions must be at least 1!");
            return NULL;
        }
    }

    return device->CreateComputePipeline(
        device->driverData,
        createinfo);
}

SDL_GPUGraphicsPipeline *SDL_CreateGPUGraphicsPipeline(
    SDL_GPUDevice *device,
    const SDL_GPUGraphicsPipelineCreateInfo *graphicsPipelineCreateInfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(graphicsPipelineCreateInfo == NULL) {
        SDL_InvalidParamError("graphicsPipelineCreateInfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (graphicsPipelineCreateInfo->vertex_shader == NULL) {
            SDL_assert_release(!"Vertex shader cannot be NULL!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->fragment_shader == NULL) {
            SDL_assert_release(!"Fragment shader cannot be NULL!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->target_info.num_color_targets > 0 && graphicsPipelineCreateInfo->target_info.color_target_descriptions == NULL) {
            SDL_assert_release(!"Color target descriptions array pointer cannot be NULL if num_color_targets is greater than zero!");
            return NULL;
        }
        for (Uint32 i = 0; i < graphicsPipelineCreateInfo->target_info.num_color_targets; i += 1) {
            CHECK_TEXTUREFORMAT_ENUM_INVALID(graphicsPipelineCreateInfo->target_info.color_target_descriptions[i].format, NULL);
            if (IsDepthFormat(graphicsPipelineCreateInfo->target_info.color_target_descriptions[i].format)) {
                SDL_assert_release(!"Color target formats cannot be a depth format!");
                return NULL;
            }
            if (!SDL_GPUTextureSupportsFormat(device, graphicsPipelineCreateInfo->target_info.color_target_descriptions[i].format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_COLOR_TARGET)) {
                SDL_assert_release(!"Format is not supported for color targets on this device!");
                return NULL;
            }
            if (graphicsPipelineCreateInfo->target_info.color_target_descriptions[i].blend_state.enable_blend) {
                const SDL_GPUColorTargetBlendState *blend_state = &graphicsPipelineCreateInfo->target_info.color_target_descriptions[i].blend_state;
                CHECK_BLENDFACTOR_ENUM_INVALID(blend_state->src_color_blendfactor, NULL);
                CHECK_BLENDFACTOR_ENUM_INVALID(blend_state->dst_color_blendfactor, NULL);
                CHECK_BLENDOP_ENUM_INVALID(blend_state->color_blend_op, NULL);
                CHECK_BLENDFACTOR_ENUM_INVALID(blend_state->src_alpha_blendfactor, NULL);
                CHECK_BLENDFACTOR_ENUM_INVALID(blend_state->dst_alpha_blendfactor, NULL);
                CHECK_BLENDOP_ENUM_INVALID(blend_state->alpha_blend_op, NULL);

                // TODO: validate that format support blending?
            }
        }
        if (graphicsPipelineCreateInfo->target_info.has_depth_stencil_target) {
            CHECK_TEXTUREFORMAT_ENUM_INVALID(graphicsPipelineCreateInfo->target_info.depth_stencil_format, NULL);
            if (!IsDepthFormat(graphicsPipelineCreateInfo->target_info.depth_stencil_format)) {
                SDL_assert_release(!"Depth-stencil target format must be a depth format!");
                return NULL;
            }
            if (!SDL_GPUTextureSupportsFormat(device, graphicsPipelineCreateInfo->target_info.depth_stencil_format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
                SDL_assert_release(!"Format is not supported for depth targets on this device!");
                return NULL;
            }
        }
        if (graphicsPipelineCreateInfo->multisample_state.enable_alpha_to_coverage) {
            if (graphicsPipelineCreateInfo->target_info.num_color_targets < 1) {
                SDL_assert_release(!"Alpha-to-coverage enabled but no color targets present!");
                return NULL;
            }
            if (!FormatHasAlpha(graphicsPipelineCreateInfo->target_info.color_target_descriptions[0].format)) {
                SDL_assert_release(!"Format is not compatible with alpha-to-coverage!");
                return NULL;
            }

            // TODO: validate that format supports belnding? This is only required on Metal.
        }
        if (graphicsPipelineCreateInfo->vertex_input_state.num_vertex_buffers > 0 && graphicsPipelineCreateInfo->vertex_input_state.vertex_buffer_descriptions == NULL) {
            SDL_assert_release(!"Vertex buffer descriptions array pointer cannot be NULL!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->vertex_input_state.num_vertex_buffers > MAX_VERTEX_BUFFERS) {
            SDL_COMPILE_TIME_ASSERT(vertex_buffers, MAX_VERTEX_BUFFERS == 16);
            SDL_assert_release(!"The number of vertex buffer descriptions in a vertex input state must not exceed 16!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->vertex_input_state.num_vertex_attributes > 0 && graphicsPipelineCreateInfo->vertex_input_state.vertex_attributes == NULL) {
            SDL_assert_release(!"Vertex attributes array pointer cannot be NULL!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->vertex_input_state.num_vertex_attributes > MAX_VERTEX_ATTRIBUTES) {
            SDL_COMPILE_TIME_ASSERT(vertex_attributes, MAX_VERTEX_ATTRIBUTES == 16);
            SDL_assert_release(!"The number of vertex attributes in a vertex input state must not exceed 16!");
            return NULL;
        }
        for (Uint32 i = 0; i < graphicsPipelineCreateInfo->vertex_input_state.num_vertex_buffers; i += 1) {
            if (graphicsPipelineCreateInfo->vertex_input_state.vertex_buffer_descriptions[i].instance_step_rate != 0) {
                SDL_assert_release(!"For all vertex buffer descriptions, instance_step_rate must be 0!");
                return NULL;
            }
        }
        Uint32 locations[MAX_VERTEX_ATTRIBUTES];
        for (Uint32 i = 0; i < graphicsPipelineCreateInfo->vertex_input_state.num_vertex_attributes; i += 1) {
            CHECK_VERTEXELEMENTFORMAT_ENUM_INVALID(graphicsPipelineCreateInfo->vertex_input_state.vertex_attributes[i].format, NULL);

            locations[i] = graphicsPipelineCreateInfo->vertex_input_state.vertex_attributes[i].location;
            for (Uint32 j = 0; j < i; j += 1) {
                if (locations[j] == locations[i]) {
                    SDL_assert_release(!"Each vertex attribute location in a vertex input state must be unique!");
                    return NULL;
                }
            }
        }
        if (graphicsPipelineCreateInfo->multisample_state.enable_mask) {
            SDL_assert_release(!"For multisample states, enable_mask must be false!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->multisample_state.sample_mask != 0) {
            SDL_assert_release(!"For multisample states, sample_mask must be 0!");
            return NULL;
        }
        if (graphicsPipelineCreateInfo->depth_stencil_state.enable_depth_test) {
            CHECK_COMPAREOP_ENUM_INVALID(graphicsPipelineCreateInfo->depth_stencil_state.compare_op, NULL);
        }
        if (graphicsPipelineCreateInfo->depth_stencil_state.enable_stencil_test) {
            const SDL_GPUStencilOpState *stencil_state = &graphicsPipelineCreateInfo->depth_stencil_state.back_stencil_state;
            CHECK_COMPAREOP_ENUM_INVALID(stencil_state->compare_op, NULL);
            CHECK_STENCILOP_ENUM_INVALID(stencil_state->fail_op, NULL);
            CHECK_STENCILOP_ENUM_INVALID(stencil_state->pass_op, NULL);
            CHECK_STENCILOP_ENUM_INVALID(stencil_state->depth_fail_op, NULL);
        }

        if (device->validate_feature_depth_clamp_disabled &&
            !graphicsPipelineCreateInfo->rasterizer_state.enable_depth_clip) {
            SDL_assert_release(!"Rasterizer state enable_depth_clip must be set to true (FEATURE_DEPTH_CLAMPING disabled)");
            return NULL;
        }
    }

    return device->CreateGraphicsPipeline(
        device->driverData,
        graphicsPipelineCreateInfo);
}

SDL_GPUSampler *SDL_CreateGPUSampler(
    SDL_GPUDevice *device,
    const SDL_GPUSamplerCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (device->validate_feature_anisotropy_disabled &&
            createinfo->enable_anisotropy) {
            SDL_assert_release(!"enable_anisotropy must be set to false (FEATURE_ANISOTROPY disabled)");
            return NULL;
        }
    }

    return device->CreateSampler(
        device->driverData,
        createinfo);
}

SDL_GPUShader *SDL_CreateGPUShader(
    SDL_GPUDevice *device,
    const SDL_GPUShaderCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (createinfo->format == SDL_GPU_SHADERFORMAT_INVALID) {
            SDL_assert_release(!"Shader format cannot be INVALID!");
            return NULL;
        }
        if (!(createinfo->format & device->shader_formats)) {
            SDL_assert_release(!"Incompatible shader format for GPU backend");
            return NULL;
        }
        if (createinfo->num_samplers > MAX_TEXTURE_SAMPLERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(shader_texture_samplers, MAX_TEXTURE_SAMPLERS_PER_STAGE == 16);
            SDL_assert_release(!"Shader sampler count cannot be higher than 16!");
            return NULL;
        }
        if (createinfo->num_storage_textures > MAX_STORAGE_TEXTURES_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(shader_storage_textures, MAX_STORAGE_TEXTURES_PER_STAGE == 8);
            SDL_assert_release(!"Shader storage texture count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_storage_buffers > MAX_STORAGE_BUFFERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(shader_storage_buffers, MAX_STORAGE_BUFFERS_PER_STAGE == 8);
            SDL_assert_release(!"Shader storage buffer count cannot be higher than 8!");
            return NULL;
        }
        if (createinfo->num_uniform_buffers > MAX_UNIFORM_BUFFERS_PER_STAGE) {
            SDL_COMPILE_TIME_ASSERT(shader_uniform_buffers, MAX_UNIFORM_BUFFERS_PER_STAGE == 4);
            SDL_assert_release(!"Shader uniform buffer count cannot be higher than 4!");
            return NULL;
        }
    }

    return device->CreateShader(
        device->driverData,
        createinfo);
}

SDL_GPUTexture *SDL_CreateGPUTexture(
    SDL_GPUDevice *device,
    const SDL_GPUTextureCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        bool failed = false;

        const Uint32 MAX_2D_DIMENSION = 16384;
        const Uint32 MAX_3D_DIMENSION = 2048;

        // Common checks for all texture types
        CHECK_TEXTUREFORMAT_ENUM_INVALID(createinfo->format, NULL);

        if (createinfo->width <= 0 || createinfo->height <= 0 || createinfo->layer_count_or_depth <= 0) {
            SDL_assert_release(!"For any texture: width, height, and layer_count_or_depth must be >= 1");
            failed = true;
        }
        if (createinfo->num_levels <= 0) {
            SDL_assert_release(!"For any texture: num_levels must be >= 1");
            failed = true;
        }
        if (createinfo->type == SDL_GPU_TEXTURETYPE_2D && createinfo->layer_count_or_depth != 1)
        {
            SDL_assert_release(!"2D textures must have a layer count of 1");
            failed = true;
        }
        if ((createinfo->usage & SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ) && (createinfo->usage & SDL_GPU_TEXTUREUSAGE_SAMPLER)) {
            SDL_assert_release(!"For any texture: usage cannot contain both GRAPHICS_STORAGE_READ and SAMPLER");
            failed = true;
        }
        if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1 &&
            (createinfo->usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE)) {
            SDL_assert_release(!"For multisample textures: usage cannot contain COMPUTE_STORAGE_WRITE flag");
            failed = true;
        }
        if (IsDepthFormat(createinfo->format) && (createinfo->usage & ~(SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER))) {
            SDL_assert_release(!"For depth textures: usage cannot contain any flags except for DEPTH_STENCIL_TARGET and SAMPLER");
            failed = true;
        }
        if (IsIntegerFormat(createinfo->format) && (createinfo->usage & SDL_GPU_TEXTUREUSAGE_SAMPLER)) {
            SDL_assert_release(!"For any texture: usage cannot contain SAMPLER for textures with an integer format");
            failed = true;
        }

        if (createinfo->type == SDL_GPU_TEXTURETYPE_CUBE) {
            // Cubemap validation
            if (createinfo->width != createinfo->height) {
                SDL_assert_release(!"For cube textures: width and height must be identical");
                failed = true;
            }
            if (createinfo->width > MAX_2D_DIMENSION || createinfo->height > MAX_2D_DIMENSION) {
                SDL_assert_release(!"For cube textures: width and height must be <= 16384");
                failed = true;
            }
            if (createinfo->layer_count_or_depth != 6) {
                SDL_assert_release(!"For cube textures: layer_count_or_depth must be 6");
                failed = true;
            }
            if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1) {
                SDL_assert_release(!"For cube textures: sample_count must be SDL_GPU_SAMPLECOUNT_1");
                failed = true;
            }
            if (!SDL_GPUTextureSupportsFormat(device, createinfo->format, SDL_GPU_TEXTURETYPE_CUBE, createinfo->usage)) {
                SDL_assert_release(!"For cube textures: the format is unsupported for the given usage");
                failed = true;
            }
        } else if (createinfo->type == SDL_GPU_TEXTURETYPE_CUBE_ARRAY) {
            // Cubemap array validation
            if (createinfo->width != createinfo->height) {
                SDL_assert_release(!"For cube array textures: width and height must be identical");
                failed = true;
            }
            if (createinfo->width > MAX_2D_DIMENSION || createinfo->height > MAX_2D_DIMENSION) {
                SDL_assert_release(!"For cube array textures: width and height must be <= 16384");
                failed = true;
            }
            if (createinfo->layer_count_or_depth % 6 != 0) {
                SDL_assert_release(!"For cube array textures: layer_count_or_depth must be a multiple of 6");
                failed = true;
            }
            if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1) {
                SDL_assert_release(!"For cube array textures: sample_count must be SDL_GPU_SAMPLECOUNT_1");
                failed = true;
            }
            if (!SDL_GPUTextureSupportsFormat(device, createinfo->format, SDL_GPU_TEXTURETYPE_CUBE_ARRAY, createinfo->usage)) {
                SDL_assert_release(!"For cube array textures: the format is unsupported for the given usage");
                failed = true;
            }
        } else if (createinfo->type == SDL_GPU_TEXTURETYPE_3D) {
            // 3D Texture Validation
            if (createinfo->width > MAX_3D_DIMENSION || createinfo->height > MAX_3D_DIMENSION || createinfo->layer_count_or_depth > MAX_3D_DIMENSION) {
                SDL_assert_release(!"For 3D textures: width, height, and layer_count_or_depth must be <= 2048");
                failed = true;
            }
            if (createinfo->usage & SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET) {
                SDL_assert_release(!"For 3D textures: usage must not contain DEPTH_STENCIL_TARGET");
                failed = true;
            }
            if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1) {
                SDL_assert_release(!"For 3D textures: sample_count must be SDL_GPU_SAMPLECOUNT_1");
                failed = true;
            }
            if (!SDL_GPUTextureSupportsFormat(device, createinfo->format, SDL_GPU_TEXTURETYPE_3D, createinfo->usage)) {
                SDL_assert_release(!"For 3D textures: the format is unsupported for the given usage");
                failed = true;
            }
        } else {
            if (createinfo->type == SDL_GPU_TEXTURETYPE_2D_ARRAY) {
                // Array Texture Validation
                if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1) {
                    SDL_assert_release(!"For array textures: sample_count must be SDL_GPU_SAMPLECOUNT_1");
                    failed = true;
                }
            }
            if (createinfo->sample_count > SDL_GPU_SAMPLECOUNT_1 && createinfo->num_levels > 1) {
                SDL_assert_release(!"For 2D multisample textures: num_levels must be 1");
                failed = true;
            }
            if (!SDL_GPUTextureSupportsFormat(device, createinfo->format, SDL_GPU_TEXTURETYPE_2D, createinfo->usage)) {
                SDL_assert_release(!"For 2D textures: the format is unsupported for the given usage");
                failed = true;
            }
        }

        if (failed) {
            return NULL;
        }
    }

    return device->CreateTexture(
        device->driverData,
        createinfo);
}

SDL_GPUBuffer *SDL_CreateGPUBuffer(
    SDL_GPUDevice *device,
    const SDL_GPUBufferCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (createinfo->size < 4) {
            SDL_assert_release(!"Cannot create a buffer with size less than 4 bytes!");
        }
    }

    const char *debugName = SDL_GetStringProperty(createinfo->props, SDL_PROP_GPU_BUFFER_CREATE_NAME_STRING, NULL);

    return device->CreateBuffer(
        device->driverData,
        createinfo->usage,
        createinfo->size,
        debugName);
}

SDL_GPUTransferBuffer *SDL_CreateGPUTransferBuffer(
    SDL_GPUDevice *device,
    const SDL_GPUTransferBufferCreateInfo *createinfo)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(createinfo == NULL) {
        SDL_InvalidParamError("createinfo");
        return NULL;
    }

    if (device->debug_mode) {
        if (createinfo->usage != SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD &&
            createinfo->usage != SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD) {
            SDL_assert_release(!"Invalid transfer buffer usage!");
        }
    }

    const char *debugName = SDL_GetStringProperty(createinfo->props, SDL_PROP_GPU_TRANSFERBUFFER_CREATE_NAME_STRING, NULL);

    return device->CreateTransferBuffer(
        device->driverData,
        createinfo->usage,
        createinfo->size,
        debugName);
}

// Debug Naming

void SDL_SetGPUBufferName(
    SDL_GPUDevice *device,
    SDL_GPUBuffer *buffer,
    const char *text)
{
    CHECK_DEVICE_MAGIC(device, );

    CHECK_PARAM(buffer == NULL) {
        SDL_InvalidParamError("buffer");
        return;
    }
    CHECK_PARAM(text == NULL) {
        SDL_InvalidParamError("text");
    }

    device->SetBufferName(
        device->driverData,
        buffer,
        text);
}

void SDL_SetGPUTextureName(
    SDL_GPUDevice *device,
    SDL_GPUTexture *texture,
    const char *text)
{
    CHECK_DEVICE_MAGIC(device, );

    CHECK_PARAM(texture == NULL) {
        SDL_InvalidParamError("texture");
        return;
    }
    CHECK_PARAM(text == NULL) {
        SDL_InvalidParamError("text");
    }

    device->SetTextureName(
        device->driverData,
        texture,
        text);
}

void SDL_InsertGPUDebugLabel(
    SDL_GPUCommandBuffer *command_buffer,
    const char *text)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(text == NULL) {
        SDL_InvalidParamError("text");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->InsertDebugLabel(
        command_buffer,
        text);
}

void SDL_PushGPUDebugGroup(
    SDL_GPUCommandBuffer *command_buffer,
    const char *name)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(name == NULL) {
        SDL_InvalidParamError("name");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->PushDebugGroup(
        command_buffer,
        name);
}

void SDL_PopGPUDebugGroup(
    SDL_GPUCommandBuffer *command_buffer)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->PopDebugGroup(
        command_buffer);
}

// Disposal

void SDL_ReleaseGPUTexture(
    SDL_GPUDevice *device,
    SDL_GPUTexture *texture)
{
    if(texture == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseTexture(
        device->driverData,
        texture);
}

void SDL_ReleaseGPUSampler(
    SDL_GPUDevice *device,
    SDL_GPUSampler *sampler)
{
    if(sampler == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseSampler(
        device->driverData,
        sampler);
}

void SDL_ReleaseGPUBuffer(
    SDL_GPUDevice *device,
    SDL_GPUBuffer *buffer)
{
    if(buffer == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseBuffer(
        device->driverData,
        buffer);
}

void SDL_ReleaseGPUTransferBuffer(
    SDL_GPUDevice *device,
    SDL_GPUTransferBuffer *transfer_buffer)
{
    if(transfer_buffer == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseTransferBuffer(
        device->driverData,
        transfer_buffer);
}

void SDL_ReleaseGPUShader(
    SDL_GPUDevice *device,
    SDL_GPUShader *shader)
{
    if(shader == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseShader(
        device->driverData,
        shader);
}

void SDL_ReleaseGPUComputePipeline(
    SDL_GPUDevice *device,
    SDL_GPUComputePipeline *compute_pipeline)
{
    if(compute_pipeline == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseComputePipeline(
        device->driverData,
        compute_pipeline);
}

void SDL_ReleaseGPUGraphicsPipeline(
    SDL_GPUDevice *device,
    SDL_GPUGraphicsPipeline *graphics_pipeline)
{
    if(graphics_pipeline == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseGraphicsPipeline(
        device->driverData,
        graphics_pipeline);
}

// Command Buffer

SDL_GPUCommandBuffer *SDL_AcquireGPUCommandBuffer(
    SDL_GPUDevice *device)
{
    SDL_GPUCommandBuffer *command_buffer;
    CommandBufferCommonHeader *commandBufferHeader;

    CHECK_DEVICE_MAGIC(device, NULL);

    command_buffer = device->AcquireCommandBuffer(
        device->driverData);

    if (command_buffer == NULL) {
        return NULL;
    }

    commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;
    commandBufferHeader->device = device;
    commandBufferHeader->render_pass.command_buffer = command_buffer;
    commandBufferHeader->compute_pass.command_buffer = command_buffer;
    commandBufferHeader->copy_pass.command_buffer = command_buffer;

    if (device->debug_mode) {
        commandBufferHeader->render_pass.in_progress = false;
        commandBufferHeader->render_pass.graphics_pipeline = NULL;
        commandBufferHeader->compute_pass.in_progress = false;
        commandBufferHeader->compute_pass.compute_pipeline = NULL;
        commandBufferHeader->copy_pass.in_progress = false;
        commandBufferHeader->swapchain_texture_acquired = false;
        commandBufferHeader->submitted = false;
        commandBufferHeader->ignore_render_pass_texture_validation = false;
        SDL_zeroa(commandBufferHeader->render_pass.vertex_sampler_bound);
        SDL_zeroa(commandBufferHeader->render_pass.vertex_storage_texture_bound);
        SDL_zeroa(commandBufferHeader->render_pass.vertex_storage_buffer_bound);
        SDL_zeroa(commandBufferHeader->render_pass.fragment_sampler_bound);
        SDL_zeroa(commandBufferHeader->render_pass.fragment_storage_texture_bound);
        SDL_zeroa(commandBufferHeader->render_pass.fragment_storage_buffer_bound);
        SDL_zeroa(commandBufferHeader->compute_pass.sampler_bound);
        SDL_zeroa(commandBufferHeader->compute_pass.read_only_storage_texture_bound);
        SDL_zeroa(commandBufferHeader->compute_pass.read_only_storage_buffer_bound);
        SDL_zeroa(commandBufferHeader->compute_pass.read_write_storage_texture_bound);
        SDL_zeroa(commandBufferHeader->compute_pass.read_write_storage_buffer_bound);
    }

    return command_buffer;
}

// Uniforms

void SDL_PushGPUVertexUniformData(
    SDL_GPUCommandBuffer *command_buffer,
    Uint32 slot_index,
    const void *data,
    Uint32 length)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(data == NULL) {
        SDL_InvalidParamError("data");
        return;
    }
    CHECK_PARAM(slot_index >= MAX_UNIFORM_BUFFERS_PER_STAGE) {
        SDL_SetError("slot_index exceeds MAX_UNIFORM_BUFFERS_PER_STAGE");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->PushVertexUniformData(
        command_buffer,
        slot_index,
        data,
        length);
}

void SDL_PushGPUFragmentUniformData(
    SDL_GPUCommandBuffer *command_buffer,
    Uint32 slot_index,
    const void *data,
    Uint32 length)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(data == NULL) {
        SDL_InvalidParamError("data");
        return;
    }
    CHECK_PARAM(slot_index >= MAX_UNIFORM_BUFFERS_PER_STAGE) {
        SDL_SetError("slot_index exceeds MAX_UNIFORM_BUFFERS_PER_STAGE");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->PushFragmentUniformData(
        command_buffer,
        slot_index,
        data,
        length);
}

void SDL_PushGPUComputeUniformData(
    SDL_GPUCommandBuffer *command_buffer,
    Uint32 slot_index,
    const void *data,
    Uint32 length)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(data == NULL) {
        SDL_InvalidParamError("data");
        return;
    }
    CHECK_PARAM(slot_index >= MAX_UNIFORM_BUFFERS_PER_STAGE) {
        SDL_SetError("slot_index exceeds MAX_UNIFORM_BUFFERS_PER_STAGE");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
    }

    COMMAND_BUFFER_DEVICE->PushComputeUniformData(
        command_buffer,
        slot_index,
        data,
        length);
}

// Render Pass

SDL_GPURenderPass *SDL_BeginGPURenderPass(
    SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUColorTargetInfo *color_target_infos,
    Uint32 num_color_targets,
    const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info)
{
    CommandBufferCommonHeader *commandBufferHeader;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return NULL;
    }
    CHECK_PARAM(color_target_infos == NULL && num_color_targets > 0) {
        SDL_InvalidParamError("color_target_infos");
        return NULL;
    }

    CHECK_PARAM(num_color_targets > MAX_COLOR_TARGET_BINDINGS) {
        SDL_SetError("num_color_targets exceeds MAX_COLOR_TARGET_BINDINGS");
        return NULL;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_NULL
        CHECK_ANY_PASS_IN_PROGRESS("Cannot begin render pass during another pass!", NULL);

        COMMAND_BUFFER_DEVICE->max_viewport_width = SDL_MAX_UINT32;
        COMMAND_BUFFER_DEVICE->max_viewport_height = SDL_MAX_UINT32;

        for (Uint32 i = 0; i < num_color_targets; i += 1) {
            TextureCommonHeader *textureHeader = (TextureCommonHeader *)color_target_infos[i].texture;

            COMMAND_BUFFER_DEVICE->max_viewport_width = SDL_min(COMMAND_BUFFER_DEVICE->max_viewport_width, textureHeader->info.width);
            COMMAND_BUFFER_DEVICE->max_viewport_height = SDL_min(COMMAND_BUFFER_DEVICE->max_viewport_height, textureHeader->info.height);

            if (color_target_infos[i].cycle && color_target_infos[i].load_op == SDL_GPU_LOADOP_LOAD) {
                SDL_assert_release(!"Cannot cycle color target when load op is LOAD!");
                return NULL;
            }

            if (color_target_infos[i].store_op == SDL_GPU_STOREOP_RESOLVE || color_target_infos[i].store_op == SDL_GPU_STOREOP_RESOLVE_AND_STORE) {
                if (color_target_infos[i].resolve_texture == NULL) {
                    SDL_assert_release(!"Store op is RESOLVE or RESOLVE_AND_STORE but resolve_texture is NULL!");
                    return NULL;
                } else {
                    TextureCommonHeader *resolveTextureHeader = (TextureCommonHeader *)color_target_infos[i].resolve_texture;
                    if (textureHeader->info.sample_count == SDL_GPU_SAMPLECOUNT_1) {
                        SDL_assert_release(!"Store op is RESOLVE or RESOLVE_AND_STORE but texture is not multisample!");
                        return NULL;
                    }
                    if (resolveTextureHeader->info.sample_count != SDL_GPU_SAMPLECOUNT_1) {
                        SDL_assert_release(!"Resolve texture must have a sample count of 1!");
                        return NULL;
                    }
                    if (resolveTextureHeader->info.format != textureHeader->info.format) {
                        SDL_assert_release(!"Resolve texture must have the same format as its corresponding color target!");
                        return NULL;
                    }
                    if (resolveTextureHeader->info.type == SDL_GPU_TEXTURETYPE_3D) {
                        SDL_assert_release(!"Resolve texture must not be of TEXTURETYPE_3D!");
                        return NULL;
                    }
                    if (!(resolveTextureHeader->info.usage & SDL_GPU_TEXTUREUSAGE_COLOR_TARGET)) {
                        SDL_assert_release(!"Resolve texture usage must include COLOR_TARGET!");
                        return NULL;
                    }
                }
            }

            if (color_target_infos[i].layer_or_depth_plane >= textureHeader->info.layer_count_or_depth) {
                SDL_assert_release(!"Color target layer index must be less than the texture's layer count!");
                return NULL;
            }

            if (color_target_infos[i].mip_level >= textureHeader->info.num_levels) {
                SDL_assert_release(!"Color target mip level must be less than the texture's level count!");
                return NULL;
            }
        }

        if (depth_stencil_target_info != NULL) {
            TextureCommonHeader *textureHeader = (TextureCommonHeader *)depth_stencil_target_info->texture;
            if (!(textureHeader->info.usage & SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
                SDL_assert_release(!"Depth target must have been created with the DEPTH_STENCIL_TARGET usage flag!");
                return NULL;
            }

            if (textureHeader->info.layer_count_or_depth > 255) {
                SDL_assert_release(!"Cannot bind a depth texture with more than 255 layers!");
                return NULL;
            }

            if (depth_stencil_target_info->cycle && (depth_stencil_target_info->load_op == SDL_GPU_LOADOP_LOAD || depth_stencil_target_info->stencil_load_op == SDL_GPU_LOADOP_LOAD)) {
                SDL_assert_release(!"Cannot cycle depth target when load op or stencil load op is LOAD!");
                return NULL;
            }

            if (depth_stencil_target_info->store_op == SDL_GPU_STOREOP_RESOLVE ||
                depth_stencil_target_info->stencil_store_op == SDL_GPU_STOREOP_RESOLVE ||
                depth_stencil_target_info->store_op == SDL_GPU_STOREOP_RESOLVE_AND_STORE ||
                depth_stencil_target_info->stencil_store_op == SDL_GPU_STOREOP_RESOLVE_AND_STORE) {
                SDL_assert_release(!"RESOLVE store ops are not supported for depth-stencil targets!");
                return NULL;
            }
        }
    }

    COMMAND_BUFFER_DEVICE->BeginRenderPass(
        command_buffer,
        color_target_infos,
        num_color_targets,
        depth_stencil_target_info);

    commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        commandBufferHeader->render_pass.in_progress = true;
        for (Uint32 i = 0; i < num_color_targets; i += 1) {
            commandBufferHeader->render_pass.color_targets[i] = color_target_infos[i].texture;
        }
        commandBufferHeader->render_pass.num_color_targets = num_color_targets;
        if (depth_stencil_target_info != NULL) {
            commandBufferHeader->render_pass.depth_stencil_target = depth_stencil_target_info->texture;
        } else {
            commandBufferHeader->render_pass.depth_stencil_target = NULL;
        }
    }

    return (SDL_GPURenderPass *)&(commandBufferHeader->render_pass);
}

void SDL_BindGPUGraphicsPipeline(
    SDL_GPURenderPass *render_pass,
    SDL_GPUGraphicsPipeline *graphics_pipeline)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(graphics_pipeline == NULL) {
        SDL_InvalidParamError("graphics_pipeline");
        return;
    }

    RENDERPASS_DEVICE->BindGraphicsPipeline(
        RENDERPASS_COMMAND_BUFFER,
        graphics_pipeline);


    if (RENDERPASS_DEVICE->debug_mode) {
        RENDERPASS_BOUND_PIPELINE = graphics_pipeline;
    }
}

void SDL_SetGPUViewport(
    SDL_GPURenderPass *render_pass,
    const SDL_GPUViewport *viewport)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(viewport == NULL) {
        SDL_InvalidParamError("viewport");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        if (((viewport->x + viewport->w) > RENDERPASS_DEVICE->max_viewport_width) ||
            ((viewport->y + viewport->h) > RENDERPASS_DEVICE->max_viewport_height)) {
            SDL_assert_release(!"Viewport size exceeds current render target dimensions");
            return;
        }
    }

    RENDERPASS_DEVICE->SetViewport(
        RENDERPASS_COMMAND_BUFFER,
        viewport);
}

void SDL_SetGPUScissor(
    SDL_GPURenderPass *render_pass,
    const SDL_Rect *scissor)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(scissor == NULL) {
        SDL_InvalidParamError("scissor");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        if (((Uint32) (scissor->x + scissor->w) > RENDERPASS_DEVICE->max_viewport_width) ||
            ((Uint32) (scissor->y + scissor->h) > RENDERPASS_DEVICE->max_viewport_height)) {
            SDL_assert_release(!"Scissor rectangle size exceeds current render target dimensions");
            return;
        }
    }

    RENDERPASS_DEVICE->SetScissor(
        RENDERPASS_COMMAND_BUFFER,
        scissor);
}

void SDL_SetGPUBlendConstants(
    SDL_GPURenderPass *render_pass,
    SDL_FColor blend_constants)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
    }

    RENDERPASS_DEVICE->SetBlendConstants(
        RENDERPASS_COMMAND_BUFFER,
        blend_constants);
}

void SDL_SetGPUStencilReference(
    SDL_GPURenderPass *render_pass,
    Uint8 reference)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
    }

    RENDERPASS_DEVICE->SetStencilReference(
        RENDERPASS_COMMAND_BUFFER,
        reference);
}

void SDL_BindGPUVertexBuffers(
    SDL_GPURenderPass *render_pass,
    Uint32 first_binding,
    const SDL_GPUBufferBinding *bindings,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    if (bindings == NULL && num_bindings > 0) {
        SDL_InvalidParamError("bindings");
        return;
    }
    CHECK_PARAM(first_binding + num_bindings > MAX_VERTEX_BUFFERS) {
        SDL_SetError("first_binding + num_bindings exceeds MAX_VERTEX_BUFFERS");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
    }

    RENDERPASS_DEVICE->BindVertexBuffers(
        RENDERPASS_COMMAND_BUFFER,
        first_binding,
        bindings,
        num_bindings);
}

void SDL_BindGPUIndexBuffer(
    SDL_GPURenderPass *render_pass,
    const SDL_GPUBufferBinding *binding,
    SDL_GPUIndexElementSize index_element_size)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    if (binding == NULL) {
        SDL_InvalidParamError("binding");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
    }

    RENDERPASS_DEVICE->BindIndexBuffer(
        RENDERPASS_COMMAND_BUFFER,
        binding,
        index_element_size);
}

void SDL_BindGPUVertexSamplers(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    const SDL_GPUTextureSamplerBinding *texture_sampler_bindings,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(texture_sampler_bindings == NULL && num_bindings > 0) {
        SDL_InvalidParamError("texture_sampler_bindings");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_TEXTURE_SAMPLERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_TEXTURE_SAMPLERS_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        if (!((CommandBufferCommonHeader *)RENDERPASS_COMMAND_BUFFER)->ignore_render_pass_texture_validation)
        {
            CHECK_SAMPLER_TEXTURES

            for (Uint32 i = 0; i < num_bindings; i += 1) {
                TextureCommonHeader *texture_header = (TextureCommonHeader *)texture_sampler_bindings[i].texture;
                if (texture_header->info.sample_count > SDL_GPU_SAMPLECOUNT_1)
                {
                    SDL_assert_release(!"Multisample textures cannot be bound as samplers!");
                }
            }
        }

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->vertex_sampler_bound[first_slot + i] = true;
        }
    }

    if (num_bindings == 0) {
        return;
    }

    RENDERPASS_DEVICE->BindVertexSamplers(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        texture_sampler_bindings,
        num_bindings);
}

void SDL_BindGPUVertexStorageTextures(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    SDL_GPUTexture *const *storage_textures,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(storage_textures == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_textures");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_TEXTURES_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_TEXTURES_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_STORAGE_TEXTURES

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->vertex_storage_texture_bound[first_slot + i] = true;
        }
    }

    RENDERPASS_DEVICE->BindVertexStorageTextures(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        storage_textures,
        num_bindings);
}

void SDL_BindGPUVertexStorageBuffers(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    SDL_GPUBuffer *const *storage_buffers,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(storage_buffers == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_buffers");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_BUFFERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_BUFFERS_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->vertex_storage_buffer_bound[first_slot + i] = true;
        }
    }

    RENDERPASS_DEVICE->BindVertexStorageBuffers(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        storage_buffers,
        num_bindings);
}

void SDL_BindGPUFragmentSamplers(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    const SDL_GPUTextureSamplerBinding *texture_sampler_bindings,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(texture_sampler_bindings == NULL && num_bindings > 0) {
        SDL_InvalidParamError("texture_sampler_bindings");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_TEXTURE_SAMPLERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_TEXTURE_SAMPLERS_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        if (!((CommandBufferCommonHeader *)RENDERPASS_COMMAND_BUFFER)->ignore_render_pass_texture_validation) {
            CHECK_SAMPLER_TEXTURES
        }

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            TextureCommonHeader *texture_header = (TextureCommonHeader *)texture_sampler_bindings[i].texture;
            if (texture_header->info.sample_count > SDL_GPU_SAMPLECOUNT_1)
            {
                SDL_assert_release(!"Multisample textures cannot be bound as samplers!");
            }
        }

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->fragment_sampler_bound[first_slot + i] = true;
        }
    }

    if (num_bindings == 0) {
        return;
    }

    RENDERPASS_DEVICE->BindFragmentSamplers(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        texture_sampler_bindings,
        num_bindings);
}

void SDL_BindGPUFragmentStorageTextures(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    SDL_GPUTexture *const *storage_textures,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(storage_textures == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_textures");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_TEXTURES_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_TEXTURES_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_STORAGE_TEXTURES

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->fragment_storage_texture_bound[first_slot + i] = true;
        }
    }

    RENDERPASS_DEVICE->BindFragmentStorageTextures(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        storage_textures,
        num_bindings);
}

void SDL_BindGPUFragmentStorageBuffers(
    SDL_GPURenderPass *render_pass,
    Uint32 first_slot,
    SDL_GPUBuffer *const *storage_buffers,
    Uint32 num_bindings)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(storage_buffers == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_buffers");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_BUFFERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_BUFFERS_PER_STAGE");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((RenderPass *)render_pass)->fragment_storage_buffer_bound[first_slot + i] = true;
        }
    }

    RENDERPASS_DEVICE->BindFragmentStorageBuffers(
        RENDERPASS_COMMAND_BUFFER,
        first_slot,
        storage_buffers,
        num_bindings);
}

void SDL_DrawGPUIndexedPrimitives(
    SDL_GPURenderPass *render_pass,
    Uint32 num_indices,
    Uint32 num_instances,
    Uint32 first_index,
    Sint32 vertex_offset,
    Uint32 first_instance)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_GRAPHICS_PIPELINE_BOUND
        SDL_GPU_CheckGraphicsBindings(render_pass);
    }

    RENDERPASS_DEVICE->DrawIndexedPrimitives(
        RENDERPASS_COMMAND_BUFFER,
        num_indices,
        num_instances,
        first_index,
        vertex_offset,
        first_instance);
}

void SDL_DrawGPUPrimitives(
    SDL_GPURenderPass *render_pass,
    Uint32 num_vertices,
    Uint32 num_instances,
    Uint32 first_vertex,
    Uint32 first_instance)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_GRAPHICS_PIPELINE_BOUND
        SDL_GPU_CheckGraphicsBindings(render_pass);
    }

    RENDERPASS_DEVICE->DrawPrimitives(
        RENDERPASS_COMMAND_BUFFER,
        num_vertices,
        num_instances,
        first_vertex,
        first_instance);
}

void SDL_DrawGPUPrimitivesIndirect(
    SDL_GPURenderPass *render_pass,
    SDL_GPUBuffer *buffer,
    Uint32 offset,
    Uint32 draw_count)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(buffer == NULL) {
        SDL_InvalidParamError("buffer");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_GRAPHICS_PIPELINE_BOUND
        SDL_GPU_CheckGraphicsBindings(render_pass);
    }

    RENDERPASS_DEVICE->DrawPrimitivesIndirect(
        RENDERPASS_COMMAND_BUFFER,
        buffer,
        offset,
        draw_count);
}

void SDL_DrawGPUIndexedPrimitivesIndirect(
    SDL_GPURenderPass *render_pass,
    SDL_GPUBuffer *buffer,
    Uint32 offset,
    Uint32 draw_count)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }
    CHECK_PARAM(buffer == NULL) {
        SDL_InvalidParamError("buffer");
        return;
    }

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
        CHECK_GRAPHICS_PIPELINE_BOUND
        SDL_GPU_CheckGraphicsBindings(render_pass);
    }

    RENDERPASS_DEVICE->DrawIndexedPrimitivesIndirect(
        RENDERPASS_COMMAND_BUFFER,
        buffer,
        offset,
        draw_count);
}

void SDL_EndGPURenderPass(
    SDL_GPURenderPass *render_pass)
{
    CHECK_PARAM(render_pass == NULL) {
        SDL_InvalidParamError("render_pass");
        return;
    }

    CommandBufferCommonHeader *commandBufferCommonHeader;
    commandBufferCommonHeader = (CommandBufferCommonHeader *)RENDERPASS_COMMAND_BUFFER;

    if (RENDERPASS_DEVICE->debug_mode) {
        CHECK_RENDERPASS
    }

    RENDERPASS_DEVICE->EndRenderPass(
        RENDERPASS_COMMAND_BUFFER);

    if (RENDERPASS_DEVICE->debug_mode) {
        commandBufferCommonHeader->render_pass.in_progress = false;
        for (Uint32 i = 0; i < MAX_COLOR_TARGET_BINDINGS; i += 1)
        {
            commandBufferCommonHeader->render_pass.color_targets[i] = NULL;
        }
        commandBufferCommonHeader->render_pass.num_color_targets = 0;
        commandBufferCommonHeader->render_pass.depth_stencil_target = NULL;
        commandBufferCommonHeader->render_pass.graphics_pipeline = NULL;
        SDL_zeroa(commandBufferCommonHeader->render_pass.vertex_sampler_bound);
        SDL_zeroa(commandBufferCommonHeader->render_pass.vertex_storage_texture_bound);
        SDL_zeroa(commandBufferCommonHeader->render_pass.vertex_storage_buffer_bound);
        SDL_zeroa(commandBufferCommonHeader->render_pass.fragment_sampler_bound);
        SDL_zeroa(commandBufferCommonHeader->render_pass.fragment_storage_texture_bound);
        SDL_zeroa(commandBufferCommonHeader->render_pass.fragment_storage_buffer_bound);
    }
}

// Compute Pass

SDL_GPUComputePass *SDL_BeginGPUComputePass(
    SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings,
    Uint32 num_storage_texture_bindings,
    const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings,
    Uint32 num_storage_buffer_bindings)
{
    CommandBufferCommonHeader *commandBufferHeader;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return NULL;
    }
    CHECK_PARAM(storage_texture_bindings == NULL && num_storage_texture_bindings > 0) {
        SDL_InvalidParamError("storage_texture_bindings");
        return NULL;
    }
    CHECK_PARAM(storage_buffer_bindings == NULL && num_storage_buffer_bindings > 0) {
        SDL_InvalidParamError("storage_buffer_bindings");
        return NULL;
    }
    CHECK_PARAM(num_storage_texture_bindings > MAX_COMPUTE_WRITE_TEXTURES) {
        SDL_InvalidParamError("num_storage_texture_bindings");
        return NULL;
    }
    CHECK_PARAM(num_storage_buffer_bindings > MAX_COMPUTE_WRITE_BUFFERS) {
        SDL_InvalidParamError("num_storage_buffer_bindings");
        return NULL;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_NULL
        CHECK_ANY_PASS_IN_PROGRESS("Cannot begin compute pass during another pass!", NULL);

        for (Uint32 i = 0; i < num_storage_texture_bindings; i += 1) {
            TextureCommonHeader *header = (TextureCommonHeader *)storage_texture_bindings[i].texture;
            if (!(header->info.usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE) && !(header->info.usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE)) {
                SDL_assert_release(!"Texture must be created with COMPUTE_STORAGE_WRITE or COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE flag");
                return NULL;
            }

            if (storage_texture_bindings[i].layer >= header->info.layer_count_or_depth) {
                SDL_assert_release(!"Storage texture layer index must be less than the texture's layer count!");
                return NULL;
            }

            if (storage_texture_bindings[i].mip_level >= header->info.num_levels) {
                SDL_assert_release(!"Storage texture mip level must be less than the texture's level count!");
                return NULL;
            }
        }

        // TODO: validate buffer usage?
    }

    COMMAND_BUFFER_DEVICE->BeginComputePass(
        command_buffer,
        storage_texture_bindings,
        num_storage_texture_bindings,
        storage_buffer_bindings,
        num_storage_buffer_bindings);

    commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        commandBufferHeader->compute_pass.in_progress = true;

        for (Uint32 i = 0; i < num_storage_texture_bindings; i += 1) {
            commandBufferHeader->compute_pass.read_write_storage_texture_bound[i] = true;
        }

        for (Uint32 i = 0; i < num_storage_buffer_bindings; i += 1) {
            commandBufferHeader->compute_pass.read_write_storage_buffer_bound[i] = true;
        }
    }

    return (SDL_GPUComputePass *)&(commandBufferHeader->compute_pass);
}

void SDL_BindGPUComputePipeline(
    SDL_GPUComputePass *compute_pass,
    SDL_GPUComputePipeline *compute_pipeline)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }
    CHECK_PARAM(compute_pipeline == NULL) {
        SDL_InvalidParamError("compute_pipeline");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS
    }

    COMPUTEPASS_DEVICE->BindComputePipeline(
        COMPUTEPASS_COMMAND_BUFFER,
        compute_pipeline);


    if (COMPUTEPASS_DEVICE->debug_mode) {
        COMPUTEPASS_BOUND_PIPELINE = compute_pipeline;
    }
}

void SDL_BindGPUComputeSamplers(
    SDL_GPUComputePass *compute_pass,
    Uint32 first_slot,
    const SDL_GPUTextureSamplerBinding *texture_sampler_bindings,
    Uint32 num_bindings)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }
    CHECK_PARAM(texture_sampler_bindings == NULL && num_bindings > 0) {
        SDL_InvalidParamError("texture_sampler_bindings");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_TEXTURE_SAMPLERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_TEXTURE_SAMPLERS_PER_STAGE");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            TextureCommonHeader *texture_header = (TextureCommonHeader *)texture_sampler_bindings[i].texture;
            if (texture_header->info.sample_count > SDL_GPU_SAMPLECOUNT_1)
            {
                SDL_assert_release(!"Multisample textures cannot be bound as samplers!");
            }
        }

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((ComputePass *)compute_pass)->sampler_bound[first_slot + i] = true;
        }
    }

    COMPUTEPASS_DEVICE->BindComputeSamplers(
        COMPUTEPASS_COMMAND_BUFFER,
        first_slot,
        texture_sampler_bindings,
        num_bindings);
}

void SDL_BindGPUComputeStorageTextures(
    SDL_GPUComputePass *compute_pass,
    Uint32 first_slot,
    SDL_GPUTexture *const *storage_textures,
    Uint32 num_bindings)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }
    CHECK_PARAM(storage_textures == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_textures");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_TEXTURES_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_TEXTURES_PER_STAGE");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((ComputePass *)compute_pass)->read_only_storage_texture_bound[first_slot + i] = true;
        }
    }

    COMPUTEPASS_DEVICE->BindComputeStorageTextures(
        COMPUTEPASS_COMMAND_BUFFER,
        first_slot,
        storage_textures,
        num_bindings);
}

void SDL_BindGPUComputeStorageBuffers(
    SDL_GPUComputePass *compute_pass,
    Uint32 first_slot,
    SDL_GPUBuffer *const *storage_buffers,
    Uint32 num_bindings)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }
    CHECK_PARAM(storage_buffers == NULL && num_bindings > 0) {
        SDL_InvalidParamError("storage_buffers");
        return;
    }
    CHECK_PARAM(first_slot + num_bindings > MAX_STORAGE_BUFFERS_PER_STAGE) {
        SDL_SetError("first_slot + num_bindings exceeds MAX_STORAGE_BUFFERS_PER_STAGE");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS

        for (Uint32 i = 0; i < num_bindings; i += 1) {
            ((ComputePass *)compute_pass)->read_only_storage_buffer_bound[first_slot + i] = true;
        }
    }

    COMPUTEPASS_DEVICE->BindComputeStorageBuffers(
        COMPUTEPASS_COMMAND_BUFFER,
        first_slot,
        storage_buffers,
        num_bindings);
}

void SDL_DispatchGPUCompute(
    SDL_GPUComputePass *compute_pass,
    Uint32 groupcount_x,
    Uint32 groupcount_y,
    Uint32 groupcount_z)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS
        CHECK_COMPUTE_PIPELINE_BOUND
        SDL_GPU_CheckComputeBindings(compute_pass);
    }

    COMPUTEPASS_DEVICE->DispatchCompute(
        COMPUTEPASS_COMMAND_BUFFER,
        groupcount_x,
        groupcount_y,
        groupcount_z);
}

void SDL_DispatchGPUComputeIndirect(
    SDL_GPUComputePass *compute_pass,
    SDL_GPUBuffer *buffer,
    Uint32 offset)
{
    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS
        CHECK_COMPUTE_PIPELINE_BOUND
        SDL_GPU_CheckComputeBindings(compute_pass);
    }

    COMPUTEPASS_DEVICE->DispatchComputeIndirect(
        COMPUTEPASS_COMMAND_BUFFER,
        buffer,
        offset);
}

void SDL_EndGPUComputePass(
    SDL_GPUComputePass *compute_pass)
{
    CommandBufferCommonHeader *commandBufferCommonHeader;

    CHECK_PARAM(compute_pass == NULL) {
        SDL_InvalidParamError("compute_pass");
        return;
    }

    if (COMPUTEPASS_DEVICE->debug_mode) {
        CHECK_COMPUTEPASS
    }

    COMPUTEPASS_DEVICE->EndComputePass(
        COMPUTEPASS_COMMAND_BUFFER);

    if (COMPUTEPASS_DEVICE->debug_mode) {
        commandBufferCommonHeader = (CommandBufferCommonHeader *)COMPUTEPASS_COMMAND_BUFFER;
        commandBufferCommonHeader->compute_pass.in_progress = false;
        commandBufferCommonHeader->compute_pass.compute_pipeline = NULL;
        SDL_zeroa(commandBufferCommonHeader->compute_pass.sampler_bound);
        SDL_zeroa(commandBufferCommonHeader->compute_pass.read_only_storage_texture_bound);
        SDL_zeroa(commandBufferCommonHeader->compute_pass.read_only_storage_buffer_bound);
        SDL_zeroa(commandBufferCommonHeader->compute_pass.read_write_storage_texture_bound);
        SDL_zeroa(commandBufferCommonHeader->compute_pass.read_write_storage_buffer_bound);
    }
}

// TransferBuffer Data

void *SDL_MapGPUTransferBuffer(
    SDL_GPUDevice *device,
    SDL_GPUTransferBuffer *transfer_buffer,
    bool cycle)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    CHECK_PARAM(transfer_buffer == NULL) {
        SDL_InvalidParamError("transfer_buffer");
        return NULL;
    }

    return device->MapTransferBuffer(
        device->driverData,
        transfer_buffer,
        cycle);
}

void SDL_UnmapGPUTransferBuffer(
    SDL_GPUDevice *device,
    SDL_GPUTransferBuffer *transfer_buffer)
{
    CHECK_DEVICE_MAGIC(device, );

    CHECK_PARAM(transfer_buffer == NULL) {
        SDL_InvalidParamError("transfer_buffer");
        return;
    }

    device->UnmapTransferBuffer(
        device->driverData,
        transfer_buffer);
}

void SDL_GenerateMipmapsForGPUTexture(
    SDL_GPUCommandBuffer *command_buffer,
    SDL_GPUTexture *texture)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(texture == NULL) {
        SDL_InvalidParamError("texture");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
        CHECK_ANY_PASS_IN_PROGRESS("Cannot generate mipmaps during a pass!", );

        TextureCommonHeader *header = (TextureCommonHeader *)texture;
        if (header->info.num_levels <= 1) {
            SDL_assert_release(!"Cannot generate mipmaps for texture with num_levels <= 1!");
            return;
        }

        if (!(header->info.usage & SDL_GPU_TEXTUREUSAGE_SAMPLER) || !(header->info.usage & SDL_GPU_TEXTUREUSAGE_COLOR_TARGET)) {
            SDL_assert_release(!"GenerateMipmaps texture must be created with SAMPLER and COLOR_TARGET usage flags!");
            return;
        }

        CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;
        commandBufferHeader->ignore_render_pass_texture_validation = true;
    }

    COMMAND_BUFFER_DEVICE->GenerateMipmaps(
        command_buffer,
        texture);

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;
        commandBufferHeader->ignore_render_pass_texture_validation = false;
    }
}

void SDL_BlitGPUTexture(
    SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUBlitInfo *info)
{
    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return;
    }
    CHECK_PARAM(info == NULL) {
        SDL_InvalidParamError("info");
        return;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER
        CHECK_ANY_PASS_IN_PROGRESS("Cannot blit during a pass!", );

        // Validation
        bool failed = false;
        TextureCommonHeader *srcHeader = (TextureCommonHeader *)info->source.texture;
        TextureCommonHeader *dstHeader = (TextureCommonHeader *)info->destination.texture;

        if (srcHeader == NULL) {
            SDL_assert_release(!"Blit source texture must be non-NULL");
            return; // attempting to proceed will crash
        }
        if (dstHeader == NULL) {
            SDL_assert_release(!"Blit destination texture must be non-NULL");
            return; // attempting to proceed will crash
        }
        if (srcHeader->info.sample_count != SDL_GPU_SAMPLECOUNT_1) {
            SDL_assert_release(!"Blit source texture must have a sample count of 1");
            failed = true;
        }
        if ((srcHeader->info.usage & SDL_GPU_TEXTUREUSAGE_SAMPLER) == 0) {
            SDL_assert_release(!"Blit source texture must be created with the SAMPLER usage flag");
            failed = true;
        }
        if ((dstHeader->info.usage & SDL_GPU_TEXTUREUSAGE_COLOR_TARGET) == 0) {
            SDL_assert_release(!"Blit destination texture must be created with the COLOR_TARGET usage flag");
            failed = true;
        }
        if (IsDepthFormat(srcHeader->info.format)) {
            SDL_assert_release(!"Blit source texture cannot have a depth format");
            failed = true;
        }
        if (info->source.w == 0 || info->source.h == 0 || info->destination.w == 0 || info->destination.h == 0) {
            SDL_assert_release(!"Blit source/destination regions must have non-zero width, height, and depth");
            failed = true;
        }

        if (failed) {
            return;
        }
    }

    COMMAND_BUFFER_DEVICE->Blit(
        command_buffer,
        info);
}

// Submission/Presentation

bool SDL_WindowSupportsGPUSwapchainComposition(
    SDL_GPUDevice *device,
    SDL_Window *window,
    SDL_GPUSwapchainComposition swapchain_composition)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(window == NULL) {
        SDL_InvalidParamError("window");
        return false;
    }

    if (device->debug_mode) {
        CHECK_SWAPCHAINCOMPOSITION_ENUM_INVALID(swapchain_composition, false);
    }

    return device->SupportsSwapchainComposition(
        device->driverData,
        window,
        swapchain_composition);
}

bool SDL_WindowSupportsGPUPresentMode(
    SDL_GPUDevice *device,
    SDL_Window *window,
    SDL_GPUPresentMode present_mode)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(window == NULL) {
        SDL_InvalidParamError("window");
        return false;
    }

    if (device->debug_mode) {
        CHECK_PRESENTMODE_ENUM_INVALID(present_mode, false);
    }

    return device->SupportsPresentMode(
        device->driverData,
        window,
        present_mode);
}

bool SDL_ClaimWindowForGPUDevice(
    SDL_GPUDevice *device,
    SDL_Window *window)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(window == NULL) {
        return SDL_InvalidParamError("window");
    }

    if ((window->flags & SDL_WINDOW_TRANSPARENT) != 0) {
        return SDL_SetError("The GPU API doesn't support transparent windows");
    }

    return device->ClaimWindow(
        device->driverData,
        window);
}

void SDL_ReleaseWindowFromGPUDevice(
    SDL_GPUDevice *device,
    SDL_Window *window)
{
    CHECK_DEVICE_MAGIC(device, );

    CHECK_PARAM(window == NULL) {
        SDL_InvalidParamError("window");
        return;
    }

    device->ReleaseWindow(
        device->driverData,
        window);
}

bool SDL_SetGPUSwapchainParameters(
    SDL_GPUDevice *device,
    SDL_Window *window,
    SDL_GPUSwapchainComposition swapchain_composition,
    SDL_GPUPresentMode present_mode)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(window == NULL) {
        SDL_InvalidParamError("window");
        return false;
    }

    if (device->debug_mode) {
        CHECK_SWAPCHAINCOMPOSITION_ENUM_INVALID(swapchain_composition, false);
        CHECK_PRESENTMODE_ENUM_INVALID(present_mode, false);
    }

    return device->SetSwapchainParameters(
        device->driverData,
        window,
        swapchain_composition,
        present_mode);
}

bool SDL_SetGPUAllowedFramesInFlight(
    SDL_GPUDevice *device,
    Uint32 allowed_frames_in_flight)
{
    CHECK_DEVICE_MAGIC(device, false);

    if (device->debug_mode) {
        if (allowed_frames_in_flight < 1 || allowed_frames_in_flight > 3)
        {
            SDL_COMPILE_TIME_ASSERT(max_frames_in_flight, MAX_FRAMES_IN_FLIGHT == 3);
            SDL_assert_release(!"allowed_frames_in_flight value must be between 1 and 3!");
        }
    }

    allowed_frames_in_flight = SDL_clamp(allowed_frames_in_flight, 1, 3);
    return device->SetAllowedFramesInFlight(
        device->driverData,
        allowed_frames_in_flight);
}

SDL_GPUTextureFormat SDL_GetGPUSwapchainTextureFormat(
    SDL_GPUDevice *device,
    SDL_Window *window)
{
    CHECK_DEVICE_MAGIC(device, SDL_GPU_TEXTUREFORMAT_INVALID);

    CHECK_PARAM(window == NULL) {
        SDL_InvalidParamError("window");
        return SDL_GPU_TEXTUREFORMAT_INVALID;
    }

    return device->GetSwapchainTextureFormat(
        device->driverData,
        window);
}

bool SDL_AcquireGPUSwapchainTexture(
    SDL_GPUCommandBuffer *command_buffer,
    SDL_Window *window,
    SDL_GPUTexture **swapchain_texture,
    Uint32 *swapchain_texture_width,
    Uint32 *swapchain_texture_height)
{
    CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    CHECK_PARAM(command_buffer == NULL) {
        return SDL_InvalidParamError("command_buffer");
    }
    CHECK_PARAM(window == NULL) {
        return SDL_InvalidParamError("window");
    }
    CHECK_PARAM(swapchain_texture == NULL) {
        return SDL_InvalidParamError("swapchain_texture");
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_FALSE
        CHECK_ANY_PASS_IN_PROGRESS("Cannot acquire a swapchain texture during a pass!", false);
    }

    bool result = COMMAND_BUFFER_DEVICE->AcquireSwapchainTexture(
        command_buffer,
        window,
        swapchain_texture,
        swapchain_texture_width,
        swapchain_texture_height);

    if (*swapchain_texture != NULL){
        commandBufferHeader->swapchain_texture_acquired = true;
    }

    return result;
}

bool SDL_WaitForGPUSwapchain(
    SDL_GPUDevice *device,
    SDL_Window *window)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(window == NULL) {
        return SDL_InvalidParamError("window");
    }

    return device->WaitForSwapchain(
        device->driverData,
        window);
}

bool SDL_WaitAndAcquireGPUSwapchainTexture(
    SDL_GPUCommandBuffer *command_buffer,
    SDL_Window *window,
    SDL_GPUTexture **swapchain_texture,
    Uint32 *swapchain_texture_width,
    Uint32 *swapchain_texture_height)
{
    CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    CHECK_PARAM(command_buffer == NULL) {
        return SDL_InvalidParamError("command_buffer");
    }
    CHECK_PARAM(window == NULL) {
        return SDL_InvalidParamError("window");
    }
    CHECK_PARAM(swapchain_texture == NULL) {
        return SDL_InvalidParamError("swapchain_texture");
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_FALSE
        CHECK_ANY_PASS_IN_PROGRESS("Cannot acquire a swapchain texture during a pass!", false);
    }

    bool result = COMMAND_BUFFER_DEVICE->WaitAndAcquireSwapchainTexture(
        command_buffer,
        window,
        swapchain_texture,
        swapchain_texture_width,
        swapchain_texture_height);

    if (*swapchain_texture != NULL){
        commandBufferHeader->swapchain_texture_acquired = true;
    }

    return result;
}

bool SDL_SubmitGPUCommandBuffer(
    SDL_GPUCommandBuffer *command_buffer)
{
    CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return false;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_FALSE
        if (
            commandBufferHeader->render_pass.in_progress ||
            commandBufferHeader->compute_pass.in_progress ||
            commandBufferHeader->copy_pass.in_progress) {
            SDL_assert_release(!"Cannot submit command buffer while a pass is in progress!");
            return false;
        }
    }

    commandBufferHeader->submitted = true;

    return COMMAND_BUFFER_DEVICE->Submit(
        command_buffer);
}

SDL_GPUFence *SDL_SubmitGPUCommandBufferAndAcquireFence(
    SDL_GPUCommandBuffer *command_buffer)
{
    CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return NULL;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_NULL
        if (
            commandBufferHeader->render_pass.in_progress ||
            commandBufferHeader->compute_pass.in_progress ||
            commandBufferHeader->copy_pass.in_progress) {
            SDL_assert_release(!"Cannot submit command buffer while a pass is in progress!");
            return NULL;
        }
    }

    commandBufferHeader->submitted = true;

    return COMMAND_BUFFER_DEVICE->SubmitAndAcquireFence(
        command_buffer);
}

bool SDL_CancelGPUCommandBuffer(
    SDL_GPUCommandBuffer *command_buffer)
{
    CommandBufferCommonHeader *commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return false;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        if (commandBufferHeader->swapchain_texture_acquired) {
            SDL_assert_release(!"Cannot cancel command buffer after a swapchain texture has been acquired!");
            return false;
        }
    }

    return COMMAND_BUFFER_DEVICE->Cancel(
        command_buffer);
}

bool SDL_WaitForGPUIdle(
    SDL_GPUDevice *device)
{
    CHECK_DEVICE_MAGIC(device, false);

    return device->Wait(
        device->driverData);
}

bool SDL_WaitForGPUFences(
    SDL_GPUDevice *device,
    bool wait_all,
    SDL_GPUFence *const *fences,
    Uint32 num_fences)
{
    CHECK_DEVICE_MAGIC(device, false);

    if (!num_fences) {
        return true;
    }

    CHECK_PARAM(fences == NULL) {
        SDL_InvalidParamError("fences");
        return false;
    }

    return device->WaitForFences(
        device->driverData,
        wait_all,
        fences,
        num_fences);
}

bool SDL_QueryGPUFence(
    SDL_GPUDevice *device,
    SDL_GPUFence *fence)
{
    CHECK_DEVICE_MAGIC(device, false);

    CHECK_PARAM(fence == NULL) {
        SDL_InvalidParamError("fence");
        return false;
    }

    return device->QueryFence(
        device->driverData,
        fence);
}

void SDL_ReleaseGPUFence(
    SDL_GPUDevice *device,
    SDL_GPUFence *fence)
{
    if(fence == NULL) {
        return;
    }

    CHECK_DEVICE_MAGIC(device, );

    device->ReleaseFence(
        device->driverData,
        fence);
}

XrResult SDL_CreateGPUXRSession(
    SDL_GPUDevice *device,
    const XrSessionCreateInfo *createinfo,
    XrSession *session)
{
    CHECK_DEVICE_MAGIC(device, XR_ERROR_HANDLE_INVALID);

    return device->CreateXRSession(device->driverData, createinfo, session);
}

SDL_GPUTextureFormat* SDL_GetGPUXRSwapchainFormats(
    SDL_GPUDevice *device,
    XrSession session,
    int *num_formats)
{
    CHECK_DEVICE_MAGIC(device, NULL);

    return device->GetXRSwapchainFormats(device->driverData, session, num_formats);
}

XrResult SDL_CreateGPUXRSwapchain(
    SDL_GPUDevice *device,
    XrSession session,
    const XrSwapchainCreateInfo *createinfo,
    SDL_GPUTextureFormat format,
    XrSwapchain *swapchain,
    SDL_GPUTexture ***textures)
{
    CHECK_DEVICE_MAGIC(device, XR_ERROR_HANDLE_INVALID);

    return device->CreateXRSwapchain(device->driverData, session, createinfo, format, swapchain, textures);
}
