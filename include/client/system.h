#ifndef SYSTEM_H
#define SYSTEM_H

#include <X11/Xlib.h>
#include <X11/keysym.h>

double elapsed_time_seconds(void);

typedef struct window_s {
    Display *display;
    Window handle;
} window_t;

void window_init(window_t *window);
void window_update(window_t *window);
int window_is_key_pressed(const window_t *window, KeySym keysym);

#endif
