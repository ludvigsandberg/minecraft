/*****************************************************************************
 * File:        main.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "client/gl.h"
#include "client/chunk.h"
#include "client/world.h"
#include "client/sky.h"
#include "client/camera.h"
#include "client/gui.h"

int main(int argc, char **argv) {
    camera_t camera;
    SDL_Window *window;
    SDL_GLContext context;
    world_t world;
    sky_t sky;
    gui_t gui;
    f32 prev_timestamp;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    camera_new(&camera);

    window = SDL_CreateWindow("minecraft", camera.viewport.width,
                              camera.viewport.height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        exit(EXIT_FAILURE);
    }

    context = SDL_GL_CreateContext(window);

    if (!context) {
        exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_callback, NULL);
#endif

    world_new(&world);
    sky_new(&sky);
    gui_new(&gui);

    prev_timestamp = (f32)SDL_GetTicks() / 1000.f;

    while (true) {
        SDL_Event event;
        f32 timestamp;
        f32 delta_time;
        coord_t center;
        f32 radius = 5;
        s64 x;
        s64 y;
        s64 z;
        int vertical = 16;

        /* Update. */

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    goto end;
                }

                case SDL_EVENT_WINDOW_RESIZED: {
                    glViewport(0, 0, event.window.data1, event.window.data2);
                    camera_update_viewport(&camera, event.window.data1,
                                           event.window.data2);
                    break;
                }
            }
        }

        timestamp      = (f32)SDL_GetTicks() / 1000.f;
        delta_time     = timestamp - prev_timestamp;
        prev_timestamp = timestamp;

        camera_update(&camera, window, delta_time);

        /* Carve out blocks around camera. */

#ifdef 0
        center = {{(s64)camera.pos.x, (s64)camera.pos.y, (s64)camera.pos.z}};

        for (x = center.x - radius; x <= center.x + radius; x++) {
            for (y = center.y - radius; y <= center.y + radius; y++) {
                for (z = center.z - radius; z <= center.z + radius; z++) {
                    coord_t diff;
                    f32 len;

                    diff.x = x - center.x;
                    diff.y = y - center.y;
                    diff.z = z - center.z;

                    len = sqrtf((f32)(diff.x * diff.x + diff.y * diff.y +
                                      diff.z * diff.z));

                    if (len <= radius) {
                        coord_t current = {{x, y, z}};

                        world_set_block(&world, &current, BLOCK_AIR);
                    }
                }
            }
        }
#endif

        world_update(&world, &camera);

        // draw

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sky_draw(&sky, &camera);

        world_draw(&world, &camera);

        gui_text(&gui, 10, 10, "minecraft");
        gui_text(&gui, 10, 10 + vertical, "frame %.1fms", delta_time * 1000.f);

        gui_text(&gui, 10, 10 + 3 * vertical, "x %.2f", VEC_X(camera.pos));
        gui_text(&gui, 10, 10 + 4 * vertical, "y %.2f", VEC_Y(camera.pos));
        gui_text(&gui, 10, 10 + 5 * vertical, "z %.2f", VEC_Z(camera.pos));

        gui_text(&gui, 10, 10 + 7 * vertical, "yaw %.2f", camera.yaw);
        gui_text(&gui, 10, 10 + 8 * vertical, "pitch %.2f", camera.pitch);

        gui_draw(&gui, camera.viewport.width, camera.viewport.height);

        SDL_GL_SwapWindow(window);
    }

end:;
}
