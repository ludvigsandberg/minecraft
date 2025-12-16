#ifndef SKY_H
#define SKY_H

#include <darkcraft/gl.h>
#include <darkcraft/camera.h>

typedef struct {
    GLuint shader_program;
    GLuint vertex_array;
    GLuint vertex_buffer;

    struct {
        GLint inv_view_matrix;
        GLint inv_projection_matrix;
    } uniform_loc;
} sky_t;

void sky_new(sky_t *sky);
void sky_draw(sky_t *sky, camera_t *camera);

#endif
