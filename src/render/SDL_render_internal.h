/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#ifndef SDL_render_internal_h_
#define SDL_render_internal_h_

#define CHECK_RENDERER_MAGIC_BUT_NOT_DESTROYED_FLAG(renderer, result)   \
    CHECK_PARAM(!SDL_ObjectValid(renderer, SDL_OBJECT_TYPE_RENDERER)) { \
        SDL_InvalidParamError("renderer");                              \
        return result;                                                  \
    }

#define CHECK_RENDERER_MAGIC(renderer, result)                          \
    CHECK_RENDERER_MAGIC_BUT_NOT_DESTROYED_FLAG(renderer, result);      \
    CHECK_PARAM(renderer->destroyed) {                                  \
        SDL_SetError("Renderer's window has been destroyed, can't use further"); \
        return result;                                                  \
    }

#define CHECK_TEXTURE_MAGIC(texture, result)                            \
    CHECK_PARAM(!SDL_ObjectValid(texture, SDL_OBJECT_TYPE_TEXTURE)) {   \
        SDL_InvalidParamError("texture");                               \
        return result;                                                  \
    }

#endif /* SDL_render_internal_h_ */
