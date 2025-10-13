#ifndef GFX_H
#define GFX_H

#include <stddef.h>

#include <glad/glad.h>
#include <meta.h>

GLuint shader_program_new(const char *vs_path, const char *fs_path);

void APIENTRY opengl_debug_cb(GLenum src, GLenum type, GLuint id, GLenum sev,
                              GLsizei len, const GLchar *msg, const void *ctx);

#endif
