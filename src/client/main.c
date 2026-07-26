#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <glad/glad.h>
#include <GL/glx.h>

#include "macros.h"
#include "client/system.h"
#include "client/renderer.h"
#include "client/opengl.h"
#include "client/chunk.h"
#include "client/world.h"
#include "client/camera.h"
#include "client/gui.h"

int main(int argc, char **argv) {
    window_t window;
    renderer_t renderer;
    camera_t camera;
    world_t world;
    gui_t gui;
    double last_time;

    (void)argc;
    (void)argv;

    window_init(&window);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);
#endif

    camera_init(&camera);
    world_init(&world);
    renderer_init(&renderer);
    gui_init(&gui);

    last_time = elapsed_time_seconds();

    while (TRUE) {
        double current_time;
        float delta_time;
        int vertical      = 16;
        vec3_t player_pos = {{4.0f, 0.0f, 4.0f}};
        vec3_t player_vel = {{0.5f, 0.0f, 0.5f}};

        /* Update. */

        while (XPending(window.display)) {
            XEvent event;
            XNextEvent(window.display, &event);
        }

        window_update(&window);

        current_time = elapsed_time_seconds();
        delta_time   = (float)(current_time - last_time);
        last_time    = current_time;

        camera_update(&camera, &window, delta_time);

        world_update(&world, &camera);

        /* Draw. */

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer_draw_sky(&renderer, &camera);

        world_draw(&world, &renderer, &camera);

        renderer_draw_player(&renderer, &player_pos, &player_vel, 0.0f, 90.0f,
                             &camera);

        gui_text(&gui, 10, 10, "Minecraft");
        gui_text(&gui, 10, 10 + vertical, "Frame %.1fms",
                 (double)(delta_time * 1000.0f));

        gui_text(&gui, 10, 10 + 3 * vertical, "X %.2f",
                 (double)camera.pos.VEC_X);
        gui_text(&gui, 10, 10 + 4 * vertical, "Y %.2f",
                 (double)camera.pos.VEC_Y);
        gui_text(&gui, 10, 10 + 5 * vertical, "Z %.2f",
                 (double)camera.pos.VEC_Z);

        gui_text(&gui, 10, 10 + 7 * vertical, "Yaw %.2f", (double)camera.yaw);
        gui_text(&gui, 10, 10 + 8 * vertical, "Pitch %.2f",
                 (double)camera.pitch);

        gui_flush(&gui, &camera);

        glXSwapBuffers(window.display, window.handle);
    }
}
