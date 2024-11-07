#include "PickVertex.h"

#include <iostream>

#include "Utils.h"

MyGL::PickVertex::PickVertex()
    : vertex_id_shader(read_file_to_string("data/shaders/pick_vertex.vert"),
                       read_file_to_string("data/shaders/pick_vertex.frag")),
      round_point_shader(read_file_to_string("data/shaders/basic.vert"),
                         read_file_to_string("data/shaders/round_point.frag")),
      basic_shader(read_file_to_string("data/shaders/basic.vert"), read_file_to_string("data/shaders/basic.frag"))
{
}

int MyGL::PickVertex::pick(const glm::ivec2 &pos, const Mesh &mesh,
                           const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp)
{
    GLboolean multisample_enabled;
    glGetBooleanv(GL_MULTISAMPLE, &multisample_enabled);
    glDisable(GL_MULTISAMPLE);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update Z-buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    basic_shader.use();
    basic_shader.set_MVP(mvp);
    mesh.draw();

    // Draw vertices
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPointSize(pick_point_size);
    vertex_id_shader.use();
    vertex_id_shader.set_MVP(mvp);
    mesh.draw(Mesh::DrawMode::POINTS);

    // Read pixel
    GLubyte pixel[PIXEL_COMPONENTS];
    glReadPixels(pos.x, pos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    int index = static_cast<int>(pixel[0] + (pixel[1] << 8) + (pixel[2] << 16)) - 1;
    vertex_id = index;

    if (multisample_enabled)
        glEnable(GL_MULTISAMPLE);

    return index;
}

void MyGL::PickVertex::highlight_hovered_vertex(const Mesh &mesh,
                                                const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp)
{
    if (vertex_id == -1)
    {
        highlighted_vertex.update({glm::vec3(0.0f)});
        return;
    }

    highlighted_vertex.update({mesh.get_vertex_position(vertex_id)});

    round_point_shader.use();
    round_point_shader.set_MVP(mvp);
    round_point_shader.set_uniform("color", highlight_color);
    highlighted_vertex.draw();
}
