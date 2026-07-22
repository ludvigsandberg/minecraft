#include "client/gui.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "client/opengl.h"
#include "client/file_io.h"

#define GLYPH_PIXELS 12

/*static const float glyph_vertices[8] = {0.0f, 0.0f, 1.0f, 0.0f,
                                      1.0f, 1.0f, 0.0f, 1.0f};

static const float glyph_uvs[8] = {0.0f,    0.0f,    0.0625f, 0.0f,
                                 0.0625f, 0.0625f, 0.0f,    0.0625f};*/

static const unsigned int glyph_indices[6] = {0, 1, 2, 2, 3, 0};

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

void gui_init(gui_t *gui) {
    unsigned char *data;
    int x = 0;
    int y = 0;

    assert(gui);

    gui->glyph_count = 0;

    /* Load charset. */

    data = load_bmp("res/charset.bmp", &x, &y);

    if (!data) {
        printf("Failed to load %s\r\n", "res/charset.bmp");
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &gui->charset_texture);
    glBindTexture(GL_TEXTURE_2D, gui->charset_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 128, 128, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* Load shaders. */

    gui->shader_program =
        opengl_shader_program("res/charset_vs.glsl", "res/charset_fs.glsl");

    gui->charset_texture_uniform_loc =
        glGetUniformLocation(gui->shader_program, "charset");

    /* Create buffers. */

    glGenVertexArrays(1, &gui->vertex_array);
    glBindVertexArray(gui->vertex_array);

    glGenBuffers(1, &gui->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, gui->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_GLYPHS * 4 * 4 * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &gui->element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 MAX_GLYPHS * 6 * sizeof(unsigned int), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */
}

void gui_text(gui_t *gui, int x, int y, const char *fmt, ...) {
    va_list args;
    char buf[512];
    int len;
    int cursor;
    int i;

    /* Format. */

    va_start(args, fmt);

    len = vsprintf(buf, fmt, args);

    va_end(args);

    /* Store. */

    cursor = x;

    for (i = 0; i < len; i++) {
        glyph_t glyph;
        char c = buf[i];

        if (gui->glyph_count == MAX_GLYPHS) {
            break;
        }

        glyph.charset.x   = c % 16;
        glyph.charset.y   = c / 16;
        glyph.charset.w   = glyph_widths[(size_t)c];
        glyph.screen.x    = cursor;
        glyph.screen.y    = y;
        glyph.screen.w    = GLYPH_PIXELS * glyph.charset.w / 8;
        glyph.screen.h    = GLYPH_PIXELS;
        glyph.color.VEC_R = 1.0f;
        glyph.color.VEC_G = 1.0f;
        glyph.color.VEC_B = 1.0f;

        gui->glyphs[gui->glyph_count++] = glyph;

        cursor += glyph.screen.w;
    }
}

void gui_flush(gui_t *gui, const camera_t *camera) {
    float mesh_vertices[MAX_GLYPHS * 16];
    size_t mesh_vertex_count = 0;

    unsigned int mesh_indices[MAX_GLYPHS * 6];
    size_t mesh_index_count = 0;

    size_t glyph_idx;
    int i;

    assert(gui);
    assert(camera);

    if (gui->glyph_count == 0) {
        return;
    }

    /* Batch glyphs. */

    for (glyph_idx = 0; glyph_idx < gui->glyph_count; glyph_idx++) {
        glyph_t glyph;
        size_t num_vertices;
        float x0;
        float x1;
        float y_top;
        float y_bottom;
        float y0;
        float y1;
        float u0;
        float u1;
        float v0;
        float v1;

        glyph = gui->glyphs[glyph_idx];

        /* Append indices. */

        num_vertices = mesh_vertex_count / 4;

        for (i = 0; i < 6; i++) {
            unsigned int index = (unsigned int)num_vertices + glyph_indices[i];

            mesh_indices[mesh_index_count++] = index;
        }

        /* Append vertices. */

        x0 = ((float)glyph.screen.x * 2.0f / (float)camera->viewport.width) -
             1.0f;
        x1 = ((float)(glyph.screen.x + glyph.screen.w) * 2.0f /
              (float)camera->viewport.width) -
             1.0f;
        y_top    = (float)(camera->viewport.height - glyph.screen.y);
        y_bottom = y_top - (float)glyph.screen.h;
        y0       = (y_top * 2.0f / (float)camera->viewport.height) - 1.0f;
        y1       = (y_bottom * 2.0f / (float)camera->viewport.height) - 1.0f;
        u0       = (float)(glyph.charset.x * 8 + 0) / 128.0f;
        u1       = (float)(glyph.charset.x * 8 + glyph.charset.w) / 128.0f;
        v0       = (float)(glyph.charset.y * 8 + 0) / 128.0f;
        v1       = (float)(glyph.charset.y * 8 + 8) / 128.0f;

        (mesh_vertices + mesh_vertex_count)[0]  = x0;
        (mesh_vertices + mesh_vertex_count)[1]  = y1;
        (mesh_vertices + mesh_vertex_count)[2]  = u0;
        (mesh_vertices + mesh_vertex_count)[3]  = v1;
        (mesh_vertices + mesh_vertex_count)[4]  = x1;
        (mesh_vertices + mesh_vertex_count)[5]  = y1;
        (mesh_vertices + mesh_vertex_count)[6]  = u1;
        (mesh_vertices + mesh_vertex_count)[7]  = v1;
        (mesh_vertices + mesh_vertex_count)[8]  = x1;
        (mesh_vertices + mesh_vertex_count)[9]  = y0;
        (mesh_vertices + mesh_vertex_count)[10] = u1;
        (mesh_vertices + mesh_vertex_count)[11] = v0;
        (mesh_vertices + mesh_vertex_count)[12] = x0;
        (mesh_vertices + mesh_vertex_count)[13] = y0;
        (mesh_vertices + mesh_vertex_count)[14] = u0;
        (mesh_vertices + mesh_vertex_count)[15] = v0;

        mesh_vertex_count += 16;
    }

    /* Upload mesh to GPU. */

    glBindVertexArray(gui->vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, gui->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh_vertex_count * sizeof(float)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_vertex_count * sizeof(float)),
                    mesh_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(mesh_index_count * sizeof(unsigned int)), NULL,
                 GL_DYNAMIC_DRAW); /* Buffer orphaning. */
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(mesh_index_count * sizeof(unsigned int)),
                    mesh_indices);

    /* Draw. */

    glUseProgram(gui->shader_program);

    glUniform1i(gui->charset_texture_uniform_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui->charset_texture);

    glBindVertexArray(gui->vertex_array);

    glDrawElements(GL_TRIANGLES, (GLsizei)mesh_index_count, GL_UNSIGNED_INT,
                   0);

    /* Reset. */
    gui->glyph_count = 0;
}