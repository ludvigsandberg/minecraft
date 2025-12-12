#include <minecraft/camera.h>

#include <string.h>
#include <math.h>

#define PI   3.14159265358979323846
#define PI_2 (PI / 2.0)

void camera_new(camera_t *camera) {
    vec3 pos = {0.f, 0.f, 0.f};
    memcpy(camera->pos, pos, sizeof(vec3));

    camera->pitch = 0.f;
    camera->yaw   = 0.f;

    camera_update_viewport(camera, 900, 600);
}

void camera_update_viewport(camera_t *camera, int width, int height) {
    camera->viewport.width  = width;
    camera->viewport.height = height;

    float fov          = 70.f * PI / 180.f;
    float aspect_ratio = (float)width / (float)height;

    mat4x4_perspective(camera->viewport.projection_matrix, fov, aspect_ratio,
                       0.1f, 10000.f);
}
void camera_update(camera_t *camera, SDL_Window *window, float delta_time) {
    const bool *keys = SDL_GetKeyboardState(NULL);

    float speed = 20.f * delta_time;

    float yaw_rad   = camera->yaw * PI / 180.f;
    float pitch_rad = camera->pitch * PI / 180.f;

    vec3 forward = {cosf(pitch_rad) * sinf(yaw_rad), sinf(pitch_rad),
                    cosf(pitch_rad) * cosf(yaw_rad)};

    vec3 right = {sinf(yaw_rad - PI_2), 0.f, cosf(yaw_rad - PI_2)};

    if (keys[SDL_SCANCODE_W]) {
        camera->pos[0] += forward[0] * speed;
        camera->pos[1] += forward[1] * speed;
        camera->pos[2] += forward[2] * speed;
    }
    if (keys[SDL_SCANCODE_S]) {
        camera->pos[0] -= forward[0] * speed;
        camera->pos[1] -= forward[1] * speed;
        camera->pos[2] -= forward[2] * speed;
    }
    if (keys[SDL_SCANCODE_A]) {
        camera->pos[0] -= right[0] * speed;
        camera->pos[2] -= right[2] * speed;
    }
    if (keys[SDL_SCANCODE_D]) {
        camera->pos[0] += right[0] * speed;
        camera->pos[2] += right[2] * speed;
    }
    if (keys[SDL_SCANCODE_SPACE]) {
        camera->pos[1] += speed;
    }
    if (keys[SDL_SCANCODE_LSHIFT]) {
        camera->pos[1] -= speed;
    }

    float look_speed = 95.f * delta_time;

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

    vec3 center;
    vec3_add(center, camera->pos, forward);

    mat4x4_look_at(camera->view_matrix, camera->pos, center,
                   (vec3){0.f, 1.f, 0.f});
}
