#pragma once

#include <format>
#include <string>
#include <vector>

#include <imgui.h>

namespace MyGL
{
class LogConsole
{
  public:
    LogConsole();

    void clear();

    template <typename... Args> void log(std::string_view fmt, Args &&...args)
    {
        std::string message = std::vformat(fmt, std::make_format_args(args...));
        items.push_back(std::move(message));
        if (auto_scroll)
            scroll_to_bottom = true;
    }

    void log(const std::string &msg);

    void draw(const char *title = "Log", bool *p_open = nullptr);

  private:
    std::vector<std::string> items;
    ImGuiTextFilter filter;
    bool auto_scroll = true;

    // Scroll-related variables
    bool scroll_to_bottom = false;
    bool was_at_bottom = true;
    const float scroll_tolerance = 1.0f;
};
} // namespace MyGL