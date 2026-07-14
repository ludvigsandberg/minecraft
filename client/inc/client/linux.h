/*****************************************************************************
 * File:        linux.h
 * Author:      ludvigsandberg
 * Date:        2026-07-05
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_LINUX_H
#define CLIENT_LINUX_H

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "common/types.h"

f64 get_time_in_seconds();

typedef struct window_s {
    Display *display;
    Window handle;
} window_t;

void window_new(window_t *window);
void window_update(window_t *window);
int window_is_key_pressed(const window_t *window, KeySym keysym);

#endif
