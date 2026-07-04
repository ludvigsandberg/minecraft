/*****************************************************************************
 * File:        gui.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_GUI_H
#define CLIENT_GUI_H

#include <glad/glad.h>

#include "client/arr_glyph.h"

#define MAX_GLYPHS   4096
#define GLYPH_PIXELS 12

typedef struct {
    struct {
        arr_glyph_t glyphs;

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
