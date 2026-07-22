#ifndef SKY_H
#define SKY_H

#include <glad/glad.h>

#include "client/opengl.h"
#include "client/camera.h"

typedef struct {
    GLuint shader_program;
    GLuint vertex_array;
    GLuint vertex_buffer;

    struct {
        GLint inv_view_matrix;
    } uniform_loc;
} sky_t;

void sky_init(sky_t *sky);
void sky_draw(sky_t *sky, camera_t *camera);

#endif
