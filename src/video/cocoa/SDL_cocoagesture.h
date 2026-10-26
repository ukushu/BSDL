#ifndef SDL_cocoagesture_h_
#define SDL_cocoagesture_h_

#include <stdbool.h>

typedef enum SDL_CocoaTrackpadGestureSource
{
    SDL_COCOA_TRACKPAD_GESTURE_MAGNIFICATION,
    SDL_COCOA_TRACKPAD_GESTURE_ROTATION
} SDL_CocoaTrackpadGestureSource;

typedef struct SDL_CocoaTrackpadGestureSession
{
    bool magnification_active;
    bool rotation_active;
} SDL_CocoaTrackpadGestureSession;

static inline bool SDL_CocoaTrackpadGestureBegin(SDL_CocoaTrackpadGestureSession *session,
                                                  SDL_CocoaTrackpadGestureSource source)
{
    bool *active = source == SDL_COCOA_TRACKPAD_GESTURE_ROTATION
        ? &session->rotation_active : &session->magnification_active;
    if (*active) {
        return false;
    }
    const bool was_active = session->magnification_active || session->rotation_active;
    *active = true;
    return !was_active;
}

static inline bool SDL_CocoaTrackpadGestureEnd(SDL_CocoaTrackpadGestureSession *session,
                                                SDL_CocoaTrackpadGestureSource source)
{
    bool *active = source == SDL_COCOA_TRACKPAD_GESTURE_ROTATION
        ? &session->rotation_active : &session->magnification_active;
    if (!*active) {
        return false;
    }
    *active = false;
    return !session->magnification_active && !session->rotation_active;
}

#endif
