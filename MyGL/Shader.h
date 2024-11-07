#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace MyGL
{
constexpr size_t MAX_ERROR_LOG_LENGTH = 1024;

class Shader
{
  public:
    Shader(GLenum shader_type, const std::string &source);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(Shader &&other) noexcept : ID(other.ID), type(other.type)
    {
        other.ID = 0;
    }
    Shader &operator=(Shader &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteShader(ID);
            ID = other.ID;
            type = other.type;
            other.ID = 0;
        }
        return *this;
    }

    GLuint get_ID() const
    {
        return ID;
    }

  private:
    GLuint ID{0};
    GLenum type;

    void create_shader(GLenum shader_type);
    void compile_from_source(const std::string &source);

    // TODO: Hot reloading
    // std::string source_path; // for hot reloading
    // std::chrono::system_clock::time_point last_modified_time;
};

class ShaderProgram
{
  public:
    ShaderProgram(const std::string &vertex_source, const std::string &fragment_source,
                  const std::string &geometry_source = "");
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    ShaderProgram(ShaderProgram &&other) noexcept : ID(other.ID)
    {
        other.ID = 0;
    }
    ShaderProgram &operator=(ShaderProgram &&other) noexcept
    {
        if (this != &other)
        {
            glDeleteProgram(ID);
            ID = other.ID;
            other.ID = 0;
        }
    }

    GLuint get_ID() const
    {
        return ID;
    }

    void use() const
    {
        glUseProgram(ID);
    }
    void unuse() const
    {
        glUseProgram(0);
    }

    // TODO: Uniform caching
    void set_uniform(const std::string &name, const bool &value) const;
    void set_uniform(const std::string &name, const int &value) const;
    void set_uniform(const std::string &name, const float &value) const;
    void set_uniform(const std::string &name, const glm::vec3 &value) const;
    void set_uniform(const std::string &name, const glm::vec4 &value) const;
    void set_uniform(const std::string &name, const glm::mat3 &value) const;
    void set_uniform(const std::string &name, const glm::mat4 &value) const;

    void set_MVP(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const;
    void set_MVP(const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp) const;

  private:
    GLuint ID{0};

    void create_shader_program();
    void attach_shader(Shader &&shader);
    void attach_shader(const Shader &shader);
    void link_shader_program();

    GLint get_uniform_location(const std::string &name) const;
};
} // namespace MyGL