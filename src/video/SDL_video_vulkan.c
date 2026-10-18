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

#define CHECK_WINDOW_MAGIC(window, result)                              \
    CHECK_PARAM(!_this) {                                               \
        SDL_UninitializedVideo();                                       \
        return result;                                                  \
    }                                                                   \
    CHECK_PARAM(!SDL_ObjectValid(window, SDL_OBJECT_TYPE_WINDOW)) {     \
        SDL_SetError("Invalid window");                                 \
        return result;                                                  \
    }

bool SDL_Vulkan_LoadLibrary(const char *path)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    bool result;

    if (!_this) {
        return SDL_UninitializedVideo();
    }
    if (_this->vulkan_config.loader_loaded) {
        if (path && SDL_strcmp(path, _this->vulkan_config.loader_path) != 0) {
            return SDL_SetError("Vulkan loader library already loaded");
        }
        result = true;
    } else {
        if (!_this->Vulkan_LoadLibrary) {
            return SDL_SetError("No dynamic %s support in current SDL video driver (%s)", "Vulkan", _this->name);
        }
        result = _this->Vulkan_LoadLibrary(_this, path);
    }
    if (result) {
        _this->vulkan_config.loader_loaded++;
    }
    return result;
}

SDL_FunctionPointer SDL_Vulkan_GetVkGetInstanceProcAddr(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return NULL;
    }
    if (!_this->vulkan_config.loader_loaded) {
        SDL_SetError("No Vulkan loader has been loaded");
        return NULL;
    }
    return (SDL_FunctionPointer)_this->vulkan_config.vkGetInstanceProcAddr;
}

void SDL_Vulkan_UnloadLibrary(void)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (!_this) {
        SDL_UninitializedVideo();
        return;
    }
    if (_this->vulkan_config.loader_loaded > 0) {
        if (--_this->vulkan_config.loader_loaded > 0) {
            return;
        }
        if (_this->Vulkan_UnloadLibrary) {
            _this->Vulkan_UnloadLibrary(_this);
        }
    }
}

char const * const *SDL_Vulkan_GetInstanceExtensions(Uint32 *count)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    Uint32 tmpcount = 0;
    if (!count) {
        count = &tmpcount;
    }

    if (!_this->Vulkan_GetInstanceExtensions) {
        *count = 0;
        SDL_Unsupported();
        return NULL;
    }

    return _this->Vulkan_GetInstanceExtensions(_this, count);
}

bool SDL_Vulkan_CreateSurface(SDL_Window *window,
                                  VkInstance instance,
                                  const struct VkAllocationCallbacks *allocator,
                                  VkSurfaceKHR *surface)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_WINDOW_MAGIC(window, false);

    if (!_this->Vulkan_CreateSurface) {
        SDL_Unsupported();
        return false;
    }

    CHECK_PARAM(!instance) {
        return SDL_InvalidParamError("instance");
    }

    CHECK_PARAM(!surface) {
        return SDL_InvalidParamError("surface");
    }

    if (!(window->flags & SDL_WINDOW_VULKAN)) {
        // No problem, we can convert to Vulkan
        if (window->flags & SDL_WINDOW_OPENGL) {
            window->flags &= ~SDL_WINDOW_OPENGL;
            SDL_GL_UnloadLibrary();
        }
        if (window->flags & SDL_WINDOW_METAL) {
            window->flags &= ~SDL_WINDOW_METAL;
            // Nothing more to do for Metal.
        }
        if (SDL_Vulkan_LoadLibrary(NULL)) {
            window->flags |= SDL_WINDOW_VULKAN;
        } else {
            return SDL_SetError("failed to load Vulkan library");
        }
    }

    return _this->Vulkan_CreateSurface(_this, window, instance, allocator, surface);
}

void SDL_Vulkan_DestroySurface(VkInstance instance,
                               VkSurfaceKHR surface,
                               const struct VkAllocationCallbacks *allocator)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    if (_this && instance && surface && _this->Vulkan_DestroySurface) {
        _this->Vulkan_DestroySurface(_this, instance, surface, allocator);
    }
}

bool SDL_Vulkan_GetPresentationSupport(VkInstance instance,
                                           VkPhysicalDevice physicalDevice,
                                           Uint32 queueFamilyIndex)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    CHECK_PARAM(!_this) {
        SDL_UninitializedVideo();
        return false;
    }

    CHECK_PARAM(!instance) {
        SDL_InvalidParamError("instance");
        return false;
    }

    CHECK_PARAM(!physicalDevice) {
        SDL_InvalidParamError("physicalDevice");
        return false;
    }

    if (_this->Vulkan_GetPresentationSupport) {
        return _this->Vulkan_GetPresentationSupport(_this, instance, physicalDevice, queueFamilyIndex);
    }

    /* If the backend does not have this function then it does not have a
     * WSI function to query it; in other words it's not necessary to check
     * as it is always supported.
     */
    return true;
}
