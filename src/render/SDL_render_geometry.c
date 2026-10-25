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
  2. Altered source versions must be plainly marked as such, and must not
     be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_internal.h"

#include "SDL_sysrender.h"
#include "SDL_render_internal.h"
#include "software/SDL_render_sw_geometry.h"

bool SDL_RenderGeometry(SDL_Renderer *renderer,
                       SDL_Texture *texture,
                       const SDL_Vertex *vertices, int num_vertices,
                       const int *indices, int num_indices)
{
    if (vertices) {
        const float *xy = &vertices->position.x;
        int xy_stride = sizeof(SDL_Vertex);
        const SDL_FColor *color = &vertices->color;
        int color_stride = sizeof(SDL_Vertex);
        const float *uv = &vertices->tex_coord.x;
        int uv_stride = sizeof(SDL_Vertex);
        int size_indices = 4;
        return SDL_RenderGeometryRaw(renderer, texture, xy, xy_stride, color, color_stride, uv, uv_stride, num_vertices, indices, num_indices, size_indices);
    } else {
        return SDL_InvalidParamError("vertices");
    }
}

static void ResolveAutoGeometryTextureAddressModes(const float *uv, int uv_stride, int num_vertices,
                                                   SDL_TextureAddressMode *u_mode, SDL_TextureAddressMode *v_mode)
{
    if (*u_mode == SDL_TEXTURE_ADDRESS_AUTO || *v_mode == SDL_TEXTURE_ADDRESS_AUTO) {
        for (int i = 0; i < num_vertices; ++i) {
            const float *uv_ = (const float *)((const char *)uv + i * uv_stride);
            float u = uv_[0];
            float v = uv_[1];
            if (u < 0.0f || u > 1.0f) {
                if (*u_mode == SDL_TEXTURE_ADDRESS_AUTO) {
                    *u_mode = SDL_TEXTURE_ADDRESS_WRAP;
                    if (*v_mode != SDL_TEXTURE_ADDRESS_AUTO) {
                        break;
                    }
                }
            }
            if (v < 0.0f || v > 1.0f) {
                if (*v_mode == SDL_TEXTURE_ADDRESS_AUTO) {
                    *v_mode = SDL_TEXTURE_ADDRESS_WRAP;
                    if (*u_mode != SDL_TEXTURE_ADDRESS_AUTO) {
                        break;
                    }
                }
            }
        }
        if (*u_mode == SDL_TEXTURE_ADDRESS_AUTO) {
            *u_mode = SDL_TEXTURE_ADDRESS_CLAMP;
        }
        if (*v_mode == SDL_TEXTURE_ADDRESS_AUTO) {
            *v_mode = SDL_TEXTURE_ADDRESS_CLAMP;
        }
    }
}

static bool ValidateGeometryIndices(const void *indices, int num_indices, int size_indices, int num_vertices)
{
    for (int i = 0; i < num_indices; ++i) {
        int j;
        if (size_indices == 4) {
            j = ((const Uint32 *)indices)[i];
        } else if (size_indices == 2) {
            j = ((const Uint16 *)indices)[i];
        } else {
            j = ((const Uint8 *)indices)[i];
        }
        if (j < 0 || j >= num_vertices) {
            return SDL_SetError("Values of 'indices' out of bounds");
        }
    }
    return true;
}

bool SDL_RenderGeometryRaw(SDL_Renderer *renderer,
                          SDL_Texture *texture,
                          const float *xy, int xy_stride,
                          const SDL_FColor *color, int color_stride,
                          const float *uv, int uv_stride,
                          int num_vertices,
                          const void *indices, int num_indices, int size_indices)
{
    int count = indices ? num_indices : num_vertices;
    SDL_TextureAddressMode texture_address_mode_u = SDL_TEXTURE_ADDRESS_CLAMP;
    SDL_TextureAddressMode texture_address_mode_v = SDL_TEXTURE_ADDRESS_CLAMP;

    CHECK_RENDERER_MAGIC(renderer, false);

    if (texture) {
        CHECK_TEXTURE_MAGIC(texture, false);

        CHECK_PARAM(renderer != texture->renderer) {
            return SDL_SetError("Texture was not created with this renderer");
        }
    }

    CHECK_PARAM(!xy) {
        return SDL_InvalidParamError("xy");
    }

    CHECK_PARAM(!color) {
        return SDL_InvalidParamError("color");
    }

    CHECK_PARAM(texture && !uv) {
        return SDL_InvalidParamError("uv");
    }

    (void)count;    // In case parameter checking is disabled
    CHECK_PARAM(count % 3 != 0) {
        return SDL_InvalidParamError(indices ? "num_indices" : "num_vertices");
    }

    if (indices) {
        CHECK_PARAM(size_indices != 1 && size_indices != 2 && size_indices != 4) {
            return SDL_InvalidParamError("size_indices");
        }
    }

    if (!indices) {
        size_indices = 0;
    }

    if (!renderer->QueueGeometry) {
        return SDL_Unsupported();
    }

    if (num_vertices < 3) {
        return true;
    }

    if (texture) {
        if (!SDL_UpdateTexturePalette(texture)) {
            return false;
        }

        if (texture->native) {
            texture = texture->native;
        }

        if (renderer->npot_texture_wrap_unsupported && SDL_IsNPOT(texture->w)) {
            texture_address_mode_u = SDL_TEXTURE_ADDRESS_CLAMP;
        } else {
            texture_address_mode_u = renderer->texture_address_mode_u;
        }
        if (renderer->npot_texture_wrap_unsupported && SDL_IsNPOT(texture->h)) {
            texture_address_mode_v = SDL_TEXTURE_ADDRESS_CLAMP;
        } else {
            texture_address_mode_v = renderer->texture_address_mode_v;
        }

        ResolveAutoGeometryTextureAddressModes(uv, uv_stride, num_vertices, &texture_address_mode_u, &texture_address_mode_v);
    }

    if (indices && !ValidateGeometryIndices(indices, num_indices, size_indices, num_vertices)) {
        return false;
    }

    if (texture) {
        texture->last_command_generation = renderer->render_command_generation;
    }

    // For the software renderer, try to reinterpret triangles as SDL_Rect
#ifdef SDL_VIDEO_RENDER_SW
    if (renderer->software &&
        texture_address_mode_u == SDL_TEXTURE_ADDRESS_CLAMP &&
        texture_address_mode_v == SDL_TEXTURE_ADDRESS_CLAMP) {
        return SDL_SW_RenderGeometryRaw(renderer, texture,
                                        xy, xy_stride, color, color_stride, uv, uv_stride, num_vertices,
                                        indices, num_indices, size_indices, SDL_QueueCmdGeometry);
    }
#endif

    const SDL_RenderViewState *view = renderer->view;
    return SDL_QueueCmdGeometry(renderer, texture,
                            xy, xy_stride, color, color_stride, uv, uv_stride,
                            num_vertices, indices, num_indices, size_indices,
                            view->current_scale.x, view->current_scale.y,
                            texture_address_mode_u, texture_address_mode_v);
}

