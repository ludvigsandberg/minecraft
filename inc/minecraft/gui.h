#ifndef MINECRAFT_GUI_H
#define MINECRAFT_GUI_H

#include <stdint.h>

#include <glad/glad.h>
#include <linmath.h>
#include <meta.h>

#define MAX_GLYPHS   4096
#define GLYPH_PIXELS 12

typedef struct {
    struct {
        int x;
        int y;
        int w; // glyph width 1-8px
    } charset;

    struct {
        int x;
        int y;
        int w;
        int h;
    } screen;

    vec3 color;
} glyph_t;

typedef struct {
    struct {
        arr(glyph_t) glyphs;

        GLuint texture;
        GLuint texture_uniform_loc;

        GLuint shader_program;

        GLuint vertex_array;
        GLuint vertex_buffer;
        GLuint element_buffer;
    } charset;
} gui_t;

void gui_new(gui_t *gui);
void gui_text(gui_t *gui, int x, int y, const char *fmt, ...);
void gui_draw(gui_t *gui, int window_width, int window_height);

#endif
