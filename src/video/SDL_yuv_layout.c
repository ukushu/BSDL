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

bool SDL_IsYUVPlanar1x1Format(SDL_PixelFormat format)
{
    return format == SDL_PIXELFORMAT_I444 || format == SDL_PIXELFORMAT_I4FL;
}

bool SDL_IsYUVPlanar2x2Format(SDL_PixelFormat format)
{
    return format == SDL_PIXELFORMAT_YV12 ||
           format == SDL_PIXELFORMAT_IYUV ||
           format == SDL_PIXELFORMAT_NV12 ||
           format == SDL_PIXELFORMAT_NV21 ||
           format == SDL_PIXELFORMAT_P010 ||
           format == SDL_PIXELFORMAT_I0FL;
}

bool SDL_IsYUVPacked4Format(Uint32 format)
{
    return format == SDL_PIXELFORMAT_YUY2 || format == SDL_PIXELFORMAT_UYVY || format == SDL_PIXELFORMAT_YVYU;
}

/*
 * Calculate YUV size and pitch. Check for overflow.
 * Output 'pitch' that can be used with SDL_ConvertPixels()
 */
bool SDL_CalculateYUVSize(SDL_PixelFormat format, int w, int h, size_t *size, size_t *pitch)
{
    int sz_plane = 0, sz_plane_chroma = 0, sz_plane_packed = 0;

    if (SDL_IsYUVPlanar1x1Format(format)) {
        /* sz_plane == w * h * bpp; */
        size_t s1;
        if (!SDL_size_mul_check_overflow(w, h, &s1)) {
            return SDL_SetError("width * height would overflow");
        }
        if (!SDL_size_mul_check_overflow(s1, SDL_BYTESPERPIXEL(format), &s1)) {
            return SDL_SetError("width * height * bpp would overflow");
        }
        sz_plane = (int)s1;
        sz_plane_chroma = sz_plane;
    } else if (SDL_IsYUVPlanar2x2Format(format)) {
        {
            /* sz_plane == w * h * bpp; */
            size_t s1;
            if (!SDL_size_mul_check_overflow(w, h, &s1)) {
                return SDL_SetError("width * height would overflow");
            }
            if (!SDL_size_mul_check_overflow(s1, SDL_BYTESPERPIXEL(format), &s1)) {
                return SDL_SetError("width * height * bpp would overflow");
            }
            sz_plane = (int)s1;
        }

        {
            /* sz_plane_chroma == ((w + 1) / 2) * ((h + 1) / 2) * bpp; */
            size_t s1, s2, s3;
            if (!SDL_size_add_check_overflow(w, 1, &s1)) {
                return SDL_SetError("width + 1 would overflow");
            }
            s1 = s1 / 2;
            if (!SDL_size_add_check_overflow(h, 1, &s2)) {
                return SDL_SetError("height + 1 would overflow");
            }
            s2 = s2 / 2;
            if (!SDL_size_mul_check_overflow(s1, s2, &s3)) {
                return SDL_SetError("width * height would overflow");
            }
            if (!SDL_size_mul_check_overflow(s3, SDL_BYTESPERPIXEL(format), &s3)) {
                return SDL_SetError("width * height * bpp would overflow");
            }
            sz_plane_chroma = (int)s3;
        }
    } else if (SDL_IsYUVPacked4Format(format)) {
        /* sz_plane_packed == ((w + 1) / 2) * h; */
        size_t s1, s2;
        if (!SDL_size_add_check_overflow(w, 1, &s1)) {
            return SDL_SetError("width + 1 would overflow");
        }
        s1 = s1 / 2;
        if (!SDL_size_mul_check_overflow(s1, h, &s2)) {
            return SDL_SetError("width * height would overflow");
        }
        sz_plane_packed = (int) s2;
    } else {
        return SDL_Unsupported();
    }

    switch (format) {
    case SDL_PIXELFORMAT_YV12: /**< Planar mode: Y + V + U  (3 planes) */
    case SDL_PIXELFORMAT_IYUV: /**< Planar mode: Y + U + V  (3 planes) */
    case SDL_PIXELFORMAT_I444: /**< Planar mode: Y + U + V  (3 planes) */
    case SDL_PIXELFORMAT_I0FL: /**< Planar mode: Y + U + V  (3 planes) */
    case SDL_PIXELFORMAT_I4FL: /**< Planar mode: Y + U + V  (3 planes) */

        if (pitch) {
            *pitch = w * SDL_BYTESPERPIXEL(format);
        }

        if (size) {
            // dst_size == sz_plane + sz_plane_chroma + sz_plane_chroma;
            size_t s1, s2;
            if (!SDL_size_add_check_overflow(sz_plane, sz_plane_chroma, &s1)) {
                return SDL_SetError("Y + U would overflow");
            }
            if (!SDL_size_add_check_overflow(s1, sz_plane_chroma, &s2)) {
                return SDL_SetError("Y + U + V would overflow");
            }
            *size = (int)s2;
        }
        break;

    case SDL_PIXELFORMAT_YUY2: /**< Packed mode: Y0+U0+Y1+V0 (1 plane) */
    case SDL_PIXELFORMAT_UYVY: /**< Packed mode: U0+Y0+V0+Y1 (1 plane) */
    case SDL_PIXELFORMAT_YVYU: /**< Packed mode: Y0+V0+Y1+U0 (1 plane) */

        if (pitch) {
            /* pitch == ((w + 1) / 2) * 4; */
           size_t p1, p2;
           if (!SDL_size_add_check_overflow(w, 1, &p1)) {
               return SDL_SetError("width + 1 would overflow");
           }
           p1 = p1 / 2;
           if (!SDL_size_mul_check_overflow(p1, 4, &p2)) {
               return SDL_SetError("width * 4 would overflow");
           }
           *pitch = p2;
        }

        if (size) {
            /* dst_size == 4 * sz_plane_packed; */
            size_t s1;
            if (!SDL_size_mul_check_overflow(sz_plane_packed, 4, &s1)) {
                return SDL_SetError("plane * 4 would overflow");
            }
            *size = (int) s1;
        }
        break;

    case SDL_PIXELFORMAT_NV12: /**< Planar mode: Y + U/V interleaved  (2 planes) */
    case SDL_PIXELFORMAT_NV21: /**< Planar mode: Y + V/U interleaved  (2 planes) */
    case SDL_PIXELFORMAT_P010: /**< Planar mode: Y + U/V interleaved, 10 bit  (2 planes) */
        if (pitch) {
            *pitch = w * SDL_BYTESPERPIXEL(format);
        }

        if (size) {
            // dst_size == sz_plane + sz_plane_chroma + sz_plane_chroma;
            size_t s1, s2;
            if (!SDL_size_add_check_overflow(sz_plane, sz_plane_chroma, &s1)) {
                return SDL_SetError("Y + U would overflow");
            }
            if (!SDL_size_add_check_overflow(s1, sz_plane_chroma, &s2)) {
                return SDL_SetError("Y + U + V would overflow");
            }
            *size = (int) s2;
        }
        break;

    default:
        return SDL_Unsupported();
    }

    return true;
}

#ifdef SDL_HAVE_YUV

bool SDL_GetYUVPlanes(int width, int height, SDL_PixelFormat format, const void *yuv, int yuv_pitch,
                         const Uint8 **y, const Uint8 **u, const Uint8 **v, Uint32 *y_stride, Uint32 *uv_stride)
{
    const Uint8 *planes[3] = { NULL, NULL, NULL };
    int pitches[3] = { 0, 0, 0 };
    int uv_width;

    switch (format) {
    case SDL_PIXELFORMAT_YV12:
    case SDL_PIXELFORMAT_IYUV:
    case SDL_PIXELFORMAT_I0FL:
        pitches[0] = yuv_pitch;
        pitches[1] = ((pitches[0] / SDL_BYTESPERPIXEL(format) + 1) / 2) * SDL_BYTESPERPIXEL(format);
        pitches[2] = pitches[1];
        planes[0] = (const Uint8 *)yuv;
        planes[1] = planes[0] + pitches[0] * height;
        planes[2] = planes[1] + pitches[1] * ((height + 1) / 2);
        break;
    case SDL_PIXELFORMAT_YUY2:
    case SDL_PIXELFORMAT_UYVY:
    case SDL_PIXELFORMAT_YVYU:
        pitches[0] = yuv_pitch;
        planes[0] = (const Uint8 *)yuv;
        break;
    case SDL_PIXELFORMAT_NV12:
    case SDL_PIXELFORMAT_NV21:
        pitches[0] = yuv_pitch;
        pitches[1] = 2 * ((pitches[0] + 1) / 2);
        planes[0] = (const Uint8 *)yuv;
        planes[1] = planes[0] + pitches[0] * height;
        break;
    case SDL_PIXELFORMAT_P010:
        pitches[0] = yuv_pitch;
        uv_width = ((width + 1) / 2) * 2;
        pitches[1] = SDL_max(pitches[0], (int)(uv_width * sizeof(Uint16)));
        planes[0] = (const Uint8 *)yuv;
        planes[1] = planes[0] + pitches[0] * height;
        break;
    case SDL_PIXELFORMAT_I444:
    case SDL_PIXELFORMAT_I4FL:
        pitches[0] = yuv_pitch;
        pitches[1] = pitches[0];
        pitches[2] = pitches[1];
        planes[0] = (const Uint8 *)yuv;
        planes[1] = planes[0] + pitches[0] * height;
        planes[2] = planes[1] + pitches[1] * height;
        break;
    default:
        return SDL_SetError("GetYUVPlanes(): Unsupported YUV format: %s", SDL_GetPixelFormatName(format));
    }

    switch (format) {
    case SDL_PIXELFORMAT_YV12:
        *y = planes[0];
        *y_stride = pitches[0];
        *v = planes[1];
        *u = planes[2];
        *uv_stride = pitches[1];
        break;
    case SDL_PIXELFORMAT_IYUV:
    case SDL_PIXELFORMAT_I444:
    case SDL_PIXELFORMAT_I0FL:
    case SDL_PIXELFORMAT_I4FL:
        *y = planes[0];
        *y_stride = pitches[0];
        *v = planes[2];
        *u = planes[1];
        *uv_stride = pitches[1];
        break;
    case SDL_PIXELFORMAT_YUY2:
        *y = planes[0];
        *y_stride = pitches[0];
        *v = *y + 3;
        *u = *y + 1;
        *uv_stride = pitches[0];
        break;
    case SDL_PIXELFORMAT_UYVY:
        *y = planes[0] + 1;
        *y_stride = pitches[0];
        *v = *y + 1;
        *u = *y - 1;
        *uv_stride = pitches[0];
        break;
    case SDL_PIXELFORMAT_YVYU:
        *y = planes[0];
        *y_stride = pitches[0];
        *v = *y + 1;
        *u = *y + 3;
        *uv_stride = pitches[0];
        break;
    case SDL_PIXELFORMAT_NV12:
        *y = planes[0];
        *y_stride = pitches[0];
        *u = planes[1];
        *v = *u + 1;
        *uv_stride = pitches[1];
        break;
    case SDL_PIXELFORMAT_NV21:
        *y = planes[0];
        *y_stride = pitches[0];
        *v = planes[1];
        *u = *v + 1;
        *uv_stride = pitches[1];
        break;
    case SDL_PIXELFORMAT_P010:
        *y = planes[0];
        *y_stride = pitches[0];
        *u = planes[1];
        *v = *u + sizeof(Uint16);
        *uv_stride = pitches[1];
        break;
    default:
        // Should have caught this above
        return SDL_SetError("GetYUVPlanes[2]: Unsupported YUV format: %s", SDL_GetPixelFormatName(format));
    }
    return true;
}

#endif // SDL_HAVE_YUV
