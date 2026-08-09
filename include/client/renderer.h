#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>

#include "array.h"
#include "client/chunk.h"
#include "client/camera.h"

struct glyph {
    struct {
        int x;
        int y;
        int w; /* Glyph width (1 to 8 pixels). */
    } charset;

    struct {
        int x;
        int y;
        int w;
        int h;
    } screen;

    struct vector3 color;
};

struct renderer {
    /* Textures. */

    GLuint terrain_texture;
    GLuint char_texture;
    GLuint charset_texture;

    /* Sky. */

    GLuint sky_shader_program;
    GLuint sky_vertex_buffer;
    GLuint sky_vertex_array;

    /* Chunk. */

    GLuint chunk_shader_program;

    /* Entity (box). */

    GLuint box_shader_program;
    GLuint box_vertex_buffer;
    GLuint box_element_buffer;
    GLuint box_vertex_array;

    /* GUI. */

    struct glyph *glyphs;
    struct array_info glyphs_info;

    GLuint charset_shader_program;

    GLuint charset_vertex_array;
    GLuint charset_vertex_buffer;
    GLuint charset_element_buffer;

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

        struct {
            GLint texture;
        } charset;
    } uniform_locations;
};

void renderer_init(struct renderer *renderer);

void renderer_draw_sky(const struct renderer *renderer,
                       const struct camera *camera);
void renderer_draw_chunk(const struct renderer *renderer,
                         const struct chunk *chunk,
                         const struct camera *camera);
void renderer_draw_world(const struct renderer *renderer, struct world *world,
                         struct camera *camera);
void renderer_draw_player(const struct renderer *renderer,
                          const struct vector3 *position,
                          const struct vector3 *velocity, float head_yaw,
                          float head_pitch, const struct camera *camera);

void renderer_gui_draw_text(struct renderer *renderer, int x, int y,
                            const char *fmt, ...);
void renderer_gui_flush(struct renderer *renderer,
                        const struct camera *camera);

#endif
