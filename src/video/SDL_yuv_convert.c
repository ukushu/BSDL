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

#include "SDL_pixels_c.h"
#include "SDL_yuv_c.h"

#ifdef SDL_HAVE_YUV

static bool SDL_ConvertPixels_YUV_to_YUV_Copy(int width, int height, SDL_PixelFormat format, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int i;

    if (SDL_IsYUVPlanar1x1Format(format)) {
        // YUV planes
        const size_t length = width * SDL_BYTESPERPIXEL(format);
        for (i = height * 3; i--;) {
            SDL_memcpy(dst, src, length);
            src = (const Uint8 *)src + src_pitch;
            dst = (Uint8 *)dst + dst_pitch;
        }
        return true;
    }

    if (SDL_IsYUVPlanar2x2Format(format)) {
        const int bpp = SDL_BYTESPERPIXEL(format);

        // Y plane
        const size_t length = width * bpp;
        for (i = height; i--;) {
            SDL_memcpy(dst, src, length);
            src = (const Uint8 *)src + src_pitch;
            dst = (Uint8 *)dst + dst_pitch;
        }

        if (format == SDL_PIXELFORMAT_YV12 ||
            format == SDL_PIXELFORMAT_IYUV ||
            format == SDL_PIXELFORMAT_I0FL) {
            // U and V planes are a quarter the size of the Y plane, rounded up
            width = ((width + 1) / 2) * bpp;
            height = (height + 1) / 2;
            src_pitch = ((src_pitch / bpp + 1) / 2) * bpp;
            dst_pitch = ((dst_pitch / bpp + 1) / 2) * bpp;
            for (i = height * 2; i--;) {
                SDL_memcpy(dst, src, width);
                src = (const Uint8 *)src + src_pitch;
                dst = (Uint8 *)dst + dst_pitch;
            }
        } else if (format == SDL_PIXELFORMAT_NV12 ||
                   format == SDL_PIXELFORMAT_NV21 ||
                   format == SDL_PIXELFORMAT_P010) {
            // U/V plane is half the height of the Y plane, rounded up, with packed CrCb
            width = ((width + 1) / 2) * 2 * bpp;
            height = (height + 1) / 2;
            src_pitch = ((src_pitch / bpp + 1) / 2) * 2 * bpp;
            dst_pitch = ((dst_pitch / bpp + 1) / 2) * 2 * bpp;
            for (i = height; i--;) {
                SDL_memcpy(dst, src, width);
                src = (const Uint8 *)src + src_pitch;
                dst = (Uint8 *)dst + dst_pitch;
            }
        }
        return true;
    }

    if (SDL_IsYUVPacked4Format(format)) {
        // Packed planes
        width = 4 * ((width + 1) / 2);
        for (i = height; i--;) {
            SDL_memcpy(dst, src, width);
            src = (const Uint8 *)src + src_pitch;
            dst = (Uint8 *)dst + dst_pitch;
        }
        return true;
    }

    return SDL_SetError("SDL_ConvertPixels_YUV_to_YUV_Copy: Unsupported YUV format: %s", SDL_GetPixelFormatName(format));
}

static bool SDL_ConvertPixels_SwapUVPlanes(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    if (src == dst) {
        int UVpitch = (dst_pitch + 1) / 2;
        Uint8 *tmp;
        Uint8 *row1 = (Uint8 *)dst;
        Uint8 *row2 = row1 + UVheight * UVpitch;

        // Allocate a temporary row for the swap
        tmp = (Uint8 *)SDL_malloc(UVwidth);
        if (!tmp) {
            return false;
        }
        for (y = 0; y < UVheight; ++y) {
            SDL_memcpy(tmp, row1, UVwidth);
            SDL_memcpy(row1, row2, UVwidth);
            SDL_memcpy(row2, tmp, UVwidth);
            row1 += UVpitch;
            row2 += UVpitch;
        }
        SDL_free(tmp);
    } else {
        const Uint8 *srcUV;
        Uint8 *dstUV;
        int srcUVPitch = ((src_pitch + 1) / 2);
        int dstUVPitch = ((dst_pitch + 1) / 2);

        // Copy the first plane
        srcUV = (const Uint8 *)src;
        dstUV = (Uint8 *)dst + UVheight * dstUVPitch;
        for (y = 0; y < UVheight; ++y) {
            SDL_memcpy(dstUV, srcUV, UVwidth);
            srcUV += srcUVPitch;
            dstUV += dstUVPitch;
        }

        // Copy the second plane
        dstUV = (Uint8 *)dst;
        for (y = 0; y < UVheight; ++y) {
            SDL_memcpy(dstUV, srcUV, UVwidth);
            srcUV += srcUVPitch;
            dstUV += dstUVPitch;
        }
    }
    return true;
}

#ifdef SDL_SSE2_INTRINSICS
static bool SDL_TARGETING("sse2") SDL_ConvertPixels_PackUVPlanes_to_NV_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2);
    const int srcUVPitchLeft = srcUVPitch - UVwidth;
    const int dstUVPitch = ((dst_pitch + 1) / 2) * 2;
    const int dstUVPitchLeft = dstUVPitch - UVwidth * 2;
    const Uint8 *src1, *src2;
    Uint8 *dstUV;
    Uint8 *tmp = NULL;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    if (src == dst) {
        // Need to make a copy of the buffer so we don't clobber it while converting
        tmp = (Uint8 *)SDL_malloc((size_t)2 * UVheight * srcUVPitch);
        if (tmp == NULL) {
            return false;
        }
        SDL_memcpy(tmp, src, (size_t)2 * UVheight * srcUVPitch);
        src = tmp;
    }

    if (reverseUV) {
        src2 = (const Uint8 *)src;
        src1 = src2 + UVheight * srcUVPitch;
    } else {
        src1 = (const Uint8 *)src;
        src2 = src1 + UVheight * srcUVPitch;
    }
    dstUV = (Uint8 *)dst;

    y = UVheight;
    while (y--) {
        x = UVwidth;
        while (x >= 16) {
            __m128i u = _mm_loadu_si128((__m128i *)src1);
            __m128i v = _mm_loadu_si128((__m128i *)src2);
            __m128i uv1 = _mm_unpacklo_epi8(u, v);
            __m128i uv2 = _mm_unpackhi_epi8(u, v);
            _mm_storeu_si128((__m128i *)dstUV, uv1);
            _mm_storeu_si128((__m128i *)(dstUV + 16), uv2);
            src1 += 16;
            src2 += 16;
            dstUV += 32;
            x -= 16;
        }
        while (x--) {
            *dstUV++ = *src1++;
            *dstUV++ = *src2++;
        }
        src1 += srcUVPitchLeft;
        src2 += srcUVPitchLeft;
        dstUV += dstUVPitchLeft;
    }

    if (tmp) {
        SDL_free(tmp);
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_SplitNV_to_UVPlanes_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2) * 2;
    const int srcUVPitchLeft = srcUVPitch - UVwidth * 2;
    const int dstUVPitch = ((dst_pitch + 1) / 2);
    const int dstUVPitchLeft = dstUVPitch - UVwidth;
    const Uint8 *srcUV;
    Uint8 *dst1, *dst2;
    Uint8 *tmp = NULL;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    if (src == dst) {
        // Need to make a copy of the buffer so we don't clobber it while converting
        tmp = (Uint8 *)SDL_malloc((size_t)UVheight * srcUVPitch);
        if (tmp == NULL) {
            return false;
        }
        SDL_memcpy(tmp, src, (size_t)UVheight * srcUVPitch);
        src = tmp;
    }

    if (reverseUV) {
        dst2 = (Uint8 *)dst;
        dst1 = dst2 + UVheight * dstUVPitch;
    } else {
        dst1 = (Uint8 *)dst;
        dst2 = dst1 + UVheight * dstUVPitch;
    }
    srcUV = (const Uint8 *)src;

    y = UVheight;
    while (y--) {
        __m128i mask = _mm_set1_epi16(0x00FF);
        x = UVwidth;
        while (x >= 16) {
            __m128i uv1 = _mm_loadu_si128((__m128i *)srcUV);
            __m128i uv2 = _mm_loadu_si128((__m128i *)(srcUV + 16));
            __m128i u1 = _mm_and_si128(uv1, mask);
            __m128i u2 = _mm_and_si128(uv2, mask);
            __m128i u = _mm_packus_epi16(u1, u2);
            __m128i v1 = _mm_srli_epi16(uv1, 8);
            __m128i v2 = _mm_srli_epi16(uv2, 8);
            __m128i v = _mm_packus_epi16(v1, v2);
            _mm_storeu_si128((__m128i *)dst1, u);
            _mm_storeu_si128((__m128i *)dst2, v);
            srcUV += 32;
            dst1 += 16;
            dst2 += 16;
            x -= 16;
        }
        while (x--) {
            *dst1++ = *srcUV++;
            *dst2++ = *srcUV++;
        }
        srcUV += srcUVPitchLeft;
        dst1 += dstUVPitchLeft;
        dst2 += dstUVPitchLeft;
    }

    if (tmp) {
        SDL_free(tmp);
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_SwapNV_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2) * 2;
    const int srcUVPitchLeft = (srcUVPitch - UVwidth * 2) / sizeof(Uint16);
    const int dstUVPitch = ((dst_pitch + 1) / 2) * 2;
    const int dstUVPitchLeft = (dstUVPitch - UVwidth * 2) / sizeof(Uint16);
    const Uint16 *srcUV;
    Uint16 *dstUV;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    srcUV = (const Uint16 *)src;
    dstUV = (Uint16 *)dst;
    y = UVheight;
    while (y--) {
        x = UVwidth;
        while (x >= 8) {
            __m128i uv = _mm_loadu_si128((__m128i *)srcUV);
            __m128i v = _mm_slli_epi16(uv, 8);
            __m128i u = _mm_srli_epi16(uv, 8);
            __m128i vu = _mm_or_si128(v, u);
            _mm_storeu_si128((__m128i *)dstUV, vu);
            srcUV += 8;
            dstUV += 8;
            x -= 8;
        }
        if (x > 0) {
            const Uint8 *srcUV8 = (const Uint8 *)srcUV;
            Uint8 *dstUV8 = (Uint8 *)dstUV;
            srcUV += x;
            dstUV += x;
            while (x--) {
                Uint8 u = *srcUV8++;
                Uint8 v = *srcUV8++;
                *dstUV8++ = v;
                *dstUV8++ = u;
            }
        }
        srcUV += srcUVPitchLeft;
        dstUV += dstUVPitchLeft;
    }
    return true;
}
#endif

static bool SDL_ConvertPixels_PackUVPlanes_to_NV_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2);
    const int srcUVPitchLeft = srcUVPitch - UVwidth;
    const int dstUVPitch = ((dst_pitch + 1) / 2) * 2;
    const int dstUVPitchLeft = dstUVPitch - UVwidth * 2;
    const Uint8 *src1, *src2;
    Uint8 *dstUV;
    Uint8 *tmp = NULL;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    if (src == dst) {
        // Need to make a copy of the buffer so we don't clobber it while converting
        tmp = (Uint8 *)SDL_malloc((size_t)2 * UVheight * srcUVPitch);
        if (!tmp) {
            return false;
        }
        SDL_memcpy(tmp, src, (size_t)2 * UVheight * srcUVPitch);
        src = tmp;
    }

    if (reverseUV) {
        src2 = (const Uint8 *)src;
        src1 = src2 + UVheight * srcUVPitch;
    } else {
        src1 = (const Uint8 *)src;
        src2 = src1 + UVheight * srcUVPitch;
    }
    dstUV = (Uint8 *)dst;

    y = UVheight;
    while (y--) {
        x = UVwidth;
        while (x--) {
            *dstUV++ = *src1++;
            *dstUV++ = *src2++;
        }
        src1 += srcUVPitchLeft;
        src2 += srcUVPitchLeft;
        dstUV += dstUVPitchLeft;
    }

    SDL_free(tmp);
    return true;
}

static bool SDL_ConvertPixels_SplitNV_to_UVPlanes_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2) * 2;
    const int srcUVPitchLeft = srcUVPitch - UVwidth * 2;
    const int dstUVPitch = ((dst_pitch + 1) / 2);
    const int dstUVPitchLeft = dstUVPitch - UVwidth;
    const Uint8 *srcUV;
    Uint8 *dst1, *dst2;
    Uint8 *tmp = NULL;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    if (src == dst) {
        // Need to make a copy of the buffer so we don't clobber it while converting
        tmp = (Uint8 *)SDL_malloc((size_t)UVheight * srcUVPitch);
        if (!tmp) {
            return false;
        }
        SDL_memcpy(tmp, src, (size_t)UVheight * srcUVPitch);
        src = tmp;
    }

    if (reverseUV) {
        dst2 = (Uint8 *)dst;
        dst1 = dst2 + UVheight * dstUVPitch;
    } else {
        dst1 = (Uint8 *)dst;
        dst2 = dst1 + UVheight * dstUVPitch;
    }
    srcUV = (const Uint8 *)src;

    y = UVheight;
    while (y--) {
        x = UVwidth;
        while (x--) {
            *dst1++ = *srcUV++;
            *dst2++ = *srcUV++;
        }
        srcUV += srcUVPitchLeft;
        dst1 += dstUVPitchLeft;
        dst2 += dstUVPitchLeft;
    }

    SDL_free(tmp);
    return true;
}

static bool SDL_ConvertPixels_SwapNV_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int UVwidth = (width + 1) / 2;
    const int UVheight = (height + 1) / 2;
    const int srcUVPitch = ((src_pitch + 1) / 2) * 2;
    const int dstUVPitch = ((dst_pitch + 1) / 2) * 2;

    // Skip the Y plane
    src = (const Uint8 *)src + height * src_pitch;
    dst = (Uint8 *)dst + height * dst_pitch;

    bool aligned = (((uintptr_t)src | (uintptr_t)dst) & 1) == 0;
    if (aligned) {
        const int srcUVPitchLeft = (srcUVPitch - UVwidth * 2) / sizeof(Uint16);
        const int dstUVPitchLeft = (dstUVPitch - UVwidth * 2) / sizeof(Uint16);
        const Uint16 *srcUV = (const Uint16 *)src;
        Uint16 *dstUV = (Uint16 *)dst;
        y = UVheight;
        while (y--) {
            x = UVwidth;
            while (x--) {
                *dstUV++ = SDL_Swap16(*srcUV++);
            }
            srcUV += srcUVPitchLeft;
            dstUV += dstUVPitchLeft;
        }
    } else {
        const int srcUVPitchLeft = (srcUVPitch - UVwidth * 2);
        const int dstUVPitchLeft = (dstUVPitch - UVwidth * 2);
        const Uint8 *srcUV = (const Uint8 *)src;
        Uint8 *dstUV = (Uint8 *)dst;
        y = UVheight;
        while (y--) {
            x = UVwidth;
            while (x--) {
                Uint8 u = *srcUV++;
                Uint8 v = *srcUV++;
                *dstUV++ = v;
                *dstUV++ = u;
            }
            srcUV += srcUVPitchLeft;
            dstUV += dstUVPitchLeft;
        }
    }
    return true;
}

static bool SDL_ConvertPixels_PackUVPlanes_to_NV(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_PackUVPlanes_to_NV_SSE2(width, height, src, src_pitch, dst, dst_pitch, reverseUV);
    }
#endif
    return SDL_ConvertPixels_PackUVPlanes_to_NV_std(width, height, src, src_pitch, dst, dst_pitch, reverseUV);
}

static bool SDL_ConvertPixels_SplitNV_to_UVPlanes(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch, bool reverseUV)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_SplitNV_to_UVPlanes_SSE2(width, height, src, src_pitch, dst, dst_pitch, reverseUV);
    }
#endif
    return SDL_ConvertPixels_SplitNV_to_UVPlanes_std(width, height, src, src_pitch, dst, dst_pitch, reverseUV);
}

static bool SDL_ConvertPixels_SwapNV(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_SwapNV_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_SwapNV_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_Planar2x2_to_Planar2x2(int width, int height,
                                                     SDL_PixelFormat src_format, const void *src, int src_pitch,
                                                     SDL_PixelFormat dst_format, void *dst, int dst_pitch)
{
    if (src != dst) {
        // Copy Y plane
        int i;
        const Uint8 *srcY = (const Uint8 *)src;
        Uint8 *dstY = (Uint8 *)dst;
        for (i = height; i--;) {
            SDL_memcpy(dstY, srcY, width);
            srcY += src_pitch;
            dstY += dst_pitch;
        }
    }

    switch (src_format) {
    case SDL_PIXELFORMAT_YV12:
        switch (dst_format) {
        case SDL_PIXELFORMAT_IYUV:
            return SDL_ConvertPixels_SwapUVPlanes(width, height, src, src_pitch, dst, dst_pitch);
        case SDL_PIXELFORMAT_NV12:
            return SDL_ConvertPixels_PackUVPlanes_to_NV(width, height, src, src_pitch, dst, dst_pitch, true);
        case SDL_PIXELFORMAT_NV21:
            return SDL_ConvertPixels_PackUVPlanes_to_NV(width, height, src, src_pitch, dst, dst_pitch, false);
        default:
            break;
        }
        break;
    case SDL_PIXELFORMAT_IYUV:
        switch (dst_format) {
        case SDL_PIXELFORMAT_YV12:
            return SDL_ConvertPixels_SwapUVPlanes(width, height, src, src_pitch, dst, dst_pitch);
        case SDL_PIXELFORMAT_NV12:
            return SDL_ConvertPixels_PackUVPlanes_to_NV(width, height, src, src_pitch, dst, dst_pitch, false);
        case SDL_PIXELFORMAT_NV21:
            return SDL_ConvertPixels_PackUVPlanes_to_NV(width, height, src, src_pitch, dst, dst_pitch, true);
        default:
            break;
        }
        break;
    case SDL_PIXELFORMAT_NV12:
        switch (dst_format) {
        case SDL_PIXELFORMAT_YV12:
            return SDL_ConvertPixels_SplitNV_to_UVPlanes(width, height, src, src_pitch, dst, dst_pitch, true);
        case SDL_PIXELFORMAT_IYUV:
            return SDL_ConvertPixels_SplitNV_to_UVPlanes(width, height, src, src_pitch, dst, dst_pitch, false);
        case SDL_PIXELFORMAT_NV21:
            return SDL_ConvertPixels_SwapNV(width, height, src, src_pitch, dst, dst_pitch);
        default:
            break;
        }
        break;
    case SDL_PIXELFORMAT_NV21:
        switch (dst_format) {
        case SDL_PIXELFORMAT_YV12:
            return SDL_ConvertPixels_SplitNV_to_UVPlanes(width, height, src, src_pitch, dst, dst_pitch, false);
        case SDL_PIXELFORMAT_IYUV:
            return SDL_ConvertPixels_SplitNV_to_UVPlanes(width, height, src, src_pitch, dst, dst_pitch, true);
        case SDL_PIXELFORMAT_NV12:
            return SDL_ConvertPixels_SwapNV(width, height, src, src_pitch, dst, dst_pitch);
        default:
            break;
        }
        break;
    default:
        break;
    }
    return SDL_SetError("SDL_ConvertPixels_Planar2x2_to_Planar2x2: Unsupported YUV conversion: %s -> %s", SDL_GetPixelFormatName(src_format),
                        SDL_GetPixelFormatName(dst_format));
}

#ifdef SDL_SSE2_INTRINSICS
#define PACKED4_TO_PACKED4_ROW_SSE2(shuffle)                      \
    while (x >= 4) {                                              \
        __m128i yuv = _mm_loadu_si128((__m128i *)srcYUV);         \
        __m128i lo = _mm_unpacklo_epi8(yuv, _mm_setzero_si128()); \
        __m128i hi = _mm_unpackhi_epi8(yuv, _mm_setzero_si128()); \
        lo = _mm_shufflelo_epi16(lo, shuffle);                    \
        lo = _mm_shufflehi_epi16(lo, shuffle);                    \
        hi = _mm_shufflelo_epi16(hi, shuffle);                    \
        hi = _mm_shufflehi_epi16(hi, shuffle);                    \
        yuv = _mm_packus_epi16(lo, hi);                           \
        _mm_storeu_si128((__m128i *)dstYUV, yuv);                 \
        srcYUV += 16;                                             \
        dstYUV += 16;                                             \
        x -= 4;                                                   \
    }

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_YUY2_to_UYVY_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(2, 3, 0, 1));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            U = srcYUV[1];
            Y2 = srcYUV[2];
            V = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = U;
            dstYUV[1] = Y1;
            dstYUV[2] = V;
            dstYUV[3] = Y2;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_YUY2_to_YVYU_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(1, 2, 3, 0));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            U = srcYUV[1];
            Y2 = srcYUV[2];
            V = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = V;
            dstYUV[2] = Y2;
            dstYUV[3] = U;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_UYVY_to_YUY2_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(2, 3, 0, 1));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            U = srcYUV[0];
            Y1 = srcYUV[1];
            V = srcYUV[2];
            Y2 = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = U;
            dstYUV[2] = Y2;
            dstYUV[3] = V;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_UYVY_to_YVYU_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(0, 3, 2, 1));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            U = srcYUV[0];
            Y1 = srcYUV[1];
            V = srcYUV[2];
            Y2 = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = V;
            dstYUV[2] = Y2;
            dstYUV[3] = U;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_YVYU_to_YUY2_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(1, 2, 3, 0));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            V = srcYUV[1];
            Y2 = srcYUV[2];
            U = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = U;
            dstYUV[2] = Y2;
            dstYUV[3] = V;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}

static bool SDL_TARGETING("sse2") SDL_ConvertPixels_YVYU_to_UYVY_SSE2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    x = YUVwidth;
    while (y--) {
        PACKED4_TO_PACKED4_ROW_SSE2(_MM_SHUFFLE(2, 1, 0, 3));
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            V = srcYUV[1];
            Y2 = srcYUV[2];
            U = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = U;
            dstYUV[1] = Y1;
            dstYUV[2] = V;
            dstYUV[3] = Y2;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
        x = YUVwidth;
    }
    return true;
}
#endif

static bool SDL_ConvertPixels_YUY2_to_UYVY_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            U = srcYUV[1];
            Y2 = srcYUV[2];
            V = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = U;
            dstYUV[1] = Y1;
            dstYUV[2] = V;
            dstYUV[3] = Y2;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_YUY2_to_YVYU_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            U = srcYUV[1];
            Y2 = srcYUV[2];
            V = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = V;
            dstYUV[2] = Y2;
            dstYUV[3] = U;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_UYVY_to_YUY2_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            U = srcYUV[0];
            Y1 = srcYUV[1];
            V = srcYUV[2];
            Y2 = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = U;
            dstYUV[2] = Y2;
            dstYUV[3] = V;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_UYVY_to_YVYU_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            U = srcYUV[0];
            Y1 = srcYUV[1];
            V = srcYUV[2];
            Y2 = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = V;
            dstYUV[2] = Y2;
            dstYUV[3] = U;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_YVYU_to_YUY2_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            V = srcYUV[1];
            Y2 = srcYUV[2];
            U = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = Y1;
            dstYUV[1] = U;
            dstYUV[2] = Y2;
            dstYUV[3] = V;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_YVYU_to_UYVY_std(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
    int x, y;
    const int YUVwidth = (width + 1) / 2;
    const int srcYUVPitchLeft = (src_pitch - YUVwidth * 4);
    const int dstYUVPitchLeft = (dst_pitch - YUVwidth * 4);
    const Uint8 *srcYUV = (const Uint8 *)src;
    Uint8 *dstYUV = (Uint8 *)dst;

    y = height;
    while (y--) {
        x = YUVwidth;
        while (x--) {
            Uint8 Y1, U, Y2, V;

            Y1 = srcYUV[0];
            V = srcYUV[1];
            Y2 = srcYUV[2];
            U = srcYUV[3];
            srcYUV += 4;

            dstYUV[0] = U;
            dstYUV[1] = Y1;
            dstYUV[2] = V;
            dstYUV[3] = Y2;
            dstYUV += 4;
        }
        srcYUV += srcYUVPitchLeft;
        dstYUV += dstYUVPitchLeft;
    }
    return true;
}

static bool SDL_ConvertPixels_YUY2_to_UYVY(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_YUY2_to_UYVY_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_YUY2_to_UYVY_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_YUY2_to_YVYU(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_YUY2_to_YVYU_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_YUY2_to_YVYU_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_UYVY_to_YUY2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_UYVY_to_YUY2_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_UYVY_to_YUY2_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_UYVY_to_YVYU(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_UYVY_to_YVYU_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_UYVY_to_YVYU_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_YVYU_to_YUY2(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_YVYU_to_YUY2_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_YVYU_to_YUY2_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_YVYU_to_UYVY(int width, int height, const void *src, int src_pitch, void *dst, int dst_pitch)
{
#ifdef SDL_SSE2_INTRINSICS
    if (SDL_HasSSE2()) {
      return SDL_ConvertPixels_YVYU_to_UYVY_SSE2(width, height, src, src_pitch, dst, dst_pitch);
    }
#endif
    return SDL_ConvertPixels_YVYU_to_UYVY_std(width, height, src, src_pitch, dst, dst_pitch);
}

static bool SDL_ConvertPixels_Packed4_to_Packed4(int width, int height,
                                                SDL_PixelFormat src_format, const void *src, int src_pitch,
                                                SDL_PixelFormat dst_format, void *dst, int dst_pitch)
{
    switch (src_format) {
    case SDL_PIXELFORMAT_YUY2:
        switch (dst_format) {
        case SDL_PIXELFORMAT_UYVY:
            return SDL_ConvertPixels_YUY2_to_UYVY(width, height, src, src_pitch, dst, dst_pitch);
        case SDL_PIXELFORMAT_YVYU:
            return SDL_ConvertPixels_YUY2_to_YVYU(width, height, src, src_pitch, dst, dst_pitch);
        default:
            break;
        }
        break;
    case SDL_PIXELFORMAT_UYVY:
        switch (dst_format) {
        case SDL_PIXELFORMAT_YUY2:
            return SDL_ConvertPixels_UYVY_to_YUY2(width, height, src, src_pitch, dst, dst_pitch);
        case SDL_PIXELFORMAT_YVYU:
            return SDL_ConvertPixels_UYVY_to_YVYU(width, height, src, src_pitch, dst, dst_pitch);
        default:
            break;
        }
        break;
    case SDL_PIXELFORMAT_YVYU:
        switch (dst_format) {
        case SDL_PIXELFORMAT_YUY2:
            return SDL_ConvertPixels_YVYU_to_YUY2(width, height, src, src_pitch, dst, dst_pitch);
        case SDL_PIXELFORMAT_UYVY:
            return SDL_ConvertPixels_YVYU_to_UYVY(width, height, src, src_pitch, dst, dst_pitch);
        default:
            break;
        }
        break;
    default:
        break;
    }
    return SDL_SetError("SDL_ConvertPixels_Packed4_to_Packed4: Unsupported YUV conversion: %s -> %s", SDL_GetPixelFormatName(src_format),
                        SDL_GetPixelFormatName(dst_format));
}

static bool SDL_ConvertPixels_Planar2x2_to_Packed4(int width, int height,
                                                   SDL_PixelFormat src_format, const void *src, int src_pitch,
                                                   SDL_PixelFormat dst_format, void *dst, int dst_pitch)
{
    int x, y;
    const Uint8 *srcY1, *srcY2, *srcU, *srcV;
    Uint32 srcY_pitch, srcUV_pitch;
    Uint32 srcY_pitch_left, srcUV_pitch_left, srcUV_pixel_stride;
    Uint8 *dstY1, *dstY2, *dstU1, *dstU2, *dstV1, *dstV2;
    Uint32 dstY_pitch, dstUV_pitch;
    Uint32 dst_pitch_left;

    if (src == dst) {
        return SDL_SetError("Can't change YUV plane types in-place");
    }

    if (!SDL_GetYUVPlanes(width, height, src_format, src, src_pitch,
                      &srcY1, &srcU, &srcV, &srcY_pitch, &srcUV_pitch)) {
        return false;
    }
    srcY2 = srcY1 + srcY_pitch;
    srcY_pitch_left = (srcY_pitch - width);

    if (src_format == SDL_PIXELFORMAT_NV12 || src_format == SDL_PIXELFORMAT_NV21) {
        srcUV_pixel_stride = 2;
        srcUV_pitch_left = (srcUV_pitch - 2 * ((width + 1) / 2));
    } else {
        srcUV_pixel_stride = 1;
        srcUV_pitch_left = (srcUV_pitch - ((width + 1) / 2));
    }

    if (!SDL_GetYUVPlanes(width, height, dst_format, dst, dst_pitch,
                      (const Uint8 **)&dstY1, (const Uint8 **)&dstU1, (const Uint8 **)&dstV1,
                      &dstY_pitch, &dstUV_pitch)) {
        return false;
    }
    dstY2 = dstY1 + dstY_pitch;
    dstU2 = dstU1 + dstUV_pitch;
    dstV2 = dstV1 + dstUV_pitch;
    dst_pitch_left = (dstY_pitch - 4 * ((width + 1) / 2));

    // Copy 2x2 blocks of pixels at a time
    for (y = 0; y < (height - 1); y += 2) {
        for (x = 0; x < (width - 1); x += 2) {
            // Row 1
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstU1 = *srcU;
            *dstV1 = *srcV;

            // Row 2
            *dstY2 = *srcY2++;
            dstY2 += 2;
            *dstY2 = *srcY2++;
            dstY2 += 2;
            *dstU2 = *srcU;
            *dstV2 = *srcV;

            srcU += srcUV_pixel_stride;
            srcV += srcUV_pixel_stride;
            dstU1 += 4;
            dstU2 += 4;
            dstV1 += 4;
            dstV2 += 4;
        }

        // Last column
        if (x == (width - 1)) {
            // Row 1
            *dstY1 = *srcY1;
            dstY1 += 2;
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstU1 = *srcU;
            *dstV1 = *srcV;

            // Row 2
            *dstY2 = *srcY2;
            dstY2 += 2;
            *dstY2 = *srcY2++;
            dstY2 += 2;
            *dstU2 = *srcU;
            *dstV2 = *srcV;

            srcU += srcUV_pixel_stride;
            srcV += srcUV_pixel_stride;
            dstU1 += 4;
            dstU2 += 4;
            dstV1 += 4;
            dstV2 += 4;
        }

        srcY1 += srcY_pitch_left + srcY_pitch;
        srcY2 += srcY_pitch_left + srcY_pitch;
        srcU += srcUV_pitch_left;
        srcV += srcUV_pitch_left;
        dstY1 += dst_pitch_left + dstY_pitch;
        dstY2 += dst_pitch_left + dstY_pitch;
        dstU1 += dst_pitch_left + dstUV_pitch;
        dstU2 += dst_pitch_left + dstUV_pitch;
        dstV1 += dst_pitch_left + dstUV_pitch;
        dstV2 += dst_pitch_left + dstUV_pitch;
    }

    // Last row
    if (y == (height - 1)) {
        for (x = 0; x < (width - 1); x += 2) {
            // Row 1
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstU1 = *srcU;
            *dstV1 = *srcV;

            srcU += srcUV_pixel_stride;
            srcV += srcUV_pixel_stride;
            dstU1 += 4;
            dstV1 += 4;
        }

        // Last column
        if (x == (width - 1)) {
            // Row 1
            *dstY1 = *srcY1;
            dstY1 += 2;
            *dstY1 = *srcY1++;
            dstY1 += 2;
            *dstU1 = *srcU;
            *dstV1 = *srcV;

            srcU += srcUV_pixel_stride;
            srcV += srcUV_pixel_stride;
            dstU1 += 4;
            dstV1 += 4;
        }
    }
    return true;
}

static bool SDL_ConvertPixels_Packed4_to_Planar2x2(int width, int height,
                                                   SDL_PixelFormat src_format, const void *src, int src_pitch,
                                                   SDL_PixelFormat dst_format, void *dst, int dst_pitch)
{
    int x, y;
    const Uint8 *srcY1, *srcY2, *srcU1, *srcU2, *srcV1, *srcV2;
    Uint32 srcY_pitch, srcUV_pitch;
    Uint32 src_pitch_left;
    Uint8 *dstY1, *dstY2, *dstU, *dstV;
    Uint32 dstY_pitch, dstUV_pitch;
    Uint32 dstY_pitch_left, dstUV_pitch_left, dstUV_pixel_stride;

    if (src == dst) {
        return SDL_SetError("Can't change YUV plane types in-place");
    }

    if (!SDL_GetYUVPlanes(width, height, src_format, src, src_pitch,
                      &srcY1, &srcU1, &srcV1, &srcY_pitch, &srcUV_pitch)) {
        return false;
    }
    srcY2 = srcY1 + srcY_pitch;
    srcU2 = srcU1 + srcUV_pitch;
    srcV2 = srcV1 + srcUV_pitch;
    src_pitch_left = (srcY_pitch - 4 * ((width + 1) / 2));

    if (!SDL_GetYUVPlanes(width, height, dst_format, dst, dst_pitch,
                      (const Uint8 **)&dstY1, (const Uint8 **)&dstU, (const Uint8 **)&dstV,
                      &dstY_pitch, &dstUV_pitch)) {
        return false;
    }
    dstY2 = dstY1 + dstY_pitch;
    dstY_pitch_left = (dstY_pitch - width);

    if (dst_format == SDL_PIXELFORMAT_NV12 || dst_format == SDL_PIXELFORMAT_NV21) {
        dstUV_pixel_stride = 2;
        dstUV_pitch_left = (dstUV_pitch - 2 * ((width + 1) / 2));
    } else {
        dstUV_pixel_stride = 1;
        dstUV_pitch_left = (dstUV_pitch - ((width + 1) / 2));
    }

    // Copy 2x2 blocks of pixels at a time
    for (y = 0; y < (height - 1); y += 2) {
        for (x = 0; x < (width - 1); x += 2) {
            // Row 1
            *dstY1++ = *srcY1;
            srcY1 += 2;
            *dstY1++ = *srcY1;
            srcY1 += 2;

            // Row 2
            *dstY2++ = *srcY2;
            srcY2 += 2;
            *dstY2++ = *srcY2;
            srcY2 += 2;

            *dstU = (Uint8)(((Uint32)*srcU1 + *srcU2) / 2);
            *dstV = (Uint8)(((Uint32)*srcV1 + *srcV2) / 2);

            srcU1 += 4;
            srcU2 += 4;
            srcV1 += 4;
            srcV2 += 4;
            dstU += dstUV_pixel_stride;
            dstV += dstUV_pixel_stride;
        }

        // Last column
        if (x == (width - 1)) {
            // Row 1
            *dstY1 = *srcY1;
            srcY1 += 2;
            *dstY1++ = *srcY1;
            srcY1 += 2;

            // Row 2
            *dstY2 = *srcY2;
            srcY2 += 2;
            *dstY2++ = *srcY2;
            srcY2 += 2;

            *dstU = (Uint8)(((Uint32)*srcU1 + *srcU2) / 2);
            *dstV = (Uint8)(((Uint32)*srcV1 + *srcV2) / 2);

            srcU1 += 4;
            srcU2 += 4;
            srcV1 += 4;
            srcV2 += 4;
            dstU += dstUV_pixel_stride;
            dstV += dstUV_pixel_stride;
        }

        srcY1 += src_pitch_left + srcY_pitch;
        srcY2 += src_pitch_left + srcY_pitch;
        srcU1 += src_pitch_left + srcUV_pitch;
        srcU2 += src_pitch_left + srcUV_pitch;
        srcV1 += src_pitch_left + srcUV_pitch;
        srcV2 += src_pitch_left + srcUV_pitch;
        dstY1 += dstY_pitch_left + dstY_pitch;
        dstY2 += dstY_pitch_left + dstY_pitch;
        dstU += dstUV_pitch_left;
        dstV += dstUV_pitch_left;
    }

    // Last row
    if (y == (height - 1)) {
        for (x = 0; x < (width - 1); x += 2) {
            *dstY1++ = *srcY1;
            srcY1 += 2;
            *dstY1++ = *srcY1;
            srcY1 += 2;

            *dstU = *srcU1;
            *dstV = *srcV1;

            srcU1 += 4;
            srcV1 += 4;
            dstU += dstUV_pixel_stride;
            dstV += dstUV_pixel_stride;
        }

        // Last column
        if (x == (width - 1)) {
            *dstY1 = *srcY1;
            *dstU = *srcU1;
            *dstV = *srcV1;
        }
    }
    return true;
}

#endif // SDL_HAVE_YUV

bool SDL_ConvertPixels_YUV_to_YUV(int width, int height,
                                  SDL_PixelFormat src_format, SDL_Colorspace src_colorspace, SDL_PropertiesID src_properties, const void *src, int src_pitch,
                                  SDL_PixelFormat dst_format, SDL_Colorspace dst_colorspace, SDL_PropertiesID dst_properties, void *dst, int dst_pitch)
{
#ifdef SDL_HAVE_YUV
    if (src_colorspace != dst_colorspace) {
        return SDL_SetError("SDL_ConvertPixels_YUV_to_YUV: colorspace conversion not supported");
    }

    if (src_format == dst_format) {
        if (src == dst) {
            // Nothing to do
            return true;
        }
        return SDL_ConvertPixels_YUV_to_YUV_Copy(width, height, src_format, src, src_pitch, dst, dst_pitch);
    }

    if (SDL_IsYUVPlanar2x2Format(src_format) && SDL_IsYUVPlanar2x2Format(dst_format)) {
        return SDL_ConvertPixels_Planar2x2_to_Planar2x2(width, height, src_format, src, src_pitch, dst_format, dst, dst_pitch);
    } else if (SDL_IsYUVPacked4Format(src_format) && SDL_IsYUVPacked4Format(dst_format)) {
        return SDL_ConvertPixels_Packed4_to_Packed4(width, height, src_format, src, src_pitch, dst_format, dst, dst_pitch);
    } else if (SDL_IsYUVPlanar2x2Format(src_format) && SDL_IsYUVPacked4Format(dst_format)) {
        return SDL_ConvertPixels_Planar2x2_to_Packed4(width, height, src_format, src, src_pitch, dst_format, dst, dst_pitch);
    } else if (SDL_IsYUVPacked4Format(src_format) && SDL_IsYUVPlanar2x2Format(dst_format)) {
        return SDL_ConvertPixels_Packed4_to_Planar2x2(width, height, src_format, src, src_pitch, dst_format, dst, dst_pitch);
    } else {
        return SDL_SetError("SDL_ConvertPixels_YUV_to_YUV: Unsupported YUV conversion: %s -> %s", SDL_GetPixelFormatName(src_format),
                            SDL_GetPixelFormatName(dst_format));
    }
#else
    return SDL_SetError("SDL not built with YUV support");
#endif
}
