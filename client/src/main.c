/*****************************************************************************
 * File:        main.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: Client entry point.
 *****************************************************************************/

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

#include "common/types.h"
#include "client/linux.h"
#include "client/gl.h"
#include "client/chunk.h"
#include "client/world.h"
#include "client/sky.h"
#include "client/camera.h"
#include "client/gui.h"
#include "client/player.h"

int main(int argc, char **argv) {
    window_t window;
    camera_t camera;
    world_t world;
    sky_t sky;
    gui_t gui;
    f64 last_time;

    (void)argc;
    (void)argv;

    window_new(&window);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);
#endif

    camera_new(&camera);
    world_new(&world);
    sky_new(&sky);
    gui_new(&gui);
    player_new();

    last_time = get_time_in_seconds();

    while (TRUE) {
        f64 current_time;
        f32 delta_time;
        int vertical      = 16;
        vec3_t player_pos = {{4.0f, 0.0f, 4.0f}};
        vec3_t player_vel = {{0.5f, 0.0f, 0.5f}};

        /* Update. */

        while (XPending(window.display)) {
            XEvent event;
            XNextEvent(window.display, &event);
        }

        window_update(&window);

        current_time = get_time_in_seconds();
        delta_time   = (f32)(current_time - last_time);
        last_time    = current_time;

        camera_update(&camera, &window, delta_time);

        world_update(&world, &camera);

        /* Draw. */

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sky_draw(&sky, &camera);

        world_draw(&world, &camera);

        gui_text(&gui, 10, 10, "minecraft");
        gui_text(&gui, 10, 10 + vertical, "frame %.1fms",
                 delta_time * 1000.0f);

        gui_text(&gui, 10, 10 + 3 * vertical, "x %.2f", camera.pos.VEC_X);
        gui_text(&gui, 10, 10 + 4 * vertical, "y %.2f", camera.pos.VEC_Y);
        gui_text(&gui, 10, 10 + 5 * vertical, "z %.2f", camera.pos.VEC_Z);

        gui_text(&gui, 10, 10 + 7 * vertical, "yaw %.2f", camera.yaw);
        gui_text(&gui, 10, 10 + 8 * vertical, "pitch %.2f", camera.pitch);

        gui_draw(&gui, camera.viewport.width, camera.viewport.height);

        player_draw(&player_pos, 45.0f, &player_vel, delta_time, &camera);

        glXSwapBuffers(window.display, window.handle);
    }
}
