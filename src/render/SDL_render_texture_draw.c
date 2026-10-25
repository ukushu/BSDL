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

#include "SDL_sysrender.h"
#include "SDL_render_internal.h"
#include "../video/SDL_video_c.h"

static bool QueueCmdCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect)
{
    SDL_RenderCommand *cmd = SDL_PrepQueueCmdDraw(renderer, SDL_RENDERCMD_COPY, texture);
    bool result = false;
    if (cmd) {
        result = renderer->QueueCopy(renderer, cmd, texture, srcrect, dstrect);
        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    return result;
}

static bool QueueCmdCopyEx(SDL_Renderer *renderer, SDL_Texture *texture,
                          const SDL_FRect *srcquad, const SDL_FRect *dstrect,
                          const double angle, const SDL_FPoint *center, const SDL_FlipMode flip, float scale_x, float scale_y)
{
    SDL_RenderCommand *cmd = SDL_PrepQueueCmdDraw(renderer, SDL_RENDERCMD_COPY_EX, texture);
    bool result = false;
    if (cmd) {
        result = renderer->QueueCopyEx(renderer, cmd, texture, srcquad, dstrect, angle, center, flip, scale_x, scale_y);
        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    return result;
}

static bool SDL_RenderTextureInternal(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect)
{
    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    const bool use_rendergeometry = (!renderer->QueueCopy);
    bool result;

    if (use_rendergeometry) {
        float xy[8];
        const int xy_stride = 2 * sizeof(float);
        float uv[8];
        const int uv_stride = 2 * sizeof(float);
        const int num_vertices = 4;
        const int *indices = SDL_rect_index_order;
        const int num_indices = 6;
        const int size_indices = 4;
        float minu, minv, maxu, maxv;
        float minx, miny, maxx, maxy;

        minu = srcrect->x / texture->w;
        minv = srcrect->y / texture->h;
        maxu = (srcrect->x + srcrect->w) / texture->w;
        maxv = (srcrect->y + srcrect->h) / texture->h;

        minx = dstrect->x;
        miny = dstrect->y;
        maxx = dstrect->x + dstrect->w;
        maxy = dstrect->y + dstrect->h;

        uv[0] = minu;
        uv[1] = minv;
        uv[2] = maxu;
        uv[3] = minv;
        uv[4] = maxu;
        uv[5] = maxv;
        uv[6] = minu;
        uv[7] = maxv;

        xy[0] = minx;
        xy[1] = miny;
        xy[2] = maxx;
        xy[3] = miny;
        xy[4] = maxx;
        xy[5] = maxy;
        xy[6] = minx;
        xy[7] = maxy;

        result = SDL_QueueCmdGeometry(renderer, texture,
                                  xy, xy_stride, &texture->color, 0 /* color_stride */, uv, uv_stride,
                                  num_vertices, indices, num_indices, size_indices,
                                  scale_x, scale_y, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);
    } else {
        const SDL_FRect rect = { dstrect->x * scale_x, dstrect->y * scale_y, dstrect->w * scale_x, dstrect->h * scale_y };
        result = QueueCmdCopy(renderer, texture, srcrect, &rect);
    }
    return result;
}

bool SDL_RenderTexture(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect)
{
    CHECK_RENDERER_MAGIC(renderer, false);
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(renderer != texture->renderer) {
        return SDL_SetError("Texture was not created with this renderer");
    }

    SDL_FRect real_srcrect;
    real_srcrect.x = 0.0f;
    real_srcrect.y = 0.0f;
    real_srcrect.w = (float)texture->w;
    real_srcrect.h = (float)texture->h;
    if (srcrect) {
        if (!SDL_GetRectIntersectionFloat(srcrect, &real_srcrect, &real_srcrect)) {
            return true;
        }
    }

    SDL_FRect full_dstrect;
    if (!dstrect) {
        SDL_GetRenderViewportSize(renderer, &full_dstrect);
        dstrect = &full_dstrect;
    }

    if (!SDL_UpdateTexturePalette(texture)) {
        return false;
    }

    if (texture->native) {
        texture = texture->native;
    }

    texture->last_command_generation = renderer->render_command_generation;

    return SDL_RenderTextureInternal(renderer, texture, &real_srcrect, dstrect);
}

bool SDL_RenderTextureAffine(SDL_Renderer *renderer, SDL_Texture *texture,
    const SDL_FRect *srcrect, const SDL_FPoint *origin, const SDL_FPoint *right, const SDL_FPoint *down)
{
    SDL_FRect real_srcrect;
    SDL_FRect real_dstrect;
    bool result;

    CHECK_RENDERER_MAGIC(renderer, false);
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(renderer != texture->renderer) {
        return SDL_SetError("Texture was not created with this renderer");
    }
    if (!renderer->QueueCopyEx && !renderer->QueueGeometry) {
        return SDL_SetError("Renderer does not support RenderCopyEx");
    }

    real_srcrect.x = 0.0f;
    real_srcrect.y = 0.0f;
    real_srcrect.w = (float)texture->w;
    real_srcrect.h = (float)texture->h;
    if (srcrect) {
        if (!SDL_GetRectIntersectionFloat(srcrect, &real_srcrect, &real_srcrect)) {
            return true;
        }
    }

    SDL_GetRenderViewportSize(renderer, &real_dstrect);

    if (!SDL_UpdateTexturePalette(texture)) {
        return false;
    }

    if (texture->native) {
        texture = texture->native;
    }

    texture->last_command_generation = renderer->render_command_generation;

    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;

    {
        float xy[8];
        const int xy_stride = 2 * sizeof(float);
        float uv[8];
        const int uv_stride = 2 * sizeof(float);
        const int num_vertices = 4;
        const int *indices = SDL_rect_index_order;
        const int num_indices = 6;
        const int size_indices = 4;

        float minu = real_srcrect.x / texture->w;
        float minv = real_srcrect.y / texture->h;
        float maxu = (real_srcrect.x + real_srcrect.w) / texture->w;
        float maxv = (real_srcrect.y + real_srcrect.h) / texture->h;

        uv[0] = minu;
        uv[1] = minv;
        uv[2] = maxu;
        uv[3] = minv;
        uv[4] = maxu;
        uv[5] = maxv;
        uv[6] = minu;
        uv[7] = maxv;

        // (minx, miny)
        if (origin) {
            xy[0] = origin->x;
            xy[1] = origin->y;
        } else {
            xy[0] = real_dstrect.x;
            xy[1] = real_dstrect.y;
        }

        // (maxx, miny)
        if (right) {
            xy[2] = right->x;
            xy[3] = right->y;
        } else {
            xy[2] = real_dstrect.x + real_dstrect.w;
            xy[3] = real_dstrect.y;
        }

        // (minx, maxy)
        if (down) {
            xy[6] = down->x;
            xy[7] = down->y;
        } else {
            xy[6] = real_dstrect.x;
            xy[7] = real_dstrect.y + real_dstrect.h;
        }

        // (maxx, maxy)
        if (origin || right || down) {
            xy[4] = xy[2] + xy[6] - xy[0];
            xy[5] = xy[3] + xy[7] - xy[1];
        } else {
            xy[4] = real_dstrect.x + real_dstrect.w;
            xy[5] = real_dstrect.y + real_dstrect.h;
        }

        result = SDL_QueueCmdGeometry(
            renderer, texture,
            xy, xy_stride,
            &texture->color, 0 /* color_stride */,
            uv, uv_stride,
            num_vertices, indices, num_indices, size_indices,
            scale_x, scale_y, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP
        );
    }
    return result;
}

static bool SDL_QueueRotatedTextureGeometry(SDL_Renderer *renderer, SDL_Texture *texture,
                                            const SDL_FRect *srcrect, const SDL_FRect *dstrect,
                                            double angle, const SDL_FPoint *center, SDL_FlipMode flip,
                                            float scale_x, float scale_y)
{
    float xy[8];
    const int xy_stride = 2 * sizeof(float);
    float uv[8];
    const int uv_stride = 2 * sizeof(float);
    const int num_vertices = 4;
    const int *indices = SDL_rect_index_order;
    const int num_indices = 6;
    const int size_indices = 4;
    float minu, minv, maxu, maxv;
    float minx, miny, maxx, maxy;
    float centerx, centery;
    float s_minx, s_miny, s_maxx, s_maxy;
    float c_minx, c_miny, c_maxx, c_maxy;

    const float radian_angle = (float)((SDL_PI_D * angle) / 180.0);
    const float s = SDL_sinf(radian_angle);
    const float c = SDL_cosf(radian_angle);

    minu = srcrect->x / texture->w;
    minv = srcrect->y / texture->h;
    maxu = (srcrect->x + srcrect->w) / texture->w;
    maxv = (srcrect->y + srcrect->h) / texture->h;

    centerx = center->x + dstrect->x;
    centery = center->y + dstrect->y;

    if (flip & SDL_FLIP_HORIZONTAL) {
        minx = dstrect->x + dstrect->w;
        maxx = dstrect->x;
    } else {
        minx = dstrect->x;
        maxx = dstrect->x + dstrect->w;
    }

    if (flip & SDL_FLIP_VERTICAL) {
        miny = dstrect->y + dstrect->h;
        maxy = dstrect->y;
    } else {
        miny = dstrect->y;
        maxy = dstrect->y + dstrect->h;
    }

    uv[0] = minu;
    uv[1] = minv;
    uv[2] = maxu;
    uv[3] = minv;
    uv[4] = maxu;
    uv[5] = maxv;
    uv[6] = minu;
    uv[7] = maxv;

    /* apply rotation with 2x2 matrix ( c -s )
     *                                ( s  c ) */
    s_minx = s * (minx - centerx);
    s_miny = s * (miny - centery);
    s_maxx = s * (maxx - centerx);
    s_maxy = s * (maxy - centery);
    c_minx = c * (minx - centerx);
    c_miny = c * (miny - centery);
    c_maxx = c * (maxx - centerx);
    c_maxy = c * (maxy - centery);

    // (minx, miny)
    xy[0] = (c_minx - s_miny) + centerx;
    xy[1] = (s_minx + c_miny) + centery;
    // (maxx, miny)
    xy[2] = (c_maxx - s_miny) + centerx;
    xy[3] = (s_maxx + c_miny) + centery;
    // (maxx, maxy)
    xy[4] = (c_maxx - s_maxy) + centerx;
    xy[5] = (s_maxx + c_maxy) + centery;
    // (minx, maxy)
    xy[6] = (c_minx - s_maxy) + centerx;
    xy[7] = (s_minx + c_maxy) + centery;

    return SDL_QueueCmdGeometry(renderer, texture,
                            xy, xy_stride, &texture->color, 0 /* color_stride */, uv, uv_stride,
                            num_vertices, indices, num_indices, size_indices,
                            scale_x, scale_y, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);
}

bool SDL_RenderTextureRotated(SDL_Renderer *renderer, SDL_Texture *texture,
                      const SDL_FRect *srcrect, const SDL_FRect *dstrect,
                      const double angle, const SDL_FPoint *center, const SDL_FlipMode flip)
{
    SDL_FRect real_srcrect;
    SDL_FPoint real_center;
    bool result;

    if (flip == SDL_FLIP_NONE && (int)(angle / 360) == angle / 360) { // fast path when we don't need rotation or flipping
        return SDL_RenderTexture(renderer, texture, srcrect, dstrect);
    }

    CHECK_RENDERER_MAGIC(renderer, false);
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(renderer != texture->renderer) {
        return SDL_SetError("Texture was not created with this renderer");
    }
    if (!renderer->QueueCopyEx && !renderer->QueueGeometry) {
        return SDL_SetError("Renderer does not support RenderCopyEx");
    }

    real_srcrect.x = 0.0f;
    real_srcrect.y = 0.0f;
    real_srcrect.w = (float)texture->w;
    real_srcrect.h = (float)texture->h;
    if (srcrect) {
        if (!SDL_GetRectIntersectionFloat(srcrect, &real_srcrect, &real_srcrect)) {
            return true;
        }
    }

    // We don't intersect the dstrect with the viewport as RenderCopy does because of potential rotation clipping issues... TODO: should we?
    SDL_FRect full_dstrect;
    if (!dstrect) {
        SDL_GetRenderViewportSize(renderer, &full_dstrect);
        dstrect = &full_dstrect;
    }

    if (!SDL_UpdateTexturePalette(texture)) {
        return false;
    }

    if (texture->native) {
        texture = texture->native;
    }

    if (center) {
        real_center = *center;
    } else {
        real_center.x = dstrect->w / 2.0f;
        real_center.y = dstrect->h / 2.0f;
    }

    texture->last_command_generation = renderer->render_command_generation;

    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;

    const bool use_rendergeometry = (!renderer->QueueCopyEx);
    if (use_rendergeometry) {
        result = SDL_QueueRotatedTextureGeometry(renderer, texture, &real_srcrect, dstrect,
                                                 angle, &real_center, flip, scale_x, scale_y);
    } else {
        result = QueueCmdCopyEx(renderer, texture, &real_srcrect, dstrect, angle, &real_center, flip, scale_x, scale_y);
    }
    return result;
}

static bool SDL_RenderTextureTiled_Wrap(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float scale, const SDL_FRect *dstrect)
{
    float xy[8];
    const int xy_stride = 2 * sizeof(float);
    float uv[8];
    const int uv_stride = 2 * sizeof(float);
    const int num_vertices = 4;
    const int *indices = SDL_rect_index_order;
    const int num_indices = 6;
    const int size_indices = 4;
    float minu, minv, maxu, maxv;
    float minx, miny, maxx, maxy;

    minu = 0.0f;
    minv = 0.0f;
    maxu = dstrect->w / (srcrect->w * scale);
    maxv = dstrect->h / (srcrect->h * scale);

    minx = dstrect->x;
    miny = dstrect->y;
    maxx = dstrect->x + dstrect->w;
    maxy = dstrect->y + dstrect->h;

    uv[0] = minu;
    uv[1] = minv;
    uv[2] = maxu;
    uv[3] = minv;
    uv[4] = maxu;
    uv[5] = maxv;
    uv[6] = minu;
    uv[7] = maxv;

    xy[0] = minx;
    xy[1] = miny;
    xy[2] = maxx;
    xy[3] = miny;
    xy[4] = maxx;
    xy[5] = maxy;
    xy[6] = minx;
    xy[7] = maxy;

    const SDL_RenderViewState *view = renderer->view;
    return SDL_QueueCmdGeometry(renderer, texture,
                            xy, xy_stride, &texture->color, 0 /* color_stride */, uv, uv_stride,
                            num_vertices, indices, num_indices, size_indices,
                            view->current_scale.x, view->current_scale.y,
                            SDL_TEXTURE_ADDRESS_WRAP, SDL_TEXTURE_ADDRESS_WRAP);
}

static bool SDL_RenderTextureTiled_Iterate(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float scale, const SDL_FRect *dstrect)
{
    float tile_width = srcrect->w * scale;
    float tile_height = srcrect->h * scale;
    float float_rows, float_cols;
    float remaining_w = SDL_modff(dstrect->w / tile_width, &float_cols);
    float remaining_h = SDL_modff(dstrect->h / tile_height, &float_rows);
    float remaining_src_w = remaining_w * srcrect->w;
    float remaining_src_h = remaining_h * srcrect->h;
    float remaining_dst_w = remaining_w * tile_width;
    float remaining_dst_h = remaining_h * tile_height;
    int rows = (int)float_rows;
    int cols = (int)float_cols;
    SDL_FRect curr_src, curr_dst;

    SDL_copyp(&curr_src, srcrect);
    curr_dst.y = dstrect->y;
    curr_dst.w = tile_width;
    curr_dst.h = tile_height;
    for (int y = 0; y < rows; ++y) {
        curr_dst.x = dstrect->x;
        for (int x = 0; x < cols; ++x) {
            if (!SDL_RenderTextureInternal(renderer, texture, &curr_src, &curr_dst)) {
                return false;
            }
            curr_dst.x += curr_dst.w;
        }
        if (remaining_dst_w > 0.0f) {
            curr_src.w = remaining_src_w;
            curr_dst.w = remaining_dst_w;
            if (!SDL_RenderTextureInternal(renderer, texture, &curr_src, &curr_dst)) {
                return false;
            }
            curr_src.w = srcrect->w;
            curr_dst.w = tile_width;
        }
        curr_dst.y += curr_dst.h;
    }
    if (remaining_dst_h > 0.0f) {
        curr_src.h = remaining_src_h;
        curr_dst.h = remaining_dst_h;
        curr_dst.x = dstrect->x;
        for (int x = 0; x < cols; ++x) {
            if (!SDL_RenderTextureInternal(renderer, texture, &curr_src, &curr_dst)) {
                return false;
            }
            curr_dst.x += curr_dst.w;
        }
        if (remaining_dst_w > 0.0f) {
            curr_src.w = remaining_src_w;
            curr_dst.w = remaining_dst_w;
            if (!SDL_RenderTextureInternal(renderer, texture, &curr_src, &curr_dst)) {
                return false;
            }
        }
    }
    return true;
}

bool SDL_IsNPOT(int x)
{
    return (x <= 0) || ((x & (x - 1)) != 0);
}

bool SDL_RenderTextureTiled(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float scale, const SDL_FRect *dstrect)
{
    SDL_FRect real_srcrect;

    CHECK_RENDERER_MAGIC(renderer, false);
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(renderer != texture->renderer) {
        return SDL_SetError("Texture was not created with this renderer");
    }

    CHECK_PARAM(scale <= 0.0f) {
        return SDL_InvalidParamError("scale");
    }

    real_srcrect.x = 0.0f;
    real_srcrect.y = 0.0f;
    real_srcrect.w = (float)texture->w;
    real_srcrect.h = (float)texture->h;
    if (srcrect) {
        if (!SDL_GetRectIntersectionFloat(srcrect, &real_srcrect, &real_srcrect)) {
            return true;
        }
    }

    SDL_FRect full_dstrect;
    if (!dstrect) {
        SDL_GetRenderViewportSize(renderer, &full_dstrect);
        dstrect = &full_dstrect;
    }

    if (!SDL_UpdateTexturePalette(texture)) {
        return false;
    }

    if (texture->native) {
        texture = texture->native;
    }

    texture->last_command_generation = renderer->render_command_generation;

    bool do_wrapping = !renderer->software &&
                        (!srcrect ||
                            (real_srcrect.x == 0.0f && real_srcrect.y == 0.0f &&
                             real_srcrect.w == (float)texture->w && real_srcrect.h == (float)texture->h));
    if (do_wrapping && renderer->npot_texture_wrap_unsupported) {
        if (SDL_IsNPOT(texture->w) || SDL_IsNPOT(texture->h)) {
            do_wrapping = false;
        }
    }

    // See if we can use geometry with repeating texture coordinates
    if (do_wrapping) {
        return SDL_RenderTextureTiled_Wrap(renderer, texture, &real_srcrect, scale, dstrect);
    } else {
        return SDL_RenderTextureTiled_Iterate(renderer, texture, &real_srcrect, scale, dstrect);
    }
}

static bool SDL_RenderTexture9GridSegment(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, const SDL_FRect *dstrect, bool tiled, float tileScale)
{
    return tiled ? SDL_RenderTextureTiled(renderer, texture, srcrect, tileScale, dstrect)
                 : SDL_RenderTexture(renderer, texture, srcrect, dstrect);
}

static bool SDL_RenderTexture9GridInternal(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect, bool tiled, float tileScale)
{
    SDL_FRect full_src, full_dst;
    SDL_FRect curr_src, curr_dst;
    float dst_left_width;
    float dst_right_width;
    float dst_top_height;
    float dst_bottom_height;

    CHECK_RENDERER_MAGIC(renderer, false);
    CHECK_TEXTURE_MAGIC(texture, false);

    CHECK_PARAM(renderer != texture->renderer) {
        return SDL_SetError("Texture was not created with this renderer");
    }

    if (!srcrect) {
        full_src.x = 0;
        full_src.y = 0;
        full_src.w = (float)texture->w;
        full_src.h = (float)texture->h;
        srcrect = &full_src;
    }

    if (!dstrect) {
        SDL_GetRenderViewportSize(renderer, &full_dst);
        dstrect = &full_dst;
    }

    if (scale <= 0.0f || scale == 1.0f) {
        dst_left_width = SDL_ceilf(left_width);
        dst_right_width = SDL_ceilf(right_width);
        dst_top_height = SDL_ceilf(top_height);
        dst_bottom_height = SDL_ceilf(bottom_height);
    } else {
        dst_left_width = SDL_ceilf(left_width * scale);
        dst_right_width = SDL_ceilf(right_width * scale);
        dst_top_height = SDL_ceilf(top_height * scale);
        dst_bottom_height = SDL_ceilf(bottom_height * scale);
    }

    // Center
    curr_src.x = srcrect->x + left_width;
    curr_src.y = srcrect->y + top_height;
    curr_src.w = srcrect->w - left_width - right_width;
    curr_src.h = srcrect->h - top_height - bottom_height;
    curr_dst.x = dstrect->x + dst_left_width;
    curr_dst.y = dstrect->y + dst_top_height;
    curr_dst.w = dstrect->w - dst_left_width - dst_right_width;
    curr_dst.h = dstrect->h - dst_top_height - dst_bottom_height;
    if (!SDL_RenderTexture9GridSegment(renderer, texture, &curr_src, &curr_dst, tiled, tileScale)) {
        return false;
    }

    // Upper-left corner
    curr_src.x = srcrect->x;
    curr_src.y = srcrect->y;
    curr_src.w = left_width;
    curr_src.h = top_height;
    curr_dst.x = dstrect->x;
    curr_dst.y = dstrect->y;
    curr_dst.w = dst_left_width;
    curr_dst.h = dst_top_height;
    if (!SDL_RenderTexture(renderer, texture, &curr_src, &curr_dst)) {
        return false;
    }

    // Upper-right corner
    curr_src.x = srcrect->x + srcrect->w - right_width;
    curr_src.w = right_width;
    curr_dst.x = dstrect->x + dstrect->w - dst_right_width;
    curr_dst.w = dst_right_width;
    if (!SDL_RenderTexture(renderer, texture, &curr_src, &curr_dst)) {
        return false;
    }

    // Lower-right corner
    curr_src.y = srcrect->y + srcrect->h - bottom_height;
    curr_src.h = bottom_height;
    curr_dst.y = dstrect->y + dstrect->h - dst_bottom_height;
    curr_dst.h = dst_bottom_height;
    if (!SDL_RenderTexture(renderer, texture, &curr_src, &curr_dst)) {
        return false;
    }

    // Lower-left corner
    curr_src.x = srcrect->x;
    curr_src.w = left_width;
    curr_dst.x = dstrect->x;
    curr_dst.w = dst_left_width;
    if (!SDL_RenderTexture(renderer, texture, &curr_src, &curr_dst)) {
        return false;
    }

    // Left
    curr_src.y = srcrect->y + top_height;
    curr_src.h = srcrect->h - top_height - bottom_height;
    curr_dst.y = dstrect->y + dst_top_height;
    curr_dst.h = dstrect->h - dst_top_height - dst_bottom_height;
    if (!SDL_RenderTexture9GridSegment(renderer, texture, &curr_src, &curr_dst, tiled, tileScale)) {
        return false;
    }

    // Right
    curr_src.x = srcrect->x + srcrect->w - right_width;
    curr_src.w = right_width;
    curr_dst.x = dstrect->x + dstrect->w - dst_right_width;
    curr_dst.w = dst_right_width;
    if (!SDL_RenderTexture9GridSegment(renderer, texture, &curr_src, &curr_dst, tiled, tileScale)) {
        return false;
    }

    // Top
    curr_src.x = srcrect->x + left_width;
    curr_src.y = srcrect->y;
    curr_src.w = srcrect->w - left_width - right_width;
    curr_src.h = top_height;
    curr_dst.x = dstrect->x + dst_left_width;
    curr_dst.y = dstrect->y;
    curr_dst.w = dstrect->w - dst_left_width - dst_right_width;
    curr_dst.h = dst_top_height;
    if (!SDL_RenderTexture9GridSegment(renderer, texture, &curr_src, &curr_dst, tiled, tileScale)) {
        return false;
    }

    // Bottom
    curr_src.y = srcrect->y + srcrect->h - bottom_height;
    curr_src.h = bottom_height;
    curr_dst.y = dstrect->y + dstrect->h - dst_bottom_height;
    curr_dst.h = dst_bottom_height;
    if (!SDL_RenderTexture9GridSegment(renderer, texture, &curr_src, &curr_dst, tiled, tileScale)) {
        return false;
    }

    return true;
}

bool SDL_RenderTexture9Grid(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect)
{
    return SDL_RenderTexture9GridInternal(renderer, texture, srcrect, left_width, right_width, top_height, bottom_height, scale, dstrect, false, 0.0f);
}

bool SDL_RenderTexture9GridTiled(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_FRect *srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect *dstrect, float tileScale)
{
    return SDL_RenderTexture9GridInternal(renderer, texture, srcrect, left_width, right_width, top_height, bottom_height, scale, dstrect, true, tileScale);
}
