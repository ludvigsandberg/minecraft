#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>
#include <linmath.h>

typedef struct {
    vec3 pos;
    float yaw;
    float pitch;
    mat4x4 view_matrix;

    struct {
        int width;
        int height;
        mat4x4 projection_matrix;
    } viewport;
} camera_t;

void camera_new(camera_t *camera);
void camera_update_viewport(camera_t *camera, int width, int height);
void camera_update(camera_t *camera, SDL_Window *window, float delta_time);

#endif
