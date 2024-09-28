#include "tab_misc.h"
#include <libs/ImGui/imgui.h>
#include <Windows.h>
#include "../callback_manager.h"
#include <Menu.h>

static bool toggle_fps_counter = true;

void ChaosEngine::Tabs::tab_misc::render_tab()
{
	if (ImGui::BeginChild("Misc"))
	{
		ImGui::CheckboxCustom("Lock on_imgui_draw callback thread with mutex", &ChaosEngine::callback_manager::use_mut_on_imgui_draw);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Disabling this might make things faster but might cause instability.");

		ImGui::Text("Information window: ");
		ImGui::CheckboxCustom("Display FPS", &toggle_fps_counter);

		if (static auto& imgui_io = ImGui::GetIO(); toggle_fps_counter)
			ImGui::Text("FPS: %.1f", imgui_io.Framerate);

		ImGui::EndChild();
	}
}

//void ChaosEngine::Tabs::tab_misc::render_window()
//{
//	if (!toggle_fps_counter)
//		return;
//
//	ImGuiWindowFlags w_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
//	if (!ChaosEngine::is_open)
//		w_flags |= ImGuiWindowFlags_NoInputs;
//
//	ImGui::SetNextWindowPos({ 4.f, 4.f }, ImGuiCond_Once);
//	if (ImGui::Begin("##info_window", nullptr, w_flags))
//	{
//		if (static auto &imgui_io = ImGui::GetIO(); toggle_fps_counter)
//			ImGui::Text("FPS: %.1f", imgui_io.Framerate);
//	}
//	ImGui::End();
//}
