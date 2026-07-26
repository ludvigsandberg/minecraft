#include "client/system.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xutil.h>

#include <glad/glad.h>
#include <GL/glx.h>

#include "macros.h"

#define GLX_CONTEXT_MAJOR_VERSION_ARB    0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB    0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB     0x9126
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig,
                                                     GLXContext, int,
                                                     const int *);

static const int visual_attribs[] = {GLX_X_RENDERABLE,
                                     True,
                                     GLX_DRAWABLE_TYPE,
                                     GLX_WINDOW_BIT,
                                     GLX_RENDER_TYPE,
                                     GLX_RGBA_BIT,
                                     GLX_X_VISUAL_TYPE,
                                     GLX_TRUE_COLOR,
                                     GLX_RED_SIZE,
                                     8,
                                     GLX_GREEN_SIZE,
                                     8,
                                     GLX_BLUE_SIZE,
                                     8,
                                     GLX_ALPHA_SIZE,
                                     8,
                                     GLX_DEPTH_SIZE,
                                     24,
                                     GLX_STENCIL_SIZE,
                                     8,
                                     GLX_DOUBLEBUFFER,
                                     True,
                                     None};

static const int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
                                      3,
                                      GLX_CONTEXT_MINOR_VERSION_ARB,
                                      3,
                                      GLX_CONTEXT_PROFILE_MASK_ARB,
                                      GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                                      None};

static char key_state[256] = {0};

double elapsed_time_seconds(void) {
    static clock_t start   = 0;
    static int initialized = 0;

    if (!initialized) {
        start       = clock();
        initialized = 1;
    }

    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

void window_init(window_t *window) {
    int framebuffer_count;
    GLXFBConfig *framebuffer_configs;
    GLXFBConfig framebuffer_config;
    XVisualInfo *visual_info;
    Window root;
    XSetWindowAttributes window_attribs;
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    GLXContext context;

    window->display = XOpenDisplay(NULL);

    if (!window->display) {
        printf("Failed to open X display.\r\n");
        exit(EXIT_FAILURE);
    }

    framebuffer_configs =
        glXChooseFBConfig(window->display, DefaultScreen(window->display),
                          visual_attribs, &framebuffer_count);

    if (!framebuffer_configs) {
        printf("Failed to retrieve a framebuffer config.\r\n");
        exit(EXIT_FAILURE);
    }

    framebuffer_config = framebuffer_configs[0];
    XFree(framebuffer_configs);

    visual_info =
        glXGetVisualFromFBConfig(window->display, framebuffer_config);

    if (!visual_info) {
        printf("Failed to retrieve visual info.\r\n");
        exit(EXIT_FAILURE);
    }

    root = RootWindow(window->display, visual_info->screen);

    window_attribs.colormap =
        XCreateColormap(window->display, root, visual_info->visual, AllocNone);
    window_attribs.background_pixel = None;
    window_attribs.border_pixel     = 0;
    window_attribs.event_mask =
        StructureNotifyMask | KeyPressMask | KeyReleaseMask;

    window->handle = XCreateWindow(
        window->display, root, 0, 0, 800, 600, 0, visual_info->depth,
        InputOutput, visual_info->visual,
        CWBorderPixel | CWColormap | CWEventMask, &window_attribs);

    if (!window->handle) {
        printf("Failed to create X11 window.\r\n");
        exit(EXIT_FAILURE);
    }

    XFree(visual_info);

    XStoreName(window->display, window->handle, "Minecraft");
    XMapWindow(window->display, window->handle);

    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
            (const GLubyte *)"glXCreateContextAttribsARB");

    if (!glXCreateContextAttribsARB) {
        printf("glXCreateContextAttribsARB not found.\r\n");
        exit(EXIT_FAILURE);
    }

    context = glXCreateContextAttribsARB(window->display, framebuffer_config,
                                         0, True, context_attribs);
    XSync(window->display, False);

    if (!context) {
        printf("Failed to create OpenGL 3.3 context.\r\n");
        exit(EXIT_FAILURE);
    }

    glXMakeCurrent(window->display, window->handle, context);

    if (!gladLoadGLLoader((GLADloadproc)glXGetProcAddressARB)) {
        printf("Failed to initialize GLAD.\r\n");
        exit(EXIT_FAILURE);
    }
}

void window_update(window_t *window) {
    XEvent event;

    while (XPending(window->display) > 0) {
        XNextEvent(window->display, &event);

        switch (event.type) {
            case KeyPress:
                key_state[event.xkey.keycode] = 1;
                break;

            case KeyRelease:
                if (XPending(window->display) > 0) {
                    XEvent next_event;
                    XPeekEvent(window->display, &next_event);

                    if (next_event.type == KeyPress &&
                        next_event.xkey.keycode == event.xkey.keycode &&
                        next_event.xkey.time == event.xkey.time) {
                        break;
                    }
                }
                key_state[event.xkey.keycode] = 0;
                break;
        }
    }
}

int window_is_key_pressed(const window_t *window, KeySym keysym) {
    KeyCode code = XKeysymToKeycode(window->display, keysym);

    if (!code) {
        return FALSE;
    }
    return key_state[code];
}