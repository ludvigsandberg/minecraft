/*****************************************************************************
 * File:        camera.c
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#include "client/camera.h"

#include <string.h>
#include <math.h>
#include <assert.h>

#define PI        3.14159265358979323846
#define PI_2      (PI / 2.0)
#define MAX_PITCH 89.9f

void camera_new(camera_t *camera) {
    camera->pos.VEC_X = 0.0f;
    camera->pos.VEC_Y = 0.0f;
    camera->pos.VEC_Z = 0.0f;

    camera->pitch = 0.0f;
    camera->yaw   = 0.0f;

    camera_update_viewport(camera, 900, 600);
}

void camera_update_viewport(camera_t *camera, int width, int height) {
    f32 fov;
    f32 aspect_ratio;

    camera->viewport.width  = width;
    camera->viewport.height = height;

    fov          = 70.0f * (f32)PI / 180.0f;
    aspect_ratio = (f32)width / (f32)height;

    perspective(&camera->viewport.projection_matrix, aspect_ratio, fov, 0.1f,
                1000.0f);
}
void camera_update(camera_t *camera, const window_t *window, f32 delta_time) {
    f32 speed;
    f32 yaw_rad;
    f32 pitch_rad;
    vec3_t forward;
    vec3_t right;
    f32 look_speed;
    vec3_t center;
    vec3_t up = {{0.0f, 1.0f, 0.0f}};

    assert(camera);

    speed = 20.0f * delta_time;

    yaw_rad   = camera->yaw * (f32)PI / 180.0f;
    pitch_rad = camera->pitch * (f32)PI / 180.0f;

    forward.VEC_X = cosf(pitch_rad) * sinf(yaw_rad);
    forward.VEC_Y = sinf(pitch_rad);
    forward.VEC_Z = cosf(pitch_rad) * cosf(yaw_rad);

    right.VEC_X = sinf(yaw_rad - (f32)PI_2);
    right.VEC_Y = 0.0f;
    right.VEC_Z = cosf(yaw_rad - (f32)PI_2);

    if (window_is_key_pressed(window, XK_W)) {
        camera->pos.VEC_X += forward.VEC_X * speed;
        camera->pos.VEC_Y += forward.VEC_Y * speed;
        camera->pos.VEC_Z += forward.VEC_Z * speed;
    }
    if (window_is_key_pressed(window, XK_S)) {
        camera->pos.VEC_X -= forward.VEC_X * speed;
        camera->pos.VEC_Y -= forward.VEC_Y * speed;
        camera->pos.VEC_Z -= forward.VEC_Z * speed;
    }
    if (window_is_key_pressed(window, XK_A)) {
        camera->pos.VEC_X -= right.VEC_X * speed;
        camera->pos.VEC_Z -= right.VEC_Z * speed;
    }
    if (window_is_key_pressed(window, XK_D)) {
        camera->pos.VEC_X += right.VEC_X * speed;
        camera->pos.VEC_Z += right.VEC_Z * speed;
    }
    if (window_is_key_pressed(window, XK_space)) {
        camera->pos.VEC_Y += speed;
    }
    if (window_is_key_pressed(window, XK_Shift_L)) {
        camera->pos.VEC_Y -= speed;
    }

    look_speed = 95.0f * delta_time;

    if (window_is_key_pressed(window, XK_Left)) {
        camera->yaw += look_speed;
    }
    if (window_is_key_pressed(window, XK_Right)) {
        camera->yaw -= look_speed;
    }
    if (window_is_key_pressed(window, XK_Up)) {
        camera->pitch += look_speed;
    }
    if (window_is_key_pressed(window, XK_Down)) {
        camera->pitch -= look_speed;
    }

    if (camera->pitch > MAX_PITCH) {
        camera->pitch = MAX_PITCH;
    }
    if (camera->pitch < -MAX_PITCH) {
        camera->pitch = -MAX_PITCH;
    }

    vec3_add(&center, &camera->pos, &forward);
    look_at(&camera->view_matrix, &camera->pos, &center, &up);
}
