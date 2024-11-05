#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MyGL
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

class Mesh
{
  public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

    ~Mesh();

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(Mesh &&other) noexcept
        : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO), num_indices(other.num_indices),
          num_vertices(other.num_vertices)
    {
        other.VAO = other.VBO = other.EBO = 0;
    }
    Mesh &operator=(Mesh &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);

            VAO = other.VAO;
            VBO = other.VBO;
            EBO = other.EBO;
            num_indices = other.num_indices;
            num_vertices = other.num_vertices;

            other.VAO = 0;
            other.VBO = 0;
            other.EBO = 0;
        }
        return *this;
    }

    void update(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
    void update_vertices(const std::vector<Vertex> &vertices);
    void update_indices(const std::vector<GLuint> &indices);

    enum class DrawMode
    {
        FILL,
        WIREFRAME,
        POINTS
    };

    void draw(DrawMode mode = DrawMode::FILL) const;

    glm::vec3 get_vertex_position(GLuint index) const;

  private:
    GLuint VAO, VBO, EBO;
    GLuint num_indices, num_vertices;

    void setup(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
    void setup_VBO(const std::vector<Vertex> &vertices);
    void setup_EBO(const std::vector<GLuint> &indices);
};
} // namespace MyGL