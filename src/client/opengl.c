#include "client/opengl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader;
    GLint ok;

    assert(src);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if (!ok) {
        GLchar log[512];
        GLsizei len;
        glGetShaderInfoLog(shader, sizeof log, &len, log);
        printf("%s\r\n", log);
        exit(EXIT_FAILURE);
    }

    return shader;
}

static GLuint load_shader(GLenum type, const char *path) {
    size_t len;
    char *src;
    GLuint shader;
    FILE *file;

    assert(path);

    file = fopen(path, "rb");

    if (!file) {
        printf("Failed to open %s\r\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    len = (size_t)ftell(file);
    rewind(file);

    src = malloc(len + 1);

    fread(src, 1, len, file);
    fclose(file);

    src[len] = '\0';

    shader = compile_shader(type, src);

    free(src);

    return shader;
}

GLuint opengl_shader_program(const char *vs_path, const char *fs_path) {
    GLuint vertex_shader;
    GLuint fragmanet_shader;
    GLuint program;

    assert(vs_path);
    assert(fs_path);

    vertex_shader    = load_shader(GL_VERTEX_SHADER, vs_path);
    fragmanet_shader = load_shader(GL_FRAGMENT_SHADER, fs_path);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragmanet_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragmanet_shader);

    return program;
}

GLuint opengl_texture(const unsigned char *texture_data, int width,
                      int height) {
    GLuint handle;

    assert(texture_data);

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenerateMipmap(GL_TEXTURE_2D);

    return handle;
}

GLuint opengl_texture_raw(const char *path, int width, int height) {
    size_t size;
    FILE *file;
    unsigned char *texture_data;
    size_t bytes_read;
    GLuint handle;

    size = (size_t)(width * height * 4);

    file = fopen(path, "rb");

    if (!file) {
        printf("Failed to open %s\r\n", path);
        exit(EXIT_FAILURE);
    }

    texture_data = malloc(size);

    bytes_read = fread(texture_data, 1, size, file);

    if (bytes_read != size) {
        printf("Invalid file size: %s\r\n", path);
        exit(EXIT_FAILURE);
    }

    handle = opengl_texture(texture_data, width, height);

    free(texture_data);

    return handle;
}

void APIENTRY opengl_debug_callback(GLenum src, GLenum type, GLuint id,
                                    GLenum sev, GLsizei len, const GLchar *msg,
                                    const void *ctx) {
    const char *src_str;
    const char *type_str;
    const char *sev_str;

    (void)id;
    (void)len;
    (void)msg;
    (void)ctx;

    assert(msg);

    switch (src) {
        case GL_DEBUG_SOURCE_API:
            src_str = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            src_str = "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            src_str = "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            src_str = "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            src_str = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            src_str = "Other";
            break;
        default:
            src_str = "Unknown";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "Deprecated";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "Undefined";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_str = "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_str = "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_str = "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_str = "Other";
            break;
        default:
            type_str = "Unknown";
            break;
    }

    switch (sev) {
        case GL_DEBUG_SEVERITY_HIGH:
            sev_str = "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            sev_str = "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            sev_str = "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return;
            sev_str = "NOTIFICATION";
            break;
        default:
            sev_str = "UNKNOWN";
            break;
    }

    printf("OpenGL:%s:%s:%s: %s\r\n", src_str, type_str, sev_str, msg);
}
