#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <linmath.h>

typedef struct {
    vec3 pos;
    float yaw;
    float pitch;
    mat4x4 view_matrix;
} camera_t;

void camera_new(camera_t *cam);
void camera_update(camera_t *cam, GLFWwindow *window, float delta_time);

#endif
