#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

#include "client/chunk.h"
#include "client/camera.h"

typedef struct renderer_s {
    GLuint terrain_texture;
    GLuint char_texture;

    GLuint sky_shader_program;
    GLuint sky_vertex_buffer;
    GLuint sky_vertex_array;

    GLuint chunk_shader_program;

    GLuint box_shader_program;
    GLuint box_vertex_buffer;
    GLuint box_element_buffer;
    GLuint box_vertex_array;

    struct {
        struct {
            GLint inv_view_matrix;
        } sky;

        struct {
            GLint texture;
            GLint mvp_matrix;
            GLint camera_pos;
        } chunk;

        struct {
            GLint uvs;
            GLint texture;
            GLint mvp_matrix;
            GLint camera_pos;
        } box;
    } uniform_locations;
} renderer_t;

void renderer_init(renderer_t *renderer);
void renderer_draw_sky(const renderer_t *renderer, const camera_t *camera);
void renderer_draw_chunk(const renderer_t *renderer, const chunk_t *chunk,
                         const camera_t *camera);
void renderer_draw_player(const renderer_t *renderer, const vec3_t *position,
                          const vec3_t *velocity, float head_yaw,
                          float head_pitch, const camera_t *camera);

#endif
