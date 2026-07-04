/*****************************************************************************
 * File:        gui.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "client/gui.h"

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <stb_image.h>

#include "client/arr_f32.h"
#include "client/arr_u32.h"
#include "client/gl.h"

static const f32 glyph_vertices[8] = {0.0f, 0.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f};

static const f32 glyph_uvs[8] = {0.0f,    0.0f,    0.0625f, 0.0f,
                                 0.0625f, 0.0625f, 0.0f,    0.0625f};

static const u32 glyph_indices[6] = {0, 1, 2, 2, 3, 0};

static const int glyph_widths[256] = {
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 6, 2, 8, /*                       */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, /* 0-9                   */
    7, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6, 6, 6, 6, 6, /* @, A-O                */
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, /* P-Z                   */
    3, 6, 6, 6, 6, 6, 5, 6, 6, 2, 6, 5, 3, 6, 6, 6, /* ', a-o                */
    6, 6, 6, 6, 4, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, /* p-z                   */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /*                       */
};

void gui_new(gui_t *gui) {
    u8 *data;
    int x = 0;
    int y = 0;

    arr_glyph_new(&gui->charset.glyphs);

    /* Load charset. */

    data = stbi_load("res/charset.png", &x, &y, NULL, 4);

    if (!data) {
        printf("Failed to load res/charset.png: %s\n", stbi_failure_reason());
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &gui->charset.texture);
    glBindTexture(GL_TEXTURE_2D, gui->charset.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Load shaders. */

    gui->charset.shader_program =
        shader_program_new("res/charset_vs.glsl", "res/charset_fs.glsl");

    gui->charset.texture_uniform_loc =
        glGetUniformLocation(gui->charset.shader_program, "charset");

    /* Create buffers. */

    glGenVertexArrays(1, &gui->charset.vertex_array);
    glBindVertexArray(gui->charset.vertex_array);

    glGenBuffers(1, &gui->charset.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, gui->charset.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_GLYPHS * 4 * 4 * sizeof(f32), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32),
                          (void *)(2 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &gui->charset.element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->charset.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_GLYPHS * 6 * sizeof(uint32_t),
                 NULL, GL_DYNAMIC_DRAW); /* Pre allocate. */
}

void gui_text(gui_t *gui, int x, int y, const char *fmt, ...) {
    va_list args;
    char buf[512];
    int len;
    int cursor;
    int i;

    /* Format. */

    va_start(args, fmt);

    len = vsnprintf(buf, sizeof buf, fmt, args);

    va_end(args);

    /* Store. */

    cursor = x;

    for (i = 0; i < len; i++) {
        char c = buf[i];

        glyph_t glyph;
        glyph.charset.x    = c % 16;
        glyph.charset.y    = c / 16;
        glyph.charset.w    = glyph_widths[c];
        glyph.screen.x     = cursor;
        glyph.screen.y     = y;
        glyph.screen.w     = GLYPH_PIXELS * glyph.charset.w / 8;
        glyph.screen.h     = GLYPH_PIXELS;
        VEC_R(glyph.color) = 1.0f;
        VEC_G(glyph.color) = 1.0f;
        VEC_B(glyph.color) = 1.0f;

        arr_glyph_append(&gui->charset.glyphs, glyph);

        cursor += glyph.screen.w;
    }
}

void gui_draw(gui_t *gui, int window_width, int window_height) {
    arr_f32_t mesh_vertices;
    arr_u32_t mesh_indices;
    size_t glyph_idx;
    int i;

    if (gui->charset.glyphs.size == 0) {
        return;
    }

    /* Batch glyphs. */

    arr_f32_new_reserve(&mesh_vertices, 1024);
    arr_u32_new_reserve(&mesh_indices, 1024);

    for (glyph_idx = 0; glyph_idx < gui->charset.glyphs.size; glyph_idx++) {
        glyph_t glyph = ARR_AT(gui->charset.glyphs, glyph_idx);

        /* Append indices. */

        size_t num_vertices = mesh_vertices.size / 4;

        for (i = 0; i < 6; i++) {
            u32 index = num_vertices + glyph_indices[i];

            arr_u32_append(&mesh_indices, index);
        }

        /* Append vertices. */

        f32 x0 = (glyph.screen.x * 2.0f / window_width) - 1.0f;
        f32 x1 =
            ((glyph.screen.x + glyph.screen.w) * 2.0f / window_width) - 1.0f;

        f32 y_top    = window_height - glyph.screen.y;
        f32 y_bottom = y_top - glyph.screen.h;
        f32 y0       = (y_top * 2.0f / window_height) - 1.0f;
        f32 y1       = (y_bottom * 2.0f / window_height) - 1.0f;

        f32 u0 = (glyph.charset.x * 8 + 0) / 128.0f;
        f32 u1 = (glyph.charset.x * 8 + glyph.charset.w) / 128.0f;
        f32 v0 = (glyph.charset.y * 8 + 0) / 128.0f;
        f32 v1 = (glyph.charset.y * 8 + 8) / 128.0f;

        f32 vertex_data[16] = {x0, y1, u0, v1, x1, y1, u1, v1,
                               x1, y0, u1, v0, x0, y0, u0, v0};

        arr_f32_append_n(&mesh_vertices, 16, vertex_data);
    }

    /* Upload mesh to GPU. */

    glBindVertexArray(gui->charset.vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, gui->charset.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_vertices.size * sizeof(f32), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh_vertices.size * sizeof(f32),
                    mesh_vertices.data);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->charset.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_indices.size * sizeof(uint32_t),
                 NULL, GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    mesh_indices.size * sizeof(uint32_t), mesh_indices.data);

    /* Draw. */

    glUseProgram(gui->charset.shader_program);

    glUniform1i(gui->charset.texture_uniform_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui->charset.texture);

    glBindVertexArray(gui->charset.vertex_array);

    glDrawElements(GL_TRIANGLES, mesh_indices.size, GL_UNSIGNED_INT, 0);

    /* Cleanup. */

    arr_f32_free(&mesh_vertices);
    arr_u32_free(&mesh_indices);

    /* Reset. */
    gui->charset.glyphs.size = 0;
}