#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MyGL
{
    class LineSegment
    {
    public:
        LineSegment(const std::vector<glm::vec3> &vertices,
                    const std::vector<GLuint> &indices);

        ~LineSegment();

        void update();

        void draw();

    private:
        GLuint VAO, VBO, EBO;

        const std::vector<glm::vec3> &vertices;
        const std::vector<GLuint> &indices;

        void setup();
    };
}