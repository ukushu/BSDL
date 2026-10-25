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
#include "SDL_sysgpu.h"

#define CHECK_DEVICE_MAGIC(device, retval)  \
    CHECK_PARAM(device == NULL) {           \
        SDL_SetError("Invalid GPU device"); \
        return retval;                      \
    }

#define CHECK_TEXTUREFORMAT_ENUM_INVALID(enumval, retval)     \
    if (enumval <= SDL_GPU_TEXTUREFORMAT_INVALID || enumval >= SDL_GPU_TEXTUREFORMAT_MAX_ENUM_VALUE) {               \
        SDL_assert_release(!"Invalid texture format enum!"); \
        return retval;                                       \
    }

static bool TextureFormatIsComputeWritable[] = {
    false, // INVALID
    false, // A8_UNORM
    true,  // R8_UNORM
    true,  // R8G8_UNORM
    true,  // R8G8B8A8_UNORM
    true,  // R16_UNORM
    true,  // R16G16_UNORM
    true,  // R16G16B16A16_UNORM
    true,  // R10G10B10A2_UNORM
    false, // B5G6R5_UNORM
    false, // B5G5R5A1_UNORM
    false, // B4G4R4A4_UNORM
    false, // B8G8R8A8_UNORM
    false, // BC1_UNORM
    false, // BC2_UNORM
    false, // BC3_UNORM
    false, // BC4_UNORM
    false, // BC5_UNORM
    false, // BC7_UNORM
    false, // BC6H_FLOAT
    false, // BC6H_UFLOAT
    true,  // R8_SNORM
    true,  // R8G8_SNORM
    true,  // R8G8B8A8_SNORM
    true,  // R16_SNORM
    true,  // R16G16_SNORM
    true,  // R16G16B16A16_SNORM
    true,  // R16_FLOAT
    true,  // R16G16_FLOAT
    true,  // R16G16B16A16_FLOAT
    true,  // R32_FLOAT
    true,  // R32G32_FLOAT
    true,  // R32G32B32A32_FLOAT
    true,  // R11G11B10_UFLOAT
    true,  // R8_UINT
    true,  // R8G8_UINT
    true,  // R8G8B8A8_UINT
    true,  // R16_UINT
    true,  // R16G16_UINT
    true,  // R16G16B16A16_UINT
    true,  // R32_UINT
    true,  // R32G32_UINT
    true,  // R32G32B32A32_UINT
    true,  // R8_INT
    true,  // R8G8_INT
    true,  // R8G8B8A8_INT
    true,  // R16_INT
    true,  // R16G16_INT
    true,  // R16G16B16A16_INT
    true,  // R32_INT
    true,  // R32G32_INT
    true,  // R32G32B32A32_INT
    false, // R8G8B8A8_UNORM_SRGB
    false, // B8G8R8A8_UNORM_SRGB
    false, // BC1_UNORM_SRGB
    false, // BC3_UNORM_SRGB
    false, // BC3_UNORM_SRGB
    false, // BC7_UNORM_SRGB
    false, // D16_UNORM
    false, // D24_UNORM
    false, // D32_FLOAT
    false, // D24_UNORM_S8_UINT
    false, // D32_FLOAT_S8_UINT
    false, // ASTC_4x4_UNORM
    false, // ASTC_5x4_UNORM
    false, // ASTC_5x5_UNORM
    false, // ASTC_6x5_UNORM
    false, // ASTC_6x6_UNORM
    false, // ASTC_8x5_UNORM
    false, // ASTC_8x6_UNORM
    false, // ASTC_8x8_UNORM
    false, // ASTC_10x5_UNORM
    false, // ASTC_10x6_UNORM
    false, // ASTC_10x8_UNORM
    false, // ASTC_10x10_UNORM
    false, // ASTC_12x10_UNORM
    false, // ASTC_12x12_UNORM
    false, // ASTC_4x4_UNORM_SRGB
    false, // ASTC_5x4_UNORM_SRGB
    false, // ASTC_5x5_UNORM_SRGB
    false, // ASTC_6x5_UNORM_SRGB
    false, // ASTC_6x6_UNORM_SRGB
    false, // ASTC_8x5_UNORM_SRGB
    false, // ASTC_8x6_UNORM_SRGB
    false, // ASTC_8x8_UNORM_SRGB
    false, // ASTC_10x5_UNORM_SRGB
    false, // ASTC_10x6_UNORM_SRGB
    false, // ASTC_10x8_UNORM_SRGB
    false, // ASTC_10x10_UNORM_SRGB
    false, // ASTC_12x10_UNORM_SRGB
    false, // ASTC_12x12_UNORM_SRGB
    false, // ASTC_4x4_FLOAT
    false, // ASTC_5x4_FLOAT
    false, // ASTC_5x5_FLOAT
    false, // ASTC_6x5_FLOAT
    false, // ASTC_6x6_FLOAT
    false, // ASTC_8x5_FLOAT
    false, // ASTC_8x6_FLOAT
    false, // ASTC_8x8_FLOAT
    false, // ASTC_10x5_FLOAT
    false, // ASTC_10x6_FLOAT
    false, // ASTC_10x8_FLOAT
    false, // ASTC_10x10_FLOAT
    false, // ASTC_12x10_FLOAT
    false  // ASTC_12x12_FLOAT
};

// Drivers

#ifndef SDL_GPU_DISABLED
static const SDL_GPUBootstrap *backends[] = {
#ifdef SDL_GPU_PRIVATE
    &PrivateGPUDriver,
#endif
#ifdef SDL_GPU_METAL
    &MetalDriver,
#endif
#ifdef SDL_GPU_D3D12
    &D3D12Driver,
#endif
#ifdef SDL_GPU_VULKAN
    &VulkanDriver,
#endif
    NULL
};
#endif // !SDL_GPU_DISABLED

// Driver Functions

#ifndef SDL_GPU_DISABLED
static const SDL_GPUBootstrap * SDL_GPUSelectBackend(SDL_PropertiesID props)
{
    Uint32 i;
    const char *gpudriver;
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    if (_this == NULL) {
        SDL_SetError("Video subsystem not initialized");
        return NULL;
    }

#ifndef HAVE_GPU_OPENXR
    if (SDL_GetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_XR_ENABLE_BOOLEAN, false)) {
        SDL_SetError("OpenXR is not enabled in this build of SDL");
        return NULL;
    }
#endif

    gpudriver = SDL_GetHint(SDL_HINT_GPU_DRIVER);
    if (gpudriver == NULL) {
        gpudriver = SDL_GetStringProperty(props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, NULL);
    }

    // Environment/Properties override...
    if (gpudriver != NULL) {
        for (i = 0; backends[i]; i += 1) {
            if (SDL_strcasecmp(gpudriver, backends[i]->name) == 0) {
                if (backends[i]->PrepareDriver(_this, props)) {
                    return backends[i];
                }
            }
        }

        SDL_SetError("SDL_HINT_GPU_DRIVER %s unsupported!", gpudriver);
        return NULL;
    }

    for (i = 0; backends[i]; i += 1) {
        if (backends[i]->PrepareDriver(_this, props)) {
            return backends[i];
        }
    }

    SDL_SetError("No supported SDL_GPU backend found!");
    return NULL;
}

static void SDL_GPU_FillProperties(
    SDL_PropertiesID props,
    SDL_GPUShaderFormat format_flags,
    bool debug_mode,
    const char *name)
{
    if (format_flags & SDL_GPU_SHADERFORMAT_PRIVATE) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN, true);
    }
    if (format_flags & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
    }
    if (format_flags & SDL_GPU_SHADERFORMAT_DXBC) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN, true);
    }
    if (format_flags & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
    }
    if (format_flags & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);
    }
    if (format_flags & SDL_GPU_SHADERFORMAT_METALLIB) {
        SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, true);
    }
    SDL_SetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, debug_mode);
    SDL_SetStringProperty(props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, name);
}
#endif // SDL_GPU_DISABLED

bool SDL_GPUSupportsShaderFormats(
    SDL_GPUShaderFormat format_flags,
    const char *name)
{
#ifndef SDL_GPU_DISABLED
    bool result;
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_GPU_FillProperties(props, format_flags, false, name);
    result = SDL_GPUSupportsProperties(props);
    SDL_DestroyProperties(props);
    return result;
#else
    SDL_SetError("SDL not built with GPU support");
    return false;
#endif
}

bool SDL_GPUSupportsProperties(SDL_PropertiesID props)
{
#ifndef SDL_GPU_DISABLED
    return (SDL_GPUSelectBackend(props) != NULL);
#else
    SDL_SetError("SDL not built with GPU support");
    return false;
#endif
}

SDL_GPUDevice *SDL_CreateGPUDevice(
    SDL_GPUShaderFormat format_flags,
    bool debug_mode,
    const char *name)
{
#ifndef SDL_GPU_DISABLED
    SDL_GPUDevice *result;
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_GPU_FillProperties(props, format_flags, debug_mode, name);
    result = SDL_CreateGPUDeviceWithProperties(props);
    SDL_DestroyProperties(props);
    return result;
#else
    SDL_SetError("SDL not built with GPU support");
    return NULL;
#endif // SDL_GPU_DISABLED
}

SDL_GPUDevice *SDL_CreateGPUDeviceWithProperties(SDL_PropertiesID props)
{
#ifndef SDL_GPU_DISABLED
    bool debug_mode;
    bool preferLowPower;
    SDL_GPUDevice *result = NULL;
    const SDL_GPUBootstrap *selectedBackend;

    selectedBackend = SDL_GPUSelectBackend(props);
    if (selectedBackend != NULL) {
        SDL_DebugLogBackend("gpu", selectedBackend->name);
        debug_mode = SDL_GetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, true);
        preferLowPower = SDL_GetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, false);

        result = selectedBackend->CreateDevice(debug_mode, preferLowPower, props);
        if (result != NULL) {
            result->backend = selectedBackend->name;
            result->debug_mode = debug_mode;
            if (SDL_GetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_DEPTH_CLAMPING_BOOLEAN, true)) {
                result->default_enable_depth_clip = false;
            } else {
                result->default_enable_depth_clip = true;
                result->validate_feature_depth_clamp_disabled = true;
            }
            if (!SDL_GetBooleanProperty(props, SDL_PROP_GPU_DEVICE_CREATE_FEATURE_ANISOTROPY_BOOLEAN, true)) {
                result->validate_feature_anisotropy_disabled = true;
            }
            if (result->debug_mode) {
                result->max_viewport_width = SDL_MAX_UINT32;
                result->max_viewport_height = SDL_MAX_UINT32;
            }
        }
    }
    return result;
#else
    SDL_SetError("SDL not built with GPU support");
    return NULL;
#endif // SDL_GPU_DISABLED
}

void SDL_DestroyGPUDevice(SDL_GPUDevice *device)
{
    CHECK_DEVICE_MAGIC(device, );
    device->DestroyDevice(device);
}

XrResult SDL_DestroyGPUXRSwapchain(SDL_GPUDevice *device, XrSwapchain swapchain, SDL_GPUTexture **swapchainImages)
{
    CHECK_DEVICE_MAGIC(device, XR_ERROR_HANDLE_INVALID);

    return device->DestroyXRSwapchain(device->driverData, swapchain, swapchainImages);
}

int SDL_GetNumGPUDrivers(void)
{
#ifndef SDL_GPU_DISABLED
    return SDL_arraysize(backends) - 1;
#else
    return 0;
#endif
}

const char * SDL_GetGPUDriver(int index)
{
    CHECK_PARAM(index < 0 || index >= SDL_GetNumGPUDrivers()) {
        SDL_InvalidParamError("index");
        return NULL;
    }
#ifndef SDL_GPU_DISABLED
    return backends[index]->name;
#else
    return NULL;
#endif
}

const char * SDL_GetGPUDeviceDriver(SDL_GPUDevice *device)
{
    CHECK_DEVICE_MAGIC(device, NULL);
    return device->backend;
}

SDL_GPUShaderFormat SDL_GetGPUShaderFormats(SDL_GPUDevice *device)
{
    CHECK_DEVICE_MAGIC(device, SDL_GPU_SHADERFORMAT_INVALID);
    return device->shader_formats;
}

SDL_PropertiesID SDL_GetGPUDeviceProperties(SDL_GPUDevice *device)
{
    CHECK_DEVICE_MAGIC(device, 0);
    return device->GetDeviceProperties(device);
}

bool SDL_GPUTextureSupportsFormat(
    SDL_GPUDevice *device,
    SDL_GPUTextureFormat format,
    SDL_GPUTextureType type,
    SDL_GPUTextureUsageFlags usage)
{
    CHECK_DEVICE_MAGIC(device, false);

    if (device->debug_mode) {
        CHECK_TEXTUREFORMAT_ENUM_INVALID(format, false);
    }

    if ((usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE) ||
        (usage & SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_SIMULTANEOUS_READ_WRITE)) {
        if (!TextureFormatIsComputeWritable[format]) {
            return false;
        }
    }

    return device->SupportsTextureFormat(
        device->driverData,
        format,
        type,
        usage);
}

bool SDL_GPUTextureSupportsSampleCount(
    SDL_GPUDevice *device,
    SDL_GPUTextureFormat format,
    SDL_GPUSampleCount sample_count)
{
    CHECK_DEVICE_MAGIC(device, 0);

    if (device->debug_mode) {
        CHECK_TEXTUREFORMAT_ENUM_INVALID(format, 0);
    }

    return device->SupportsSampleCount(
        device->driverData,
        format,
        sample_count);
}

