#include <sky.h>

#include <linmath.h>

static const char *vertex_shader_src =
    "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "out vec2 vUV;\n"
    "void main() {\n"
    "    vUV = aPos * 0.5 + 0.5;\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const char *fragment_shader_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 vUV;\n"
    "uniform mat4 invProjection;\n"
    "uniform mat4 invView;\n"
    "uniform vec3 topColor;\n"
    "uniform vec3 horizonColor;\n"
    "uniform vec3 bottomColor;\n"
    "void main() {\n"
    "    vec2 ndc = vUV * 2.0 - 1.0;\n"
    "    vec4 clip = vec4(ndc, -1.0, 1.0);\n"
    "    vec4 view = invProjection * clip;\n"
    "    view /= view.w;\n"
    "    vec4 world = invView * vec4(view.xyz, 0.0);\n"
    "    vec3 viewDir = normalize(world.xyz);\n"
    "\n"
    "    // Horizon is at y = 0\n"
    "    float y = viewDir.y;\n"
    "    vec3 skyColor;\n"
    "\n"
    "    if (y >= 0.0) {\n"
    "        // Above horizon: blend from horizon to top\n"
    "        float t = pow(y, 0.75);\n"
    "        skyColor = mix(horizonColor, topColor, t);\n"
    "    } else {\n"
    "        // Below horizon: blend from horizon to bottom\n"
    "        float t = pow(-y, 0.75);\n"
    "        skyColor = mix(horizonColor, bottomColor, t);\n"
    "    }\n"
    "\n"
    "    FragColor = vec4(skyColor, 1.0);\n"
    "}\n";

static const float vertices[] = {-1.f, 1.f, -1.f, -1.f, 1.f, -1.f,
                                 -1.f, 1.f, 1.f,  -1.f, 1.f, 1.f};

void sky_new(sky_t *sky) {
    sky->shader_program =
        shader_program_new(vertex_shader_src, fragment_shader_src);

    sky->uniform_loc.inv_view_matrix =
        glGetUniformLocation(sky->shader_program, "invView");
    sky->uniform_loc.inv_projection_matrix =
        glGetUniformLocation(sky->shader_program, "invProjection");
    sky->uniform_loc.top_color =
        glGetUniformLocation(sky->shader_program, "topColor");
    sky->uniform_loc.horizon_color =
        glGetUniformLocation(sky->shader_program, "horizonColor");
    sky->uniform_loc.bottom_color =
        glGetUniformLocation(sky->shader_program, "bottomColor");

    glGenVertexArrays(1, &sky->vertex_array);
    glBindVertexArray(sky->vertex_array);

    glGenBuffers(1, &sky->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sky->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
}

void sky_draw(sky_t *sky, camera_t *camera) {
    glDepthMask(GL_FALSE);

    glUseProgram(sky->shader_program);

    mat4x4 inv_view_matrix;
    mat4x4_invert(inv_view_matrix, camera->view_matrix);
    glUniformMatrix4fv(sky->uniform_loc.inv_view_matrix, 1, GL_FALSE,
                       (const GLfloat *)inv_view_matrix);

    mat4x4 inv_projection_matrix;
    mat4x4_invert(inv_projection_matrix, camera->viewport.projection_matrix);
    glUniformMatrix4fv(sky->uniform_loc.inv_projection_matrix, 1, GL_FALSE,
                       (const GLfloat *)inv_projection_matrix);

    glUniform3f(sky->uniform_loc.top_color, .4f, .6f, 1.f);
    glUniform3f(sky->uniform_loc.horizon_color, .9f, .9f, 1.f);
    glUniform3f(sky->uniform_loc.bottom_color, 0.05f, 0.1f, 0.2f);

    glBindVertexArray(sky->vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthMask(GL_TRUE);
}