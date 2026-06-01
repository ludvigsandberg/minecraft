#include <minecraft/sky.h>

#include <x/mat.h>

static const float vertices[] = {-1.f, 1.f, -1.f, -1.f, 1.f, -1.f,
                                 -1.f, 1.f, 1.f,  -1.f, 1.f, 1.f};

void sky_new(sky_t *sky) {
    sky->shader_program =
        shader_program_new("res/sky_vs.glsl", "res/sky_fs.glsl");

    sky->uniform_loc.inv_view_matrix =
        glGetUniformLocation(sky->shader_program, "invView");

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

    xmat4f32_t inv_view_matrix;
    xmat4_invert_view_f32(camera->view_matrix, inv_view_matrix);
    glUniformMatrix4fv(sky->uniform_loc.inv_view_matrix, 1, GL_FALSE,
                       (const GLfloat *)inv_view_matrix.nth);

    glBindVertexArray(sky->vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDepthMask(GL_TRUE);
}