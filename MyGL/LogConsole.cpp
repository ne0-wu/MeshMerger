#include "LogConsole.h"

MyGL::LogConsole::LogConsole() {}

void MyGL::LogConsole::clear()
{
    items.clear();
}

void MyGL::LogConsole::log(const std::string &msg)
{
    log("{}", msg);
}

void MyGL::LogConsole::draw(const char *title, bool *p_open)
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin(title, p_open);

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &auto_scroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool should_clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool should_copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

    ImGui::Separator();

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (should_clear)
        clear();
    if (should_copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    for (const auto &item : items)
    {
        if (!filter.PassFilter(item.c_str()))
            continue;
        ImGui::TextUnformatted(item.c_str());
    }
    ImGui::PopStyleVar();

    // Scroll to bottom if needed
    if (scroll_to_bottom && was_at_bottom)
    {
        scroll_to_bottom = false;
        ImGui::SetScrollHereY(1.0f);
    }
    was_at_bottom = ImGui::GetScrollMaxY() - ImGui::GetScrollY() < scroll_tolerance;

    ImGui::EndChild();
    ImGui::End();
}
