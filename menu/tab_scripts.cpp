#include "tab_scripts.h"
#include <libs/ImGui/imgui.h>
#include <string>
#include "../script_manager.h"
#include "../helpers/imgui_prompts.h"
#include <utils/misc_utils.h>
#include <thread>
#include <mutex>
#include "Menu.h"

// TODO: shift click / ctrl click for multi select

static const char         *import_prompt_error_message         = "No error message provided.";
static bool                import_prompt_error_message_visible = false;
static utils::fader_float  import_prompt_error_message_fader   = utils::fader_float(1000, 3000);

void callback_import_prompt_ui()
{
    static char buffer_import[256] = { '\0' };

    ImGui::Text("Script Path:");
    ImGui::SameLine();
    bool input_enter_keyed = ImGui::InputText("##script_import_buffer", buffer_import, sizeof(buffer_import), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Import") || input_enter_keyed)
    {
        if (ChaosEngine::script_manager::script_import(buffer_import))
        {
            std::memset(buffer_import, '\0', sizeof(buffer_import));
            ImGui::CloseCurrentPopup();
        }
        else
        {
            import_prompt_error_message = "Failed to import script!";
            import_prompt_error_message_fader.mark();
            import_prompt_error_message_visible = true;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();

    if (import_prompt_error_message_visible)
    {
        float errmsg_alpha_scale = import_prompt_error_message_fader.get();
        if (errmsg_alpha_scale > 0.f)
        {
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, ImVec4{ 1.f, 0.f, 0.f, errmsg_alpha_scale });
            ImGui::TextWrapped(import_prompt_error_message);
            ImGui::PopStyleColor();
        }
        else
        {
            import_prompt_error_message_visible = false;
        }
    }
}
static helpers::imgui_popup_modal import_prompt = helpers::imgui_popup_modal("Import Script", &callback_import_prompt_ui);

enum class script_state
{
    LOAD,
    UNLOAD
};

static std::mutex loading_mutex;
static void helper_nthread_set_script_state(const ChaosEngine::script &script, script_state req_state)
{
    std::thread([req_state](ChaosEngine::script *script)
    {
        const std::lock_guard lg(loading_mutex);
        switch (req_state)
        {
            case script_state::LOAD:
                script->load();
                break;
            case script_state::UNLOAD:
                script->unload();
                break;
        }
    }, const_cast<ChaosEngine::script *>(&script)).detach();
}

void ChaosEngine::Tabs::tab_scripts::render_tab()
{
	if (ImGui::BeginChild("Scripts"))
	{
        if (ImGui::BeginTable("##script_table", 3, ImGuiTableFlags_::ImGuiTableFlags_ScrollY, { 0.f , ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - 35.f }))
		{
            ImGui::TableSetupColumn("Script Name");
            ImGui::TableSetupColumn("Notice");
            ImGui::TableSetupColumn("Status");
            ImGui::TableHeadersRow();

            for (const auto &script : ChaosEngine::script_manager::get_scripts())
            {
                const auto &config = script.get_config();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Selectable(config.name.c_str(), &script._tab_script_selected, ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns);
                if (ImGui::IsItemHovered() && !config.description.empty()) ImGui::SetTooltip(config.description.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(script._tab_script_notice.c_str());
                ImGui::TableNextColumn();
                ImGui::Text(ChaosEngine::script::state_to_cstr(script.get_current_state()));
            }

			ImGui::EndTable();
		}

        ImGui::Separator();

        if (ImGui::Button("Load"))
        {
            for (const auto &script : ChaosEngine::script_manager::get_scripts())
            {
                if (script._tab_script_selected && script.get_current_state() == ChaosEngine::script::state::UNLOADED)
                    helper_nthread_set_script_state(script, script_state::LOAD);
                Menu::isLoaded = true;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Unload"))
        {
            for (const auto &script : ChaosEngine::script_manager::get_scripts())
            {
                if (script._tab_script_selected && script.get_current_state() == ChaosEngine::script::state::LOADED)
                    helper_nthread_set_script_state(script, script_state::UNLOAD);
                Menu::isLoaded = false;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Deselect All"))
        {
            for (const auto &script : ChaosEngine::script_manager::get_scripts())
                script._tab_script_selected = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Import Script"))
        {
            import_prompt.show();
        }

		ImGui::EndChild();
	}
}