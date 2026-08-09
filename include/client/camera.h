#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"
#include "client/system.h"

struct camera {
    struct vector3 pos;
    float yaw;
    float pitch;
    struct matrix4 view_matrix;

    struct {
        int width;
        int height;
        struct matrix4 projection_matrix;
    } viewport;
};

void camera_init(struct camera *camera);
void camera_update_viewport(struct camera *camera, int width, int height);
void camera_update(struct camera *camera, const struct window *window,
                   float delta_time);

#endif
