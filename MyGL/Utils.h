#pragma once

#include <iostream>
#include <fstream>
#include <iterator>

#include "glad/glad.h"

namespace MyGL
{
    inline std::string read_file_to_string(const std::string &file_path)
    {
        std::ifstream ifs{file_path};
        if (!ifs)
            throw std::runtime_error("Failed to open file: " + file_path);
        return std::string(std::istreambuf_iterator<char>{ifs}, {});
    }

    inline std::tuple<int, int> get_viewport_size()
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return {viewport[2], viewport[3]};
    }
}