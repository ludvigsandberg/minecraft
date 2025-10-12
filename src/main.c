#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>

#include <gl.h>
#include <chunk.h>
#include <world.h>
#include <sky.h>
#include <camera.h>

void window_resize_cb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    camera_t *camera = glfwGetWindowUserPointer(window);
    assert(camera);

    camera_update_viewport(camera, width, height);
}

int main() {
    /* Window. */

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    camera_t camera;
    camera_new(&camera);

    GLFWwindow *window =
        glfwCreateWindow(camera.viewport.width, camera.viewport.height,
                         "Minecraft", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(opengl_debug_cb, NULL);
#endif

    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, window_resize_cb);

    /* World. */

    world_t world;
    world_new(&world);

    /* Sky. */

    sky_t sky;
    sky_new(&sky);

    /* Main loop. */

    float prev_timepoint = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(119.f / 255.f, 170.f / 255.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Update. */

        float timepoint  = (float)glfwGetTime();
        float delta_time = timepoint - prev_timepoint;
        prev_timepoint   = timepoint;

        camera_update(&camera, window, delta_time);

        world_update(&world, &camera);

        /* Draw. */

        sky_draw(&sky, &camera);

        world_draw(&world, &camera);

        glfwSwapBuffers(window);
    }
}
