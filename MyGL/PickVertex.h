#pragma once

#include "Mesh.h"
#include "Shader.h"

#include "PointCloud.h"

namespace MyGL
{
    class PickVertex
    {
    public:
        PickVertex();

        // Returns the index of the vertex that was picked, or -1 if no vertex was picked
        int pick(const glm::ivec2 &pos, const Mesh &mesh,
                 const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp);

        void highlight_hovered_vertex(const Mesh &mesh,
                                      const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp);

    private:
        ShaderProgram vertex_id_shader;
        ShaderProgram basic_shader;

        int vertex_id = -1;
        float pick_point_size = 15.0f; // TODO: make configurable
        glm::vec4 highlight_color{1.0f, 0.0f, 0.0f, 1.0f};

        PointCloud highlighted_vertex{{glm::vec3(0.0f)}};

        static constexpr GLubyte PIXEL_COMPONENTS = 4;
    };
}