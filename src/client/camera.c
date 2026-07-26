#include "client/camera.h"

#include <string.h>
#include <math.h>
#include <assert.h>

#define PI        3.14159265358979323846
#define PI_2      (PI / 2.0)
#define MAX_PITCH 89.999f

void camera_init(camera_t *camera) {
    assert(camera);

    camera->pos.VEC_X = 0.0f;
    camera->pos.VEC_Y = 0.0f;
    camera->pos.VEC_Z = 0.0f;

    camera->pitch = 0.0f;
    camera->yaw   = 0.0f;

    camera_update_viewport(camera, 900, 600);
}

void camera_update_viewport(camera_t *camera, int width, int height) {
    float fov;
    float aspect_ratio;

    assert(camera);

    camera->viewport.width  = width;
    camera->viewport.height = height;

    fov          = 70.0f * (float)PI / 180.0f;
    aspect_ratio = (float)width / (float)height;

    camera->viewport.projection_matrix =
        perspective(aspect_ratio, fov, 0.1f, 1000.0f);
}
void camera_update(camera_t *camera, const window_t *window,
                   float delta_time) {
    float speed;
    float yaw_rad;
    float pitch_rad;
    vec3_t forward;
    vec3_t right;
    float look_speed;
    vec3_t center;
    vec3_t up = {{0.0f, 1.0f, 0.0f}};

    assert(camera);
    assert(window);

    speed = 40.0f * delta_time;

    yaw_rad   = camera->yaw * (float)PI / 180.0f;
    pitch_rad = camera->pitch * (float)PI / 180.0f;

    forward.VEC_X =
        (float)cos((double)pitch_rad) * (float)sin((double)yaw_rad);
    forward.VEC_Y = (float)sin((double)pitch_rad);
    forward.VEC_Z =
        (float)cos((double)pitch_rad) * (float)cos((double)yaw_rad);

    right.VEC_X = (float)sin((double)(yaw_rad - (float)PI_2));
    right.VEC_Y = 0.0f;
    right.VEC_Z = (float)cos((double)(yaw_rad - (float)PI_2));

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

    look_speed = 400.0f * delta_time;

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

    center              = vec3_add(&camera->pos, &forward);
    camera->view_matrix = look_at(&camera->pos, &center, &up);
}
