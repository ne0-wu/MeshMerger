#include "PointCloud.h"

#include <stdexcept>

MyGL::PointCloud::PointCloud(const std::vector<glm::vec3> &vertices)
{
    setup(vertices);
}

MyGL::PointCloud::PointCloud(std::vector<glm::vec3> &&vertices)
{
    setup(vertices);
}

MyGL::PointCloud::~PointCloud()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void MyGL::PointCloud::update(const std::vector<glm::vec3> &vertices)
{
    num_vertices = vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
}

void MyGL::PointCloud::draw() const
{
    glPointSize(15.0); // TODO: remove magic number

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, num_vertices);
    glBindVertexArray(0);
}

void MyGL::PointCloud::setup(const std::vector<glm::vec3> &vertices)
{
    num_vertices = vertices.size();

    glGenVertexArrays(1, &VAO);
    if (VAO == 0)
        throw std::runtime_error("Point cloud setup failed: Failed to generate VAO");

    glGenBuffers(1, &VBO);
    if (VBO == 0)
    {
        glDeleteVertexArrays(1, &VAO);
        throw std::runtime_error("Point cloud setup failed: Failed to generate VBO");
    }

    setup_VBO(vertices);
}

void MyGL::PointCloud::setup_VBO(const std::vector<glm::vec3> &vertices)
{
    glBindVertexArray(VAO);

    // Buffer data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    // Location 0: Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

    glBindVertexArray(0);
}