#include <camera.h>

#include <string.h>
#include <math.h>

void camera_new(camera_t *cam) {
    vec3 pos = {0.f, 0.f, 0.f};
    memcpy(cam->pos, pos, sizeof(vec3));

    cam->pitch = 0.f;
    cam->yaw   = 0.f;
}

void camera_update(camera_t *cam, GLFWwindow *window, float delta_time) {
    float speed = 20.f * delta_time;

    float yaw_rad   = cam->yaw * M_PI / 180.f;
    float pitch_rad = cam->pitch * M_PI / 180.f;

    vec3 forward = {cosf(pitch_rad) * sinf(yaw_rad), sinf(pitch_rad),
                    cosf(pitch_rad) * cosf(yaw_rad)};

    vec3 right = {sinf(yaw_rad - M_PI_2), 0.f, cosf(yaw_rad - M_PI_2)};

    if (glfwGetKey(window, GLFW_KEY_W)) {
        cam->pos[0] += forward[0] * speed;
        cam->pos[1] += forward[1] * speed;
        cam->pos[2] += forward[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        cam->pos[0] -= forward[0] * speed;
        cam->pos[1] -= forward[1] * speed;
        cam->pos[2] -= forward[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        cam->pos[0] -= right[0] * speed;
        cam->pos[2] -= right[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        cam->pos[0] += right[0] * speed;
        cam->pos[2] += right[2] * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        cam->pos[1] += speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        cam->pos[1] -= speed;
    }

    float look_speed = 95.f * delta_time;

    if (glfwGetKey(window, GLFW_KEY_LEFT)) {
        cam->yaw += look_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
        cam->yaw -= look_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_UP)) {
        cam->pitch += look_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        cam->pitch -= look_speed;
    }

    if (cam->pitch > 89.f)
        cam->pitch = 89.f;
    if (cam->pitch < -89.f)
        cam->pitch = -89.f;

    vec3 center;
    vec3_add(center, cam->pos, forward);
    mat4x4_look_at(cam->view_matrix, cam->pos, center, (vec3){0.f, 1.f, 0.f});
}
