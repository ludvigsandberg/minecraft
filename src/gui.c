#include <darkcraft/gui.h>

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include <stb_image.h>

#include <darkcraft/gl.h>

static const float glyph_vertices[8] = {0.f, 0.f, 1.f, 0.f,
                                        1.f, 1.f, 0.f, 1.f};

static const float glyph_uvs[8] = {0.f,    0.f,    .0625f, 0.f,
                                   .0625f, .0625f, 0.f,    .0625f};

static const uint32_t glyph_indices[6] = {0, 1, 2, 2, 3, 0};

static const int glyph_widths[256] = {
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 6, 2, 8, //
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, // 0-9
    7, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6, 6, 6, 6, 6, // @, A-O
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, // P-Z
    3, 6, 6, 6, 6, 6, 5, 6, 6, 2, 6, 5, 3, 6, 6, 6, // ', a-o
    6, 6, 6, 6, 4, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, // p-z
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, //
};

void gui_new(gui_t *gui) {
    arr_new(gui->charset.glyphs);

    // load charset

    uint8_t *data =
        stbi_load("res/charset.png", &(int){0}, &(int){0}, NULL, 4);

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

    // load shaders

    gui->charset.shader_program =
        shader_program_new("res/charset_vs.glsl", "res/charset_fs.glsl");

    gui->charset.texture_uniform_loc =
        glGetUniformLocation(gui->charset.shader_program, "charset");

    // create buffers

    glGenVertexArrays(1, &gui->charset.vertex_array);
    glBindVertexArray(gui->charset.vertex_array);

    glGenBuffers(1, &gui->charset.vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, gui->charset.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, MAX_GLYPHS * 4 * 4 * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW); // pre allocate

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &gui->charset.element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->charset.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_GLYPHS * 6 * sizeof(uint32_t),
                 NULL,
                 GL_DYNAMIC_DRAW); // pre allocate
}

void gui_text(gui_t *gui, int x, int y, const char *fmt, ...) {
    // format

    va_list args;
    va_start(args, fmt);

    char buf[512];
    int len = vsnprintf(buf, sizeof buf, fmt, args);

    va_end(args);

    // store

    int cursor = x;

    for (int i = 0; i < len; i++) {
        char c = buf[i];

        glyph_t glyph;
        glyph.charset.x = c % 16;
        glyph.charset.y = c / 16;
        glyph.charset.w = glyph_widths[c];
        glyph.screen.x  = cursor;
        glyph.screen.y  = y;
        glyph.screen.w  = GLYPH_PIXELS * glyph.charset.w / 8;
        glyph.screen.h  = GLYPH_PIXELS;
        glyph.color[0]  = 1.f;
        glyph.color[1]  = 1.f;
        glyph.color[2]  = 1.f;

        arr_append(gui->charset.glyphs, glyph);

        cursor += glyph.screen.w;
    }
}

void gui_draw(gui_t *gui, int window_width, int window_height) {
    if (alen(gui->charset.glyphs) == 0) {
        return;
    }

    // batch glyphs

    arr(float) mesh_vertices;
    arr_new_reserve(mesh_vertices, 1024);

    arr(uint32_t) mesh_indices;
    arr_new_reserve(mesh_indices, 1024);

    arr_foreach(gui->charset.glyphs, glyph_idx) {
        glyph_t glyph = gui->charset.glyphs[glyph_idx];

        // append indices

        size_t num_vertices = alen(mesh_vertices) / 4;

        for (int i = 0; i < 6; i++) {
            uint32_t index = num_vertices + glyph_indices[i];

            arr_append(mesh_indices, index);
        }

        // append vertices

        float x0 = (glyph.screen.x * 2.0f / window_width) - 1.0f;
        float x1 =
            ((glyph.screen.x + glyph.screen.w) * 2.0f / window_width) - 1.0f;

        float y_top    = window_height - glyph.screen.y;
        float y_bottom = y_top - glyph.screen.h;
        float y0       = (y_top * 2.0f / window_height) - 1.0f;
        float y1       = (y_bottom * 2.0f / window_height) - 1.0f;

        float u0 = (glyph.charset.x * 8 + 0) / 128.0f;
        float u1 = (glyph.charset.x * 8 + glyph.charset.w) / 128.0f;
        float v0 = (glyph.charset.y * 8 + 0) / 128.0f;
        float v1 = (glyph.charset.y * 8 + 8) / 128.0f;

        float vertex_data[16] = {x0, y1, u0, v1, x1, y1, u1, v1,
                                 x1, y0, u1, v0, x0, y0, u0, v0};

        arr_append_n(mesh_vertices, 16, vertex_data);
    }

    // upload mesh to gpu

    glBindVertexArray(gui->charset.vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, gui->charset.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, alen(mesh_vertices) * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW); // buffer orphaning
    glBufferSubData(GL_ARRAY_BUFFER, 0, alen(mesh_vertices) * sizeof(float),
                    mesh_vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui->charset.element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 alen(mesh_indices) * sizeof(uint32_t), NULL,
                 GL_DYNAMIC_DRAW); // buffer orphaning
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    alen(mesh_indices) * sizeof(uint32_t), mesh_indices);

    // draw

    glUseProgram(gui->charset.shader_program);

    glUniform1i(gui->charset.texture_uniform_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gui->charset.texture);

    glBindVertexArray(gui->charset.vertex_array);

    glDrawElements(GL_TRIANGLES, alen(mesh_indices), GL_UNSIGNED_INT, 0);

    // cleanup

    arr_free(mesh_vertices);
    arr_free(mesh_indices);

    // reset
    alen(gui->charset.glyphs) = 0;
}