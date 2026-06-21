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

#include "common/types.h"
#include "common/vec_f32.h"

#define PI   3.14159265358979323846
#define PI_2 (PI / 2.0)

void camera_new(camera_t *camera) {
    camera->pos = (vec3_f32_t){{0.f, 0.f, 0.f}};

    camera->pitch = 0.f;
    camera->yaw   = 0.f;

    camera_update_viewport(camera, 900, 600);
}

void camera_update_viewport(camera_t *camera, int width, int height) {
    f32 fov;
    f32 aspect_ratio;

    camera->viewport.width  = width;
    camera->viewport.height = height;

    fov          = 70.f * PI / 180.f;
    aspect_ratio = (f32)width / (f32)height;

    xperspective_f32(aspect_ratio, fov, 0.1f, 1000.f,
                     camera->viewport.projection_matrix);
}
void camera_update(camera_t *camera, SDL_Window *window, f32 delta_time) {
    const bool *keys;
    f32 speed;
    f32 yaw_rad;
    f32 pitch_rad;
    vec3_f32_t forward;
    vec3_f32_t right;
    f32 look_speed;
    vec3_f32_t center;
    vec3_f32_t up = {{0.f, 1.f, 0.f}};

    assert(camera);
    assert(window);

    keys = SDL_GetKeyboardState(NULL);

    speed = 20.f * delta_time;

    yaw_rad   = camera->yaw * PI / 180.f;
    pitch_rad = camera->pitch * PI / 180.f;

    forward.pos.x = cosf(pitch_rad) * sinf(yaw_rad);
    forward.pos.y = sinf(pitch_rad);
    forward.pos.z = cosf(pitch_rad) * cosf(yaw_rad);

    right.pos.x = sinf(yaw_rad - PI_2);
    right.pos.y = 0.0f;
    right.pos.z = cosf(yaw_rad - PI_2);

    if (keys[SDL_SCANCODE_W]) {
        camera->pos.pos.x += forward.pos.x * speed;
        camera->pos.pos.y += forward.pos.y * speed;
        camera->pos.pos.z += forward.pos.z * speed;
    }
    if (keys[SDL_SCANCODE_S]) {
        camera->pos.pos.x -= forward.pos.x * speed;
        camera->pos.pos.y -= forward.pos.y * speed;
        camera->pos.pos.z -= forward.pos.z * speed;
    }
    if (keys[SDL_SCANCODE_A]) {
        camera->pos.pos.x -= right.pos.x * speed;
        camera->pos.pos.z -= right.pos.z * speed;
    }
    if (keys[SDL_SCANCODE_D]) {
        camera->pos.pos.x += right.pos.x * speed;
        camera->pos.pos.z += right.pos.z * speed;
    }
    if (keys[SDL_SCANCODE_SPACE]) {
        camera->pos.pos.y += speed;
    }
    if (keys[SDL_SCANCODE_LSHIFT]) {
        camera->pos.pos.y -= speed;
    }

    look_speed = 95.f * delta_time;

    if (keys[SDL_SCANCODE_LEFT]) {
        camera->yaw += look_speed;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        camera->yaw -= look_speed;
    }
    if (keys[SDL_SCANCODE_UP]) {
        camera->pitch += look_speed;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        camera->pitch -= look_speed;
    }

    if (camera->pitch > 89.f) {
        camera->pitch = 89.f;
    }
    if (camera->pitch < -89.f) {
        camera->pitch = -89.f;
    }

    vec3_f32_add(&center, &camera->pos, &forward);
    xlook_at_f32(camera->pos, center, up, camera->view_matrix);
}
