/*****************************************************************************
 * File:        gl.h
 * Author:      ludvigsandberg
 * Date:        2026-06-20
 * Description: -
 *****************************************************************************/

#ifndef CLIENT_GL_H
#define CLIENT_GL_H

#include <glad/glad.h>

GLuint shader_program_new(const char *vs_path, const char *fs_path);

void APIENTRY opengl_debug_callback(GLenum src, GLenum type, GLuint id,
                                    GLenum sev, GLsizei len, const GLchar *msg,
                                    const void *ctx);

#endif
