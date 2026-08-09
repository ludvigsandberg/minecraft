#ifndef OPENGL_H
#define OPENGL_H

#include <glad/glad.h>

GLuint opengl_shader_program(const char *vs_path, const char *fs_path);

GLuint opengl_texture(const unsigned char *texture_data, int width,
                      int height);

/* Load raw 32-bit RGBA texture data into GPU memory.  */
GLuint opengl_texture_raw(const char *path, int width, int height);

void APIENTRY opengl_debug_callback(GLenum src, GLenum type, GLuint id,
                                    GLenum sev, GLsizei len, const GLchar *msg,
                                    const void *ctx);

#endif
