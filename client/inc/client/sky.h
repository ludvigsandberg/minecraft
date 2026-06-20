/*****************************************************************************
 * File:        sky.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_SKY_H
#define CLIENT_SKY_H

#include <glad/glad.h>

#include "client/gl.h"
#include "client/camera.h"

typedef struct {
    GLuint shader_program;
    GLuint vertex_array;
    GLuint vertex_buffer;

    struct {
        GLint inv_view_matrix;
    } uniform_loc;
} sky_t;

void sky_new(sky_t *sky);
void sky_draw(sky_t *sky, camera_t *camera);

#endif
