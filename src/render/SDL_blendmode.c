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

// Predefined blend modes
#define SDL_COMPOSE_BLENDMODE(srcColorFactor, dstColorFactor, colorOperation, \
                              srcAlphaFactor, dstAlphaFactor, alphaOperation) \
    (SDL_BlendMode)(((Uint32)(colorOperation) << 0) |                         \
                    ((Uint32)(srcColorFactor) << 4) |                         \
                    ((Uint32)(dstColorFactor) << 8) |                         \
                    ((Uint32)(alphaOperation) << 16) |                        \
                    ((Uint32)(srcAlphaFactor) << 20) |                        \
                    ((Uint32)(dstAlphaFactor) << 24))

#define SDL_BLENDMODE_NONE_FULL                                                              \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_BLEND_FULL                                                                                  \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_BLEND_PREMULTIPLIED_FULL                                                              \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_ADD_FULL                                                                    \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_ADD_PREMULTIPLIED_FULL                                                 \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_ONE,  SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_MOD_FULL                                                                     \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_COLOR, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD)

#define SDL_BLENDMODE_MUL_FULL                                                                                    \
    SDL_COMPOSE_BLENDMODE(SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, \
                          SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD)

static SDL_BlendMode SDL_GetShortBlendMode(SDL_BlendMode blendMode)
{
    if (blendMode == SDL_BLENDMODE_NONE_FULL) {
        return SDL_BLENDMODE_NONE;
    }
    if (blendMode == SDL_BLENDMODE_BLEND_FULL) {
        return SDL_BLENDMODE_BLEND;
    }
    if (blendMode == SDL_BLENDMODE_BLEND_PREMULTIPLIED_FULL) {
        return SDL_BLENDMODE_BLEND_PREMULTIPLIED;
    }
    if (blendMode == SDL_BLENDMODE_ADD_FULL) {
        return SDL_BLENDMODE_ADD;
    }
    if (blendMode == SDL_BLENDMODE_ADD_PREMULTIPLIED_FULL) {
        return SDL_BLENDMODE_ADD_PREMULTIPLIED;
    }
    if (blendMode == SDL_BLENDMODE_MOD_FULL) {
        return SDL_BLENDMODE_MOD;
    }
    if (blendMode == SDL_BLENDMODE_MUL_FULL) {
        return SDL_BLENDMODE_MUL;
    }
    return blendMode;
}

static SDL_BlendMode SDL_GetLongBlendMode(SDL_BlendMode blendMode)
{
    if (blendMode == SDL_BLENDMODE_NONE) {
        return SDL_BLENDMODE_NONE_FULL;
    }
    if (blendMode == SDL_BLENDMODE_BLEND) {
        return SDL_BLENDMODE_BLEND_FULL;
    }
    if (blendMode == SDL_BLENDMODE_BLEND_PREMULTIPLIED) {
        return SDL_BLENDMODE_BLEND_PREMULTIPLIED_FULL;
    }
    if (blendMode == SDL_BLENDMODE_ADD) {
        return SDL_BLENDMODE_ADD_FULL;
    }
    if (blendMode == SDL_BLENDMODE_ADD_PREMULTIPLIED) {
        return SDL_BLENDMODE_ADD_PREMULTIPLIED_FULL;
    }
    if (blendMode == SDL_BLENDMODE_MOD) {
        return SDL_BLENDMODE_MOD_FULL;
    }
    if (blendMode == SDL_BLENDMODE_MUL) {
        return SDL_BLENDMODE_MUL_FULL;
    }
    return blendMode;
}

SDL_BlendMode SDL_ComposeCustomBlendMode(SDL_BlendFactor srcColorFactor, SDL_BlendFactor dstColorFactor,
                                         SDL_BlendOperation colorOperation,
                                         SDL_BlendFactor srcAlphaFactor, SDL_BlendFactor dstAlphaFactor,
                                         SDL_BlendOperation alphaOperation)
{
    SDL_BlendMode blendMode = SDL_COMPOSE_BLENDMODE(srcColorFactor, dstColorFactor, colorOperation,
                                                    srcAlphaFactor, dstAlphaFactor, alphaOperation);
    return SDL_GetShortBlendMode(blendMode);
}

SDL_BlendFactor SDL_GetBlendModeSrcColorFactor(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendFactor)(((Uint32)blendMode >> 4) & 0xF);
}

SDL_BlendFactor SDL_GetBlendModeDstColorFactor(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendFactor)(((Uint32)blendMode >> 8) & 0xF);
}

SDL_BlendOperation SDL_GetBlendModeColorOperation(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendOperation)(((Uint32)blendMode >> 0) & 0xF);
}

SDL_BlendFactor SDL_GetBlendModeSrcAlphaFactor(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendFactor)(((Uint32)blendMode >> 20) & 0xF);
}

SDL_BlendFactor SDL_GetBlendModeDstAlphaFactor(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendFactor)(((Uint32)blendMode >> 24) & 0xF);
}

SDL_BlendOperation SDL_GetBlendModeAlphaOperation(SDL_BlendMode blendMode)
{
    blendMode = SDL_GetLongBlendMode(blendMode);
    return (SDL_BlendOperation)(((Uint32)blendMode >> 16) & 0xF);
}

