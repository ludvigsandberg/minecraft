#include "client/renderer.h"

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "matrix.h"
#include "client/opengl.h"

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

void renderer_init(renderer_t *renderer) {
    /* Textures. */

    renderer->terrain_texture =
        opengl_texture_raw("res/terrain.raw", 256, 256);
    renderer->char_texture = opengl_texture_raw("res/char.raw", 64, 32);

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
}

void renderer_draw_sky(const renderer_t *renderer, const camera_t *camera) {
    mat4x4_t inv_view_matrix;

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

void renderer_draw_chunk(const renderer_t *renderer, const chunk_t *chunk,
                         const camera_t *camera) {
    mat4x4_t mvp_matrix;

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
        mat4x4_mul(&camera->viewport.projection_matrix, &camera->view_matrix);

    glUniformMatrix4fv(renderer->uniform_locations.chunk.mvp_matrix, 1,
                       GL_FALSE, (const GLfloat *)mvp_matrix.elems);

    glBindVertexArray(chunk->vertex_array);

    glDrawElements(GL_TRIANGLES, (GLsizei)chunk->index_count, GL_UNSIGNED_INT,
                   0);
}

static void draw_box(const renderer_t *renderer, const float *uvs,
                     const mat4x4_t *model_matrix, const camera_t *camera) {
    mat4x4_t mvp_matrix;

    glUniform2fv(renderer->uniform_locations.box.uvs, 24,
                 (const GLfloat *)uvs);

    mvp_matrix =
        mat4x4_mul(&camera->viewport.projection_matrix, &camera->view_matrix);
    mvp_matrix = mat4x4_mul(&mvp_matrix, model_matrix);

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

void renderer_draw_player(const renderer_t *renderer, const vec3_t *position,
                          const vec3_t *velocity, float head_yaw,
                          float head_pitch, const camera_t *camera) {
    mat4x4_t head_matrix      = MAT4X4_IDENTITY;
    mat4x4_t torso_matrix     = MAT4X4_IDENTITY;
    mat4x4_t right_arm_matrix = MAT4X4_IDENTITY;
    mat4x4_t left_arm_matrix  = MAT4X4_IDENTITY;
    mat4x4_t right_leg_matrix = MAT4X4_IDENTITY;
    mat4x4_t left_leg_matrix  = MAT4X4_IDENTITY;

    mat4x4_t body_rot      = MAT4X4_IDENTITY;
    mat4x4_t head_rot      = MAT4X4_IDENTITY;
    mat4x4_t right_arm_rot = MAT4X4_IDENTITY;
    mat4x4_t left_arm_rot  = MAT4X4_IDENTITY;
    mat4x4_t right_leg_rot = MAT4X4_IDENTITY;
    mat4x4_t left_leg_rot  = MAT4X4_IDENTITY;

    vec3_t head_pos;
    vec3_t torso_pos;
    vec3_t right_arm_pos;
    vec3_t left_arm_pos;
    vec3_t right_leg_pos;
    vec3_t left_leg_pos;

    vec3_t head_scale;
    vec3_t torso_scale;
    vec3_t limb_scale;

    vec3_t pivot_top;
    vec3_t pivot_top_neg;
    vec3_t pivot_hip;
    vec3_t pivot_hip_neg;

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

    body_rot = mat4x4_rotate_y(&body_rot, body_yaw);

    head_rot = mat4x4_rotate_y(&head_rot, DEG2RAD(head_yaw));
    head_rot = mat4x4_rotate_x(&head_rot, DEG2RAD(head_pitch));

    right_arm_rot = mat4x4_translate(&right_arm_rot, &pivot_top);
    right_arm_rot = mat4x4_rotate_x(&right_arm_rot, -swing_angle);
    right_arm_rot = mat4x4_translate(&right_arm_rot, &pivot_top_neg);

    left_arm_rot = mat4x4_translate(&left_arm_rot, &pivot_top);
    left_arm_rot = mat4x4_rotate_x(&left_arm_rot, swing_angle);
    left_arm_rot = mat4x4_translate(&left_arm_rot, &pivot_top_neg);

    right_leg_rot = mat4x4_translate(&right_leg_rot, &pivot_hip);
    right_leg_rot = mat4x4_rotate_x(&right_leg_rot, swing_angle);
    right_leg_rot = mat4x4_translate(&right_leg_rot, &pivot_hip_neg);

    left_leg_rot = mat4x4_translate(&left_leg_rot, &pivot_hip);
    left_leg_rot = mat4x4_rotate_x(&left_leg_rot, -swing_angle);
    left_leg_rot = mat4x4_translate(&left_leg_rot, &pivot_hip_neg);

    head_matrix = mat4x4_translate(&head_matrix, position);
    head_matrix = mat4x4_translate(&head_matrix, &head_pos);
    head_matrix = mat4x4_mul(&head_matrix, &head_rot);
    head_matrix = mat4x4_scale(&head_matrix, &head_scale);

    torso_matrix = mat4x4_translate(&torso_matrix, position);
    torso_matrix = mat4x4_mul(&torso_matrix, &body_rot);
    torso_matrix = mat4x4_translate(&torso_matrix, &torso_pos);
    torso_matrix = mat4x4_scale(&torso_matrix, &torso_scale);

    right_arm_matrix = mat4x4_translate(&right_arm_matrix, position);
    right_arm_matrix = mat4x4_mul(&right_arm_matrix, &body_rot);
    right_arm_matrix = mat4x4_translate(&right_arm_matrix, &right_arm_pos);
    right_arm_matrix = mat4x4_mul(&right_arm_matrix, &right_arm_rot);
    right_arm_matrix = mat4x4_scale(&right_arm_matrix, &limb_scale);

    left_arm_matrix = mat4x4_translate(&left_arm_matrix, position);
    left_arm_matrix = mat4x4_mul(&left_arm_matrix, &body_rot);
    left_arm_matrix = mat4x4_translate(&left_arm_matrix, &left_arm_pos);
    left_arm_matrix = mat4x4_mul(&left_arm_matrix, &left_arm_rot);
    left_arm_matrix = mat4x4_scale(&left_arm_matrix, &limb_scale);

    right_leg_matrix = mat4x4_translate(&right_leg_matrix, position);
    right_leg_matrix = mat4x4_mul(&right_leg_matrix, &body_rot);
    right_leg_matrix = mat4x4_translate(&right_leg_matrix, &right_leg_pos);
    right_leg_matrix = mat4x4_mul(&right_leg_matrix, &right_leg_rot);
    right_leg_matrix = mat4x4_scale(&right_leg_matrix, &limb_scale);

    left_leg_matrix = mat4x4_translate(&left_leg_matrix, position);
    left_leg_matrix = mat4x4_mul(&left_leg_matrix, &body_rot);
    left_leg_matrix = mat4x4_translate(&left_leg_matrix, &left_leg_pos);
    left_leg_matrix = mat4x4_mul(&left_leg_matrix, &left_leg_rot);
    left_leg_matrix = mat4x4_scale(&left_leg_matrix, &limb_scale);

    draw_box(renderer, player_head_uvs, &head_matrix, camera);
    draw_box(renderer, player_torso_uvs, &torso_matrix, camera);
    draw_box(renderer, player_right_arm_uvs, &right_arm_matrix, camera);
    draw_box(renderer, player_left_arm_uvs, &left_arm_matrix, camera);
    draw_box(renderer, player_right_leg_uvs, &right_leg_matrix, camera);
    draw_box(renderer, player_left_leg_uvs, &left_leg_matrix, camera);
}