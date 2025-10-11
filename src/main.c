#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <linmath.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <gl.h>
#include <chunk.h>
#include <world.h>
#include <camera.h>

static const char *vertex_shader_src =
    "#version 450 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec2 aTex;\n"
    "out vec2 vTex;\n"
    "uniform mat4 mvp;\n"
    "void main() {\n"
    "    vTex = aTex;\n"
    "    gl_Position = mvp * vec4(aPos, 1.0);\n"
    "}\n";

static const char *fragment_shader_src =
    "#version 450 core\n"
    "in vec2 vTex;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D atlas;\n"
    "void main() {\n"
    "    FragColor = texture(atlas, vTex);\n"
    "}\n";

typedef struct {
    int width;
    int height;
    mat4x4 projection_matrix;
} viewport_t;

void window_resize_cb(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    viewport_t *viewport = glfwGetWindowUserPointer(window);
    assert(viewport);

    if (viewport) {
        viewport->width  = width;
        viewport->height = height;

        float fov          = 70.f * M_PI / 180.f;
        float aspect_ratio = (float)viewport->width / (float)viewport->height;
        mat4x4_perspective(viewport->projection_matrix, fov, aspect_ratio,
                           0.1f, 10000.f);
    }
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

    viewport_t viewport;
    viewport.width  = 900;
    viewport.height = 600;

    GLFWwindow *window = glfwCreateWindow(viewport.width, viewport.height,
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

    glfwSetWindowUserPointer(window, &viewport);
    glfwSetFramebufferSizeCallback(window, window_resize_cb);

    /* Shaders. */

    GLuint shader_program =
        shader_program_new(vertex_shader_src, fragment_shader_src);

    GLint mvp_uniform_location = glGetUniformLocation(shader_program, "mvp");
    GLint texture_uniform_location =
        glGetUniformLocation(shader_program, "atlas");

    /* Texture atlas. */

    struct {
        uint8_t *data;
        int width;
        int height;
        int channels;
    } atlas;

    atlas.data = stbi_load("res/atlas.png", &atlas.width, &atlas.height,
                           &atlas.channels, 4);
    if (!atlas.data) {
        printf("Failed to load res/atlas.png: %s\n", stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, atlas.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* World. */

    world_t world;
    world_new(&world);

    /* Camera. */

    camera_t cam;
    camera_new(&cam);

    float fov          = 70.f * M_PI / 180.f;
    float aspect_ratio = (float)viewport.width / (float)viewport.height;
    mat4x4_perspective(viewport.projection_matrix, fov, aspect_ratio, 0.1f,
                       10000.f);

    float prev_timepoint = (float)glfwGetTime();

    /* Main loop. */

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(119.f / 255.f, 170.f / 255.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Update. */

        float timepoint  = (float)glfwGetTime();
        float delta_time = timepoint - prev_timepoint;
        prev_timepoint   = timepoint;

        camera_update(&cam, window, delta_time);

        world_update(&world, &cam);

        /* Bind & draw. */

        glUseProgram(shader_program);

        glUniform1i(texture_uniform_location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        /* Draw loaded chunks. */
        for (size_t x = 0; x < LOADED_CHUNKS_LEN; x++) {
            for (size_t y = 0; y < LOADED_CHUNKS_LEN; y++) {
                for (size_t z = 0; z < LOADED_CHUNKS_LEN; z++) {
                    /* Translate loaded chunks to world coordinates. */

                    coord_t world_chunk_coord = {
                        x + world.center_chunk_coord[0] - RENDER_DISTANCE,
                        y + world.center_chunk_coord[1] - RENDER_DISTANCE,
                        z + world.center_chunk_coord[2] - RENDER_DISTANCE};

                    mat4x4 model;
                    mat4x4_identity(model);
                    mat4x4_translate_in_place(
                        model, (float)(world_chunk_coord[0] * CHUNK_SIZE),
                        (float)(world_chunk_coord[1] * CHUNK_SIZE),
                        (float)(world_chunk_coord[2] * CHUNK_SIZE));

                    mat4x4 mvp;
                    mat4x4_mul(mvp, cam.view_matrix, model);
                    mat4x4_mul(mvp, viewport.projection_matrix, mvp);

                    glUniformMatrix4fv(mvp_uniform_location, 1, GL_FALSE,
                                       (const GLfloat *)mvp);

                    /* Draw. */

                    chunk_t *chunk = world.loaded_chunks[chunk_coord_to_index(
                        world_chunk_coord, world.center_chunk_coord)];

                    if (!chunk) {
                        continue;
                    }

                    glBindVertexArray(chunk->vertex_array);

                    glDrawElements(GL_TRIANGLES, chunk->num_indices,
                                   GL_UNSIGNED_INT, 0);
                }
            }
        }

        glfwSwapBuffers(window);
    }
}
