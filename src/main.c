#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <linmath.h>

#include <minecraft/gl.h>
#include <minecraft/chunk.h>
#include <minecraft/world.h>
#include <minecraft/sky.h>
#include <minecraft/camera.h>
#include <minecraft/gui.h>

int main(int argc, char **argv) {
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

    camera_t camera;
    camera_new(&camera);

    SDL_Window *window = SDL_CreateWindow(
        "Minecraft", camera.viewport.width, camera.viewport.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        exit(EXIT_FAILURE);
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);

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
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_cb, NULL);
#endif

    world_t world;
    world_new(&world);

    sky_t sky;
    sky_new(&sky);

    gui_t gui;
    gui_new(&gui);

    float prev_timepoint = (float)SDL_GetTicks() / 1000.f;

    while (true) {
        // update

        SDL_Event event;
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

        float timepoint  = (float)SDL_GetTicks() / 1000.f;
        float delta_time = timepoint - prev_timepoint;
        prev_timepoint   = timepoint;

        camera_update(&camera, window, delta_time);

        world_update(&world, &camera);

        // draw

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sky_draw(&sky, &camera);

        world_draw(&world, &camera);

        gui_text(&gui, 10, 10, "Minecraft");
        gui_text(&gui, 10, 30, "Frame %.1fms", delta_time * 1000.f);

        gui_draw(&gui, camera.viewport.width, camera.viewport.height);

        SDL_GL_SwapWindow(window);
    }

end:;
}
