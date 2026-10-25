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

// The SDL 2D rendering system

#include "SDL_sysrender.h"
#include "SDL_render_internal.h"
#include "software/SDL_render_sw_c.h"
#include "software/SDL_render_sw_geometry.h"
#include "../events/SDL_windowevents_c.h"
#include "../video/SDL_pixels_c.h"
#include "../video/SDL_sysvideo.h"
#include "../video/SDL_video_c.h"

#ifdef SDL_PLATFORM_ANDROID
#include "../core/android/SDL_android.h"
#include "../video/android/SDL_androidevents.h"
#endif

/* as a courtesy to iOS apps, we don't try to draw when in the background, as
that will crash the app. However, these apps _should_ have used
SDL_AddEventWatch to catch SDL_EVENT_WILL_ENTER_BACKGROUND events and stopped
drawing themselves. Other platforms still draw, as the compositor can use it,
and more importantly: drawing to render targets isn't lost. But I still think
this should probably be removed at some point in the future.  --ryan. */
#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_TVOS) || defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_OPENHARMONY)
#define DONT_DRAW_WHILE_HIDDEN 1
#else
#define DONT_DRAW_WHILE_HIDDEN 0
#endif


#ifndef SDL_RENDER_DISABLED
static const SDL_RenderDriver *render_drivers[] = {
#ifdef SDL_VIDEO_RENDER_D3D11
    &D3D11_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_D3D12
    &D3D12_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_D3D
    &D3D_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_METAL
    &METAL_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_NGAGE
    &NGAGE_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_OGL
    &GL_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_OGL_ES2
    &GLES2_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_OGL_ES
    &GLES_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_PS2
    &PS2_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_PSP
    &PSP_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_VITA_GXM
    &VITA_GXM_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_VULKAN
    &VULKAN_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_GPU
    &GPU_RenderDriver,
#endif
#ifdef SDL_VIDEO_RENDER_SW
    &SW_RenderDriver,
#endif
    NULL
};
#endif // !SDL_RENDER_DISABLED

static SDL_Renderer *SDL_renderers;

const int SDL_rect_index_order[] = { 0, 1, 2, 0, 2, 3 };

void SDL_QuitRender(void)
{
    while (SDL_renderers) {
        SDL_DestroyRenderer(SDL_renderers);
    }
}

bool SDL_AddSupportedTextureFormat(SDL_Renderer *renderer, SDL_PixelFormat format)
{
    SDL_PixelFormat *texture_formats = (SDL_PixelFormat *)SDL_realloc((void *)renderer->texture_formats, (renderer->num_texture_formats + 2) * sizeof(SDL_PixelFormat));
    if (!texture_formats) {
        return false;
    }
    texture_formats[renderer->num_texture_formats++] = format;
    texture_formats[renderer->num_texture_formats] = SDL_PIXELFORMAT_UNKNOWN;
    renderer->texture_formats = texture_formats;
    SDL_SetPointerProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, texture_formats);
    return true;
}

static void RecycleRenderCommandQueue(SDL_Renderer *renderer)
{
    if (renderer->render_commands_tail) {
        renderer->render_commands_tail->next = renderer->render_commands_pool;
        renderer->render_commands_pool = renderer->render_commands;
        renderer->render_commands_tail = NULL;
        renderer->render_commands = NULL;
    }
    renderer->vertex_data_used = 0;
    renderer->render_command_generation++;
    renderer->color_queued = false;
    renderer->viewport_queued = false;
    renderer->cliprect_queued = false;
}

static bool FlushRenderCommands(SDL_Renderer *renderer)
{
    bool result;

    SDL_assert((renderer->render_commands == NULL) == (renderer->render_commands_tail == NULL));

    if (!renderer->render_commands) { // nothing to do!
        SDL_assert(renderer->vertex_data_used == 0);
        return true;
    }

    if (renderer->window && !renderer->target && renderer->window->native_window_destroyed) {
        result = SDL_SetError("Renderer's window has been destroyed, can't present");
        goto discard_commands;
    }

    DebugLogRenderCommands(renderer->render_commands);
#if DONT_DRAW_WHILE_HIDDEN
    // Don't send commands to the GPU while we're hidden
    if (renderer->hidden) {
        result = true;
    } else
#endif
    result = renderer->RunCommandQueue(renderer, renderer->render_commands, renderer->vertex_data, renderer->vertex_data_used);

discard_commands:
    RecycleRenderCommandQueue(renderer);
    return result;
}

bool SDL_FlushRenderCommandsIfTextureNeeded(SDL_Texture *texture)
{
    SDL_Renderer *renderer = texture->renderer;
    if (texture->last_command_generation == renderer->render_command_generation) {
        // the current command queue depends on this texture, flush the queue now before it changes
        return FlushRenderCommands(renderer);
    }
    return true;
}

bool SDL_FlushRenderCommandsIfPaletteNeeded(SDL_Renderer *renderer, SDL_TexturePalette *palette)
{
    if (palette->last_command_generation == renderer->render_command_generation) {
        // the current command queue depends on this palette, flush the queue now before it changes
        return FlushRenderCommands(renderer);
    }
    return true;
}

bool SDL_FlushRenderCommandsIfGPURenderStateNeeded(SDL_GPURenderState *state)
{
    SDL_Renderer *renderer = state->renderer;
    if (state->last_command_generation == renderer->render_command_generation) {
        // the current command queue depends on this state, flush the queue now before it changes
        return FlushRenderCommands(renderer);
    }
    return true;
}

bool SDL_FlushRenderer(SDL_Renderer *renderer)
{
    if (!FlushRenderCommands(renderer)) {
        return false;
    }
    renderer->InvalidateCachedState(renderer);
    return true;
}

void *SDL_AllocateRenderVertices(SDL_Renderer *renderer, size_t numbytes, size_t alignment, size_t *offset)
{
    const size_t needed = renderer->vertex_data_used + numbytes + alignment;
    const size_t current_offset = renderer->vertex_data_used;

    const size_t aligner = (alignment && ((current_offset & (alignment - 1)) != 0)) ? (alignment - (current_offset & (alignment - 1))) : 0;
    const size_t aligned = current_offset + aligner;

    if (renderer->vertex_data_allocation < needed) {
        const size_t current_allocation = renderer->vertex_data ? renderer->vertex_data_allocation : 1024;
        size_t newsize = current_allocation * 2;
        void *ptr;
        while (newsize < needed) {
            newsize *= 2;
        }

        ptr = SDL_realloc(renderer->vertex_data, newsize);

        if (!ptr) {
            return NULL;
        }
        renderer->vertex_data = ptr;
        renderer->vertex_data_allocation = newsize;
    }

    if (offset) {
        *offset = aligned;
    }

    renderer->vertex_data_used += aligner + numbytes;

    return ((Uint8 *)renderer->vertex_data) + aligned;
}

static SDL_RenderCommand *AllocateRenderCommand(SDL_Renderer *renderer)
{
    SDL_RenderCommand *result = NULL;

    result = renderer->render_commands_pool;
    if (result) {
        renderer->render_commands_pool = result->next;
        result->next = NULL;
    } else {
        result = (SDL_RenderCommand *)SDL_calloc(1, sizeof(*result));
        if (!result) {
            return NULL;
        }
    }

    SDL_assert((renderer->render_commands == NULL) == (renderer->render_commands_tail == NULL));
    if (renderer->render_commands_tail) {
        renderer->render_commands_tail->next = result;
    } else {
        renderer->render_commands = result;
    }
    renderer->render_commands_tail = result;

    return result;
}

bool SDL_QueueCmdSetViewport(SDL_Renderer *renderer)
{
    bool result = true;

    SDL_Rect viewport = renderer->view->pixel_viewport;

    if (!renderer->viewport_queued ||
        SDL_memcmp(&viewport, &renderer->last_queued_viewport, sizeof(viewport)) != 0) {
        SDL_RenderCommand *cmd = AllocateRenderCommand(renderer);
        if (cmd) {
            cmd->command = SDL_RENDERCMD_SETVIEWPORT;
            cmd->data.viewport.first = 0; // render backend will fill this in.
            SDL_copyp(&cmd->data.viewport.rect, &viewport);
            result = renderer->QueueSetViewport(renderer, cmd);
            if (!result) {
                cmd->command = SDL_RENDERCMD_NO_OP;
            } else {
                SDL_copyp(&renderer->last_queued_viewport, &viewport);
                renderer->viewport_queued = true;
            }
        } else {
            result = false;
        }
    }
    return result;
}

bool SDL_QueueCmdSetClipRect(SDL_Renderer *renderer)
{
    bool result = true;

    const SDL_RenderViewState *view = renderer->view;
    SDL_Rect clip_rect = view->pixel_clip_rect;
    if (!renderer->cliprect_queued ||
        view->clipping_enabled != renderer->last_queued_cliprect_enabled ||
        SDL_memcmp(&clip_rect, &renderer->last_queued_cliprect, sizeof(clip_rect)) != 0) {
        SDL_RenderCommand *cmd = AllocateRenderCommand(renderer);
        if (cmd) {
            cmd->command = SDL_RENDERCMD_SETCLIPRECT;
            cmd->data.cliprect.enabled = view->clipping_enabled;
            SDL_copyp(&cmd->data.cliprect.rect, &clip_rect);
            SDL_copyp(&renderer->last_queued_cliprect, &clip_rect);
            renderer->last_queued_cliprect_enabled = view->clipping_enabled;
            renderer->cliprect_queued = true;
        } else {
            result = false;
        }
    }
    return result;
}

static bool QueueCmdSetDrawColor(SDL_Renderer *renderer, SDL_FColor *color)
{
    bool result = true;

    if (!renderer->color_queued ||
        color->r != renderer->last_queued_color.r ||
        color->g != renderer->last_queued_color.g ||
        color->b != renderer->last_queued_color.b ||
        color->a != renderer->last_queued_color.a) {
        SDL_RenderCommand *cmd = AllocateRenderCommand(renderer);
        result = false;

        if (cmd) {
            cmd->command = SDL_RENDERCMD_SETDRAWCOLOR;
            cmd->data.color.first = 0; // render backend will fill this in.
            cmd->data.color.color_scale = renderer->color_scale;
            cmd->data.color.color = *color;
            result = renderer->QueueSetDrawColor(renderer, cmd);
            if (!result) {
                cmd->command = SDL_RENDERCMD_NO_OP;
            } else {
                renderer->last_queued_color = *color;
                renderer->color_queued = true;
            }
        }
    }
    return result;
}

static bool QueueCmdClear(SDL_Renderer *renderer)
{
    SDL_RenderCommand *cmd = AllocateRenderCommand(renderer);
    if (!cmd) {
        return false;
    }

    cmd->command = SDL_RENDERCMD_CLEAR;
    cmd->data.color.first = 0;
    cmd->data.color.color_scale = renderer->color_scale;
    cmd->data.color.color = renderer->color;
    return true;
}

static bool QueueCmdDrawState(SDL_Renderer *renderer, const SDL_RenderCommandType cmdtype, SDL_FColor *color)
{
    bool result = true;

    if (cmdtype != SDL_RENDERCMD_GEOMETRY) {
        result = QueueCmdSetDrawColor(renderer, color);
    }

    /* Set the viewport and clip rect directly before draws, so the backends
     * don't have to worry about that state not being valid at draw time. */
    if (result && !renderer->viewport_queued) {
        result = SDL_QueueCmdSetViewport(renderer);
    }
    if (result && !renderer->cliprect_queued) {
        result = SDL_QueueCmdSetClipRect(renderer);
    }
    return result;
}

SDL_RenderCommand *SDL_PrepQueueCmdDraw(SDL_Renderer *renderer, const SDL_RenderCommandType cmdtype, SDL_Texture *texture)
{
    SDL_RenderCommand *cmd = NULL;
    SDL_FColor *color;
    SDL_BlendMode blendMode;

    if (texture) {
        color = &texture->color;
        blendMode = texture->blendMode;
    } else {
        color = &renderer->color;
        blendMode = renderer->blendMode;
    }

    if (QueueCmdDrawState(renderer, cmdtype, color)) {
        cmd = AllocateRenderCommand(renderer);
        if (cmd) {
            cmd->command = cmdtype;
            cmd->data.draw.first = 0; // render backend will fill this in.
            cmd->data.draw.count = 0; // render backend will fill this in.
            cmd->data.draw.color_scale = renderer->color_scale;
            cmd->data.draw.color = *color;
            cmd->data.draw.blend = blendMode;
            cmd->data.draw.texture = texture;
            if (texture) {
                cmd->data.draw.texture_scale_mode = texture->scaleMode;
            }
            cmd->data.draw.texture_address_mode_u = SDL_TEXTURE_ADDRESS_CLAMP;
            cmd->data.draw.texture_address_mode_v = SDL_TEXTURE_ADDRESS_CLAMP;
            cmd->data.draw.gpu_render_state = renderer->gpu_render_state;
            if (renderer->gpu_render_state) {
                renderer->gpu_render_state->last_command_generation = renderer->render_command_generation;
            }
        }
    }
    return cmd;
}

bool SDL_QueueCmdGeometry(SDL_Renderer *renderer, SDL_Texture *texture,
                            const float *xy, int xy_stride,
                            const SDL_FColor *color, int color_stride,
                            const float *uv, int uv_stride,
                            int num_vertices,
                            const void *indices, int num_indices, int size_indices,
                            float scale_x, float scale_y,
                            SDL_TextureAddressMode texture_address_mode_u, SDL_TextureAddressMode texture_address_mode_v)
{
    SDL_RenderCommand *cmd;
    bool result = false;
    cmd = SDL_PrepQueueCmdDraw(renderer, SDL_RENDERCMD_GEOMETRY, texture);
    if (cmd) {
        cmd->data.draw.texture_address_mode_u = texture_address_mode_u;
        cmd->data.draw.texture_address_mode_v = texture_address_mode_v;
        result = renderer->QueueGeometry(renderer, cmd, texture,
                                         xy, xy_stride,
                                         color, color_stride, uv, uv_stride,
                                         num_vertices, indices, num_indices, size_indices,
                                         scale_x, scale_y);
        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    return result;
}

int SDL_GetNumRenderDrivers(void)
{
#ifndef SDL_RENDER_DISABLED
    return SDL_arraysize(render_drivers) - 1;
#else
    return 0;
#endif
}

const char *SDL_GetRenderDriver(int index)
{
#ifndef SDL_RENDER_DISABLED
    CHECK_PARAM(index < 0 || index >= SDL_GetNumRenderDrivers()) {
        SDL_InvalidParamError("index");
        return NULL;
    }
    return render_drivers[index]->name;
#else
    SDL_SetError("SDL not built with rendering support");
    return NULL;
#endif
}

static bool SDL_RendererEventWatch(void *userdata, SDL_Event *event)
{
    SDL_Renderer *renderer = (SDL_Renderer *)userdata;
    SDL_Window *window = renderer->window;

    if (event->window.windowID != SDL_GetWindowID(window)) {
        return true;
    }

    if (renderer->WindowEvent) {
        renderer->WindowEvent(renderer, &event->window);
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED ||
        event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED ||
        event->type == SDL_EVENT_WINDOW_METAL_VIEW_RESIZED) {
            SDL_RenderViewState *view = renderer->view;
            renderer->view = &renderer->main_view;  // only update the main_view (the window framebuffer) for window changes.
            SDL_UpdateLogicalPresentation(renderer);
            renderer->view = view;  // put us back on whatever the current render target's actual view is.
    } else if (event->type == SDL_EVENT_WINDOW_HIDDEN) {
        renderer->hidden = true;
    } else if (event->type == SDL_EVENT_WINDOW_SHOWN) {
        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
            renderer->hidden = false;
        }
    } else if (event->type == SDL_EVENT_WINDOW_MINIMIZED) {
        renderer->hidden = true;
    } else if (event->type == SDL_EVENT_WINDOW_RESTORED ||
               event->type == SDL_EVENT_WINDOW_MAXIMIZED) {
        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_HIDDEN)) {
            renderer->hidden = false;
        }
    } else if (event->type == SDL_EVENT_WINDOW_DISPLAY_CHANGED ||
               event->type == SDL_EVENT_WINDOW_HDR_STATE_CHANGED) {
        SDL_UpdateHDRProperties(renderer);
    }
    return true;
}

bool SDL_CreateWindowAndRenderer(const char *title, int width, int height, SDL_WindowFlags window_flags, SDL_Window **window, SDL_Renderer **renderer)
{
    CHECK_PARAM(!window) {
        return SDL_InvalidParamError("window");
    }

    CHECK_PARAM(!renderer) {
        return SDL_InvalidParamError("renderer");
    }

    // Hide the window so if the renderer recreates it, we don't get a visual flash on screen
    bool hidden = (window_flags & SDL_WINDOW_HIDDEN) != 0;
    window_flags |= SDL_WINDOW_HIDDEN;
    *window = SDL_CreateWindow(title, width, height, window_flags);
    if (!*window) {
        *renderer = NULL;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, NULL);
    if (!*renderer) {
        SDL_DestroyWindow(*window);
        *window = NULL;
        return false;
    }

    if (!hidden) {
        SDL_ShowWindow(*window);
    }

    return true;
}

#ifndef SDL_RENDER_DISABLED
static SDL_INLINE void VerifyDrawQueueFunctions(const SDL_Renderer *renderer)
{
    /* all of these functions are required to be implemented, even as no-ops, so we don't
        have to check that they aren't NULL over and over. */
    SDL_assert(renderer->QueueSetViewport != NULL);
    SDL_assert(renderer->QueueSetDrawColor != NULL);
    SDL_assert(renderer->QueueDrawPoints != NULL);
    SDL_assert(renderer->QueueDrawLines != NULL || renderer->QueueGeometry != NULL);
    SDL_assert(renderer->QueueFillRects != NULL || renderer->QueueGeometry != NULL);
    SDL_assert(renderer->QueueCopy != NULL || renderer->QueueGeometry != NULL);
    SDL_assert(renderer->RunCommandQueue != NULL);
}

static SDL_RenderLineMethod SDL_GetRenderLineMethod(void)
{
    const char *hint = SDL_GetHint(SDL_HINT_RENDER_LINE_METHOD);

    int method = 0;
    if (hint) {
        method = SDL_atoi(hint);
    }
    switch (method) {
    case 1:
        return SDL_RENDERLINEMETHOD_POINTS;
    case 2:
        return SDL_RENDERLINEMETHOD_LINES;
    case 3:
        return SDL_RENDERLINEMETHOD_GEOMETRY;
    default:
        return SDL_RENDERLINEMETHOD_POINTS;
    }
}

static void SDL_CalculateSimulatedVSyncInterval(SDL_Renderer *renderer, SDL_Window *window)
{
    SDL_DisplayID displayID = window ? SDL_GetDisplayForWindow(window) : 0;
    const SDL_DisplayMode *mode;
    int refresh_num, refresh_den;

    if (displayID == 0) {
        displayID = SDL_GetPrimaryDisplay();
    }
    mode = SDL_GetDesktopDisplayMode(displayID);
    if (mode && mode->refresh_rate_numerator > 0 && mode->refresh_rate_denominator > 0) {
        refresh_num = mode->refresh_rate_numerator;
        refresh_den = mode->refresh_rate_denominator;
    } else {
        // Pick a good default refresh rate
        refresh_num = 60;
        refresh_den = 1;
    }
    // Flip numerator and denominator to change from framerate to interval
    renderer->simulate_vsync_interval_ns = (SDL_NS_PER_SECOND * refresh_den) / refresh_num;
}

#endif // !SDL_RENDER_DISABLED


static bool SDL_UpdateSoftwareRendererOutput(SDL_Renderer *renderer, SDL_Window *window, SDL_Surface *surface)
{
#ifdef SDL_VIDEO_RENDER_SW
    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(!renderer->software) {
        return SDL_InvalidParamError("renderer");
    }

    SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
    if (window == renderer->window &&
        surface == SDL_GetPointerProperty(props, SDL_PROP_RENDERER_SURFACE_POINTER, NULL)) {
        return true;
    }

    CHECK_PARAM(window && SDL_WindowHasSurface(window)) {
        return SDL_SetError("Window already has an associated surface");
    }

    CHECK_PARAM(window && SDL_GetRenderer(window)) {
        return SDL_SetError("Window already has an associated renderer");
    }

    // Make sure all drawing to a surface is complete
    FlushRenderCommands(renderer);

    if (window) {
        surface = SDL_GetWindowSurface(window);

        if (!SDL_SurfaceValid(surface)) {
            return false;
        }

        if (!SDL_AddWindowRenderer(window, renderer)) {
            SDL_DestroyWindowSurface(window);
            return false;
        }
    }

    if (!SW_UpdateRendererSurface(renderer, surface)) {
        if (window) {
            SDL_RemoveWindowRenderer(window, renderer);
            SDL_DestroyWindowSurface(window);
        }
        return false;
    }

    if (renderer->window) {
        SDL_RemoveWindowEventWatch(SDL_WINDOW_EVENT_WATCH_NORMAL, SDL_RendererEventWatch, renderer);
        SDL_RemoveWindowRenderer(renderer->window, renderer);
        SDL_DestroyWindowSurface(renderer->window);
    }

    renderer->window = window;

    if (surface) {
        renderer->main_view.pixel_w = surface->w;
        renderer->main_view.pixel_h = surface->h;
    } else {
        renderer->main_view.pixel_w = 0;
        renderer->main_view.pixel_h = 0;
    }
    renderer->main_view.viewport.w = -1;
    renderer->main_view.viewport.h = -1;
    SDL_UpdatePixelViewport(renderer, &renderer->main_view);
    SDL_UpdatePixelClipRect(renderer, &renderer->main_view);
    SDL_UpdateMainViewDimensions(renderer);

    renderer->SDR_white_point = 1.0f;
    renderer->HDR_headroom = 1.0f;

    renderer->transparent_window = false;
    renderer->hidden = false;

    if (window) {
        SDL_SetPointerProperty(props, SDL_PROP_RENDERER_WINDOW_POINTER, window);
        SDL_SetPointerProperty(props, SDL_PROP_RENDERER_SURFACE_POINTER, NULL);

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_TRANSPARENT) {
            renderer->transparent_window = true;
        }

        if (SDL_GetWindowFlags(window) & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED)) {
            renderer->hidden = true;
        }

        SDL_UpdateHDRProperties(renderer);
        SDL_AddWindowEventWatch(SDL_WINDOW_EVENT_WATCH_NORMAL, SDL_RendererEventWatch, renderer);
    } else {
        SDL_SetPointerProperty(props, SDL_PROP_RENDERER_WINDOW_POINTER, NULL);
        SDL_SetPointerProperty(props, SDL_PROP_RENDERER_SURFACE_POINTER, surface);
    }

    int vsync = (int)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_VSYNC_NUMBER, 0);
    SDL_SetRenderVSync(renderer, vsync);
    SDL_CalculateSimulatedVSyncInterval(renderer, window);

    if (!renderer->target) {
        SDL_SetRenderViewport(renderer, NULL);
    }

    SDL_ClearError();

    return true;
#else
    return SDL_Unsupported();
#endif
}

bool SDL_SetSoftwareRendererWindow(SDL_Renderer *renderer, SDL_Window *window)
{
    return SDL_UpdateSoftwareRendererOutput(renderer, window, NULL);
}

bool SDL_SetSoftwareRendererSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    return SDL_UpdateSoftwareRendererOutput(renderer, NULL, surface);
}

#ifndef SDL_RENDER_DISABLED
static bool SDL_SelectRenderDriver(SDL_Renderer *renderer, SDL_Window *window, SDL_PropertiesID props, const char *driver_name)
{
    char *driver_error = NULL;
    bool rc = false;
    if (!driver_name) {
        driver_name = SDL_GetHint(SDL_HINT_RENDER_DRIVER);
    }

    if (driver_name && *driver_name != 0) {
        const char *driver_attempt = driver_name;
        while (driver_attempt && *driver_attempt != 0 && !rc) {
            const char *driver_attempt_end = SDL_strchr(driver_attempt, ',');
            const size_t driver_attempt_len = (driver_attempt_end) ? (driver_attempt_end - driver_attempt) : SDL_strlen(driver_attempt);

            for (int i = 0; render_drivers[i]; i++) {
                const SDL_RenderDriver *driver = render_drivers[i];
                if ((driver_attempt_len == SDL_strlen(driver->name)) && (SDL_strncasecmp(driver->name, driver_attempt, driver_attempt_len) == 0)) {
                    if (driver_error) {
                        // Free any previous driver error
                        SDL_free(driver_error);
                        driver_error = NULL;
                    }

                    rc = driver->CreateRenderer(renderer, window, props);
                    if (rc) {
                        break;
                    }
                    driver_error = SDL_strdup(SDL_GetError());
                    SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Couldn't create renderer %s: %s\n", driver->name, driver_error);
                    SDL_DestroyRendererWithoutFreeing(renderer);
                    SDL_zerop(renderer); // make sure we don't leave function pointers from a previous CreateRenderer() in this struct.
                }
            }

            driver_attempt = (driver_attempt_end) ? (driver_attempt_end + 1) : NULL;
        }
    } else {
        for (int i = 0; render_drivers[i]; i++) {
            const SDL_RenderDriver *driver = render_drivers[i];
            rc = driver->CreateRenderer(renderer, window, props);
            if (rc) {
                break;
            }
            SDL_DestroyRendererWithoutFreeing(renderer);
            SDL_zerop(renderer);  // make sure we don't leave function pointers from a previous CreateRenderer() in this struct.
        }
    }

    if (rc) {
        SDL_DebugLogBackend("render", renderer->name);
    } else if (driver_name) {
        if (driver_error) {
            SDL_SetError("%s", driver_error);
        } else {
            SDL_SetError("%s not available", driver_name);
        }
    } else {
        SDL_SetError("Couldn't find matching render driver");
    }
    SDL_free(driver_error);
    return rc;
}
#endif

#ifndef SDL_RENDER_DISABLED
static void InitializeRendererView(SDL_Renderer *renderer, SDL_Surface *surface)
{
    if (surface) {
        renderer->main_view.pixel_w = surface->w;
        renderer->main_view.pixel_h = surface->h;
    }
    renderer->main_view.viewport.w = -1.0f;
    renderer->main_view.viewport.h = -1.0f;
    renderer->main_view.scale.x = 1.0f;
    renderer->main_view.scale.y = 1.0f;
    renderer->main_view.logical_scale.x = 1.0f;
    renderer->main_view.logical_scale.y = 1.0f;
    renderer->main_view.current_scale.x = 1.0f;
    renderer->main_view.current_scale.y = 1.0f;
    renderer->view = &renderer->main_view;
    SDL_UpdatePixelViewport(renderer, &renderer->main_view);
    SDL_UpdatePixelClipRect(renderer, &renderer->main_view);
    SDL_UpdateMainViewDimensions(renderer);
}
#endif

SDL_Renderer *SDL_CreateRendererWithProperties(SDL_PropertiesID props)
{
#ifndef SDL_RENDER_DISABLED
    SDL_Window *window = (SDL_Window *)SDL_GetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, NULL);
    SDL_Surface *surface = (SDL_Surface *)SDL_GetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, NULL);
    const char *driver_name;
    const char *hint;
    SDL_PropertiesID new_props;

    if (surface) {
        driver_name = SDL_SOFTWARE_RENDERER;
    } else {
        driver_name = SDL_GetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, NULL);
    }

    // The GPU and software renderers are the only ones that can be created without a window or surface
    CHECK_PARAM(!window && !surface &&
                (!driver_name || (SDL_strcmp(driver_name, SDL_GPU_RENDERER) != 0 &&
                                  SDL_strcmp(driver_name, SDL_SOFTWARE_RENDERER) != 0))) {
        SDL_InvalidParamError("window");
        return NULL;
    }

    CHECK_PARAM(window && surface) {
        SDL_SetError("A renderer can't target both a window and surface");
        return NULL;
    }

    CHECK_PARAM(window && SDL_WindowHasSurface(window)) {
        SDL_SetError("Window already has an associated surface");
        return NULL;
    }

    CHECK_PARAM(window && SDL_GetRenderer(window)) {
        SDL_SetError("Window already has an associated renderer");
        return NULL;
    }

#ifdef SDL_PLATFORM_ANDROID
    if (!Android_WaitActiveAndLockActivity()) {
        return NULL;
    }
#endif

    SDL_Renderer *renderer = (SDL_Renderer *)SDL_calloc(1, sizeof(*renderer));
    if (!renderer) {
        goto error;
    }

    SDL_SetObjectValid(renderer, SDL_OBJECT_TYPE_RENDERER, true);

    hint = SDL_GetHint(SDL_HINT_RENDER_VSYNC);
    if (hint && *hint) {
        SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, SDL_GetHintBoolean(SDL_HINT_RENDER_VSYNC, true));
    }

    if (surface) {
#ifdef SDL_VIDEO_RENDER_SW
        const bool rc = SW_CreateRendererForSurface(renderer, surface, props);
#else
        const bool rc = SDL_SetError("SDL not built with software renderer");
#endif
        if (!rc) {
            goto error;
        }
    } else if (!SDL_SelectRenderDriver(renderer, window, props, driver_name)) {
        goto error;
    }

    VerifyDrawQueueFunctions(renderer);

    renderer->window = window;
    renderer->target_mutex = SDL_CreateMutex();
    InitializeRendererView(renderer, surface);

    renderer->palettes = SDL_CreateHashTable(0, false, SDL_HashPointer, SDL_KeyMatchPointer, SDL_DestroyHashValue, NULL);
    if (!renderer->palettes) {
        goto error;
    }

    // new textures start at zero, so we start at 1 so first render doesn't flush by accident.
    renderer->render_command_generation = 1;

    if (renderer->software) {
        // Software renderer always uses line method, for speed
        renderer->line_method = SDL_RENDERLINEMETHOD_LINES;
    } else {
        renderer->line_method = SDL_GetRenderLineMethod();
    }

    renderer->scale_mode = SDL_SCALEMODE_LINEAR;

    renderer->SDR_white_point = 1.0f;
    renderer->HDR_headroom = 1.0f;
    renderer->desired_color_scale = 1.0f;
    renderer->color_scale = 1.0f;

    if (window) {
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_TRANSPARENT) {
            renderer->transparent_window = true;
        }

        if (SDL_GetWindowFlags(window) & (SDL_WINDOW_HIDDEN | SDL_WINDOW_MINIMIZED)) {
            renderer->hidden = true;
        }
    }

    new_props = SDL_GetRendererProperties(renderer);
    SDL_SetStringProperty(new_props, SDL_PROP_RENDERER_NAME_STRING, renderer->name);
    if (window) {
        SDL_SetPointerProperty(new_props, SDL_PROP_RENDERER_WINDOW_POINTER, window);
    }
    if (surface) {
        SDL_SetPointerProperty(new_props, SDL_PROP_RENDERER_SURFACE_POINTER, surface);
    }
    SDL_SetNumberProperty(new_props, SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER, renderer->output_colorspace);
    SDL_SetBooleanProperty(new_props, SDL_PROP_RENDERER_TEXTURE_WRAPPING_BOOLEAN, !renderer->npot_texture_wrap_unsupported);

    if (window) {
        SDL_UpdateHDRProperties(renderer);
        if (!SDL_AddWindowRenderer(window, renderer)) {
            goto error;
        }
        SDL_AddWindowEventWatch(SDL_WINDOW_EVENT_WATCH_NORMAL, SDL_RendererEventWatch, renderer);
    }

    int vsync = (int)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, 0);
    SDL_SetRenderVSync(renderer, vsync);
    SDL_CalculateSimulatedVSyncInterval(renderer, window);

    SDL_SetRenderViewport(renderer, NULL);

    SDL_LogInfo(SDL_LOG_CATEGORY_RENDER,
                "Created renderer: %s", renderer->name);

    renderer->next = SDL_renderers;
    SDL_renderers = renderer;

#ifdef SDL_PLATFORM_ANDROID
    Android_UnlockActivityMutex();
#endif

    SDL_ClearError();

    return renderer;

error:
#ifdef SDL_PLATFORM_ANDROID
    Android_UnlockActivityMutex();
#endif

    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    return NULL;

#else
    SDL_SetError("SDL not built with rendering support");
    return NULL;
#endif
}

SDL_Renderer *SDL_CreateRenderer(SDL_Window *window, const char *name)
{
    SDL_Renderer *renderer;
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, name);
    renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);
    return renderer;
}

SDL_Renderer *SDL_CreateGPURenderer(SDL_GPUDevice *device, SDL_Window *window)
{
    SDL_Renderer *renderer;

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_GPU_DEVICE_POINTER, device);
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, window);
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, SDL_GPU_RENDERER);

    renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);
    return renderer;
}

SDL_GPUDevice *SDL_GetGPURendererDevice(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    SDL_GPUDevice *device = SDL_GetPointerProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_GPU_DEVICE_POINTER, NULL);
    if (!device) {
        SDL_SetError("Renderer isn't a GPU renderer");
        return NULL;
    }
    return device;
}

SDL_Renderer *SDL_CreateSoftwareRenderer(SDL_Surface *surface)
{
#ifdef SDL_VIDEO_RENDER_SW
    SDL_Renderer *renderer;

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, SDL_SOFTWARE_RENDERER);
    SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_SURFACE_POINTER, surface);
    renderer = SDL_CreateRendererWithProperties(props);
    SDL_DestroyProperties(props);
    return renderer;
#else
    SDL_SetError("SDL not built with rendering support");
    return NULL;
#endif // SDL_VIDEO_RENDER_SW
}

bool SDL_SetRenderTarget(SDL_Renderer *renderer, SDL_Texture *texture)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    // texture == NULL is valid and means reset the target to the window
    if (texture) {
        CHECK_TEXTURE_MAGIC(texture, false);

        CHECK_PARAM(renderer != texture->renderer) {
            return SDL_SetError("Texture was not created with this renderer");
        }
        CHECK_PARAM(texture->access != SDL_TEXTUREACCESS_TARGET) {
            return SDL_SetError("Texture not created with SDL_TEXTUREACCESS_TARGET");
        }

        if (texture->native) {
            // Always render to the native texture
            texture = texture->native;
        }
    }

    if (texture == renderer->target) {
        // Nothing to do!
        return true;
    }

    FlushRenderCommands(renderer); // time to send everything to the GPU!

    SDL_LockMutex(renderer->target_mutex);

    renderer->target = texture;
    if (texture) {
        renderer->view = &texture->view;
        renderer->current_colorspace = texture->colorspace;
    } else {
        renderer->view = &renderer->main_view;
        renderer->current_colorspace = renderer->output_colorspace;
    }
    SDL_UpdateColorScale(renderer);

    if (!renderer->SetRenderTarget(renderer, texture)) {
        SDL_UnlockMutex(renderer->target_mutex);
        return false;
    }

    SDL_UnlockMutex(renderer->target_mutex);

    if (!SDL_QueueCmdSetViewport(renderer)) {
        return false;
    }
    if (!SDL_QueueCmdSetClipRect(renderer)) {
        return false;
    }

    // All set!
    return true;
}

SDL_Texture *SDL_GetRenderTarget(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    if (!renderer->target) {
        return NULL;
    }
    return (SDL_Texture *) SDL_GetPointerProperty(SDL_GetTextureProperties(renderer->target), SDL_PROP_TEXTURE_PARENT_POINTER, renderer->target);
}


bool SDL_RenderClear(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    return QueueCmdClear(renderer);
}

SDL_Surface *SDL_RenderReadPixels(SDL_Renderer *renderer, const SDL_Rect *rect)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    if (!renderer->RenderReadPixels) {
        SDL_Unsupported();
        return NULL;
    }

    FlushRenderCommands(renderer); // we need to render before we read the results.

    SDL_Rect real_rect = renderer->view->pixel_viewport;

    if (rect) {
        if (!SDL_GetRectIntersection(rect, &real_rect, &real_rect)) {
            SDL_SetError("Can't read outside the current viewport");
            return NULL;
        }
    }

    SDL_Surface *surface = renderer->RenderReadPixels(renderer, &real_rect);
    if (surface) {
        SDL_PropertiesID props = SDL_GetSurfaceProperties(surface);

        if (renderer->target) {
            SDL_Texture *target = renderer->target;
            SDL_Texture *parent = SDL_GetPointerProperty(SDL_GetTextureProperties(target), SDL_PROP_TEXTURE_PARENT_POINTER, NULL);
            SDL_PixelFormat expected_format = (parent ? parent->format : target->format);

            if (SDL_COLORSPACETRANSFER(target->colorspace) == SDL_TRANSFER_CHARACTERISTICS_PQ) {
                SDL_SetFloatProperty(props, SDL_PROP_SURFACE_SDR_WHITE_POINT_FLOAT, target->SDR_white_point * SCRGB_NITS);
            } else {
                SDL_SetFloatProperty(props, SDL_PROP_SURFACE_SDR_WHITE_POINT_FLOAT, target->SDR_white_point);
            }
            SDL_SetFloatProperty(props, SDL_PROP_SURFACE_HDR_HEADROOM_FLOAT, target->HDR_headroom);

            // Set the expected surface format
            if ((surface->format == SDL_PIXELFORMAT_ARGB8888 && expected_format == SDL_PIXELFORMAT_XRGB8888) ||
                (surface->format == SDL_PIXELFORMAT_RGBA8888 && expected_format == SDL_PIXELFORMAT_RGBX8888) ||
                (surface->format == SDL_PIXELFORMAT_ABGR8888 && expected_format == SDL_PIXELFORMAT_XBGR8888) ||
                (surface->format == SDL_PIXELFORMAT_BGRA8888 && expected_format == SDL_PIXELFORMAT_BGRX8888)) {
                surface->format = expected_format;
                surface->fmt = SDL_GetPixelFormatDetails(expected_format);
            }
        } else {
            if (SDL_COLORSPACETRANSFER(renderer->output_colorspace) == SDL_TRANSFER_CHARACTERISTICS_PQ) {
                SDL_SetFloatProperty(props, SDL_PROP_SURFACE_SDR_WHITE_POINT_FLOAT, renderer->SDR_white_point * SCRGB_NITS);
            } else {
                SDL_SetFloatProperty(props, SDL_PROP_SURFACE_SDR_WHITE_POINT_FLOAT, renderer->SDR_white_point);
            }
            SDL_SetFloatProperty(props, SDL_PROP_SURFACE_HDR_HEADROOM_FLOAT, renderer->HDR_headroom);
        }
    }
    return surface;
}

static void SDL_RenderApplyWindowShape(SDL_Renderer *renderer)
{
    SDL_Surface *shape = (SDL_Surface *)SDL_GetPointerProperty(SDL_GetWindowProperties(renderer->window), SDL_PROP_WINDOW_SHAPE_POINTER, NULL);
    if (shape != renderer->shape_surface) {
        if (renderer->shape_texture) {
            SDL_DestroyTexture(renderer->shape_texture);
            renderer->shape_texture = NULL;
        }

        if (shape) {
            // There's nothing we can do if this fails, so just keep on going
            renderer->shape_texture = SDL_CreateTextureFromSurface(renderer, shape);

            SDL_SetTextureBlendMode(renderer->shape_texture,
                SDL_ComposeCustomBlendMode(
                    SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD,
                    SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD));
        }
        renderer->shape_surface = shape;
    }

    if (renderer->shape_texture) {
        SDL_RenderTexture(renderer, renderer->shape_texture, NULL, NULL);
    }
}

static void SDL_SimulateRenderVSync(SDL_Renderer *renderer)
{
    Uint64 now, elapsed;
    const Uint64 interval = renderer->simulate_vsync_interval_ns;

    if (!interval) {
        // We can't do sub-ns delay, so just return here
        return;
    }

    now = SDL_GetTicksNS();
    elapsed = (now - renderer->last_present);
    if (elapsed < interval) {
        Uint64 duration = (interval - elapsed);
        SDL_DelayPrecise(duration);
        now = SDL_GetTicksNS();
    }

    elapsed = (now - renderer->last_present);
    if (!renderer->last_present || elapsed > SDL_MS_TO_NS(1000)) {
        // It's been too long, reset the presentation timeline
        renderer->last_present = now;
    } else {
        renderer->last_present += (elapsed / interval) * interval;
    }
}

bool SDL_RenderPresent(SDL_Renderer *renderer)
{
    bool presented = true;

    CHECK_RENDERER_MAGIC(renderer, false);

    if (renderer->target) {
        if (!renderer->window && SDL_strcmp(renderer->name, SDL_GPU_RENDERER) == 0) {
            // We're an offscreen renderer, we must submit the command queue
        } else {
            return SDL_SetError("You can't present on a render target");
        }
    }

    if (renderer->window && renderer->window->native_window_destroyed) {
        return FlushRenderCommands(renderer) ? SDL_SetError("Renderer's window has been destroyed, can't present") : false;
    }

    if (renderer->transparent_window) {
        SDL_RenderApplyWindowShape(renderer);
    }

    if (!FlushRenderCommands(renderer)) {
        return false;
    }

#if DONT_DRAW_WHILE_HIDDEN
    // Don't present while we're hidden
    if (renderer->hidden) {
        presented = false;
    } else
#endif
    if (!renderer->RenderPresent(renderer)) {
        presented = false;
        if (renderer->window && renderer->window->native_window_destroyed) {
            return false;
        }
    }

    if (renderer->simulate_vsync ||
        (!presented && renderer->wanted_vsync)) {
        SDL_SimulateRenderVSync(renderer);
    }
    return true;
}

static void SDL_DestroyTextureInternal(SDL_Texture *texture, bool is_destroying)
{
    SDL_Renderer *renderer;

    if (texture->public_palette) {
        SDL_SetTexturePalette(texture, NULL);
    }

    SDL_DestroyProperties(texture->props);

    renderer = texture->renderer;
    if (is_destroying) {
        // Renderer get destroyed, avoid to queue more commands
    } else {
        if (texture == renderer->target) {
            SDL_SetRenderTarget(renderer, NULL); // implies command queue flush
        } else {
            SDL_FlushRenderCommandsIfTextureNeeded(texture);
        }
    }

    SDL_SetObjectValid(texture, SDL_OBJECT_TYPE_TEXTURE, false);

    if (texture->next) {
        texture->next->prev = texture->prev;
    }
    if (texture->prev) {
        texture->prev->next = texture->next;
    } else {
        renderer->textures = texture->next;
    }

    if (texture->native) {
        SDL_DestroyTextureInternal(texture->native, is_destroying);
    }
#ifdef SDL_HAVE_YUV
    if (texture->yuv) {
        SDL_SW_DestroyYUVTexture(texture->yuv);
    }
#endif
    SDL_free(texture->pixels);

    renderer->DestroyTexture(renderer, texture);

    if (texture->palette_surface) {
        SDL_DestroySurface(texture->palette_surface);
        texture->palette_surface = NULL;
    }
    if (texture->locked_surface) {
        SDL_DestroySurface(texture->locked_surface);
        texture->locked_surface = NULL;
    }

    SDL_free(texture);
}

void SDL_DestroyTexture(SDL_Texture *texture)
{
    CHECK_TEXTURE_MAGIC(texture, );

    if (--texture->refcount > 0) {
        return;
    }

    SDL_DestroyTextureInternal(texture, false /* is_destroying */);
}

static void SDL_DiscardAllCommands(SDL_Renderer *renderer)
{
    SDL_RenderCommand *cmd;

    if (renderer->render_commands_tail) {
        renderer->render_commands_tail->next = renderer->render_commands_pool;
        cmd = renderer->render_commands;
    } else {
        cmd = renderer->render_commands_pool;
    }

    renderer->render_commands_pool = NULL;
    renderer->render_commands_tail = NULL;
    renderer->render_commands = NULL;
    renderer->vertex_data_used = 0;

    while (cmd) {
        SDL_RenderCommand *next = cmd->next;
        SDL_free(cmd);
        cmd = next;
    }
}

void SDL_DestroyRendererWithoutFreeing(SDL_Renderer *renderer)
{
    SDL_assert(renderer != NULL);
    SDL_assert(!renderer->destroyed);

    renderer->destroyed = true;

    if (renderer->window) {
        SDL_RemoveWindowEventWatch(SDL_WINDOW_EVENT_WATCH_NORMAL, SDL_RendererEventWatch, renderer);
        SDL_RemoveWindowRenderer(renderer->window, renderer);
    }

    if (renderer->software) {
        // Make sure all drawing to a surface is complete
        FlushRenderCommands(renderer);
    }
    SDL_DiscardAllCommands(renderer);

    if (renderer->debug_char_texture_atlas) {
        SDL_DestroyTexture(renderer->debug_char_texture_atlas);
        renderer->debug_char_texture_atlas = NULL;
    }

    // Free existing textures for this renderer
    while (renderer->textures) {
        SDL_Texture *tex = renderer->textures;
        SDL_DestroyTextureInternal(renderer->textures, true /* is_destroying */);
        SDL_assert(tex != renderer->textures); // satisfy static analysis.
    }

    // Free palette cache, which should be empty now
    if (renderer->palettes) {
        SDL_assert(SDL_HashTableEmpty(renderer->palettes));
        SDL_DestroyHashTable(renderer->palettes);
        renderer->palettes = NULL;
    }

    // Clean up renderer-specific resources
    if (renderer->DestroyRenderer) {
        renderer->DestroyRenderer(renderer);
    }

    if (renderer->target_mutex) {
        SDL_DestroyMutex(renderer->target_mutex);
        renderer->target_mutex = NULL;
    }
    if (renderer->vertex_data) {
        SDL_free(renderer->vertex_data);
        renderer->vertex_data = NULL;
    }
    if (renderer->texture_formats) {
        SDL_free(renderer->texture_formats);
        renderer->texture_formats = NULL;
    }
    if (renderer->props) {
        SDL_DestroyProperties(renderer->props);
        renderer->props = 0;
    }
}

void SDL_DestroyRenderer(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC_BUT_NOT_DESTROYED_FLAG(renderer,);

    // if we've already destroyed the renderer through SDL_DestroyWindow, we just need
    // to free the renderer pointer. This lets apps destroy the window and renderer
    // in either order.
    if (!renderer->destroyed) {
        SDL_DestroyRendererWithoutFreeing(renderer);
    }

    SDL_Renderer *curr = SDL_renderers;
    SDL_Renderer *prev = NULL;
    while (curr) {
        if (curr == renderer) {
            if (prev) {
                prev->next = renderer->next;
            } else {
                SDL_renderers = renderer->next;
            }
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    SDL_SetObjectValid(renderer, SDL_OBJECT_TYPE_RENDERER, false);  // It's no longer magical...

    SDL_free(renderer);
}

void *SDL_GetRenderMetalLayer(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    if (renderer->GetMetalLayer) {
        FlushRenderCommands(renderer); // in case the app is going to mess with it.
        return renderer->GetMetalLayer(renderer);
    }
    return NULL;
}

void *SDL_GetRenderMetalCommandEncoder(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer, NULL);

    if (renderer->GetMetalCommandEncoder) {
        FlushRenderCommands(renderer); // in case the app is going to mess with it.
        return renderer->GetMetalCommandEncoder(renderer);
    }
    return NULL;
}

bool SDL_AddVulkanRenderSemaphores(SDL_Renderer *renderer, Uint32 wait_stage_mask, Sint64 wait_semaphore, Sint64 signal_semaphore)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    if (!renderer->AddVulkanRenderSemaphores) {
        return SDL_Unsupported();
    }
    return renderer->AddVulkanRenderSemaphores(renderer, wait_stage_mask, wait_semaphore, signal_semaphore);
}

bool SDL_SetRenderVSync(SDL_Renderer *renderer, int vsync)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    renderer->wanted_vsync = vsync ? true : false;

    // for the software renderer, forward the call to the WindowTexture renderer
#ifdef SDL_VIDEO_RENDER_SW
    if (renderer->software) {
        if (!renderer->window) {
            if (!vsync) {
                return true;
            } else {
                return SDL_Unsupported();
            }
        }
        if (SDL_SetWindowTextureVSync(NULL, renderer->window, vsync)) {
            renderer->simulate_vsync = false;
            return true;
        }
    }
#endif

    if (!renderer->SetVSync ||
        !renderer->SetVSync(renderer, vsync)) {
        switch (vsync) {
        case 0:
            renderer->simulate_vsync = false;
            break;
        case 1:
            renderer->simulate_vsync = true;
            break;
        default:
            return SDL_Unsupported();
        }
    }
    SDL_SetNumberProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_VSYNC_NUMBER, vsync);
    return true;
}

bool SDL_GetRenderVSync(SDL_Renderer *renderer, int *vsync)
{
    if (vsync) {
        *vsync = 0;
    }

    CHECK_RENDERER_MAGIC(renderer, false);

    if (vsync) {
        *vsync = (int)SDL_GetNumberProperty(SDL_GetRendererProperties(renderer), SDL_PROP_RENDERER_VSYNC_NUMBER, 0);
    }
    return true;
}


#ifdef SDL_PLATFORM_GDK

void SDLCALL SDL_GDKSuspendRenderer(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer,);
    if (renderer->GDKSuspendRenderer != NULL) {
        renderer->GDKSuspendRenderer(renderer);
    }
}

void SDLCALL SDL_GDKResumeRenderer(SDL_Renderer *renderer)
{
    CHECK_RENDERER_MAGIC(renderer,);
    if (renderer->GDKResumeRenderer != NULL) {
        renderer->GDKResumeRenderer(renderer);
    }
}

#endif /* SDL_PLATFORM_GDK */
