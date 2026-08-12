#include "client/renderer.h"

#include <stdlib.h>
#include <varargs.h>
#include <assert.h>
#include <math.h>

#include "matrix.h"
#include "client/opengl.h"

#define GLYPH_PIXELS 12

static const float sky_vertices[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
                                     -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f};

static const float box_vertices[72] = {
    -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,
    -0.5f, 0.5f,  0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, 0.5f,  -0.5f,
    0.5f,  -0.5f, 0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  0.5f,  -0.5f,
    0.5f,  0.5f,  0.5f,  -0.5f, 0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, -0.5f, -0.5f, -0.5f,
    0.5f,  -0.5f, -0.5f, 0.5f,  -0.5f, 0.5f,  -0.5f, -0.5f, 0.5f};

static const unsigned int box_indices[36] = {
    0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
    12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

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

void renderer_init(struct renderer *renderer) {
    /* Textures. */

    renderer->terrain_texture =
        opengl_texture_raw("res/terrain.raw", 256, 256);
    renderer->char_texture = opengl_texture_raw("res/char.raw", 64, 32);
    renderer->charset_texture =
        opengl_texture_raw("res/default.raw", 128, 128);

    /* Sky. */

    renderer->sky_shader_program =
        opengl_shader_program("res/sky_vs.glsl", "res/sky_fs.glsl");

    renderer->uniform_locations.sky.inv_view_matrix =
        glGetUniformLocation(renderer->sky_shader_program, "invView");

    glGenVertexArrays(1, &renderer->sky_vertex_array);
    glBindVertexArray(renderer->sky_vertex_array);

    glGenBuffers(1, &renderer->sky_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->sky_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof sky_vertices, sky_vertices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);

    /* Chunk. */

    renderer->chunk_shader_program =
        opengl_shader_program("res/chunk_vs.glsl", "res/chunk_fs.glsl");

    renderer->uniform_locations.chunk.texture =
        glGetUniformLocation(renderer->chunk_shader_program, "atlas");
    renderer->uniform_locations.chunk.mvp_matrix =
        glGetUniformLocation(renderer->chunk_shader_program, "mvp");
    renderer->uniform_locations.chunk.camera_pos =
        glGetUniformLocation(renderer->chunk_shader_program, "cameraPos");

    /* Box. */

    renderer->box_shader_program =
        opengl_shader_program("res/box_vs.glsl", "res/box_fs.glsl");

    renderer->uniform_locations.box.uvs =
        glGetUniformLocation(renderer->box_shader_program, "uvs");
    renderer->uniform_locations.box.texture =
        glGetUniformLocation(renderer->box_shader_program, "atlas");
    renderer->uniform_locations.box.mvp_matrix =
        glGetUniformLocation(renderer->box_shader_program, "mvp");
    renderer->uniform_locations.box.camera_pos =
        glGetUniformLocation(renderer->box_shader_program, "cameraPos");

    glGenVertexArrays(1, &renderer->box_vertex_array);
    glBindVertexArray(renderer->box_vertex_array);

    glGenBuffers(1, &renderer->box_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->box_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &renderer->box_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->box_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(box_indices), box_indices,
                 GL_STATIC_DRAW);

    /* GUI. */

    renderer->glyphs = malloc(1024 * sizeof(struct glyph));
    if (!renderer->glyphs) {
        printf("%s:%d Out of memory!\r\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    renderer->glyphs_info.size = 0;
    renderer->glyphs_info.cap  = 1024;

    renderer->charset_shader_program =
        opengl_shader_program("res/charset_vs.glsl", "res/charset_fs.glsl");

    renderer->uniform_locations.charset.texture =
        glGetUniformLocation(renderer->charset_shader_program, "charset");

    /* Create buffers. */

    glGenVertexArrays(1, &renderer->charset_vertex_array);
    glBindVertexArray(renderer->charset_vertex_array);

    glGenBuffers(1, &renderer->charset_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->charset_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 1024 * 4 * 4 * sizeof(float), NULL,
                 GL_DYNAMIC_DRAW); /* Pre allocate. */

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &renderer->charset_element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->charset_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1024 * 6 * sizeof(unsigned int),
                 NULL, GL_DYNAMIC_DRAW); /* Pre allocate. */
}

void renderer_draw_sky(const struct renderer *renderer,
                       const struct camera *camera) {
    struct matrix4 inv_view_matrix;

    assert(renderer);
    assert(camera);

    glDepthMask(GL_FALSE);

    glUseProgram(renderer->sky_shader_program);

    inv_view_matrix = invert_view(&camera->view_matrix);
    glUniformMatrix4fv(renderer->uniform_locations.sky.inv_view_matrix, 1,
                       GL_FALSE, (const GLfloat *)inv_view_matrix.elems);

    glBindVertexArray(renderer->sky_vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthMask(GL_TRUE);
}

void renderer_draw_chunk(const struct renderer *renderer,
                         const struct chunk *chunk,
                         const struct camera *camera) {
    struct matrix4 mvp_matrix;

    assert(renderer);
    assert(chunk);
    assert(camera);

    glUseProgram(renderer->chunk_shader_program);

    glUniform1i(renderer->uniform_locations.chunk.texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->terrain_texture);

    glUniform3fv(renderer->uniform_locations.chunk.camera_pos, 1,
                 (const GLfloat *)camera->pos.elems);

    mvp_matrix =
        matrix4_mul(&camera->viewport.projection_matrix, &camera->view_matrix);

    glUniformMatrix4fv(renderer->uniform_locations.chunk.mvp_matrix, 1,
                       GL_FALSE, (const GLfloat *)mvp_matrix.elems);

    glBindVertexArray(chunk->vertex_array);

    glDrawElements(GL_TRIANGLES, (GLsizei)chunk->index_count, GL_UNSIGNED_INT,
                   0);
}

void renderer_draw_world(const struct renderer *renderer, struct world *world,
                         struct camera *camera) {
    int i;

    for (i = 0; i < LOADED_CHUNKS_TOTAL; i++) {
        if (world->loaded_chunks[i]) {
            renderer_draw_chunk(renderer, world->loaded_chunks[i], camera);
        }
    }
}

static void draw_box(const struct renderer *renderer, const float *uvs,
                     const struct matrix4 *model_matrix,
                     const struct camera *camera) {
    struct matrix4 mvp_matrix;

    glUniform2fv(renderer->uniform_locations.box.uvs, 24,
                 (const GLfloat *)uvs);

    mvp_matrix =
        matrix4_mul(&camera->viewport.projection_matrix, &camera->view_matrix);
    mvp_matrix = matrix4_mul(&mvp_matrix, model_matrix);

    glUniformMatrix4fv(renderer->uniform_locations.box.mvp_matrix, 1, GL_FALSE,
                       (const GLfloat *)mvp_matrix.elems);

    glBindVertexArray(renderer->box_vertex_array);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

#define PLAYER_UV(X, Y, WIDTH, HEIGHT)                                        \
    (X) / 64.0f, ((Y) + (HEIGHT)) / 32.0f, ((X) + (WIDTH)) / 64.0f,           \
        ((Y) + (HEIGHT)) / 32.0f, ((X) + (WIDTH)) / 64.0f, (Y) / 32.0f,       \
        (X) / 64.0f, (Y) / 32.0f

static const float player_head_uvs[] = {
    PLAYER_UV(8.0f, 8.0f, 8.0f, 8.0f), PLAYER_UV(24.0f, 8.0f, 8.0f, 8.0f),
    PLAYER_UV(0.0f, 8.0f, 8.0f, 8.0f), PLAYER_UV(16.0f, 8.0f, 8.0f, 8.0f),
    PLAYER_UV(8.0f, 0.0f, 8.0f, 8.0f), PLAYER_UV(16.0f, 0.0f, 8.0f, 8.0f)};

static const float player_torso_uvs[48] = {
    PLAYER_UV(20.0f, 20.0f, 8.0f, 12.0f), PLAYER_UV(32.0f, 20.0f, 8.0f, 12.0f),
    PLAYER_UV(16.0f, 20.0f, 4.0f, 12.0f), PLAYER_UV(28.0f, 20.0f, 4.0f, 12.0f),
    PLAYER_UV(20.0f, 16.0f, 8.0f, 4.0f),  PLAYER_UV(28.0f, 16.0f, 8.0f, 4.0f)};

static const float player_right_arm_uvs[48] = {
    PLAYER_UV(44.0f, 20.0f, 4.0f, 12.0f), PLAYER_UV(52.0f, 20.0f, 4.0f, 12.0f),
    PLAYER_UV(40.0f, 20.0f, 4.0f, 12.0f), PLAYER_UV(48.0f, 20.0f, 4.0f, 12.0f),
    PLAYER_UV(44.0f, 16.0f, 4.0f, 4.0f),  PLAYER_UV(48.0f, 16.0f, 4.0f, 4.0f)};

static const float player_left_arm_uvs[48] = {
    PLAYER_UV(48.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(56.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(52.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(44.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(48.0f, 16.0f, -4.0f, 4.0f),
    PLAYER_UV(52.0f, 16.0f, -4.0f, 4.0f)};

static const float player_right_leg_uvs[48] = {
    PLAYER_UV(4.0f, 20.0f, 4.0f, 12.0f), PLAYER_UV(12.0f, 20.0f, 4.0f, 12.0f),
    PLAYER_UV(0.0f, 20.0f, 4.0f, 12.0f), PLAYER_UV(8.0f, 20.0f, 4.0f, 12.0f),
    PLAYER_UV(4.0f, 16.0f, 4.0f, 4.0f),  PLAYER_UV(8.0f, 16.0f, 4.0f, 4.0f)};

static const float player_left_leg_uvs[48] = {
    PLAYER_UV(8.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(16.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(12.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(4.0f, 20.0f, -4.0f, 12.0f),
    PLAYER_UV(8.0f, 16.0f, -4.0f, 4.0f),
    PLAYER_UV(12.0f, 16.0f, -4.0f, 4.0f)};

#ifndef DEG2RAD
#define DEG2RAD(deg) ((deg) * (3.14159265358979323846f / 180.0f))
#endif

void renderer_draw_player(const struct renderer *renderer,
                          const struct vector3 *position,
                          const struct vector3 *velocity, float head_yaw,
                          float head_pitch, const struct camera *camera) {
    struct matrix4 head_matrix      = MATRIX4_IDENTITY;
    struct matrix4 torso_matrix     = MATRIX4_IDENTITY;
    struct matrix4 right_arm_matrix = MATRIX4_IDENTITY;
    struct matrix4 left_arm_matrix  = MATRIX4_IDENTITY;
    struct matrix4 right_leg_matrix = MATRIX4_IDENTITY;
    struct matrix4 left_leg_matrix  = MATRIX4_IDENTITY;

    struct matrix4 body_rot      = MATRIX4_IDENTITY;
    struct matrix4 head_rot      = MATRIX4_IDENTITY;
    struct matrix4 right_arm_rot = MATRIX4_IDENTITY;
    struct matrix4 left_arm_rot  = MATRIX4_IDENTITY;
    struct matrix4 right_leg_rot = MATRIX4_IDENTITY;
    struct matrix4 left_leg_rot  = MATRIX4_IDENTITY;

    struct vector3 head_pos;
    struct vector3 torso_pos;
    struct vector3 right_arm_pos;
    struct vector3 left_arm_pos;
    struct vector3 right_leg_pos;
    struct vector3 left_leg_pos;

    struct vector3 head_scale;
    struct vector3 torso_scale;
    struct vector3 limb_scale;

    struct vector3 pivot_top;
    struct vector3 pivot_top_neg;
    struct vector3 pivot_hip;
    struct vector3 pivot_hip_neg;

    float body_yaw;
    float speed;
    float walk_phase;
    float swing_angle;
    float is_moving;

    assert(renderer);
    assert(position);
    assert(velocity);
    assert(camera);

    glUseProgram(renderer->box_shader_program);

    glUniform1i(renderer->uniform_locations.box.texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->char_texture);

    glUniform3fv(renderer->uniform_locations.box.camera_pos, 1,
                 (const GLfloat *)camera->pos.elems);

    speed = (float)sqrt((double)velocity->VEC_X * (double)velocity->VEC_X +
                        (double)velocity->VEC_Z * (double)velocity->VEC_Z);

    if (speed > 0.01f) {
        body_yaw =
            (float)atan2((double)-velocity->VEC_X, (double)-velocity->VEC_Z);
        is_moving = 1.0f;
    } else {
        body_yaw  = DEG2RAD(head_yaw);
        is_moving = 0.0f;
    }

    /* Animate limb swing based on elapsed time when player is moving */
    walk_phase  = (float)(elapsed_time_seconds() * 100.0);
    swing_angle = (float)sin((double)walk_phase) * 0.6662f * is_moving;

    head_scale.VEC_X = 0.5f;
    head_scale.VEC_Y = 0.5f;
    head_scale.VEC_Z = 0.5f;

    torso_scale.VEC_X = 0.5f;
    torso_scale.VEC_Y = 0.75f;
    torso_scale.VEC_Z = 0.25f;

    limb_scale.VEC_X = 0.25f;
    limb_scale.VEC_Y = 0.75f;
    limb_scale.VEC_Z = 0.25f;

    pivot_top.VEC_X = 0.0f;
    pivot_top.VEC_Y = 0.375f;
    pivot_top.VEC_Z = 0.0f;

    pivot_top_neg.VEC_X = 0.0f;
    pivot_top_neg.VEC_Y = -0.375f;
    pivot_top_neg.VEC_Z = 0.0f;

    pivot_hip.VEC_X = 0.0f;
    pivot_hip.VEC_Y = 0.375f;
    pivot_hip.VEC_Z = 0.0f;

    pivot_hip_neg.VEC_X = 0.0f;
    pivot_hip_neg.VEC_Y = -0.375f;
    pivot_hip_neg.VEC_Z = 0.0f;

    right_leg_pos.VEC_X = -0.125f;
    right_leg_pos.VEC_Y = 0.375f;
    right_leg_pos.VEC_Z = 0.0f;

    left_leg_pos.VEC_X = 0.125f;
    left_leg_pos.VEC_Y = 0.375f;
    left_leg_pos.VEC_Z = 0.0f;

    torso_pos.VEC_X = 0.0f;
    torso_pos.VEC_Y = 1.125f;
    torso_pos.VEC_Z = 0.0f;

    right_arm_pos.VEC_X = -0.375f;
    right_arm_pos.VEC_Y = 1.125f;
    right_arm_pos.VEC_Z = 0.0f;

    left_arm_pos.VEC_X = 0.375f;
    left_arm_pos.VEC_Y = 1.125f;
    left_arm_pos.VEC_Z = 0.0f;

    head_pos.VEC_X = 0.0f;
    head_pos.VEC_Y = 1.75f;
    head_pos.VEC_Z = 0.0f;

    body_rot = matrix4_rotate_y(&body_rot, body_yaw);

    head_rot = matrix4_rotate_y(&head_rot, DEG2RAD(head_yaw));
    head_rot = matrix4_rotate_x(&head_rot, DEG2RAD(head_pitch));

    right_arm_rot = matrix4_translate(&right_arm_rot, &pivot_top);
    right_arm_rot = matrix4_rotate_x(&right_arm_rot, -swing_angle);
    right_arm_rot = matrix4_translate(&right_arm_rot, &pivot_top_neg);

    left_arm_rot = matrix4_translate(&left_arm_rot, &pivot_top);
    left_arm_rot = matrix4_rotate_x(&left_arm_rot, swing_angle);
    left_arm_rot = matrix4_translate(&left_arm_rot, &pivot_top_neg);

    right_leg_rot = matrix4_translate(&right_leg_rot, &pivot_hip);
    right_leg_rot = matrix4_rotate_x(&right_leg_rot, swing_angle);
    right_leg_rot = matrix4_translate(&right_leg_rot, &pivot_hip_neg);

    left_leg_rot = matrix4_translate(&left_leg_rot, &pivot_hip);
    left_leg_rot = matrix4_rotate_x(&left_leg_rot, -swing_angle);
    left_leg_rot = matrix4_translate(&left_leg_rot, &pivot_hip_neg);

    head_matrix = matrix4_translate(&head_matrix, position);
    head_matrix = matrix4_translate(&head_matrix, &head_pos);
    head_matrix = matrix4_mul(&head_matrix, &head_rot);
    head_matrix = matrix4_scale(&head_matrix, &head_scale);

    torso_matrix = matrix4_translate(&torso_matrix, position);
    torso_matrix = matrix4_mul(&torso_matrix, &body_rot);
    torso_matrix = matrix4_translate(&torso_matrix, &torso_pos);
    torso_matrix = matrix4_scale(&torso_matrix, &torso_scale);

    right_arm_matrix = matrix4_translate(&right_arm_matrix, position);
    right_arm_matrix = matrix4_mul(&right_arm_matrix, &body_rot);
    right_arm_matrix = matrix4_translate(&right_arm_matrix, &right_arm_pos);
    right_arm_matrix = matrix4_mul(&right_arm_matrix, &right_arm_rot);
    right_arm_matrix = matrix4_scale(&right_arm_matrix, &limb_scale);

    left_arm_matrix = matrix4_translate(&left_arm_matrix, position);
    left_arm_matrix = matrix4_mul(&left_arm_matrix, &body_rot);
    left_arm_matrix = matrix4_translate(&left_arm_matrix, &left_arm_pos);
    left_arm_matrix = matrix4_mul(&left_arm_matrix, &left_arm_rot);
    left_arm_matrix = matrix4_scale(&left_arm_matrix, &limb_scale);

    right_leg_matrix = matrix4_translate(&right_leg_matrix, position);
    right_leg_matrix = matrix4_mul(&right_leg_matrix, &body_rot);
    right_leg_matrix = matrix4_translate(&right_leg_matrix, &right_leg_pos);
    right_leg_matrix = matrix4_mul(&right_leg_matrix, &right_leg_rot);
    right_leg_matrix = matrix4_scale(&right_leg_matrix, &limb_scale);

    left_leg_matrix = matrix4_translate(&left_leg_matrix, position);
    left_leg_matrix = matrix4_mul(&left_leg_matrix, &body_rot);
    left_leg_matrix = matrix4_translate(&left_leg_matrix, &left_leg_pos);
    left_leg_matrix = matrix4_mul(&left_leg_matrix, &left_leg_rot);
    left_leg_matrix = matrix4_scale(&left_leg_matrix, &limb_scale);

    draw_box(renderer, player_head_uvs, &head_matrix, camera);
    draw_box(renderer, player_torso_uvs, &torso_matrix, camera);
    draw_box(renderer, player_right_arm_uvs, &right_arm_matrix, camera);
    draw_box(renderer, player_left_arm_uvs, &left_arm_matrix, camera);
    draw_box(renderer, player_right_leg_uvs, &right_leg_matrix, camera);
    draw_box(renderer, player_left_leg_uvs, &left_leg_matrix, camera);
}

void renderer_gui_draw_text(struct renderer *renderer, int x, int y,
                            const char *fmt, ...) {
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
        struct glyph glyph;
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

void renderer_gui_flush(struct renderer *renderer,
                        const struct camera *camera) {
    float mesh_vertices[MAX_GLYPHS * 16];
    size_t mesh_vertex_count = 0;

    unsigned int mesh_indices[MAX_GLYPHS * 6];
    size_t mesh_index_count = 0;

    size_t glyph_idx;
    int i;

    assert(renderer);
    assert(camera);

    if (gui->glyph_count == 0) {
        return;
    }

    /* Batch glyphs. */

    for (glyph_idx = 0; glyph_idx < gui->glyph_count; glyph_idx++) {
        struct glyph glyph;
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
        float *vertex;

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

        vertex     = mesh_vertices + mesh_vertex_count;
        vertex[0]  = x0;
        vertex[1]  = y1;
        vertex[2]  = u0;
        vertex[3]  = v1;
        vertex[4]  = x1;
        vertex[5]  = y1;
        vertex[6]  = u1;
        vertex[7]  = v1;
        vertex[8]  = x1;
        vertex[9]  = y0;
        vertex[10] = u1;
        vertex[11] = v0;
        vertex[12] = x0;
        vertex[13] = y0;
        vertex[14] = u0;
        vertex[15] = v0;

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