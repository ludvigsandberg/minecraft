#include <gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <meta.h>

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

    if (!ok) {
        GLchar log[512];
        GLsizei len;
        glGetShaderInfoLog(shader, sizeof log, &len, log);
        printf("%s\n", log);
        exit(EXIT_FAILURE);
    }

    return shader;
}

static GLuint load_shader(GLenum type, const char *path) {
    FILE *file = fopen(path, "rb");

    if (!file) {
        printf("Failed to open %s\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t len = (size_t)ftell(file);
    rewind(file);

    arr(char) src;
    arr_new_n(src, len);

    fread(src, 1, len, file);
    fclose(file);

    static char nt = '\0';
    arr_append(src, nt);

    GLuint shader = compile_shader(type, src);

    arr_free(src);

    return shader;
}

GLuint shader_program_new(const char *vs_path, const char *fs_path) {
    GLuint vertex_shader    = load_shader(GL_VERTEX_SHADER, vs_path);
    GLuint fragmanet_shader = load_shader(GL_FRAGMENT_SHADER, fs_path);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragmanet_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragmanet_shader);

    return program;
}

void APIENTRY opengl_debug_cb(GLenum src, GLenum type, GLuint id, GLenum sev,
                              GLsizei len, const GLchar *msg,
                              const void *ctx) {
    const char *src_str;
    const char *type_str;
    const char *sev_str;

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

    printf("OpenGL:%s:%s:%s: %s\n", src_str, type_str, sev_str, msg);
}
