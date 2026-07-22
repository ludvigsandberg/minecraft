#ifndef OPENGL_H
#define OPENGL_H

#include <glad/glad.h>

GLuint opengl_shader_program(const char *vs_path, const char *fs_path);

void APIENTRY opengl_debug_callback(GLenum src, GLenum type, GLuint id,
                                    GLenum sev, GLsizei len, const GLchar *msg,
                                    const void *ctx);

#endif
