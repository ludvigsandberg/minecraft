#ifndef SYSTEM_H
#define SYSTEM_H

#include <X11/Xlib.h>
#include <X11/keysym.h>

double elapsed_time_seconds(void);

struct window {
    Display *display;
    Window handle;
};

void window_init(struct window *window);
void window_update(struct window *window);
int window_is_key_pressed(const struct window *window, KeySym keysym);

#endif
