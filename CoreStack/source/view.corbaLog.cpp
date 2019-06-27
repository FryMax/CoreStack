#include "pch.h"
#include "imview.h"

struct call
{
    uint64_t process_id;
    uint64_t thread_id;
    std::string target;
    std::string function;
};

void imview::corbaLog(view_context*)
{
    static net::UdpSocket sock;
    static std::vector<call> buffer;

    static std::map<uint64_t,    uint64_t> by_proc;
    static std::map<uint64_t,    uint64_t> by_thread;
    static std::map<std::string, uint64_t> by_target;
    static std::map<std::string, uint64_t> by_function;

    MAKE_ONCE(
        sock.ListenOnPort(228);
    );

    for (auto& it : sock.popResivedData())
    {
        // "process: * |thread: * |target: * |operation: *
        const auto copy = it;

        const auto grub = [&](const std::string& s)
        {
            const auto begin = copy.find(s) + s.size();
            const auto end = copy.find("|", begin);
            return copy.substr(begin, end - begin);
        };

        call c
        {
            (uint64_t)std::stoi(grub("process:")),
            (uint64_t)std::stoi(grub("thread:")),
            grub("target:"),
            grub("operation:"),
        };

        by_proc[c.process_id]++;
        by_thread[c.thread_id]++;
        by_target[c.target]++;
        by_function[c.function]++;

        buffer.emplace_back(std::move(c));
    };

    // draw
    //----------------------------
    static bool auto_clean_log = false;
    if (ImGui::Button("Clean log") || auto_clean_log)
    {
        buffer.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("AutoClean", &auto_clean_log);

    if (ImGui::Button("Clean stats"))
    {
        by_proc.clear();
        by_thread.clear();
        by_target.clear();
        by_function.clear();
    }

    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
    {
        int i = 0;
        if (ImGui::BeginTabItem("By process")) { i = 1; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("By thread")) { i = 2; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("By target")) { i = 3; ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("By function")) { i = 4; ImGui::EndTabItem(); }

        ImGui::BeginChildFrame(ImGui::GetID("_child_map"), FLOAT2{ -1.0, ImGui::GetContentRegionAvail().y * 0.65f });
        {
            ImGui::Columns(2, "_columns");
            {
                // ImItem::Text("name");  ImGui::NextColumn();
                // ImItem::Text("count"); ImGui::NextColumn();
                // ImGui::Separator();

                const auto draw_map = [&](const auto& map)
                {
                    for (const auto& it : map)
                    {
                        ImItem::Text(str::format::insert("{}", it.first));  ImGui::NextColumn();
                        ImItem::Text(str::format::insert("{}", it.second)); ImGui::NextColumn();
                    }
                };

                switch (i)
                {
                default:
                case 0: ImGui::Text("error?"); break;
                case 1: draw_map(by_proc);     break;
                case 2: draw_map(by_thread);   break;
                case 3: draw_map(by_target);   break;
                case 4: draw_map(by_function); break;
                }

                ImGui::Columns(1, "_columns_end");
            }
            ImGui::EndChildFrame();
        }
        ImGui::EndTabBar();
    }

    ImGui::Text("Log:");
    ImGui::BeginChildFrame(ImGui::GetID("_child_log"), FLOAT2{ -1.0, ImGui::GetContentRegionAvail().y });

    static const char* selected = nullptr;
    bool selected_exist = false;

    for (const auto& it : buffer)
    {
        const auto s = str::format::insert("{:>6}/{:>6} : {} -> {}", it.process_id, it.thread_id, it.target, it.function);
        const bool current_selected = selected == s.c_str();

        if (current_selected)
            selected_exist = true;

        if (ImGui::Selectable(s.c_str(), current_selected))
            selected = s.c_str();

        if (selected == s.c_str())
            selected_exist = true;
    }

    // selected was removed
    if (!selected_exist)
        selected = nullptr;

    ImGui::EndChildFrame();
}