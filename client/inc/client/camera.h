#ifndef CAMERA_H
#define CAMERA_H

#include <SDL3/SDL.h>
#include <x/vec.h>
#include <x/mat.h>

typedef struct {
    xvec3f32_t pos;
    float yaw;
    float pitch;
    xmat4f32_t view_matrix;

    struct {
        int width;
        int height;
        xmat4f32_t projection_matrix;
    } viewport;
} camera_t;

void camera_new(camera_t *camera);
void camera_update_viewport(camera_t *camera, int width, int height);
void camera_update(camera_t *camera, SDL_Window *window, float delta_time);

#endif
