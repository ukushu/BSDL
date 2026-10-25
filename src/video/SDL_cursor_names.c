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

#include "SDL_video_c.h"

#if defined(SDL_VIDEO_DRIVER_X11) || defined(SDL_VIDEO_DRIVER_WAYLAND) || defined(SDL_VIDEO_DRIVER_EMSCRIPTEN)
const char *SDL_GetCSSCursorName(SDL_SystemCursor id, const char **fallback_name)
{
    // Reference: https://www.w3.org/TR/css-ui-4/#cursor
    // Also in: https://www.freedesktop.org/wiki/Specifications/cursor-spec/
    switch (id) {
    case SDL_SYSTEM_CURSOR_DEFAULT:
        return "default";

    case SDL_SYSTEM_CURSOR_TEXT:
        return "text";

    case SDL_SYSTEM_CURSOR_WAIT:
        return "wait";

    case SDL_SYSTEM_CURSOR_CROSSHAIR:
        return "crosshair";

    case SDL_SYSTEM_CURSOR_PROGRESS:
        return "progress";

    case SDL_SYSTEM_CURSOR_NWSE_RESIZE:
        if (fallback_name) {
            // only a single arrow
            *fallback_name = "nw-resize";
        }
        return "nwse-resize";

    case SDL_SYSTEM_CURSOR_NESW_RESIZE:
        if (fallback_name) {
            // only a single arrow
            *fallback_name = "ne-resize";
        }
        return "nesw-resize";

    case SDL_SYSTEM_CURSOR_EW_RESIZE:
        if (fallback_name) {
            *fallback_name = "col-resize";
        }
        return "ew-resize";

    case SDL_SYSTEM_CURSOR_NS_RESIZE:
        if (fallback_name) {
            *fallback_name = "row-resize";
        }
        return "ns-resize";

    case SDL_SYSTEM_CURSOR_MOVE:
        if (fallback_name) {
            *fallback_name = "all-scroll";
        }
        return "move";

    case SDL_SYSTEM_CURSOR_NOT_ALLOWED:
        return "not-allowed";

    case SDL_SYSTEM_CURSOR_POINTER:
        return "pointer";

    case SDL_SYSTEM_CURSOR_NW_RESIZE:
        return "nw-resize";

    case SDL_SYSTEM_CURSOR_N_RESIZE:
        return "n-resize";

    case SDL_SYSTEM_CURSOR_NE_RESIZE:
        return "ne-resize";

    case SDL_SYSTEM_CURSOR_E_RESIZE:
        return "e-resize";

    case SDL_SYSTEM_CURSOR_SE_RESIZE:
        return "se-resize";

    case SDL_SYSTEM_CURSOR_S_RESIZE:
        return "s-resize";

    case SDL_SYSTEM_CURSOR_SW_RESIZE:
        return "sw-resize";

    case SDL_SYSTEM_CURSOR_W_RESIZE:
        return "w-resize";

    case SDL_SYSTEM_CURSOR_CONTEXT_MENU:
        return "context-menu";

    case SDL_SYSTEM_CURSOR_HELP:
        return "help";

    case SDL_SYSTEM_CURSOR_CELL:
        return "cell";

    case SDL_SYSTEM_CURSOR_VERTICAL_TEXT:
        return "vertical-text";

    case SDL_SYSTEM_CURSOR_ALIAS:
        return "alias";

    case SDL_SYSTEM_CURSOR_COPY:
        return "copy";

    case SDL_SYSTEM_CURSOR_NO_DROP:
        return "no-drop";

    case SDL_SYSTEM_CURSOR_GRAB:
        return "grab";

    case SDL_SYSTEM_CURSOR_GRABBING:
        return "grabbing";

    case SDL_SYSTEM_CURSOR_COL_RESIZE:
        return "col-resize";

    case SDL_SYSTEM_CURSOR_ROW_RESIZE:
        return "row-resize";

    case SDL_SYSTEM_CURSOR_ALL_SCROLL:
        if (fallback_name) {
            *fallback_name = "move";
        }
        return "all-scroll";

    case SDL_SYSTEM_CURSOR_ZOOM_IN:
        return "zoom-in";

    case SDL_SYSTEM_CURSOR_ZOOM_OUT:
        return "zoom-out";

    default:
        return "default";
    }
}
#endif
