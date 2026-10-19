/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2026 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
*/

#include "SDL_internal.h"
#include "SDL_sysvideo.h"
#include "../events/SDL_windowevents_c.h"

bool SDL_SetWindowSize(SDL_Window *window, int w, int h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);

    CHECK_PARAM(w <= 0) {
        return SDL_InvalidParamError("w");
    }
    CHECK_PARAM(h <= 0) {
        return SDL_InvalidParamError("h");
    }

    // It is possible for the aspect ratio constraints to not satisfy the size constraints.
    // The size constraints will override the aspect ratio constraints so we will apply the
    // the aspect ratio constraints first
    float new_aspect = w / (float)h;
    if (window->max_aspect > 0.0f && new_aspect > window->max_aspect) {
        w = (int)SDL_roundf(h * window->max_aspect);
    } else if (window->min_aspect > 0.0f && new_aspect < window->min_aspect) {
        h = (int)SDL_roundf(w / window->min_aspect);
    }

    // Make sure we don't exceed any window size limits
    if (window->min_w && w < window->min_w) {
        w = window->min_w;
    }
    if (window->max_w && w > window->max_w) {
        w = window->max_w;
    }
    if (window->min_h && h < window->min_h) {
        h = window->min_h;
    }
    if (window->max_h && h > window->max_h) {
        h = window->max_h;
    }

    window->last_size_pending = true;
    window->pending.w = w;
    window->pending.h = h;

    if (_this->SetWindowSize) {
        _this->SetWindowSize(_this, window);
        SDL_SyncIfRequired(window);
    } else {
        return SDL_Unsupported();
    }
    return true;
}

bool SDL_GetWindowSize(SDL_Window *window, int *w, int *h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (w) {
        *w = window->w;
    }
    if (h) {
        *h = window->h;
    }
    return true;
}

bool SDL_SetWindowAspectRatio(SDL_Window *window, float min_aspect, float max_aspect)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);

    window->min_aspect = min_aspect;
    window->max_aspect = max_aspect;

    if (_this->SetWindowAspectRatio) {
        _this->SetWindowAspectRatio(_this, window);
    }

    // Ensure that window has the correct aspect ratio
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowAspectRatio(SDL_Window *window, float *min_aspect, float *max_aspect)
{
    CHECK_WINDOW_MAGIC(window, false);

    if (min_aspect) {
        *min_aspect = window->min_aspect;
    }
    if (max_aspect) {
        *max_aspect = window->max_aspect;
    }
    return true;
}

bool SDL_SetWindowMinimumSize(SDL_Window *window, int min_w, int min_h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_PARAM(min_w < 0) {
        return SDL_InvalidParamError("min_w");
    }
    CHECK_PARAM(min_h < 0) {
        return SDL_InvalidParamError("min_h");
    }

    CHECK_PARAM((window->max_w && min_w > window->max_w) || (window->max_h && min_h > window->max_h)) {
        return SDL_SetError("SDL_SetWindowMinimumSize(): Tried to set minimum size larger than maximum size");
    }

    window->min_w = min_w;
    window->min_h = min_h;

    if (_this->SetWindowMinimumSize) {
        _this->SetWindowMinimumSize(_this, window);
    }

    // Ensure that window is not smaller than minimal size
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowMinimumSize(SDL_Window *window, int *min_w, int *min_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (min_w) {
        *min_w = window->min_w;
    }
    if (min_h) {
        *min_h = window->min_h;
    }
    return true;
}

bool SDL_SetWindowMaximumSize(SDL_Window *window, int max_w, int max_h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    CHECK_WINDOW_MAGIC(window, false);
    CHECK_PARAM(max_w < 0) {
        return SDL_InvalidParamError("max_w");
    }
    CHECK_PARAM(max_h < 0) {
        return SDL_InvalidParamError("max_h");
    }

    if ((max_w && max_w < window->min_w) ||
        (max_h && max_h < window->min_h)) {
        return SDL_SetError("SDL_SetWindowMaximumSize(): Tried to set maximum size smaller than minimum size");
    }

    window->max_w = max_w;
    window->max_h = max_h;

    if (_this->SetWindowMaximumSize) {
        _this->SetWindowMaximumSize(_this, window);
    }

    // Ensure that window is not larger than maximal size
    int w = window->last_size_pending ? window->pending.w : window->floating.w;
    int h = window->last_size_pending ? window->pending.h : window->floating.h;
    return SDL_SetWindowSize(window, w, h);
}

bool SDL_GetWindowMaximumSize(SDL_Window *window, int *max_w, int *max_h)
{
    CHECK_WINDOW_MAGIC(window, false);
    if (max_w) {
        *max_w = window->max_w;
    }
    if (max_h) {
        *max_h = window->max_h;
    }
    return true;
}

void SDL_CheckWindowSafeAreaChanged(SDL_Window *window)
{
    SDL_Rect rect;

    rect.x = window->safe_inset_left;
    rect.y = window->safe_inset_top;
    rect.w = window->w - (window->safe_inset_right + window->safe_inset_left);
    rect.h = window->h - (window->safe_inset_top + window->safe_inset_bottom);
    if (SDL_memcmp(&rect, &window->safe_rect, sizeof(rect)) != 0) {
        SDL_copyp(&window->safe_rect, &rect);
        SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_SAFE_AREA_CHANGED, 0, 0);
    }
}

void SDL_SetWindowSafeAreaInsets(SDL_Window *window, int left, int right, int top, int bottom)
{
    window->safe_inset_left = left;
    window->safe_inset_right = right;
    window->safe_inset_top = top;
    window->safe_inset_bottom = bottom;
    SDL_CheckWindowSafeAreaChanged(window);
}

bool SDL_GetWindowSafeArea(SDL_Window *window, SDL_Rect *rect)
{
    if (rect) {
        SDL_zerop(rect);
    }

    CHECK_WINDOW_MAGIC(window, false);

    if (rect) {
        if (SDL_RectEmpty(&window->safe_rect)) {
            rect->w = window->w;
            rect->h = window->h;
        } else {
            SDL_copyp(rect, &window->safe_rect);
        }
    }
    return true;
}
