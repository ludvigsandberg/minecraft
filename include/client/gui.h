#ifndef GUI_H
#define GUI_H

#include <stddef.h>

#include <glad/glad.h>

#include "vector.h"
#include "client/camera.h"

#define MAX_GLYPHS 1024

typedef struct {
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

    vec3_t color;
} glyph_t;

typedef struct {
    glyph_t glyphs[MAX_GLYPHS];
    size_t glyph_count;

    GLuint charset_texture;
    GLint charset_texture_uniform_loc;

    GLuint shader_program;

    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint element_buffer;
} gui_t;

void gui_init(gui_t *gui);
/* Add text to internal buffer. Call gui_flush() to draw buffer. */
void gui_text(gui_t *gui, int x, int y, const char *fmt, ...);
void gui_flush(gui_t *gui, const camera_t *camera);

#endif
