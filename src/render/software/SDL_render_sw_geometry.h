/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#ifndef SDL_render_sw_geometry_h_
#define SDL_render_sw_geometry_h_

#include "../SDL_sysrender.h"

typedef bool (*SDL_SW_QueueGeometryFn)(SDL_Renderer *renderer, SDL_Texture *texture,
                                       const float *xy, int xy_stride,
                                       const SDL_FColor *color, int color_stride,
                                       const float *uv, int uv_stride,
                                       int num_vertices, const void *indices,
                                       int num_indices, int size_indices,
                                       float scale_x, float scale_y,
                                       SDL_TextureAddressMode texture_address_mode_u,
                                       SDL_TextureAddressMode texture_address_mode_v);

#ifdef SDL_VIDEO_RENDER_SW
bool SDLCALL SDL_SW_RenderGeometryRaw(SDL_Renderer *renderer, SDL_Texture *texture,
                                      const float *xy, int xy_stride,
                                      const SDL_FColor *color, int color_stride,
                                      const float *uv, int uv_stride,
                                      int num_vertices, const void *indices,
                                      int num_indices, int size_indices,
                                      SDL_SW_QueueGeometryFn queue_geometry);
#endif

#endif /* SDL_render_sw_geometry_h_ */
