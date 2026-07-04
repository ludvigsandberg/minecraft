/*****************************************************************************
 * File:        camera.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: 3D free-flight camera controller.
 *****************************************************************************/

#ifndef CLIENT_CAMERA_H
#define CLIENT_CAMERA_H

#include <SDL3/SDL.h>

#include "common/types.h"
#include "common/vec.h"
#include "common/mat.h"

typedef struct {
    vec3_t pos;
    f32 yaw;
    f32 pitch;
    mat4x4_t view_matrix;

    struct {
        int width;
        int height;
        mat4x4_t projection_matrix;
    } viewport;
} camera_t;

void camera_new(camera_t *camera);
void camera_update_viewport(camera_t *camera, int width, int height);
void camera_update(camera_t *camera, SDL_Window *window, f32 delta_time);

#endif
