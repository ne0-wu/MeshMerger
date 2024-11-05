#include "Mesh.h"

#include <stdexcept>

MyGL::Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
    setup(vertices, indices);
}

MyGL::Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void MyGL::Mesh::draw(DrawMode mode) const
{
    glBindVertexArray(VAO);

    switch (mode)
    {
    case DrawMode::FILL:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
        break;
    case DrawMode::WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f); // TODO: remove magic number
        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
        break;
    case DrawMode::POINTS:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
        glPointSize(15.0f); // TODO: remove magic number
        glDrawElements(GL_POINTS, num_indices, GL_UNSIGNED_INT, 0);
        break;
    }

    glBindVertexArray(0);
}
void MyGL::Mesh::setup(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
    auto max_index = std::max_element(indices.begin(), indices.end());
    if (max_index != indices.end() && *max_index >= vertices.size())
        throw std::runtime_error("Mesh setup failed: index out of bounds. Max index: " + std::to_string(*max_index) +
                                 ", vertex count: " + std::to_string(vertices.size()));

    if (indices.size() % 3 != 0)
        throw std::runtime_error("Mesh setup failed: index count must be multiple of 3");

    num_indices = indices.size();
    num_vertices = vertices.size();

    glGenVertexArrays(1, &VAO);
    if (VAO == 0)
        throw std::runtime_error("Mesh setup failed: Failed to generate VAO");

    glGenBuffers(1, &VBO);
    if (VBO == 0)
    {
        glDeleteVertexArrays(1, &VAO);
        throw std::runtime_error("Mesh setup failed: Failed to generate VBO");
    }

    glGenBuffers(1, &EBO);
    if (EBO == 0)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        throw std::runtime_error("Mesh setup failed: Failed to generate EBO");
    }

    setup_VBO(vertices);
    setup_EBO(indices);
}

void MyGL::Mesh::setup_VBO(const std::vector<Vertex> &vertices)
{
    glBindVertexArray(VAO);

    // Buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    // Location 0: Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // Location 1: Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    // Location 2: TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

void MyGL::Mesh::setup_EBO(const std::vector<GLuint> &indices)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void MyGL::Mesh::update(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
    setup_VBO(vertices);
    setup_EBO(indices);
}

void MyGL::Mesh::update_vertices(const std::vector<Vertex> &vertices)
{
    if (vertices.size() != num_vertices)
        throw std::runtime_error("Mesh update failed: New vertices count must match original count");

    setup_VBO(vertices);
}

void MyGL::Mesh::update_indices(const std::vector<GLuint> &indices)
{
    if (indices.size() != num_indices)
        throw std::runtime_error("Mesh update failed: New indices count must match original count");

    setup_EBO(indices);
}

glm::vec3 MyGL::Mesh::get_vertex_position(GLuint index) const
{
    glm::vec3 position;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glGetBufferSubData(GL_ARRAY_BUFFER, index * sizeof(Vertex), sizeof(glm::vec3), &position);
    return position;
}
