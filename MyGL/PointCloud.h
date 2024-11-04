#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MyGL
{
    class PointCloud
    {
    public:
        PointCloud(const std::vector<glm::vec3> &vertices);
        PointCloud(std::vector<glm::vec3> &&vertices);
        ~PointCloud();

        PointCloud(const PointCloud &) = delete;
        PointCloud &operator=(const PointCloud &) = delete;

        PointCloud(PointCloud &&other) noexcept
            : VAO(other.VAO), VBO(other.VBO), num_vertices(other.num_vertices)
        {
            other.VAO = 0;
            other.VBO = 0;
        }

        PointCloud &operator=(PointCloud &&other) noexcept
        {
            if (this != &other)
            {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);

                VAO = other.VAO;
                VBO = other.VBO;
                num_vertices = other.num_vertices;

                other.VAO = 0;
                other.VBO = 0;
            }
            return *this;
        }

        void update(const std::vector<glm::vec3> &vertices);

        void draw() const;

    private:
        GLuint VAO, VBO;
        GLuint num_vertices;

        void setup(const std::vector<glm::vec3> &vertices);
        void setup_VBO(const std::vector<glm::vec3> &vertices);
    };
}