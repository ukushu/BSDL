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

bool SDL_QueueCmdDrawPoints(SDL_Renderer *renderer, const SDL_FPoint *points, const int count)
{
    SDL_RenderCommand *cmd = SDL_PrepQueueCmdDraw(renderer, SDL_RENDERCMD_DRAW_POINTS, NULL);
    bool result = false;
    if (cmd) {
        result = renderer->QueueDrawPoints(renderer, cmd, points, count);
        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    return result;
}

bool SDL_QueueCmdDrawLines(SDL_Renderer *renderer, const SDL_FPoint *points, const int count)
{
    SDL_RenderCommand *cmd = SDL_PrepQueueCmdDraw(renderer, SDL_RENDERCMD_DRAW_LINES, NULL);
    bool result = false;
    if (cmd) {
        result = renderer->QueueDrawLines(renderer, cmd, points, count);
        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    return result;
}

static bool QueueCmdFillRectsWithGeometry(SDL_Renderer *renderer, SDL_RenderCommand *cmd, const SDL_FRect *rects, int count)
{
    bool result = false;
    bool isstack1;
    bool isstack2;
    float *xy = SDL_small_alloc(float, 4 * 2 * count, &isstack1);
    int *indices = SDL_small_alloc(int, 6 * count, &isstack2);

    if (xy && indices) {
        int i;
        float *ptr_xy = xy;
        int *ptr_indices = indices;
        const int xy_stride = 2 * sizeof(float);
        const int num_vertices = 4 * count;
        const int num_indices = 6 * count;
        const int size_indices = 4;
        int cur_index = 0;

        for (i = 0; i < count; ++i) {
            float minx, miny, maxx, maxy;

            minx = rects[i].x;
            miny = rects[i].y;
            maxx = rects[i].x + rects[i].w;
            maxy = rects[i].y + rects[i].h;

            *ptr_xy++ = minx;
            *ptr_xy++ = miny;
            *ptr_xy++ = maxx;
            *ptr_xy++ = miny;
            *ptr_xy++ = maxx;
            *ptr_xy++ = maxy;
            *ptr_xy++ = minx;
            *ptr_xy++ = maxy;

            *ptr_indices++ = cur_index + SDL_rect_index_order[0];
            *ptr_indices++ = cur_index + SDL_rect_index_order[1];
            *ptr_indices++ = cur_index + SDL_rect_index_order[2];
            *ptr_indices++ = cur_index + SDL_rect_index_order[3];
            *ptr_indices++ = cur_index + SDL_rect_index_order[4];
            *ptr_indices++ = cur_index + SDL_rect_index_order[5];
            cur_index += 4;
        }

        result = renderer->QueueGeometry(renderer, cmd, NULL,
                                         xy, xy_stride, &renderer->color, 0 /* color_stride */, NULL, 0,
                                         num_vertices, indices, num_indices, size_indices,
                                         1.0f, 1.0f);

        if (!result) {
            cmd->command = SDL_RENDERCMD_NO_OP;
        }
    }
    SDL_small_free(xy, isstack1);
    SDL_small_free(indices, isstack2);

    return result;
}

bool SDL_QueueCmdFillRects(SDL_Renderer *renderer, const SDL_FRect *rects, const int count)
{
    SDL_RenderCommand *cmd;
    bool result = false;
    const int use_rendergeometry = (!renderer->QueueFillRects);

    cmd = SDL_PrepQueueCmdDraw(renderer, (use_rendergeometry ? SDL_RENDERCMD_GEOMETRY : SDL_RENDERCMD_FILL_RECTS), NULL);

    if (cmd) {
        if (use_rendergeometry) {
            result = QueueCmdFillRectsWithGeometry(renderer, cmd, rects, count);
        } else {
            result = renderer->QueueFillRects(renderer, cmd, rects, count);
            if (!result) {
                cmd->command = SDL_RENDERCMD_NO_OP;
            }
        }
    }
    return result;
}


bool SDL_RenderPoint(SDL_Renderer *renderer, float x, float y)
{
    SDL_FPoint fpoint;
    fpoint.x = x;
    fpoint.y = y;
    return SDL_RenderPoints(renderer, &fpoint, 1);
}

static bool RenderPointsWithRects(SDL_Renderer *renderer, const SDL_FPoint *fpoints, const int count)
{
    bool result;
    bool isstack;
    SDL_FRect *frects;
    int i;

    if (count < 1) {
        return true;
    }

    frects = SDL_small_alloc(SDL_FRect, count, &isstack);
    if (!frects) {
        return false;
    }

    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    for (i = 0; i < count; ++i) {
        frects[i].x = fpoints[i].x * scale_x;
        frects[i].y = fpoints[i].y * scale_y;
        frects[i].w = scale_x;
        frects[i].h = scale_y;
    }

    result = SDL_QueueCmdFillRects(renderer, frects, count);

    SDL_small_free(frects, isstack);

    return result;
}

bool SDL_RenderPoints(SDL_Renderer *renderer, const SDL_FPoint *points, int count)
{
    bool result;

    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(!points) {
        return SDL_InvalidParamError("SDL_RenderPoints(): points");
    }

    if (count < 1) {
        return true;
    }

    const SDL_RenderViewState *view = renderer->view;
    if ((view->current_scale.x != 1.0f) || (view->current_scale.y != 1.0f)) {
        result = RenderPointsWithRects(renderer, points, count);
    } else {
        result = SDL_QueueCmdDrawPoints(renderer, points, count);
    }
    return result;
}

bool SDL_RenderLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2)
{
    SDL_FPoint points[2];
    points[0].x = x1;
    points[0].y = y1;
    points[1].x = x2;
    points[1].y = y2;
    return SDL_RenderLines(renderer, points, 2);
}

static bool RenderLineBresenham(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, bool draw_last)
{
    const SDL_RenderViewState *view = renderer->view;
    const int MAX_PIXELS = SDL_max(view->pixel_w, view->pixel_h) * 4;
    int i, deltax, deltay, numpixels;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;
    bool result;
    bool isstack;
    SDL_FPoint *points;
    SDL_Rect viewport;

    /* the backend might clip this further to the clipping rect, but we
       just want a basic safety against generating millions of points for
       massive lines. */
    viewport = view->pixel_viewport;
    viewport.x = 0;
    viewport.y = 0;
    if (!SDL_GetRectAndLineIntersection(&viewport, &x1, &y1, &x2, &y2)) {
        return true;
    }

    deltax = SDL_abs(x2 - x1);
    deltay = SDL_abs(y2 - y1);

    if (deltax >= deltay) {
        numpixels = deltax + 1;
        d = (2 * deltay) - deltax;
        dinc1 = deltay * 2;
        dinc2 = (deltay - deltax) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    } else {
        numpixels = deltay + 1;
        d = (2 * deltax) - deltay;
        dinc1 = deltax * 2;
        dinc2 = (deltax - deltay) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x1 > x2) {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }
    if (y1 > y2) {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    if (!draw_last) {
        --numpixels;
    }

    if (numpixels > MAX_PIXELS) {
        return SDL_SetError("Line too long (tried to draw %d pixels, max %d)", numpixels, MAX_PIXELS);
    }

    points = SDL_small_alloc(SDL_FPoint, numpixels, &isstack);
    if (!points) {
        return false;
    }
    for (i = 0; i < numpixels; ++i) {
        points[i].x = (float)x;
        points[i].y = (float)y;

        if (d < 0) {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        } else {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }

    if ((view->current_scale.x != 1.0f) || (view->current_scale.y != 1.0f)) {
        result = RenderPointsWithRects(renderer, points, numpixels);
    } else {
        result = SDL_QueueCmdDrawPoints(renderer, points, numpixels);
    }

    SDL_small_free(points, isstack);

    return result;
}

static bool RenderLinesWithRectsF(SDL_Renderer *renderer, const SDL_FPoint *points, const int count)
{
    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    SDL_FRect *frect;
    SDL_FRect *frects;
    int i, nrects = 0;
    bool result = true;
    bool isstack;
    bool drew_line = false;
    bool draw_last = false;

    frects = SDL_small_alloc(SDL_FRect, count - 1, &isstack);
    if (!frects) {
        return false;
    }

    for (i = 0; i < count - 1; ++i) {
        bool same_x = (points[i].x == points[i + 1].x);
        bool same_y = (points[i].y == points[i + 1].y);

        if (i == (count - 2)) {
            if (!drew_line || points[i + 1].x != points[0].x || points[i + 1].y != points[0].y) {
                draw_last = true;
            }
        } else {
            if (same_x && same_y) {
                continue;
            }
        }
        if (same_x) {
            const float minY = SDL_min(points[i].y, points[i + 1].y);
            const float maxY = SDL_max(points[i].y, points[i + 1].y);

            frect = &frects[nrects++];
            frect->x = points[i].x * scale_x;
            frect->y = minY * scale_y;
            frect->w = scale_x;
            frect->h = (maxY - minY + draw_last) * scale_y;
            if (!draw_last && points[i + 1].y < points[i].y) {
                frect->y += scale_y;
            }
        } else if (same_y) {
            const float minX = SDL_min(points[i].x, points[i + 1].x);
            const float maxX = SDL_max(points[i].x, points[i + 1].x);

            frect = &frects[nrects++];
            frect->x = minX * scale_x;
            frect->y = points[i].y * scale_y;
            frect->w = (maxX - minX + draw_last) * scale_x;
            frect->h = scale_y;
            if (!draw_last && points[i + 1].x < points[i].x) {
                frect->x += scale_x;
            }
        } else {
            result &= RenderLineBresenham(renderer, (int)SDL_roundf(points[i].x), (int)SDL_roundf(points[i].y),
                                              (int)SDL_roundf(points[i + 1].x), (int)SDL_roundf(points[i + 1].y), draw_last);
        }
        drew_line = true;
    }

    if (nrects) {
        result &= SDL_QueueCmdFillRects(renderer, frects, nrects);
    }

    SDL_small_free(frects, isstack);

    return result;
}

static bool RenderLinesWithGeometry(SDL_Renderer *renderer, const SDL_FPoint *points, int count)
{
    bool result = true;
    SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    bool isstack1;
    bool isstack2;
    float *xy = SDL_small_alloc(float, 4 * 2 * count, &isstack1);
    int *indices = SDL_small_alloc(int, (4) * 3 * (count - 1) + (2) * 3 * (count), &isstack2);

    if (xy && indices) {
        int i;
        float *ptr_xy = xy;
        int *ptr_indices = indices;
        const int xy_stride = 2 * sizeof(float);
        int num_vertices = 4 * count;
        int num_indices = 0;
        const int size_indices = 4;
        int cur_index = -4;
        const bool is_looping = (points[0].x == points[count - 1].x && points[0].y == points[count - 1].y);
        SDL_FPoint p; // previous point
        p.x = p.y = 0.0f;
        /*       p            q

                0----1------ 4----5
                | \  |``\    | \  |
                |  \ |   ` `\|  \ |
                3----2-------7----6
        */
        for (i = 0; i < count; ++i) {
            SDL_FPoint q = points[i]; // current point

            q.x *= scale_x;
            q.y *= scale_y;

            *ptr_xy++ = q.x;
            *ptr_xy++ = q.y;
            *ptr_xy++ = q.x + scale_x;
            *ptr_xy++ = q.y;
            *ptr_xy++ = q.x + scale_x;
            *ptr_xy++ = q.y + scale_y;
            *ptr_xy++ = q.x;
            *ptr_xy++ = q.y + scale_y;

#define ADD_TRIANGLE(i1, i2, i3)        \
    *ptr_indices++ = cur_index + (i1);  \
    *ptr_indices++ = cur_index + (i2);  \
    *ptr_indices++ = cur_index + (i3);  \
    num_indices += 3;

            // closed polyline, don´t draw twice the point
            if (i || !is_looping) {
                ADD_TRIANGLE(4, 5, 6)
                ADD_TRIANGLE(4, 6, 7)
            }

            // first point only, no segment
            if (i == 0) {
                p = q;
                cur_index += 4;
                continue;
            }

            // draw segment
            if (p.y == q.y) {
                if (p.x < q.x) {
                    ADD_TRIANGLE(1, 4, 7)
                    ADD_TRIANGLE(1, 7, 2)
                } else {
                    ADD_TRIANGLE(5, 0, 3)
                    ADD_TRIANGLE(5, 3, 6)
                }
            } else if (p.x == q.x) {
                if (p.y < q.y) {
                    ADD_TRIANGLE(2, 5, 4)
                    ADD_TRIANGLE(2, 4, 3)
                } else {
                    ADD_TRIANGLE(6, 1, 0)
                    ADD_TRIANGLE(6, 0, 7)
                }
            } else {
                if (p.y < q.y) {
                    if (p.x < q.x) {
                        ADD_TRIANGLE(1, 5, 4)
                        ADD_TRIANGLE(1, 4, 2)
                        ADD_TRIANGLE(2, 4, 7)
                        ADD_TRIANGLE(2, 7, 3)
                    } else {
                        ADD_TRIANGLE(4, 0, 5)
                        ADD_TRIANGLE(5, 0, 3)
                        ADD_TRIANGLE(5, 3, 6)
                        ADD_TRIANGLE(6, 3, 2)
                    }
                } else {
                    if (p.x < q.x) {
                        ADD_TRIANGLE(0, 4, 7)
                        ADD_TRIANGLE(0, 7, 1)
                        ADD_TRIANGLE(1, 7, 6)
                        ADD_TRIANGLE(1, 6, 2)
                    } else {
                        ADD_TRIANGLE(6, 5, 1)
                        ADD_TRIANGLE(6, 1, 0)
                        ADD_TRIANGLE(7, 6, 0)
                        ADD_TRIANGLE(7, 0, 3)
                    }
                }
            }

#undef ADD_TRIANGLE

            p = q;
            cur_index += 4;
        }

        result = SDL_QueueCmdGeometry(renderer, NULL,
                                  xy, xy_stride, &renderer->color, 0 /* color_stride */, NULL, 0,
                                  num_vertices, indices, num_indices, size_indices,
                                  1.0f, 1.0f, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);
    } else {
        result = false;
    }

    SDL_small_free(xy, isstack1);
    SDL_small_free(indices, isstack2);

    return result;
}

bool SDL_RenderLines(SDL_Renderer *renderer, const SDL_FPoint *points, int count)
{
    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(!points) {
        return SDL_InvalidParamError("SDL_RenderLines(): points");
    }

    if (count < 2) {
        return true;
    }

    SDL_RenderViewState *view = renderer->view;
    const bool islogical = (view->logical_presentation_mode != SDL_LOGICAL_PRESENTATION_DISABLED);

    if (islogical || (renderer->line_method == SDL_RENDERLINEMETHOD_GEOMETRY)) {
        return RenderLinesWithGeometry(renderer, points, count);
    }
    if (renderer->line_method == SDL_RENDERLINEMETHOD_POINTS || view->scale.x != 1.0f || view->scale.y != 1.0f) {
        return RenderLinesWithRectsF(renderer, points, count);
    }
    return SDL_QueueCmdDrawLines(renderer, points, count);
}

bool SDL_RenderRect(SDL_Renderer *renderer, const SDL_FRect *rect)
{
    SDL_FRect frect;
    SDL_FPoint points[5];

    CHECK_RENDERER_MAGIC(renderer, false);

    // If 'rect' == NULL, then outline the whole surface
    if (!rect) {
        SDL_GetRenderViewportSize(renderer, &frect);
        rect = &frect;
    }

    points[0].x = rect->x;
    points[0].y = rect->y;
    points[1].x = rect->x + rect->w - 1;
    points[1].y = rect->y;
    points[2].x = rect->x + rect->w - 1;
    points[2].y = rect->y + rect->h - 1;
    points[3].x = rect->x;
    points[3].y = rect->y + rect->h - 1;
    points[4].x = rect->x;
    points[4].y = rect->y;
    return SDL_RenderLines(renderer, points, 5);
}

bool SDL_RenderRects(SDL_Renderer *renderer, const SDL_FRect *rects, int count)
{
    int i;

    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(!rects) {
        return SDL_InvalidParamError("SDL_RenderRects(): rects");
    }

    if (count < 1) {
        return true;
    }

    for (i = 0; i < count; ++i) {
        if (!SDL_RenderRect(renderer, &rects[i])) {
            return false;
        }
    }
    return true;
}

bool SDL_RenderFillRect(SDL_Renderer *renderer, const SDL_FRect *rect)
{
    SDL_FRect frect;

    CHECK_RENDERER_MAGIC(renderer, false);

    // If 'rect' == NULL, then fill the whole surface
    if (!rect) {
        SDL_GetRenderViewportSize(renderer, &frect);
        rect = &frect;
    }
    return SDL_RenderFillRects(renderer, rect, 1);
}

bool SDL_RenderFillRects(SDL_Renderer *renderer, const SDL_FRect *rects, int count)
{
    SDL_FRect *frects;
    int i;
    bool result;
    bool isstack;

    CHECK_RENDERER_MAGIC(renderer, false);

    CHECK_PARAM(!rects) {
        return SDL_InvalidParamError("SDL_RenderFillRects(): rects");
    }

    if (count < 1) {
        return true;
    }

    frects = SDL_small_alloc(SDL_FRect, count, &isstack);
    if (!frects) {
        return false;
    }

    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;
    for (i = 0; i < count; ++i) {
        frects[i].x = rects[i].x * scale_x;
        frects[i].y = rects[i].y * scale_y;
        frects[i].w = rects[i].w * scale_x;
        frects[i].h = rects[i].h * scale_y;
    }

    result = SDL_QueueCmdFillRects(renderer, frects, count);

    SDL_small_free(frects, isstack);

    return result;
}
