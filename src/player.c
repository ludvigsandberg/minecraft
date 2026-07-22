#include "client/player.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "macros.h"
#include "client/opengl.h"
#include "client/file_io.h"
#include "client/world.h"
#include "client/camera.h"

/* Limb definition */
typedef struct {
    vec3_t offset;
    vec3_t size;
    vec2_t uv_offset; /* top-left pixel */
    vec2_t uv_size;   /* pixels */
} limb_t;

static limb_t steve_limbs[6] = {
    {{{0.0f, 1.625f, 0.0f}},
     {{0.5f, 0.5f, 0.5f}},
     {{8.0f, 0.0f}},
     {{8.0f, 8.0f}}}, /* Head */
    {{{0.0f, 0.875f, 0.0f}},
     {{0.5f, 0.75f, 0.25f}},
     {{20.0f, 16.0f}},
     {{8.0f, 12.0f}}}, /* Body */
    {{{0.375f, 1.0f, 0.0f}},
     {{0.25f, 0.75f, 0.25f}},
     {{44.0f, 16.0f}},
     {{4.0f, 12.0f}}}, /* Right Arm */
    {{{-0.375f, 1.0f, 0.0f}},
     {{0.25f, 0.75f, 0.25f}},
     {{36.0f, 48.0f}},
     {{4.0f, 12.0f}}}, /* Left Arm */
    {{{0.125f, 0.0f, 0.0f}},
     {{0.25f, 0.75f, 0.25f}},
     {{4.0f, 16.0f}},
     {{4.0f, 12.0f}}}, /* Right Leg */
    {{{-0.125f, 0.0f, 0.0f}},
     {{0.25f, 0.75f, 0.25f}},
     {{20.0f, 48.0f}},
     {{4.0f, 12.0f}}} /* Left Leg */
};

static GLuint player_shader = 0;
static GLuint steve_texture = 0;
static GLuint limb_vao      = 0;
static GLuint limb_vbo      = 0;
static GLuint limb_ebo      = 0;

static struct {
    GLint atlas;
    GLint model;
    GLint view;
    GLint projection;
    GLint cameraPos;
} uniforms;

static float walk_cycle = 0.0f;

/* Cube data (from chunk.c) */
static const float cube_positions[72] = {
    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, 0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
    0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f};

static const float cube_uvs[48] = {
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

static const float cube_shadows[6] = {0.90f, 0.60f, 0.50f,
                                      0.70f, 1.00f, 0.55f};

void player_new(void) {
    unsigned char *data;
    int w, h;

    /* Shader */
    player_shader =
        shader_program_new("res/chunk_vs.glsl", "res/chunk_fs.glsl");

    uniforms.atlas      = glGetUniformLocation(player_shader, "atlas");
    uniforms.model      = glGetUniformLocation(player_shader, "model");
    uniforms.view       = glGetUniformLocation(player_shader, "view");
    uniforms.projection = glGetUniformLocation(player_shader, "projection");
    uniforms.cameraPos  = glGetUniformLocation(player_shader, "cameraPos");

    /* Texture */
    bitmap_load(&data, &w, &h, "res/char.bmp");

    glGenTextures(1, &steve_texture);
    glBindTexture(GL_TEXTURE_2D, steve_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(data);

    /* VAO */
    glGenVertexArrays(1, &limb_vao);
    glBindVertexArray(limb_vao);

    glGenBuffers(1, &limb_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, limb_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * 7 * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &limb_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, limb_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 6 * sizeof(unsigned int), NULL,
                 GL_DYNAMIC_DRAW);

    walk_cycle = 0.0f;
}

static void upload_limb_mesh(const limb_t *limb) {
    float vertices[6 * 4 * 7];
    unsigned int indices[36];
    size_t vcount = 0;
    size_t icount = 0;
    int face;

    for (face = 0; face < 6; face++) {
        int i;
        unsigned int base        = (unsigned int)(vcount / 7);
        unsigned int face_idx[6] = {0, 1, 2, 2, 3, 0};

        for (i = 0; i < 6; i++) {
            indices[icount++] = base + face_idx[i];
        }

        for (i = 0; i < 4; i++) {
            int p = face * 12 + i * 3;
            int t = face * 8 + i * 2;
            float u;
            float v;

            vertices[vcount++] = cube_positions[p + 0];
            vertices[vcount++] = cube_positions[p + 1];
            vertices[vcount++] = cube_positions[p + 2];

            u = (cube_uvs[t + 0] * limb->uv_size.VEC_U +
                 limb->uv_offset.VEC_U) /
                64.0f;
            v = (cube_uvs[t + 1] * limb->uv_size.VEC_V +
                 limb->uv_offset.VEC_V) /
                32.0f;

            vertices[vcount++] = u;
            vertices[vcount++] = v;

            vertices[vcount++] = cube_shadows[face];
            vertices[vcount++] = 16.0f;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, limb_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(vcount * sizeof(float)),
                    vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, limb_ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    (GLsizeiptr)(icount * sizeof(unsigned int)), indices);
}

static void draw_limb(int limb_idx, const mat4x4_t *base_model, float rx,
                      float rz, const camera_t *cam) {
    limb_t *limb   = &steve_limbs[limb_idx];
    mat4x4_t model = *base_model;

    upload_limb_mesh(limb);

    /* Translate -> Rotate -> Scale */
    mat4x4_translate(&model, &model, &limb->offset);
    if (rx != 0.0f)
        mat4x4_rotate_x(&model, &model, rx * ((float)M_PI / 180.0f));
    if (rz != 0.0f)
        mat4x4_rotate_z(&model, &model, rz * ((float)M_PI / 180.0f));
    mat4x4_scale(&model, &model, &limb->size);

    glUniformMatrix4fv(uniforms.model, 1, GL_FALSE,
                       (const GLfloat *)model.elems);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void player_draw(const vec3_t *position, float yaw, const vec3_t *velocity,
                 float dt, const camera_t *camera) {
    float speed;
    float leg_angle;
    float arm_angle;
    mat4x4_t base_model = MAT4X4_IDENTITY;
    int i;

    if (!player_shader)
        return;

    speed = vec3_len(velocity);
    if (speed > 0.05f) {
        walk_cycle += dt * 13.0f;
    }
    leg_angle = sinf(walk_cycle) * 35.0f;
    arm_angle = sinf(walk_cycle * 1.1f) * 45.0f;

    glUseProgram(player_shader);
    glUniform1i(uniforms.atlas, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, steve_texture);

    glUniform3fv(uniforms.cameraPos, 1, (const GLfloat *)camera->pos.elems);
    glUniformMatrix4fv(uniforms.view, 1, GL_FALSE,
                       (const GLfloat *)camera->view_matrix.elems);
    glUniformMatrix4fv(
        uniforms.projection, 1, GL_FALSE,
        (const GLfloat *)camera->viewport.projection_matrix.elems);

    mat4x4_translate(&base_model, &base_model, position);
    mat4x4_rotate_y(&base_model, &base_model, yaw * ((float)M_PI / 180.0f));

    glBindVertexArray(limb_vao);

    for (i = 0; i < 6; i++) {
        float rx = 0.0f, rz = 0.0f;
        if (i == 2)
            rx = -arm_angle;
        if (i == 3)
            rx = arm_angle;
        if (i == 4)
            rx = leg_angle;
        if (i == 5)
            rx = -leg_angle;

        upload_limb_mesh(&steve_limbs[i]);
        draw_limb(i, &base_model, rx, rz, camera);
    }
}

void player_free(void) {
    if (player_shader)
        glDeleteProgram(player_shader);
    if (steve_texture)
        glDeleteTextures(1, &steve_texture);
    if (limb_vao)
        glDeleteVertexArrays(1, &limb_vao);
    if (limb_vbo)
        glDeleteBuffers(1, &limb_vbo);
    if (limb_ebo)
        glDeleteBuffers(1, &limb_ebo);
}