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

#include "SDL_sysvideo.h"
#include "SDL_egl_c.h"
#include "../SDL_hints_c.h"

#ifdef SDL_VIDEO_OPENGL
#include <SDL3/SDL_opengl.h>
#endif // SDL_VIDEO_OPENGL

#if defined(SDL_VIDEO_OPENGL_ES) && !defined(SDL_VIDEO_OPENGL)
#include <SDL3/SDL_opengles.h>
#endif // SDL_VIDEO_OPENGL_ES && !SDL_VIDEO_OPENGL

// GL and GLES2 headers conflict on Linux 32 bits
#if defined(SDL_VIDEO_OPENGL_ES2) && !defined(SDL_VIDEO_OPENGL)
#include <SDL3/SDL_opengles2.h>
#endif // SDL_VIDEO_OPENGL_ES2 && !SDL_VIDEO_OPENGL

// GL_CONTEXT_RELEASE_BEHAVIOR and GL_CONTEXT_RELEASE_BEHAVIOR_KHR have the same number.
#ifndef GL_CONTEXT_RELEASE_BEHAVIOR
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82FB
#endif

// GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH and GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR have the same number.
#ifndef GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82FC
#endif

// This is always the same number between the various EXT/ARB/GLES extensions.
#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#endif

#ifndef GL_RGBA_FLOAT_MODE_ARB
#define GL_RGBA_FLOAT_MODE_ARB 0x8820
#endif /* GL_RGBA_FLOAT_MODE_ARB */

bool SDL_GL_LoadLibrary(const char *path)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    bool result;

    if (!_this) {
        return SDL_UninitializedVideo();
    }
    if (_this->gl_config.driver_loaded) {
        if (path && SDL_strcmp(path, _this->gl_config.driver_path) != 0) {
            return SDL_SetError("OpenGL library already loaded");
        }
        result = true;
    } else {
        if (!_this->GL_LoadLibrary) {
            return SDL_SetError("No dynamic %s support in current SDL video driver (%s)", "OpenGL", _this->name);
        }
        result = _this->GL_LoadLibrary(_this, path);
    }
    if (result) {
        ++_this->gl_config.driver_loaded;
    } else {
        if (_this->GL_UnloadLibrary) {
            _this->GL_UnloadLibrary(_this);
        }
    }
    return result;
}

SDL_FunctionPointer SDL_GL_GetProcAddress(const char *proc)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_FunctionPointer func;

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    func = NULL;
    if (_this->GL_GetProcAddress) {
        if (_this->gl_config.driver_loaded) {
            func = _this->GL_GetProcAddress(_this, proc);
        } else {
            SDL_SetError("No GL driver has been loaded");
        }
    } else {
        SDL_SetError("No dynamic GL support in current SDL video driver (%s)", _this->name);
    }
    return func;
}

SDL_FunctionPointer SDL_EGL_GetProcAddress(const char *proc)
{
#ifdef SDL_VIDEO_OPENGL_EGL
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_FunctionPointer func;

    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    func = NULL;

    if (_this->egl_data) {
        func = SDL_EGL_GetProcAddressInternal(_this, proc);
    } else {
        SDL_SetError("No EGL library has been loaded");
    }

    return func;
#else
    SDL_SetError("SDL was not built with EGL support");
    return NULL;
#endif
}

void SDL_GL_UnloadLibrary(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return;
    }
    if (_this->gl_config.driver_loaded > 0) {
        if (--_this->gl_config.driver_loaded > 0) {
            return;
        }
        if (_this->GL_UnloadLibrary) {
            _this->GL_UnloadLibrary(_this);
        }
    }
}

#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
typedef GLenum (APIENTRY* PFNGLGETERRORPROC) (void);
typedef void (APIENTRY* PFNGLGETINTEGERVPROC) (GLenum pname, GLint *params);
typedef const GLubyte *(APIENTRY* PFNGLGETSTRINGPROC) (GLenum name);
typedef void (APIENTRY* PFNGLENABLEPROC) (GLenum cap);
#ifndef SDL_VIDEO_OPENGL
typedef const GLubyte *(APIENTRY* PFNGLGETSTRINGIPROC) (GLenum name, GLuint index);
#endif

static SDL_INLINE bool isAtLeastGL3(const char *verstr)
{
    return verstr && (SDL_atoi(verstr) >= 3);
}
#endif // SDL_VIDEO_OPENGL || SDL_VIDEO_OPENGL_ES || SDL_VIDEO_OPENGL_ES2

bool SDL_GL_ExtensionSupported(const char *extension)
{
#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    PFNGLGETSTRINGPROC glGetStringFunc;
    const char *extensions;
    const char *start;
    const char *where, *terminator;

    // Extension names should not have spaces.
    where = SDL_strchr(extension, ' ');
    if (where || *extension == '\0') {
        return false;
    }
    // See if there's a hint or environment variable override
    start = SDL_GetHint(extension);
    if (start && *start == '0') {
        return false;
    }

    // Lookup the available extensions

    glGetStringFunc = (PFNGLGETSTRINGPROC)SDL_GL_GetProcAddress("glGetString");
    if (!glGetStringFunc) {
        return false;
    }

    if (isAtLeastGL3((const char *)glGetStringFunc(GL_VERSION))) {
        PFNGLGETSTRINGIPROC glGetStringiFunc;
        PFNGLGETINTEGERVPROC glGetIntegervFunc;
        GLint num_exts = 0;
        GLint i;

        glGetStringiFunc = (PFNGLGETSTRINGIPROC)SDL_GL_GetProcAddress("glGetStringi");
        glGetIntegervFunc = (PFNGLGETINTEGERVPROC)SDL_GL_GetProcAddress("glGetIntegerv");
        if ((!glGetStringiFunc) || (!glGetIntegervFunc)) {
            return false;
        }

#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS 0x821D
#endif
        glGetIntegervFunc(GL_NUM_EXTENSIONS, &num_exts);
        for (i = 0; i < num_exts; i++) {
            const char *thisext = (const char *)glGetStringiFunc(GL_EXTENSIONS, i);
            if (SDL_strcmp(thisext, extension) == 0) {
                return true;
            }
        }

        return false;
    }

    // Try the old way with glGetString(GL_EXTENSIONS) ...

    extensions = (const char *)glGetStringFunc(GL_EXTENSIONS);
    if (!extensions) {
        return false;
    }
    /*
     * It takes a bit of care to be fool-proof about parsing the OpenGL
     * extensions string. Don't be fooled by sub-strings, etc.
     */

    start = extensions;

    for (;;) {
        where = SDL_strstr(start, extension);
        if (!where) {
            break;
        }

        terminator = where + SDL_strlen(extension);
        if (where == extensions || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return true;
            }
        }

        start = terminator;
    }
    return false;
#else
    return false;
#endif
}

/* Deduce supported ES profile versions from the supported
   ARB_ES*_compatibility extensions. There is no direct query.

   This is normally only called when the OpenGL driver supports
   {GLX,WGL}_EXT_create_context_es2_profile.
 */
void SDL_GL_DeduceMaxSupportedESProfile(int *major, int *minor)
{
// THIS REQUIRES AN EXISTING GL CONTEXT THAT HAS BEEN MADE CURRENT.
// Please refer to https://bugzilla.libsdl.org/show_bug.cgi?id=3725 for discussion.
#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    /* XXX This is fragile; it will break in the event of release of
     * new versions of OpenGL ES.
     */
    if (SDL_GL_ExtensionSupported("GL_ARB_ES3_2_compatibility")) {
        *major = 3;
        *minor = 2;
    } else if (SDL_GL_ExtensionSupported("GL_ARB_ES3_1_compatibility")) {
        *major = 3;
        *minor = 1;
    } else if (SDL_GL_ExtensionSupported("GL_ARB_ES3_compatibility")) {
        *major = 3;
        *minor = 0;
    } else {
        *major = 2;
        *minor = 0;
    }
#endif
}

void SDL_EGL_SetAttributeCallbacks(SDL_EGLAttribArrayCallback platformAttribCallback,
                                   SDL_EGLIntArrayCallback surfaceAttribCallback,
                                   SDL_EGLIntArrayCallback contextAttribCallback,
                                   void *userdata)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        return;
    }
    _this->egl_platformattrib_callback = platformAttribCallback;
    _this->egl_surfaceattrib_callback = surfaceAttribCallback;
    _this->egl_contextattrib_callback = contextAttribCallback;
    _this->egl_attrib_callback_userdata = userdata;
}

void SDL_GL_ResetAttributes(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        return;
    }

    _this->egl_platformattrib_callback = NULL;
    _this->egl_surfaceattrib_callback = NULL;
    _this->egl_contextattrib_callback = NULL;
    _this->egl_attrib_callback_userdata = NULL;

    _this->gl_config.red_size = 8;
    _this->gl_config.green_size = 8;
    _this->gl_config.blue_size = 8;
    _this->gl_config.alpha_size = 8;
    _this->gl_config.buffer_size = 0;
    _this->gl_config.depth_size = 16;
    _this->gl_config.stencil_size = 0;
    _this->gl_config.double_buffer = 1;
    _this->gl_config.accum_red_size = 0;
    _this->gl_config.accum_green_size = 0;
    _this->gl_config.accum_blue_size = 0;
    _this->gl_config.accum_alpha_size = 0;
    _this->gl_config.stereo = 0;
    _this->gl_config.multisamplebuffers = 0;
    _this->gl_config.multisamplesamples = 0;
    _this->gl_config.floatbuffers = 0;
    _this->gl_config.retained_backing = 1;
    _this->gl_config.accelerated = -1; // accelerated or not, both are fine

#ifdef SDL_VIDEO_OPENGL
    _this->gl_config.major_version = 2;
    _this->gl_config.minor_version = 1;
    _this->gl_config.profile_mask = 0;
#elif defined(SDL_VIDEO_OPENGL_ES2)
    _this->gl_config.major_version = 2;
    _this->gl_config.minor_version = 0;
    _this->gl_config.profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
#elif defined(SDL_VIDEO_OPENGL_ES)
    _this->gl_config.major_version = 1;
    _this->gl_config.minor_version = 1;
    _this->gl_config.profile_mask = SDL_GL_CONTEXT_PROFILE_ES;
#endif

    _this->gl_config.flags = 0;
    _this->gl_config.framebuffer_srgb_capable = 0;
    _this->gl_config.no_error = 0;
    _this->gl_config.release_behavior = SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH;
    _this->gl_config.reset_notification = SDL_GL_CONTEXT_RESET_NO_NOTIFICATION;

    _this->gl_config.share_with_current_context = 0;

    _this->gl_config.egl_platform = 0;

    if (_this->GL_SetDefaultProfileConfig) {
        _this->GL_SetDefaultProfileConfig(_this);
    }
}

bool SDL_GL_SetAttribute(SDL_GLAttr attr, int value)
{
#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    bool result;

    if (!_this) {
        return SDL_UninitializedVideo();
    }
    result = true;
    switch (attr) {
    case SDL_GL_RED_SIZE:
        _this->gl_config.red_size = value;
        break;
    case SDL_GL_GREEN_SIZE:
        _this->gl_config.green_size = value;
        break;
    case SDL_GL_BLUE_SIZE:
        _this->gl_config.blue_size = value;
        break;
    case SDL_GL_ALPHA_SIZE:
        _this->gl_config.alpha_size = value;
        break;
    case SDL_GL_DOUBLEBUFFER:
        _this->gl_config.double_buffer = value;
        break;
    case SDL_GL_BUFFER_SIZE:
        _this->gl_config.buffer_size = value;
        break;
    case SDL_GL_DEPTH_SIZE:
        _this->gl_config.depth_size = value;
        break;
    case SDL_GL_STENCIL_SIZE:
        _this->gl_config.stencil_size = value;
        break;
    case SDL_GL_ACCUM_RED_SIZE:
        _this->gl_config.accum_red_size = value;
        break;
    case SDL_GL_ACCUM_GREEN_SIZE:
        _this->gl_config.accum_green_size = value;
        break;
    case SDL_GL_ACCUM_BLUE_SIZE:
        _this->gl_config.accum_blue_size = value;
        break;
    case SDL_GL_ACCUM_ALPHA_SIZE:
        _this->gl_config.accum_alpha_size = value;
        break;
    case SDL_GL_STEREO:
        _this->gl_config.stereo = value;
        break;
    case SDL_GL_MULTISAMPLEBUFFERS:
        _this->gl_config.multisamplebuffers = value;
        break;
    case SDL_GL_MULTISAMPLESAMPLES:
        _this->gl_config.multisamplesamples = value;
        break;
    case SDL_GL_FLOATBUFFERS:
        _this->gl_config.floatbuffers = value;
        break;
    case SDL_GL_ACCELERATED_VISUAL:
        _this->gl_config.accelerated = value;
        break;
    case SDL_GL_RETAINED_BACKING:
        _this->gl_config.retained_backing = value;
        break;
    case SDL_GL_CONTEXT_MAJOR_VERSION:
        _this->gl_config.major_version = value;
        break;
    case SDL_GL_CONTEXT_MINOR_VERSION:
        _this->gl_config.minor_version = value;
        break;
    case SDL_GL_CONTEXT_FLAGS:
        if (value & ~(SDL_GL_CONTEXT_DEBUG_FLAG |
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG |
                      SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG |
                      SDL_GL_CONTEXT_RESET_ISOLATION_FLAG)) {
            result = SDL_SetError("Unknown OpenGL context flag %d", value);
            break;
        }
        _this->gl_config.flags = value;
        break;
    case SDL_GL_CONTEXT_PROFILE_MASK:
        if (value != 0 &&
            value != SDL_GL_CONTEXT_PROFILE_CORE &&
            value != SDL_GL_CONTEXT_PROFILE_COMPATIBILITY &&
            value != SDL_GL_CONTEXT_PROFILE_ES) {
            result = SDL_SetError("Unknown OpenGL context profile %d", value);
            break;
        }
        _this->gl_config.profile_mask = value;
        break;
    case SDL_GL_SHARE_WITH_CURRENT_CONTEXT:
        _this->gl_config.share_with_current_context = value;
        break;
    case SDL_GL_FRAMEBUFFER_SRGB_CAPABLE:
        _this->gl_config.framebuffer_srgb_capable = value;
        break;
    case SDL_GL_CONTEXT_RELEASE_BEHAVIOR:
        _this->gl_config.release_behavior = value;
        break;
    case SDL_GL_CONTEXT_RESET_NOTIFICATION:
        _this->gl_config.reset_notification = value;
        break;
    case SDL_GL_CONTEXT_NO_ERROR:
        _this->gl_config.no_error = value;
        break;
    case SDL_GL_EGL_PLATFORM:
        _this->gl_config.egl_platform = value;
        break;
    default:
        result = SDL_SetError("Unknown OpenGL attribute");
        break;
    }
    return result;
#else
    return SDL_Unsupported();
#endif // SDL_VIDEO_OPENGL
}

bool SDL_GL_GetAttribute(SDL_GLAttr attr, int *value)
{
#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    PFNGLGETERRORPROC glGetErrorFunc;
    GLenum attrib = 0;
    GLenum error = 0;

    /*
     * Some queries in Core Profile desktop OpenGL 3+ contexts require
     * glGetFramebufferAttachmentParameteriv instead of glGetIntegerv. Note that
     * the enums we use for the former function don't exist in OpenGL ES 2, and
     * the function itself doesn't exist prior to OpenGL 3 and OpenGL ES 2.
     */
#ifdef SDL_VIDEO_OPENGL
    PFNGLGETSTRINGPROC glGetStringFunc;
    PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameterivFunc;
    GLenum attachment = GL_BACK_LEFT;
    GLenum attachmentattrib = 0;
#endif

    CHECK_PARAM(!value) {
        return SDL_InvalidParamError("value");
    }

    // Clear value in any case
    *value = 0;

    if (!_this) {
        return SDL_UninitializedVideo();
    }

    switch (attr) {
    case SDL_GL_RED_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE;
#endif
        attrib = GL_RED_BITS;
        break;
    case SDL_GL_BLUE_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE;
#endif
        attrib = GL_BLUE_BITS;
        break;
    case SDL_GL_GREEN_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE;
#endif
        attrib = GL_GREEN_BITS;
        break;
    case SDL_GL_ALPHA_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE;
#endif
        attrib = GL_ALPHA_BITS;
        break;
    case SDL_GL_DOUBLEBUFFER:
#ifdef SDL_VIDEO_OPENGL
        attrib = GL_DOUBLEBUFFER;
        break;
#else
        // OpenGL ES 1.0 and above specifications have EGL_SINGLE_BUFFER
        // parameter which switches double buffer to single buffer. OpenGL ES
        // SDL driver must set proper value after initialization
        *value = _this->gl_config.double_buffer;
        return true;
#endif
    case SDL_GL_DEPTH_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachment = GL_DEPTH;
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE;
#endif
        attrib = GL_DEPTH_BITS;
        break;
    case SDL_GL_STENCIL_SIZE:
#ifdef SDL_VIDEO_OPENGL
        attachment = GL_STENCIL;
        attachmentattrib = GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE;
#endif
        attrib = GL_STENCIL_BITS;
        break;
#ifdef SDL_VIDEO_OPENGL
    case SDL_GL_ACCUM_RED_SIZE:
        attrib = GL_ACCUM_RED_BITS;
        break;
    case SDL_GL_ACCUM_GREEN_SIZE:
        attrib = GL_ACCUM_GREEN_BITS;
        break;
    case SDL_GL_ACCUM_BLUE_SIZE:
        attrib = GL_ACCUM_BLUE_BITS;
        break;
    case SDL_GL_ACCUM_ALPHA_SIZE:
        attrib = GL_ACCUM_ALPHA_BITS;
        break;
    case SDL_GL_STEREO:
        attrib = GL_STEREO;
        break;
#else
    case SDL_GL_ACCUM_RED_SIZE:
    case SDL_GL_ACCUM_GREEN_SIZE:
    case SDL_GL_ACCUM_BLUE_SIZE:
    case SDL_GL_ACCUM_ALPHA_SIZE:
    case SDL_GL_STEREO:
        // none of these are supported in OpenGL ES
        *value = 0;
        return true;
#endif
    case SDL_GL_MULTISAMPLEBUFFERS:
        attrib = GL_SAMPLE_BUFFERS;
        break;
    case SDL_GL_MULTISAMPLESAMPLES:
        attrib = GL_SAMPLES;
        break;
    case SDL_GL_CONTEXT_RELEASE_BEHAVIOR:
        attrib = GL_CONTEXT_RELEASE_BEHAVIOR;
        break;
    case SDL_GL_BUFFER_SIZE:
    {
        int rsize = 0, gsize = 0, bsize = 0, asize = 0;

        // There doesn't seem to be a single flag in OpenGL for this!
        if (!SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &rsize)) {
            return false;
        }
        if (!SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &gsize)) {
            return false;
        }
        if (!SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &bsize)) {
            return false;
        }
        if (!SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &asize)) {
            return false;
        }

        *value = rsize + gsize + bsize + asize;
        return true;
    }
    case SDL_GL_ACCELERATED_VISUAL:
    {
        // Note that we can _not_ reasonably detect this once we have a context.
        *value = (_this->gl_config.accelerated != 0);
        return true;
    }
    case SDL_GL_RETAINED_BACKING:
    {
        *value = _this->gl_config.retained_backing;
        return true;
    }
    case SDL_GL_CONTEXT_MAJOR_VERSION:
    {
        *value = _this->gl_config.major_version;
        return true;
    }
    case SDL_GL_CONTEXT_MINOR_VERSION:
    {
        *value = _this->gl_config.minor_version;
        return true;
    }
    case SDL_GL_CONTEXT_FLAGS:
    {
        *value = _this->gl_config.flags;
        return true;
    }
    case SDL_GL_CONTEXT_PROFILE_MASK:
    {
        *value = _this->gl_config.profile_mask;
        return true;
    }
    case SDL_GL_SHARE_WITH_CURRENT_CONTEXT:
    {
        *value = _this->gl_config.share_with_current_context;
        return true;
    }
    case SDL_GL_FRAMEBUFFER_SRGB_CAPABLE:
    {
        *value = _this->gl_config.framebuffer_srgb_capable;
        return true;
    }
    case SDL_GL_CONTEXT_NO_ERROR:
    {
        *value = _this->gl_config.no_error;
        return true;
    }
    case SDL_GL_EGL_PLATFORM:
    {
        *value = _this->gl_config.egl_platform;
        return true;
    }
    case SDL_GL_FLOATBUFFERS:
    {
        if (_this->gl_config.HAS_GL_ARB_color_buffer_float) {
            attrib = GL_RGBA_FLOAT_MODE_ARB;
            break;
        } else {
            return 0;
        }
    }
    default:
        return SDL_SetError("Unknown OpenGL attribute");
    }

#ifdef SDL_VIDEO_OPENGL
    glGetStringFunc = (PFNGLGETSTRINGPROC)SDL_GL_GetProcAddress("glGetString");
    if (!glGetStringFunc) {
        return false;
    }

    if (attachmentattrib && isAtLeastGL3((const char *)glGetStringFunc(GL_VERSION))) {
        // glGetFramebufferAttachmentParameteriv needs to operate on the window framebuffer for this, so bind FBO 0 if necessary.
        GLint current_fbo = 0;
        PFNGLGETINTEGERVPROC glGetIntegervFunc = (PFNGLGETINTEGERVPROC) SDL_GL_GetProcAddress("glGetIntegerv");
        PFNGLBINDFRAMEBUFFERPROC glBindFramebufferFunc = (PFNGLBINDFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBindFramebuffer");
        if (glGetIntegervFunc && glBindFramebufferFunc) {
            glGetIntegervFunc(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);
        }

        glGetFramebufferAttachmentParameterivFunc = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv");
        if (glGetFramebufferAttachmentParameterivFunc) {
            if (glBindFramebufferFunc && (current_fbo != 0)) {
                glBindFramebufferFunc(GL_DRAW_FRAMEBUFFER, 0);
            }
            // glGetFramebufferAttachmentParameterivFunc may cause GL_INVALID_OPERATION when querying depth/stencil size if the
            // bits is 0. From the GL docs:
            //      If the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE, then either no framebuffer is bound to target;
            //      or a default framebuffer is queried, attachment is GL_DEPTH or GL_STENCIL, and the number of depth or stencil bits,
            //      respectively, is zero. In this case querying pname GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME will return zero, and all
            //      other queries will generate an error.
            GLint fbo_type = GL_FRAMEBUFFER_DEFAULT;
            if (attachment == GL_DEPTH || attachment == GL_STENCIL) {
                glGetFramebufferAttachmentParameterivFunc(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &fbo_type);
            }
            if (fbo_type != GL_NONE) {
                glGetFramebufferAttachmentParameterivFunc(GL_FRAMEBUFFER, attachment, attachmentattrib, (GLint *)value);
            } else {
                *value = 0;
            }
            if (glBindFramebufferFunc && (current_fbo != 0)) {
                glBindFramebufferFunc(GL_DRAW_FRAMEBUFFER, current_fbo);
            }
        } else {
            return false;
        }
    } else
#endif
    {
        PFNGLGETINTEGERVPROC glGetIntegervFunc = (PFNGLGETINTEGERVPROC)SDL_GL_GetProcAddress("glGetIntegerv");
        if (glGetIntegervFunc) {
            glGetIntegervFunc(attrib, (GLint *)value);
        } else {
            return false;
        }
    }

    glGetErrorFunc = (PFNGLGETERRORPROC)SDL_GL_GetProcAddress("glGetError");
    if (!glGetErrorFunc) {
        return false;
    }

    error = glGetErrorFunc();
    if (error != GL_NO_ERROR) {
        if (error == GL_INVALID_ENUM) {
            return SDL_SetError("OpenGL error: GL_INVALID_ENUM");
        } else if (error == GL_INVALID_VALUE) {
            return SDL_SetError("OpenGL error: GL_INVALID_VALUE");
        }
        return SDL_SetError("OpenGL error: %08X", error);
    }

    // convert GL_CONTEXT_RELEASE_BEHAVIOR values back to SDL_GL_CONTEXT_RELEASE_BEHAVIOR values
    if (attr == SDL_GL_CONTEXT_RELEASE_BEHAVIOR) {
        *value = (*value == GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH) ? SDL_GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH : SDL_GL_CONTEXT_RELEASE_BEHAVIOR_NONE;
    }

    return true;
#else
    return SDL_Unsupported();
#endif // SDL_VIDEO_OPENGL
}

#define NOT_AN_OPENGL_WINDOW "The specified window isn't an OpenGL window"

SDL_GLContext SDL_GL_CreateContext(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_GLContext ctx = NULL;
    CHECK_WINDOW_MAGIC(window, NULL);

    if (!(window->flags & SDL_WINDOW_OPENGL)) {
        SDL_SetError(NOT_AN_OPENGL_WINDOW);
        return NULL;
    }

#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    int srgb_requested = -1;
    const char *srgbhint = SDL_GetHint(SDL_HINT_OPENGL_FORCE_SRGB_FRAMEBUFFER);
    if (srgbhint && *srgbhint) {
        srgb_requested = SDL_GetStringBoolean(srgbhint, false) ? 1 : 0;
    }

#endif

    ctx = _this->GL_CreateContext(_this, window);

    // Creating a context is assumed to make it current in the SDL driver.
    if (ctx) {
        _this->current_glwin = window;
        _this->current_glctx = ctx;
        SDL_SetTLS(&_this->current_glwin_tls, window, NULL);
        SDL_SetTLS(&_this->current_glctx_tls, ctx, NULL);
    }

#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES) || defined(SDL_VIDEO_OPENGL_ES2)
    // try to force the window framebuffer to the requested sRGB state.
    if (srgb_requested != -1) {
        PFNGLENABLEPROC glToggleFunc = (PFNGLENABLEPROC) SDL_GL_GetProcAddress(srgb_requested ? "glEnable" : "glDisable");
        PFNGLGETSTRINGPROC glGetStringFunc = (PFNGLGETSTRINGPROC)SDL_GL_GetProcAddress("glGetString");
        if (glToggleFunc && glGetStringFunc) {
            bool supported = false;
            if (_this->gl_config.profile_mask & SDL_GL_CONTEXT_PROFILE_ES) {
                supported = SDL_GL_ExtensionSupported("GL_EXT_sRGB_write_control");  // GL_FRAMEBUFFER_SRGB is not core in any GLES version at the moment.
            } else {
                supported = isAtLeastGL3((const char *)glGetStringFunc(GL_VERSION)) ||  // no extensions needed in OpenGL 3+.
                            SDL_GL_ExtensionSupported("GL_EXT_framebuffer_sRGB") ||
                            SDL_GL_ExtensionSupported("GL_ARB_framebuffer_sRGB");
            }

            if (supported) {
                glToggleFunc(GL_FRAMEBUFFER_SRGB);
            }
        }
    }
#endif

    return ctx;
}

bool SDL_GL_MakeCurrent(SDL_Window *window, SDL_GLContext context)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    bool result;

    if (!_this) {
        return SDL_UninitializedVideo();
    }

    if (window == SDL_GL_GetCurrentWindow() &&
        context == SDL_GL_GetCurrentContext()) {
        // We're already current.
        return true;
    }

    if (!context) {
        window = NULL;
    } else if (window) {
        CHECK_WINDOW_MAGIC(window, false);

        if (!(window->flags & SDL_WINDOW_OPENGL)) {
            return SDL_SetError(NOT_AN_OPENGL_WINDOW);
        }
    } else if (!_this->gl_allow_no_surface) {
        return SDL_SetError("Use of OpenGL without a window is not supported on this platform");
    }

    result = _this->GL_MakeCurrent(_this, window, context);
    if (result) {
        _this->current_glwin = window;
        _this->current_glctx = context;
        SDL_SetTLS(&_this->current_glwin_tls, window, NULL);
        SDL_SetTLS(&_this->current_glctx_tls, context, NULL);
    }
    return result;
}

SDL_Window *SDL_GL_GetCurrentWindow(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    return (SDL_Window *)SDL_GetTLS(&_this->current_glwin_tls);
}

SDL_GLContext SDL_GL_GetCurrentContext(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    return (SDL_GLContext)SDL_GetTLS(&_this->current_glctx_tls);
}

SDL_EGLDisplay SDL_EGL_GetCurrentDisplay(void)
{
#ifdef SDL_VIDEO_OPENGL_EGL
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return EGL_NO_DISPLAY;
    }
    if (!_this->egl_data) {
        SDL_SetError("There is no current EGL display");
        return EGL_NO_DISPLAY;
    }
    return _this->egl_data->egl_display;
#else
    SDL_SetError("SDL was not built with EGL support");
    return NULL;
#endif
}

SDL_EGLConfig SDL_EGL_GetCurrentConfig(void)
{
#ifdef SDL_VIDEO_OPENGL_EGL
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    if (!_this->egl_data) {
        SDL_SetError("There is no current EGL display");
        return NULL;
    }
    return _this->egl_data->egl_config;
#else
    SDL_SetError("SDL was not built with EGL support");
    return NULL;
#endif
}

SDL_EGLConfig SDL_EGL_GetWindowSurface(SDL_Window *window)
{
#ifdef SDL_VIDEO_OPENGL_EGL
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    if (!_this->egl_data) {
        SDL_SetError("There is no current EGL display");
        return NULL;
    }
    if (_this->GL_GetEGLSurface) {
        return _this->GL_GetEGLSurface(_this, window);
    }
    return NULL;
#else
    SDL_SetError("SDL was not built with EGL support");
    return NULL;
#endif
}

bool SDL_GL_SetSwapInterval(int interval)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        return SDL_UninitializedVideo();
    } else if (SDL_GL_GetCurrentContext() == NULL) {
        return SDL_SetError("No OpenGL context has been made current");
    } else if (_this->GL_SetSwapInterval) {
        return _this->GL_SetSwapInterval(_this, interval);
    } else {
        return SDL_SetError("Setting the swap interval is not supported");
    }
}

bool SDL_GL_GetSwapInterval(int *interval)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_PARAM(!interval) {
       return SDL_InvalidParamError("interval");
    }

    *interval = 0;

    if (!_this) {
        return SDL_SetError("no video driver");
    } else if (SDL_GL_GetCurrentContext() == NULL) {
        return SDL_SetError("no current context");
    } else if (_this->GL_GetSwapInterval) {
        return _this->GL_GetSwapInterval(_this, interval);
    } else {
        return SDL_SetError("not implemented");
    }
}

bool SDL_GL_SwapWindow(SDL_Window *window)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (!(window->flags & SDL_WINDOW_OPENGL)) {
        return SDL_SetError(NOT_AN_OPENGL_WINDOW);
    }

    if (SDL_GL_GetCurrentWindow() != window) {
        return SDL_SetError("The specified window has not been made current");
    }

    return _this->GL_SwapWindow(_this, window);
}

bool SDL_GL_DestroyContext(SDL_GLContext context)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_PARAM(!_this) {
        return SDL_UninitializedVideo();
    }
    CHECK_PARAM(!context) {
        return SDL_InvalidParamError("context");
    }

    if (SDL_GL_GetCurrentContext() == context) {
        SDL_GL_MakeCurrent(NULL, NULL);
    }

    return _this->GL_DestroyContext(_this, context);
}

