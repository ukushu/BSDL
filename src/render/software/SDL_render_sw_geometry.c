#include "SDL_internal.h"
#include "../SDL_sysrender.h"
#include "SDL_render_sw_geometry.h"

#ifdef SDL_VIDEO_RENDER_SW
static int remap_one_indice(
    int prev,
    int k,
    SDL_Texture *texture,
    const float *xy, int xy_stride,
    const SDL_FColor *color, int color_stride,
    const float *uv, int uv_stride)
{
    const float *xy0_, *xy1_, *uv0_, *uv1_;
    const SDL_FColor *col0_, *col1_;
    xy0_ = (const float *)((const char *)xy + prev * xy_stride);
    xy1_ = (const float *)((const char *)xy + k * xy_stride);
    if (xy0_[0] != xy1_[0]) {
        return k;
    }
    if (xy0_[1] != xy1_[1]) {
        return k;
    }
    if (texture) {
        uv0_ = (const float *)((const char *)uv + prev * uv_stride);
        uv1_ = (const float *)((const char *)uv + k * uv_stride);
        if (uv0_[0] != uv1_[0]) {
            return k;
        }
        if (uv0_[1] != uv1_[1]) {
            return k;
        }
    }
    col0_ = (const SDL_FColor *)((const char *)color + prev * color_stride);
    col1_ = (const SDL_FColor *)((const char *)color + k * color_stride);

    if (SDL_memcmp(col0_, col1_, sizeof(*col0_)) != 0) {
        return k;
    }

    return prev;
}

static int remap_indices(
    int prev[3],
    int k,
    SDL_Texture *texture,
    const float *xy, int xy_stride,
    const SDL_FColor *color, int color_stride,
    const float *uv, int uv_stride)
{
    int i;
    if (prev[0] == -1) {
        return k;
    }

    for (i = 0; i < 3; i++) {
        int new_k = remap_one_indice(prev[i], k, texture, xy, xy_stride, color, color_stride, uv, uv_stride);
        if (new_k != k) {
            return new_k;
        }
    }
    return k;
}

#define DEBUG_SW_RENDER_GEOMETRY 0
static void RenderSoftwareGeometryQuad(SDL_Renderer *renderer, SDL_Texture *texture, SDL_FRect src, const SDL_FRect *dst, const SDL_FColor *color)
{
    if (texture && src.w != 0 && src.h != 0) {
        SDL_SetTextureAlphaModFloat(texture, color->a);
        SDL_SetTextureColorModFloat(texture, color->r, color->g, color->b);
        if (src.w > 0 && src.h > 0) {
            SDL_RenderTexture(renderer, texture, &src, dst);
        } else {
            int flags = 0;
            if (src.w < 0) {
                flags |= SDL_FLIP_HORIZONTAL;
                src.w *= -1;
                src.x -= src.w;
            }
            if (src.h < 0) {
                flags |= SDL_FLIP_VERTICAL;
                src.h *= -1;
                src.y -= src.h;
            }
            SDL_RenderTextureRotated(renderer, texture, &src, dst, 0, NULL, (SDL_FlipMode)flags);
        }

#if DEBUG_SW_RENDER_GEOMETRY
        SDL_Log("Rect-COPY: RGB %f %f %f - Alpha:%f - texture=%p: src=(%d,%d, %d x %d) dst (%f, %f, %f x %f)", color->r, color->g, color->b, color->a,
                (void *)texture, src.x, src.y, src.w, src.h, dst->x, dst->y, dst->w, dst->h);
#endif
    } else if (dst->w != 0.0f && dst->h != 0.0f) { // Rect, no texture
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColorFloat(renderer, color->r, color->g, color->b, color->a);
        SDL_RenderFillRect(renderer, dst);
#if DEBUG_SW_RENDER_GEOMETRY
        SDL_Log("Rect-FILL: RGB %f %f %f - Alpha:%f - texture=%p: dst (%f, %f, %f x %f)", color->r, color->g, color->b, color->a,
                (void *)texture, dst->x, dst->y, dst->w, dst->h);
    } else {
        SDL_Log("Rect-DISMISS: RGB %f %f %f - Alpha:%f - texture=%p: src=(%d,%d, %d x %d) dst (%f, %f, %f x %f)", color->r, color->g, color->b, color->a,
                (void *)texture, src.x, src.y, src.w, src.h, dst->x, dst->y, dst->w, dst->h);
#endif
    }
}

typedef struct SDL_SWGeometryQuad
{
    int A, B, C, C2;
#if DEBUG_SW_RENDER_GEOMETRY
    int is_rectangle;
#endif
} SDL_SWGeometryQuad;

static bool FindSoftwareGeometryQuad(const int prev[3], int k0, int k1, int k2, const float *xy, int xy_stride, SDL_SWGeometryQuad *quad)
{
    int is_quad = 1;
    int A = -1, B = -1, C = -1, C2 = -1;
#if DEBUG_SW_RENDER_GEOMETRY
    int is_rectangle = 1;
#endif
    /* Two triangles forming a quadrilateral,
     * prev and current triangles must have exactly 2 common vertices */
    {
        int cnt = 0, j = 3;
        while (j--) {
            int p = prev[j];
            if (p == k0 || p == k1 || p == k2) {
                cnt++;
            }
        }
        is_quad = (cnt == 2);
    }

    // Identify vertices
    if (is_quad) {
        const float *xy0_, *xy1_, *xy2_;
        float x0, x1, x2;
        float y0, y1, y2;
        xy0_ = (const float *)((const char *)xy + k0 * xy_stride);
        xy1_ = (const float *)((const char *)xy + k1 * xy_stride);
        xy2_ = (const float *)((const char *)xy + k2 * xy_stride);
        x0 = xy0_[0];
        y0 = xy0_[1];
        x1 = xy1_[0];
        y1 = xy1_[1];
        x2 = xy2_[0];
        y2 = xy2_[1];

        // Find top-left
        if (x0 <= x1 && y0 <= y1) {
            if (x0 <= x2 && y0 <= y2) {
                A = k0;
            } else {
                A = k2;
            }
        } else {
            if (x1 <= x2 && y1 <= y2) {
                A = k1;
            } else {
                A = k2;
            }
        }

        // Find bottom-right
        if (x0 >= x1 && y0 >= y1) {
            if (x0 >= x2 && y0 >= y2) {
                B = k0;
            } else {
                B = k2;
            }
        } else {
            if (x1 >= x2 && y1 >= y2) {
                B = k1;
            } else {
                B = k2;
            }
        }

        // Find C
        if (k0 != A && k0 != B) {
            C = k0;
        } else if (k1 != A && k1 != B) {
            C = k1;
        } else {
            C = k2;
        }

        // Find C2
        if (prev[0] != A && prev[0] != B) {
            C2 = prev[0];
        } else if (prev[1] != A && prev[1] != B) {
            C2 = prev[1];
        } else {
            C2 = prev[2];
        }

        xy0_ = (const float *)((const char *)xy + A * xy_stride);
        xy1_ = (const float *)((const char *)xy + B * xy_stride);
        xy2_ = (const float *)((const char *)xy + C * xy_stride);
        x0 = xy0_[0];
        y0 = xy0_[1];
        x1 = xy1_[0];
        y1 = xy1_[1];
        x2 = xy2_[0];
        y2 = xy2_[1];

        // Check if triangle A B C is rectangle
        if ((x0 == x2 && y1 == y2) || (y0 == y2 && x1 == x2)) {
            // ok
        } else {
            is_quad = 0;
#if DEBUG_SW_RENDER_GEOMETRY
            is_rectangle = 0;
#endif
        }

        xy2_ = (const float *)((const char *)xy + C2 * xy_stride);
        x2 = xy2_[0];
        y2 = xy2_[1];

        // Check if triangle A B C2 is rectangle
        if ((x0 == x2 && y1 == y2) || (y0 == y2 && x1 == x2)) {
            // ok
        } else {
            is_quad = 0;
#if DEBUG_SW_RENDER_GEOMETRY
            is_rectangle = 0;
#endif
        }
    }

    quad->A = A;
    quad->B = B;
    quad->C = C;
    quad->C2 = C2;
#if DEBUG_SW_RENDER_GEOMETRY
    quad->is_rectangle = is_rectangle;
#endif
    return is_quad;
}

static bool IsSoftwareGeometryQuadUniform(const SDL_SWGeometryQuad *quad, const SDL_FColor *color, int color_stride)
{
    const SDL_FColor *col0_ = (const SDL_FColor *)((const char *)color + quad->A * color_stride);
    const SDL_FColor *col1_ = (const SDL_FColor *)((const char *)color + quad->B * color_stride);
    const SDL_FColor *col2_ = (const SDL_FColor *)((const char *)color + quad->C * color_stride);
    const SDL_FColor *col3_ = (const SDL_FColor *)((const char *)color + quad->C2 * color_stride);
    return SDL_memcmp(col0_, col1_, sizeof(*col0_)) == 0 &&
           SDL_memcmp(col0_, col2_, sizeof(*col0_)) == 0 &&
           SDL_memcmp(col0_, col3_, sizeof(*col0_)) == 0;
}

// For the software renderer, try to reinterpret triangles as SDL_Rect
bool SDLCALL SDL_SW_RenderGeometryRaw(SDL_Renderer *renderer,
                                            SDL_Texture *texture,
                                            const float *xy, int xy_stride,
                                            const SDL_FColor *color, int color_stride,
                                            const float *uv, int uv_stride,
                                            int num_vertices,
                                            const void *indices, int num_indices, int size_indices,
                                            SDL_SW_QueueGeometryFn queue_geometry)
{
    int i;
    bool result = true;
    int count = indices ? num_indices : num_vertices;
    int prev[3]; // Previous triangle vertex indices
    float texw = 0.0f, texh = 0.0f;
    SDL_BlendMode blendMode = SDL_BLENDMODE_NONE;
    float r = 0, g = 0, b = 0, a = 0;
    const SDL_RenderViewState *view = renderer->view;
    const float scale_x = view->current_scale.x;
    const float scale_y = view->current_scale.y;

    // Save
    SDL_GetRenderDrawBlendMode(renderer, &blendMode);
    SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);

    if (texture) {
        SDL_GetTextureSize(texture, &texw, &texh);
    }

    prev[0] = -1;
    prev[1] = -1;
    prev[2] = -1;
    size_indices = indices ? size_indices : 0;

    for (i = 0; i < count; i += 3) {
        int k0, k1, k2; // Current triangle indices
        bool is_quad;
        SDL_SWGeometryQuad quad;
#if DEBUG_SW_RENDER_GEOMETRY
        int is_uniform = 1;
#endif

        if (size_indices == 4) {
            k0 = ((const Uint32 *)indices)[i];
            k1 = ((const Uint32 *)indices)[i + 1];
            k2 = ((const Uint32 *)indices)[i + 2];
        } else if (size_indices == 2) {
            k0 = ((const Uint16 *)indices)[i];
            k1 = ((const Uint16 *)indices)[i + 1];
            k2 = ((const Uint16 *)indices)[i + 2];
        } else if (size_indices == 1) {
            k0 = ((const Uint8 *)indices)[i];
            k1 = ((const Uint8 *)indices)[i + 1];
            k2 = ((const Uint8 *)indices)[i + 2];
        } else {
            /* Vertices were not provided by indices. Maybe some are duplicated.
             * We try to indentificate the duplicates by comparing with the previous three vertices */
            k0 = remap_indices(prev, i, texture, xy, xy_stride, color, color_stride, uv, uv_stride);
            k1 = remap_indices(prev, i + 1, texture, xy, xy_stride, color, color_stride, uv, uv_stride);
            k2 = remap_indices(prev, i + 2, texture, xy, xy_stride, color, color_stride, uv, uv_stride);
        }

        if (prev[0] == -1) {
            prev[0] = k0;
            prev[1] = k1;
            prev[2] = k2;
            continue;
        }

        is_quad = FindSoftwareGeometryQuad(prev, k0, k1, k2, xy, xy_stride, &quad);

        // Check if uniformly colored
        if (is_quad && !IsSoftwareGeometryQuadUniform(&quad, color, color_stride)) {
            is_quad = 0;
#if DEBUG_SW_RENDER_GEOMETRY
            is_uniform = 0;
#endif
        }

        // Check if UVs within range
        if (is_quad && uv) {
            const float *uv0_ = (const float *)((const char *)uv + quad.A * color_stride);
            const float *uv1_ = (const float *)((const char *)uv + quad.B * color_stride);
            const float *uv2_ = (const float *)((const char *)uv + quad.C * color_stride);
            const float *uv3_ = (const float *)((const char *)uv + quad.C2 * color_stride);
            if (uv0_[0] >= 0.0f && uv0_[0] <= 1.0f &&
                uv1_[0] >= 0.0f && uv1_[0] <= 1.0f &&
                uv2_[0] >= 0.0f && uv2_[0] <= 1.0f &&
                uv3_[0] >= 0.0f && uv3_[0] <= 1.0f) {
                // ok
            } else {
                is_quad = 0;
            }
        }

        // Start rendering rect
        if (is_quad) {
            SDL_FRect s;
            SDL_FRect d;
            const float *xy0_, *xy1_, *uv0_, *uv1_;
            const SDL_FColor *col0_ = (const SDL_FColor *)((const char *)color + k0 * color_stride);

            xy0_ = (const float *)((const char *)xy + quad.A * xy_stride);
            xy1_ = (const float *)((const char *)xy + quad.B * xy_stride);

            if (texture) {
                uv0_ = (const float *)((const char *)uv + quad.A * uv_stride);
                uv1_ = (const float *)((const char *)uv + quad.B * uv_stride);
                s.x = uv0_[0] * texw;
                s.y = uv0_[1] * texh;
                s.w = uv1_[0] * texw - s.x;
                s.h = uv1_[1] * texh - s.y;
            } else {
                s.x = s.y = s.w = s.h = 0;
            }

            d.x = xy0_[0];
            d.y = xy0_[1];
            d.w = xy1_[0] - d.x;
            d.h = xy1_[1] - d.y;

            RenderSoftwareGeometryQuad(renderer, texture, s, &d, col0_);

            prev[0] = -1;
        } else {
            // Render triangles
            if (prev[0] != -1) {
#if DEBUG_SW_RENDER_GEOMETRY
                SDL_Log("Triangle %d %d %d - is_uniform:%d is_rectangle:%d", prev[0], prev[1], prev[2], is_uniform, quad.is_rectangle);
#endif
                result = queue_geometry(renderer, texture,
                                          xy, xy_stride, color, color_stride, uv, uv_stride,
                                          num_vertices, prev, 3, 4,
                                          scale_x, scale_y, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);
                if (!result) {
                    goto end;
                }
            }

            prev[0] = k0;
            prev[1] = k1;
            prev[2] = k2;
        }
    } // End for (), next triangle

    if (prev[0] != -1) {
        // flush the last triangle
#if DEBUG_SW_RENDER_GEOMETRY
        SDL_Log("Last triangle %d %d %d", prev[0], prev[1], prev[2]);
#endif
        result = queue_geometry(renderer, texture,
                                  xy, xy_stride, color, color_stride, uv, uv_stride,
                                  num_vertices, prev, 3, 4,
                                  scale_x, scale_y, SDL_TEXTURE_ADDRESS_CLAMP, SDL_TEXTURE_ADDRESS_CLAMP);
        if (!result) {
            goto end;
        }
    }

end:
    // Restore
    SDL_SetRenderDrawBlendMode(renderer, blendMode);
    SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);

    return result;
}
#endif // SDL_VIDEO_RENDER_SW
