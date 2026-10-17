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

#define CHECK_COMMAND_BUFFER_RETURN_NULL                            \
    if (((CommandBufferCommonHeader *)command_buffer)->submitted) { \
        SDL_assert_release(!"Command buffer already submitted!");   \
        return NULL;                                                \
    }

#define CHECK_ANY_PASS_IN_PROGRESS(msg, retval)                                 \
    if (                                                                        \
        ((CommandBufferCommonHeader *)command_buffer)->render_pass.in_progress ||  \
        ((CommandBufferCommonHeader *)command_buffer)->compute_pass.in_progress || \
        ((CommandBufferCommonHeader *)command_buffer)->copy_pass.in_progress) {    \
        SDL_assert_release(!msg);                                               \
        return retval;                                                          \
    }

#define CHECK_COPYPASS                                     \
    if (!((Pass *)copy_pass)->in_progress) {                 \
        SDL_assert_release(!"Copy pass not in progress!"); \
        return;                                            \
    }

#define COMMAND_BUFFER_DEVICE \
    ((CommandBufferCommonHeader *)command_buffer)->device

#define COPYPASS_COMMAND_BUFFER \
    ((Pass *)copy_pass)->command_buffer

#define COPYPASS_DEVICE \
    ((CommandBufferCommonHeader *)COPYPASS_COMMAND_BUFFER)->device

// Copy Pass

SDL_GPUCopyPass *SDL_BeginGPUCopyPass(
    SDL_GPUCommandBuffer *command_buffer)
{
    CommandBufferCommonHeader *commandBufferHeader;

    CHECK_PARAM(command_buffer == NULL) {
        SDL_InvalidParamError("command_buffer");
        return NULL;
    }

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        CHECK_COMMAND_BUFFER_RETURN_NULL
        CHECK_ANY_PASS_IN_PROGRESS("Cannot begin copy pass during another pass!", NULL);
    }

    COMMAND_BUFFER_DEVICE->BeginCopyPass(
        command_buffer);

    commandBufferHeader = (CommandBufferCommonHeader *)command_buffer;

    if (COMMAND_BUFFER_DEVICE->debug_mode) {
        commandBufferHeader->copy_pass.in_progress = true;
    }

    return (SDL_GPUCopyPass *)&(commandBufferHeader->copy_pass);
}

void SDL_UploadToGPUTexture(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUTextureTransferInfo *source,
    const SDL_GPUTextureRegion *destination,
    bool cycle)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    if (destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->transfer_buffer == NULL) {
            SDL_assert_release(!"Source transfer buffer cannot be NULL!");
            return;
        }
        if (destination->texture == NULL) {
            SDL_assert_release(!"Destination texture cannot be NULL!");
            return;
        }
    }

    COPYPASS_DEVICE->UploadToTexture(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination,
        cycle);
}

void SDL_UploadToGPUBuffer(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUTransferBufferLocation *source,
    const SDL_GPUBufferRegion *destination,
    bool cycle)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    CHECK_PARAM(destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->transfer_buffer == NULL) {
            SDL_assert_release(!"Source transfer buffer cannot be NULL!");
            return;
        }
        if (destination->buffer == NULL) {
            SDL_assert_release(!"Destination buffer cannot be NULL!");
            return;
        }
    }

    COPYPASS_DEVICE->UploadToBuffer(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination,
        cycle);
}

void SDL_CopyGPUTextureToTexture(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUTextureLocation *source,
    const SDL_GPUTextureLocation *destination,
    Uint32 w,
    Uint32 h,
    Uint32 d,
    bool cycle)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    CHECK_PARAM(destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->texture == NULL) {
            SDL_assert_release(!"Source texture cannot be NULL!");
            return;
        }
        if (destination->texture == NULL) {
            SDL_assert_release(!"Destination texture cannot be NULL!");
            return;
        }

        TextureCommonHeader *srcHeader = (TextureCommonHeader *)source->texture;
        TextureCommonHeader *dstHeader = (TextureCommonHeader *)destination->texture;
        if (srcHeader->info.format != dstHeader->info.format) {
            SDL_assert_release(!"Source and destination textures must have the same format!");
            return;
        }
    }

    COPYPASS_DEVICE->CopyTextureToTexture(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination,
        w,
        h,
        d,
        cycle);
}

void SDL_CopyGPUBufferToBuffer(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUBufferLocation *source,
    const SDL_GPUBufferLocation *destination,
    Uint32 size,
    bool cycle)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    CHECK_PARAM(destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->buffer == NULL) {
            SDL_assert_release(!"Source buffer cannot be NULL!");
            return;
        }
        if (destination->buffer == NULL) {
            SDL_assert_release(!"Destination buffer cannot be NULL!");
            return;
        }
    }

    COPYPASS_DEVICE->CopyBufferToBuffer(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination,
        size,
        cycle);
}

void SDL_DownloadFromGPUTexture(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUTextureRegion *source,
    const SDL_GPUTextureTransferInfo *destination)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    CHECK_PARAM(destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->texture == NULL) {
            SDL_assert_release(!"Source texture cannot be NULL!");
            return;
        }
        if (destination->transfer_buffer == NULL) {
            SDL_assert_release(!"Destination transfer buffer cannot be NULL!");
            return;
        }
    }

    COPYPASS_DEVICE->DownloadFromTexture(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination);
}

void SDL_DownloadFromGPUBuffer(
    SDL_GPUCopyPass *copy_pass,
    const SDL_GPUBufferRegion *source,
    const SDL_GPUTransferBufferLocation *destination)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }
    CHECK_PARAM(source == NULL) {
        SDL_InvalidParamError("source");
        return;
    }
    CHECK_PARAM(destination == NULL) {
        SDL_InvalidParamError("destination");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
        if (source->buffer == NULL) {
            SDL_assert_release(!"Source buffer cannot be NULL!");
            return;
        }
        if (destination->transfer_buffer == NULL) {
            SDL_assert_release(!"Destination transfer buffer cannot be NULL!");
            return;
        }
    }

    COPYPASS_DEVICE->DownloadFromBuffer(
        COPYPASS_COMMAND_BUFFER,
        source,
        destination);
}

void SDL_EndGPUCopyPass(
    SDL_GPUCopyPass *copy_pass)
{
    CHECK_PARAM(copy_pass == NULL) {
        SDL_InvalidParamError("copy_pass");
        return;
    }

    if (COPYPASS_DEVICE->debug_mode) {
        CHECK_COPYPASS
    }

    COPYPASS_DEVICE->EndCopyPass(
        COPYPASS_COMMAND_BUFFER);

    if (COPYPASS_DEVICE->debug_mode) {
        ((CommandBufferCommonHeader *)COPYPASS_COMMAND_BUFFER)->copy_pass.in_progress = false;
    }
}

