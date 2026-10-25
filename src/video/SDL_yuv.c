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

#include "yuv2rgb/yuv_rgb.h"


#ifdef SDL_HAVE_YUV

static bool GetYUVConversionType(SDL_Colorspace colorspace, int w, int h, int bits_per_pixel, YCbCrType *yuv_type)
{
    if (SDL_ISCOLORSPACE_MATRIX_BT601(colorspace)) {
        if (SDL_ISCOLORSPACE_LIMITED_RANGE(colorspace)) {
            *yuv_type = YCBCR_601_LIMITED;
        } else {
            *yuv_type = YCBCR_601_FULL;
        }
        return true;
    }

    if (SDL_ISCOLORSPACE_MATRIX_BT709(colorspace)) {
        if (SDL_ISCOLORSPACE_LIMITED_RANGE(colorspace)) {
            *yuv_type = YCBCR_709_LIMITED;
        } else {
            *yuv_type = YCBCR_709_FULL;
        }
        return true;
    }

    if (SDL_ISCOLORSPACE_MATRIX_BT2020_NCL(colorspace)) {
        if (SDL_ISCOLORSPACE_FULL_RANGE(colorspace)) {
            *yuv_type = YCBCR_2020_NCL_FULL;
            return true;
        }
    }

    if (SDL_COLORSPACEMATRIX(colorspace) == SDL_MATRIX_COEFFICIENTS_UNSPECIFIED) {
        switch (bits_per_pixel) {
        case 8:
            if (h <= YUV_SD_THRESHOLD) {
                if (SDL_ISCOLORSPACE_LIMITED_RANGE(colorspace)) {
                    *yuv_type = YCBCR_601_LIMITED;
                } else {
                    *yuv_type = YCBCR_601_FULL;
                }
            } else {
                if (SDL_ISCOLORSPACE_LIMITED_RANGE(colorspace)) {
                    *yuv_type = YCBCR_709_LIMITED;
                } else {
                    *yuv_type = YCBCR_709_FULL;
                }
            }
			return true;
        case 10:
        case 16:
            if (SDL_ISCOLORSPACE_FULL_RANGE(colorspace)) {
                *yuv_type = YCBCR_2020_NCL_FULL;
                return true;
            }
            break;
        default:
            break;
        }
    }

    return SDL_SetError("Unsupported YUV colorspace");
}

#ifdef SDL_SSE2_INTRINSICS
static bool SDL_TARGETING("sse2") yuv_rgb_sse(
    SDL_PixelFormat src_format, SDL_PixelFormat dst_format,
    Uint32 width, Uint32 height,
    const Uint8 *y, const Uint8 *u, const Uint8 *v, Uint32 y_stride, Uint32 uv_stride,
    Uint8 *rgb, Uint32 rgb_stride,
    YCbCrType yuv_type)
{
    if (!SDL_HasSSE2()) {
        return false;
    }

    if (src_format == SDL_PIXELFORMAT_YV12 ||
        src_format == SDL_PIXELFORMAT_IYUV) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuv420_rgb565_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuv420_rgb24_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv420_rgba_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv420_bgra_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv420_argb_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv420_abgr_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_YUY2 ||
        src_format == SDL_PIXELFORMAT_UYVY ||
        src_format == SDL_PIXELFORMAT_YVYU) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuv422_rgb565_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuv422_rgb24_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv422_rgba_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv422_bgra_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv422_argb_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv422_abgr_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_NV12 ||
        src_format == SDL_PIXELFORMAT_NV21) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuvnv12_rgb565_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuvnv12_rgb24_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuvnv12_rgba_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuvnv12_bgra_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuvnv12_argb_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuvnv12_abgr_sseu(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }
    return false;
}
#else
static bool yuv_rgb_sse(
    SDL_PixelFormat src_format, SDL_PixelFormat dst_format,
    Uint32 width, Uint32 height,
    const Uint8 *y, const Uint8 *u, const Uint8 *v, Uint32 y_stride, Uint32 uv_stride,
    Uint8 *rgb, Uint32 rgb_stride,
    YCbCrType yuv_type)
{
    return false;
}
#endif

#ifdef SDL_LSX_INTRINSICS
static bool yuv_rgb_lsx(
    SDL_PixelFormat src_format, SDL_PixelFormat dst_format,
    Uint32 width, Uint32 height,
    const Uint8 *y, const Uint8 *u, const Uint8 *v, Uint32 y_stride, Uint32 uv_stride,
    Uint8 *rgb, Uint32 rgb_stride,
    YCbCrType yuv_type)
{
    if (!SDL_HasLSX()) {
        return false;
    }
    if (src_format == SDL_PIXELFORMAT_YV12 ||
        src_format == SDL_PIXELFORMAT_IYUV) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB24:
            yuv420_rgb24_lsx(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv420_rgba_lsx(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv420_bgra_lsx(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv420_argb_lsx(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv420_abgr_lsx(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }
    return false;
}
#else
static bool yuv_rgb_lsx(
    SDL_PixelFormat src_format, SDL_PixelFormat dst_format,
    Uint32 width, Uint32 height,
    const Uint8 *y, const Uint8 *u, const Uint8 *v, Uint32 y_stride, Uint32 uv_stride,
    Uint8 *rgb, Uint32 rgb_stride,
    YCbCrType yuv_type)
{
    return false;
}
#endif

static bool yuv_rgb_std(
    SDL_PixelFormat src_format, SDL_PixelFormat dst_format,
    Uint32 width, Uint32 height,
    const Uint8 *y, const Uint8 *u, const Uint8 *v, Uint32 y_stride, Uint32 uv_stride,
    Uint8 *rgb, Uint32 rgb_stride,
    YCbCrType yuv_type)
{
    if (src_format == SDL_PIXELFORMAT_YV12 ||
        src_format == SDL_PIXELFORMAT_IYUV) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuv420_rgb565_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuv420_rgb24_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv420_rgba_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv420_bgra_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv420_argb_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv420_abgr_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_I444) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv444_rgba_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv444_bgra_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv444_argb_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv444_abgr_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_YUY2 ||
        src_format == SDL_PIXELFORMAT_UYVY ||
        src_format == SDL_PIXELFORMAT_YVYU) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuv422_rgb565_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuv422_rgb24_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuv422_rgba_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuv422_bgra_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuv422_argb_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuv422_abgr_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_NV12 ||
        src_format == SDL_PIXELFORMAT_NV21) {

        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB565:
            yuvnv12_rgb565_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGB24:
            yuvnv12_rgb24_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_RGBA8888:
            yuvnv12_rgba_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_BGRA8888:
            yuvnv12_bgra_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_ARGB8888:
            yuvnv12_argb_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        case SDL_PIXELFORMAT_XBGR8888:
        case SDL_PIXELFORMAT_ABGR8888:
            yuvnv12_abgr_std(width, height, y, u, v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_P010) {
        switch (dst_format) {
        case SDL_PIXELFORMAT_XBGR2101010:
            yuvp010_xbgr2101010_std(width, height, (const uint16_t *)y, (const uint16_t *)u, (const uint16_t *)v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_I0FL) {
        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB48:
            yuvi0fl_rgb48_std(width, height, (const uint16_t *)y, (const uint16_t *)u, (const uint16_t *)v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }

    if (src_format == SDL_PIXELFORMAT_I4FL) {
        switch (dst_format) {
        case SDL_PIXELFORMAT_RGB48:
            yuvi4fl_rgb48_std(width, height, (const uint16_t *)y, (const uint16_t *)u, (const uint16_t *)v, y_stride, uv_stride, rgb, rgb_stride, yuv_type);
            return true;
        default:
            break;
        }
    }
    return false;
}

bool SDL_ConvertPixels_YUV_to_RGB(int width, int height,
                                  SDL_PixelFormat src_format, SDL_Colorspace src_colorspace, SDL_PropertiesID src_properties, const void *src, int src_pitch,
                                  SDL_PixelFormat dst_format, SDL_Colorspace dst_colorspace, SDL_PropertiesID dst_properties, void *dst, int dst_pitch)
{
    const Uint8 *y = NULL;
    const Uint8 *u = NULL;
    const Uint8 *v = NULL;
    Uint32 y_stride = 0;
    Uint32 uv_stride = 0;
    YCbCrType yuv_type = YCBCR_601_LIMITED;

    if (!SDL_GetYUVPlanes(width, height, src_format, src, src_pitch, &y, &u, &v, &y_stride, &uv_stride)) {
        return false;
    }

    if (!GetYUVConversionType(src_colorspace, width, height, 8, &yuv_type)) {
        return false;
    }

    if (yuv_rgb_sse(src_format, dst_format, width, height, y, u, v, y_stride, uv_stride, (Uint8 *)dst, dst_pitch, yuv_type)) {
        return true;
    }

    if (yuv_rgb_lsx(src_format, dst_format, width, height, y, u, v, y_stride, uv_stride, (Uint8 *)dst, dst_pitch, yuv_type)) {
        return true;
    }

    if (yuv_rgb_std(src_format, dst_format, width, height, y, u, v, y_stride, uv_stride, (Uint8 *)dst, dst_pitch, yuv_type)) {
        return true;
    }

    // No fast path for the RGB format, instead convert using an intermediate buffer
    if (src_format == SDL_PIXELFORMAT_P010 && dst_format != SDL_PIXELFORMAT_XBGR2101010) {
        bool result;
        void *tmp;
        int tmp_pitch = (width * sizeof(Uint32));

        tmp = SDL_malloc((size_t)tmp_pitch * height);
        if (!tmp) {
            return false;
        }

        // convert src/src_format to tmp/XBGR2101010
        result = SDL_ConvertPixels_YUV_to_RGB(width, height, src_format, src_colorspace, src_properties, src, src_pitch, SDL_PIXELFORMAT_XBGR2101010, src_colorspace, src_properties, tmp, tmp_pitch);
        if (!result) {
            SDL_free(tmp);
            return false;
        }

        // convert tmp/XBGR2101010 to dst/RGB
        result = SDL_ConvertPixelsAndColorspace(width, height, SDL_PIXELFORMAT_XBGR2101010, src_colorspace, src_properties, tmp, tmp_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
        SDL_free(tmp);
        return result;
    }

    if ((src_format == SDL_PIXELFORMAT_I0FL || src_format == SDL_PIXELFORMAT_I4FL) && dst_format != SDL_PIXELFORMAT_RGB48) {
        bool result;
        void *tmp;
        int tmp_pitch = (width * 3 * sizeof(Uint16));

        tmp = SDL_malloc((size_t)tmp_pitch * height);
        if (!tmp) {
            return false;
        }

        // convert src/src_format to tmp/RGB48
        result = SDL_ConvertPixels_YUV_to_RGB(width, height, src_format, src_colorspace, src_properties, src, src_pitch, SDL_PIXELFORMAT_RGB48, src_colorspace, src_properties, tmp, tmp_pitch);
        if (!result) {
            SDL_free(tmp);
            return false;
        }

        // convert tmp/RGB48 to dst/RGB
        result = SDL_ConvertPixelsAndColorspace(width, height, SDL_PIXELFORMAT_RGB48, src_colorspace, src_properties, tmp, tmp_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
        SDL_free(tmp);
        return result;
    }

    if (dst_format != SDL_PIXELFORMAT_ARGB8888) {
        bool result;
        void *tmp;
        int tmp_pitch = (width * sizeof(Uint32));

        tmp = SDL_malloc((size_t)tmp_pitch * height);
        if (!tmp) {
            return false;
        }

        // convert src/src_format to tmp/ARGB8888
        result = SDL_ConvertPixels_YUV_to_RGB(width, height, src_format, src_colorspace, src_properties, src, src_pitch, SDL_PIXELFORMAT_ARGB8888, SDL_COLORSPACE_SRGB, 0, tmp, tmp_pitch);
        if (!result) {
            SDL_free(tmp);
            return false;
        }

        // convert tmp/ARGB8888 to dst/RGB
        result = SDL_ConvertPixelsAndColorspace(width, height, SDL_PIXELFORMAT_ARGB8888, SDL_COLORSPACE_SRGB, 0, tmp, tmp_pitch, dst_format, dst_colorspace, dst_properties, dst, dst_pitch);
        SDL_free(tmp);
        return result;
    }

    return SDL_SetError("Unsupported YUV conversion");
}

struct RGB2YUVFactors
{
    int y_offset;
    float y[3]; // Rfactor, Gfactor, Bfactor
    float u[3]; // Rfactor, Gfactor, Bfactor
    float v[3]; // Rfactor, Gfactor, Bfactor
};

static struct RGB2YUVFactors RGB2YUVFactorTables[] = {
    // ITU-T T.871 (JPEG)
    {
        0,
        { 0.2990f, 0.5870f, 0.1140f },
        { -0.1687f, -0.3313f, 0.5000f },
        { 0.5000f, -0.4187f, -0.0813f },
    },
    // ITU-R BT.601-7
    {
        16,
        { 0.2568f, 0.5041f, 0.0979f },
        { -0.1482f, -0.2910f, 0.4392f },
        { 0.4392f, -0.3678f, -0.0714f },
    },
    // ITU-R BT.709-6 full range
    {
        0,
        { 0.2126f, 0.7152f, 0.0722f },
        { -0.1141f, -0.3839f, 0.498f },
        { 0.498f, -0.4524f, -0.0457f },
    },
    // ITU-R BT.709-6
    {
        16,
        { 0.1826f, 0.6142f, 0.0620f },
        { -0.1006f, -0.3386f, 0.4392f },
        { 0.4392f, -0.3989f, -0.0403f },
    },
    // ITU-R BT.2020 10-bit full range
    {
        0,
        { 0.2627f, 0.6780f, 0.0593f },
        { -0.1395f, -0.3600f, 0.4995f },
        { 0.4995f, -0.4593f, -0.0402f },
    },
};

static bool SDL_ConvertPixels_XRGB8888_to_YUV(int width, int height, const void *src, int src_pitch, SDL_PixelFormat dst_format, void *dst, int dst_pitch, YCbCrType yuv_type)
{
    const int src_pitch_x_2 = src_pitch * 2;
    const int height_half = height / 2;
    const int height_remainder = (height & 0x1);
    const int width_half = width / 2;
    const int width_remainder = (width & 0x1);
    int i, j;

    const struct RGB2YUVFactors *cvt = &RGB2YUVFactorTables[yuv_type];

#define MAKE_Y(r, g, b) (Uint8)SDL_clamp(((int)(cvt->y[0] * (r) + cvt->y[1] * (g) + cvt->y[2] * (b) + 0.5f) + cvt->y_offset), 0, 255)
#define MAKE_U(r, g, b) (Uint8)SDL_clamp(((int)(cvt->u[0] * (r) + cvt->u[1] * (g) + cvt->u[2] * (b) + 0.5f) + 128), 0, 255)
#define MAKE_V(r, g, b) (Uint8)SDL_clamp(((int)(cvt->v[0] * (r) + cvt->v[1] * (g) + cvt->v[2] * (b) + 0.5f) + 128), 0, 255)

#define READ_2x2_PIXELS                                                                                     \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];                                                    \
    const Uint32 p2 = ((const Uint32 *)curr_row)[2 * i + 1];                                                \
    const Uint32 p3 = ((const Uint32 *)next_row)[2 * i];                                                    \
    const Uint32 p4 = ((const Uint32 *)next_row)[2 * i + 1];                                                \
    const Uint32 r = ((p1 & 0x00ff0000) + (p2 & 0x00ff0000) + (p3 & 0x00ff0000) + (p4 & 0x00ff0000)) >> 18; \
    const Uint32 g = ((p1 & 0x0000ff00) + (p2 & 0x0000ff00) + (p3 & 0x0000ff00) + (p4 & 0x0000ff00)) >> 10; \
    const Uint32 b = ((p1 & 0x000000ff) + (p2 & 0x000000ff) + (p3 & 0x000000ff) + (p4 & 0x000000ff)) >> 2

#define READ_2x1_PIXELS                                             \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];            \
    const Uint32 p2 = ((const Uint32 *)next_row)[2 * i];            \
    const Uint32 r = ((p1 & 0x00ff0000) + (p2 & 0x00ff0000)) >> 17; \
    const Uint32 g = ((p1 & 0x0000ff00) + (p2 & 0x0000ff00)) >> 9;  \
    const Uint32 b = ((p1 & 0x000000ff) + (p2 & 0x000000ff)) >> 1

#define READ_1x2_PIXELS                                             \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];            \
    const Uint32 p2 = ((const Uint32 *)curr_row)[2 * i + 1];        \
    const Uint32 r = ((p1 & 0x00ff0000) + (p2 & 0x00ff0000)) >> 17; \
    const Uint32 g = ((p1 & 0x0000ff00) + (p2 & 0x0000ff00)) >> 9;  \
    const Uint32 b = ((p1 & 0x000000ff) + (p2 & 0x000000ff)) >> 1

#define READ_1x1_PIXEL                                  \
    const Uint32 p = ((const Uint32 *)curr_row)[2 * i]; \
    const Uint32 r = (p & 0x00ff0000) >> 16;            \
    const Uint32 g = (p & 0x0000ff00) >> 8;             \
    const Uint32 b = (p & 0x000000ff)

#define READ_TWO_RGB_PIXELS                                  \
    const Uint32 p = ((const Uint32 *)curr_row)[2 * i];      \
    const Uint32 r = (p & 0x00ff0000) >> 16;                 \
    const Uint32 g = (p & 0x0000ff00) >> 8;                  \
    const Uint32 b = (p & 0x000000ff);                       \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i + 1]; \
    const Uint32 r1 = (p1 & 0x00ff0000) >> 16;               \
    const Uint32 g1 = (p1 & 0x0000ff00) >> 8;                \
    const Uint32 b1 = (p1 & 0x000000ff);                     \
    const Uint32 R = (r + r1) / 2;                           \
    const Uint32 G = (g + g1) / 2;                           \
    const Uint32 B = (b + b1) / 2

#define READ_ONE_RGB_PIXEL READ_1x1_PIXEL

    switch (dst_format) {
    case SDL_PIXELFORMAT_YV12:
    case SDL_PIXELFORMAT_IYUV:
    case SDL_PIXELFORMAT_I444:
    case SDL_PIXELFORMAT_NV12:
    case SDL_PIXELFORMAT_NV21:
    {
        const Uint8 *curr_row, *next_row;

        Uint8 *plane_y;
        Uint8 *plane_u;
        Uint8 *plane_v;
        Uint8 *plane_interleaved_uv;
        Uint32 y_stride, uv_stride, y_skip, uv_skip;

        if (!SDL_GetYUVPlanes(width, height, dst_format, dst, dst_pitch,
                          (const Uint8 **)&plane_y, (const Uint8 **)&plane_u, (const Uint8 **)&plane_v,
                          &y_stride, &uv_stride)) {
            return false;
        }

        plane_interleaved_uv = (plane_y + height * y_stride);
        y_skip = (y_stride - width);

        curr_row = (const Uint8 *)src;

        // Write Y plane
        for (j = 0; j < height; j++) {
            for (i = 0; i < width; i++) {
                const Uint32 p1 = ((const Uint32 *)curr_row)[i];
                const Uint32 r = (p1 & 0x00ff0000) >> 16;
                const Uint32 g = (p1 & 0x0000ff00) >> 8;
                const Uint32 b = (p1 & 0x000000ff);
                *plane_y++ = MAKE_Y(r, g, b);
            }
            plane_y += y_skip;
            curr_row += src_pitch;
        }

        curr_row = (const Uint8 *)src;
        next_row = (const Uint8 *)src;
        next_row += src_pitch;

        if (dst_format == SDL_PIXELFORMAT_YV12 || dst_format == SDL_PIXELFORMAT_IYUV) {
            // Write UV planes, not interleaved
            uv_skip = (uv_stride - (width + 1) / 2);
            for (j = 0; j < height_half; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_2x2_PIXELS;
                    *plane_u++ = MAKE_U(r, g, b);
                    *plane_v++ = MAKE_V(r, g, b);
                }
                if (width_remainder) {
                    READ_2x1_PIXELS;
                    *plane_u++ = MAKE_U(r, g, b);
                    *plane_v++ = MAKE_V(r, g, b);
                }
                plane_u += uv_skip;
                plane_v += uv_skip;
                curr_row += src_pitch_x_2;
                next_row += src_pitch_x_2;
            }
            if (height_remainder) {
                for (i = 0; i < width_half; i++) {
                    READ_1x2_PIXELS;
                    *plane_u++ = MAKE_U(r, g, b);
                    *plane_v++ = MAKE_V(r, g, b);
                }
                if (width_remainder) {
                    READ_1x1_PIXEL;
                    *plane_u++ = MAKE_U(r, g, b);
                    *plane_v++ = MAKE_V(r, g, b);
                }
                plane_u += uv_skip;
                plane_v += uv_skip;
            }
        } else if (dst_format == SDL_PIXELFORMAT_I444) {
            // Write UV planes, not interleaved
            uv_skip = (uv_stride - width);
            for (j = 0; j < height; j++) {
                for (i = 0; i < width; i++) {
                    const Uint32 p1 = ((const Uint32 *)curr_row)[i];
                    const Uint32 r = (p1 & 0x00ff0000) >> 16;
                    const Uint32 g = (p1 & 0x0000ff00) >> 8;
                    const Uint32 b = (p1 & 0x000000ff);
                    *plane_u++ = MAKE_U(r, g, b);
                    *plane_v++ = MAKE_V(r, g, b);
                }
                plane_u += uv_skip;
                plane_v += uv_skip;
                curr_row += src_pitch;
            }
        } else if (dst_format == SDL_PIXELFORMAT_NV12) {
            uv_skip = (uv_stride - ((width + 1) / 2) * 2);
            for (j = 0; j < height_half; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_2x2_PIXELS;
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                }
                if (width_remainder) {
                    READ_2x1_PIXELS;
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                }
                plane_interleaved_uv += uv_skip;
                curr_row += src_pitch_x_2;
                next_row += src_pitch_x_2;
            }
            if (height_remainder) {
                for (i = 0; i < width_half; i++) {
                    READ_1x2_PIXELS;
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                }
                if (width_remainder) {
                    READ_1x1_PIXEL;
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                }
            }
        } else /* dst_format == SDL_PIXELFORMAT_NV21 */ {
            uv_skip = (uv_stride - ((width + 1) / 2) * 2);
            for (j = 0; j < height_half; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_2x2_PIXELS;
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                }
                if (width_remainder) {
                    READ_2x1_PIXELS;
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                }
                plane_interleaved_uv += uv_skip;
                curr_row += src_pitch_x_2;
                next_row += src_pitch_x_2;
            }
            if (height_remainder) {
                for (i = 0; i < width_half; i++) {
                    READ_1x2_PIXELS;
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                }
                if (width_remainder) {
                    READ_1x1_PIXEL;
                    *plane_interleaved_uv++ = MAKE_V(r, g, b);
                    *plane_interleaved_uv++ = MAKE_U(r, g, b);
                }
            }
        }
    } break;

    case SDL_PIXELFORMAT_YUY2:
    case SDL_PIXELFORMAT_UYVY:
    case SDL_PIXELFORMAT_YVYU:
    {
        const Uint8 *curr_row = (const Uint8 *)src;
        Uint8 *plane = (Uint8 *)dst;
        const int row_size = (4 * ((width + 1) / 2));
        int plane_skip;

        if (dst_pitch < row_size) {
            return SDL_SetError("Destination pitch is too small, expected at least %d", row_size);
        }
        plane_skip = (dst_pitch - row_size);

        // Write YUV plane, packed
        if (dst_format == SDL_PIXELFORMAT_YUY2) {
            for (j = 0; j < height; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_TWO_RGB_PIXELS;
                    // Y U Y1 V
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_U(R, G, B);
                    *plane++ = MAKE_Y(r1, g1, b1);
                    *plane++ = MAKE_V(R, G, B);
                }
                if (width_remainder) {
                    READ_ONE_RGB_PIXEL;
                    // Y U Y V
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_U(r, g, b);
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_V(r, g, b);
                }
                plane += plane_skip;
                curr_row += src_pitch;
            }
        } else if (dst_format == SDL_PIXELFORMAT_UYVY) {
            for (j = 0; j < height; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_TWO_RGB_PIXELS;
                    // U Y V Y1
                    *plane++ = MAKE_U(R, G, B);
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_V(R, G, B);
                    *plane++ = MAKE_Y(r1, g1, b1);
                }
                if (width_remainder) {
                    READ_ONE_RGB_PIXEL;
                    // U Y V Y
                    *plane++ = MAKE_U(r, g, b);
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_V(r, g, b);
                    *plane++ = MAKE_Y(r, g, b);
                }
                plane += plane_skip;
                curr_row += src_pitch;
            }
        } else if (dst_format == SDL_PIXELFORMAT_YVYU) {
            for (j = 0; j < height; j++) {
                for (i = 0; i < width_half; i++) {
                    READ_TWO_RGB_PIXELS;
                    // Y V Y1 U
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_V(R, G, B);
                    *plane++ = MAKE_Y(r1, g1, b1);
                    *plane++ = MAKE_U(R, G, B);
                }
                if (width_remainder) {
                    READ_ONE_RGB_PIXEL;
                    // Y V Y U
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_V(r, g, b);
                    *plane++ = MAKE_Y(r, g, b);
                    *plane++ = MAKE_U(r, g, b);
                }
                plane += plane_skip;
                curr_row += src_pitch;
            }
        }
    } break;

    default:
        return SDL_SetError("Unsupported YUV destination format: %s", SDL_GetPixelFormatName(dst_format));
    }
#undef MAKE_Y
#undef MAKE_U
#undef MAKE_V
#undef READ_2x2_PIXELS
#undef READ_2x1_PIXELS
#undef READ_1x2_PIXELS
#undef READ_1x1_PIXEL
#undef READ_TWO_RGB_PIXELS
#undef READ_ONE_RGB_PIXEL
    return true;
}

static bool SDL_ConvertPixels_XBGR2101010_to_P010(int width, int height, const void *src, int src_pitch, SDL_PixelFormat dst_format, void *dst, int dst_pitch, YCbCrType yuv_type)
{
    const int src_pitch_x_2 = src_pitch * 2;
    const int height_half = height / 2;
    const int height_remainder = (height & 0x1);
    const int width_half = width / 2;
    const int width_remainder = (width & 0x1);
    int i, j;

    const struct RGB2YUVFactors *cvt = &RGB2YUVFactorTables[yuv_type];

#define MAKE_Y(r, g, b) (Uint16)(((int)(cvt->y[0] * (r) + cvt->y[1] * (g) + cvt->y[2] * (b) + 0.5f) + cvt->y_offset) << 6)
#define MAKE_U(r, g, b) (Uint16)(((int)(cvt->u[0] * (r) + cvt->u[1] * (g) + cvt->u[2] * (b) + 0.5f) + 512) << 6)
#define MAKE_V(r, g, b) (Uint16)(((int)(cvt->v[0] * (r) + cvt->v[1] * (g) + cvt->v[2] * (b) + 0.5f) + 512) << 6)

#define READ_2x2_PIXELS                                                                                     \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];                                                    \
    const Uint32 p2 = ((const Uint32 *)curr_row)[2 * i + 1];                                                \
    const Uint32 p3 = ((const Uint32 *)next_row)[2 * i];                                                    \
    const Uint32 p4 = ((const Uint32 *)next_row)[2 * i + 1];                                                \
    const Uint32 r = ((p1 & 0x000003ff) + (p2 & 0x000003ff) + (p3 & 0x000003ff) + (p4 & 0x000003ff)) >> 2;  \
    const Uint32 g = ((p1 & 0x000ffc00) + (p2 & 0x000ffc00) + (p3 & 0x000ffc00) + (p4 & 0x000ffc00)) >> 12; \
    const Uint32 b = ((p1 & 0x3ff00000) + (p2 & 0x3ff00000) + (p3 & 0x3ff00000) + (p4 & 0x3ff00000)) >> 22

#define READ_2x1_PIXELS                                             \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];            \
    const Uint32 p2 = ((const Uint32 *)next_row)[2 * i];            \
    const Uint32 r = ((p1 & 0x000003ff) + (p2 & 0x000003ff)) >> 1;  \
    const Uint32 g = ((p1 & 0x000ffc00) + (p2 & 0x000ffc00)) >> 11; \
    const Uint32 b = ((p1 & 0x3ff00000) + (p2 & 0x3ff00000)) >> 21

#define READ_1x2_PIXELS                                             \
    const Uint32 p1 = ((const Uint32 *)curr_row)[2 * i];            \
    const Uint32 p2 = ((const Uint32 *)curr_row)[2 * i + 1];        \
    const Uint32 r = ((p1 & 0x000003ff) + (p2 & 0x000003ff)) >> 1;  \
    const Uint32 g = ((p1 & 0x000ffc00) + (p2 & 0x000ffc00)) >> 11; \
    const Uint32 b = ((p1 & 0x3ff00000) + (p2 & 0x3ff00000)) >> 21

#define READ_1x1_PIXEL                                  \
    const Uint32 p = ((const Uint32 *)curr_row)[2 * i]; \
    const Uint32 r = (p & 0x000003ff);                  \
    const Uint32 g = (p & 0x000ffc00) >> 10;            \
    const Uint32 b = (p & 0x3ff00000) >> 20

    const Uint8 *curr_row, *next_row;

    Uint16 *plane_y;
    Uint16 *plane_u;
    Uint16 *plane_v;
    Uint16 *plane_interleaved_uv;
    Uint32 y_stride, uv_stride, y_skip, uv_skip;

    if (!SDL_GetYUVPlanes(width, height, dst_format, dst, dst_pitch,
                      (const Uint8 **)&plane_y, (const Uint8 **)&plane_u, (const Uint8 **)&plane_v,
                      &y_stride, &uv_stride)) {
        return false;
    }

    y_stride /= sizeof(Uint16);
    uv_stride /= sizeof(Uint16);

    plane_interleaved_uv = (plane_y + height * y_stride);
    y_skip = (y_stride - width);

    curr_row = (const Uint8 *)src;

    // Write Y plane
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            const Uint32 p1 = ((const Uint32 *)curr_row)[i];
            const Uint32 r = (p1 >>  0) & 0x03ff;
            const Uint32 g = (p1 >> 10) & 0x03ff;
            const Uint32 b = (p1 >> 20) & 0x03ff;
            *plane_y++ = MAKE_Y(r, g, b);
        }
        plane_y += y_skip;
        curr_row += src_pitch;
    }

    curr_row = (const Uint8 *)src;
    next_row = (const Uint8 *)src;
    next_row += src_pitch;

    uv_skip = (uv_stride - ((width + 1) / 2) * 2);
    for (j = 0; j < height_half; j++) {
        for (i = 0; i < width_half; i++) {
            READ_2x2_PIXELS;
            *plane_interleaved_uv++ = MAKE_U(r, g, b);
            *plane_interleaved_uv++ = MAKE_V(r, g, b);
        }
        if (width_remainder) {
            READ_2x1_PIXELS;
            *plane_interleaved_uv++ = MAKE_U(r, g, b);
            *plane_interleaved_uv++ = MAKE_V(r, g, b);
        }
        plane_interleaved_uv += uv_skip;
        curr_row += src_pitch_x_2;
        next_row += src_pitch_x_2;
    }
    if (height_remainder) {
        for (i = 0; i < width_half; i++) {
            READ_1x2_PIXELS;
            *plane_interleaved_uv++ = MAKE_U(r, g, b);
            *plane_interleaved_uv++ = MAKE_V(r, g, b);
        }
        if (width_remainder) {
            READ_1x1_PIXEL;
            *plane_interleaved_uv++ = MAKE_U(r, g, b);
            *plane_interleaved_uv++ = MAKE_V(r, g, b);
        }
    }

#undef MAKE_Y
#undef MAKE_U
#undef MAKE_V
#undef READ_2x2_PIXELS
#undef READ_2x1_PIXELS
#undef READ_1x2_PIXELS
#undef READ_1x1_PIXEL
    return true;
}

bool SDL_ConvertPixels_RGB_to_YUV(int width, int height,
                                  SDL_PixelFormat src_format, SDL_Colorspace src_colorspace, SDL_PropertiesID src_properties, const void *src, int src_pitch,
                                  SDL_PixelFormat dst_format, SDL_Colorspace dst_colorspace, SDL_PropertiesID dst_properties, void *dst, int dst_pitch)
{
    YCbCrType yuv_type = YCBCR_601_LIMITED;

    if (!GetYUVConversionType(dst_colorspace, width, height, 8, &yuv_type)) {
        return false;
    }

#if 0 // Doesn't handle odd widths
    // RGB24 to FOURCC
    if (src_format == SDL_PIXELFORMAT_RGB24) {
        Uint8 *y;
        Uint8 *u;
        Uint8 *v;
        Uint32 y_stride;
        Uint32 uv_stride;

        if (SDL_GetYUVPlanes(width, height, dst_format, dst, dst_pitch, (const Uint8 **)&y, (const Uint8 **)&u, (const Uint8 **)&v, &y_stride, &uv_stride) < 0) {
            return false;
        }

        rgb24_yuv420_std(width, height, src, src_pitch, y, u, v, y_stride, uv_stride, yuv_type);
        return true;
    }
#endif

    // ARGB8888 to FOURCC
    if (src_format == SDL_PIXELFORMAT_ARGB8888 || src_format == SDL_PIXELFORMAT_XRGB8888) {
        return SDL_ConvertPixels_XRGB8888_to_YUV(width, height, src, src_pitch, dst_format, dst, dst_pitch, yuv_type);
    }

    if (dst_format == SDL_PIXELFORMAT_P010) {
        if (src_format == SDL_PIXELFORMAT_XBGR2101010) {
            return SDL_ConvertPixels_XBGR2101010_to_P010(width, height, src, src_pitch, dst_format, dst, dst_pitch, yuv_type);
        }

        // We currently only support converting from XBGR2101010 to P010
        bool result;
        void *tmp;
        int tmp_pitch = (width * sizeof(Uint32));

        tmp = SDL_malloc((size_t)tmp_pitch * height);
        if (!tmp) {
            return false;
        }

        // convert src/src_format to tmp/XBGR2101010
        result = SDL_ConvertPixelsAndColorspace(width, height, src_format, src_colorspace, src_properties, src, src_pitch, SDL_PIXELFORMAT_XBGR2101010, dst_colorspace, dst_properties, tmp, tmp_pitch);
        if (!result) {
            SDL_free(tmp);
            return false;
        }

        // convert tmp/XBGR2101010 to dst/P010
        result = SDL_ConvertPixels_XBGR2101010_to_P010(width, height, tmp, tmp_pitch, dst_format, dst, dst_pitch, yuv_type);
        SDL_free(tmp);
        return result;
    }

    // not ARGB8888 to FOURCC : need an intermediate conversion
    {
        bool result;
        void *tmp;
        int tmp_pitch = (width * sizeof(Uint32));

        tmp = SDL_malloc((size_t)tmp_pitch * height);
        if (!tmp) {
            return false;
        }

        // convert src/src_format to tmp/XRGB8888
        result = SDL_ConvertPixelsAndColorspace(width, height, src_format, src_colorspace, src_properties, src, src_pitch, SDL_PIXELFORMAT_XRGB8888, SDL_COLORSPACE_SRGB, 0, tmp, tmp_pitch);
        if (!result) {
            SDL_free(tmp);
            return false;
        }

        // convert tmp/XRGB8888 to dst/FOURCC
        result = SDL_ConvertPixels_XRGB8888_to_YUV(width, height, tmp, tmp_pitch, dst_format, dst, dst_pitch, yuv_type);
        SDL_free(tmp);
        return result;
    }
}

#endif // SDL_HAVE_YUV
