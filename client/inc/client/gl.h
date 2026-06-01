#ifndef GL_H
#define GL_H

#include <glad/glad.h>

GLuint shader_program_new(const char *vs_path, const char *fs_path);

void APIENTRY opengl_debug_cb(GLenum src, GLenum type, GLuint id, GLenum sev,
                              GLsizei len, const GLchar *msg, const void *ctx);

#endif
