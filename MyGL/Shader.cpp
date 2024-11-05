#include "Shader.h"

#include <stdexcept>
#include <vector>

MyGL::Shader::Shader(GLenum shader_type, const std::string &source) : type(shader_type)
{
    try
    {
        create_shader(type);
        compile_from_source(source);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to create shader: " + std::string(e.what()));
    }
}

MyGL::Shader::~Shader()
{
    if (ID != 0)
        glDeleteShader(ID);
}

void MyGL::Shader::create_shader(GLenum shader_type)
{
    if (!gladLoadGL())
        throw std::runtime_error("No valid OpenGL context");

    switch (type)
    {
    case GL_VERTEX_SHADER:
    case GL_FRAGMENT_SHADER:
    case GL_GEOMETRY_SHADER:
        break;
    default:
        throw std::runtime_error("Invalid shader type");
    }

    ID = glCreateShader(type);

    if (ID == 0)
        throw std::runtime_error("Failed to create shader. OpenGL error: " + std::to_string(glGetError()));
}

void MyGL::Shader::compile_from_source(const std::string &source)
{
    if (source.empty())
        throw std::invalid_argument("Shader source is empty");

    const char *sources[] = {source.c_str()};
    glShaderSource(ID, 1, sources, nullptr);
    glCompileShader(ID);

    int success;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info_log[MAX_ERROR_LOG_LENGTH];
        glGetShaderInfoLog(ID, MAX_ERROR_LOG_LENGTH, nullptr, info_log);
        throw std::runtime_error("Failed to compile shader: " + std::string(info_log));
    }
}

MyGL::ShaderProgram::ShaderProgram(const std::string &vertex_source, const std::string &fragment_source,
                                   const std::string &geometry_source)
{
    create_shader_program();

    std::vector<Shader> shaders;
    shaders.emplace_back(GL_VERTEX_SHADER, vertex_source);
    shaders.emplace_back(GL_FRAGMENT_SHADER, fragment_source);
    if (!geometry_source.empty())
        shaders.emplace_back(GL_GEOMETRY_SHADER, geometry_source);

    for (const auto &shader : shaders)
        glAttachShader(ID, shader.get_ID());

    link_shader_program();

    for (const auto &shader : shaders)
        glDetachShader(ID, shader.get_ID());
}

MyGL::ShaderProgram::~ShaderProgram()
{
    if (ID != 0)
        glDeleteProgram(ID);
}

void MyGL::ShaderProgram::create_shader_program()
{
    ID = glCreateProgram();

    if (ID == 0)
        throw std::runtime_error("Failed to create shader program. OpenGL error: " + std::to_string(glGetError()));
}

void MyGL::ShaderProgram::attach_shader(Shader &&shader)
{
    glAttachShader(ID, shader.get_ID());
}

void MyGL::ShaderProgram::attach_shader(const Shader &shader)
{
    glAttachShader(ID, shader.get_ID());
}

void MyGL::ShaderProgram::link_shader_program()
{
    glLinkProgram(ID);

    int success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        char info_log[MAX_ERROR_LOG_LENGTH];
        glGetProgramInfoLog(ID, MAX_ERROR_LOG_LENGTH, nullptr, info_log);
        throw std::runtime_error("Failed to link program: " + std::string(info_log));
    }
}

GLint MyGL::ShaderProgram::get_uniform_location(const std::string &name) const
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location == -1)
        throw std::runtime_error("Uniform " + name + " not found in shader program");
    return location;
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const bool &value) const
{
    glUniform1i(get_uniform_location(name), value);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const int &value) const
{
    glUniform1i(get_uniform_location(name), value);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const float &value) const
{
    glUniform1f(get_uniform_location(name), value);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(get_uniform_location(name), 1, &value[0]);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(get_uniform_location(name), 1, &value[0]);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const glm::mat3 &value) const
{
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
}

void MyGL::ShaderProgram::set_uniform(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
}

void MyGL::ShaderProgram::set_MVP(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const
{
    set_uniform("model", model);
    set_uniform("view", view);
    set_uniform("projection", projection);
}

void MyGL::ShaderProgram::set_MVP(const std::tuple<glm::mat4, glm::mat4, glm::mat4> &mvp) const
{
    set_uniform("model", std::get<0>(mvp));
    set_uniform("view", std::get<1>(mvp));
    set_uniform("projection", std::get<2>(mvp));
}