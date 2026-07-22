#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"
#include "client/system.h"

typedef struct {
    vec3_t pos;
    float yaw;
    float pitch;
    mat4x4_t view_matrix;

    struct {
        int width;
        int height;
        mat4x4_t projection_matrix;
    } viewport;
} camera_t;

void camera_init(camera_t *camera);
void camera_update_viewport(camera_t *camera, int width, int height);
void camera_update(camera_t *camera, const window_t *window, float delta_time);

#endif
