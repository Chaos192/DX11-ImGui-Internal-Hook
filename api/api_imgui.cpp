#include "api_imgui.h"
#include <libs/ImGui/imgui.h>

bool ChaosEngine::api_imgui::setup_api(sol::state &slua)
{
	auto namespace_imgui = slua["imgui"].get_or_create<sol::table>();

	namespace_imgui.set_function("set_data_ref_string", &ChaosEngine::api_imgui::_api_set_data_ref<const char *>, this);
	namespace_imgui.set_function("set_data_ref_bool", &ChaosEngine::api_imgui::_api_set_data_ref<bool>, this);
	namespace_imgui.set_function("set_data_ref_float", &ChaosEngine::api_imgui::_api_set_data_ref<float>, this);

	namespace_imgui.set_function("begin", &ChaosEngine::api_imgui::_api_begin, this);
	namespace_imgui.set_function("iend", &ChaosEngine::api_imgui::_api_iend, this);
	namespace_imgui.set_function("check_box", &ChaosEngine::api_imgui::_api_check_box, this);
	namespace_imgui.set_function("custom_check_box", &ChaosEngine::api_imgui::_api_custom_check_box, this);
	namespace_imgui.set_function("custom_Tab_button", &ChaosEngine::api_imgui::_api_CustomSubButton, this);
	namespace_imgui.set_function("custom_Bottom_button", &ChaosEngine::api_imgui::_api_CustomTopButton, this);
	namespace_imgui.set_function("input_text", &ChaosEngine::api_imgui::_api_input_text, this);
	namespace_imgui.set_function("slider_float", &ChaosEngine::api_imgui::_api_slider_float, this);
	namespace_imgui.set_function("text", [](const char *text) { ImGui::Text(text); });
	namespace_imgui.set_function("same_line", []() { ImGui::SameLine(); });
	namespace_imgui.set_function("button", [](const char *text) -> bool { return ImGui::Button(text); });
	namespace_imgui.set_function("separator", &ImGui::Separator);
	namespace_imgui.set_function("new_Line", &ImGui::NewLine);
	namespace_imgui.set_function("spacing", &ImGui::Spacing);
	namespace_imgui.set_function("push_id", static_cast<void(*)(int)>(&ImGui::PushID));
	namespace_imgui.set_function("pop_id", &ImGui::PopID);

	namespace_imgui.set_function("draw_text", [](const char *text, float x, float y, float r, float g, float b, float a)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImColor(r, g, b, a), text);
	});

	namespace_imgui.set_function("draw_line", [](float x1, float y1, float x2, float y2, float thickness, float r, float g, float b, float a)
	{
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a), thickness);
	});

	namespace_imgui.set_function("draw_rect", [](float x1, float y1, float x2, float y2, bool filled, float thickness, float r, float g, float b, float a)
	{
		if (filled)
		{
			ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a));
		}
		else
		{
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), ImColor(r, g, b, a), 0.f, 15, thickness);
		}
	});

	return true;
}

bool ChaosEngine::api_imgui::_api_begin(const char *text)
{
	++api_imgui::imgui_active_begin_count;
	return ImGui::Begin(text, nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
}

void ChaosEngine::api_imgui::_api_iend()
{
	--api_imgui::imgui_active_begin_count;
	return ImGui::End();
}

bool ChaosEngine::api_imgui::_api_check_box(const char *label)
{
	auto &container = *reinterpret_cast<bool *>(&this->imgui_data_ref[label]);
	ImGui::Checkbox(label, &container);
	return container;
}

bool ChaosEngine::api_imgui::_api_custom_check_box(const char* label)
{
	auto& container = *reinterpret_cast<bool*>(&this->imgui_data_ref[label]);
	ImGui::CheckboxCustom(label, &container);
	return container;
}

bool ChaosEngine::api_imgui::_api_CustomSubButton(const char* label)
{
	auto& container = *reinterpret_cast<bool*>(&this->imgui_data_ref[label]);
	return ImGui::CustomSubButton(label, &container);
}

bool ChaosEngine::api_imgui::_api_CustomTopButton(const char* label)
{
	auto& container = *reinterpret_cast<bool*>(&this->imgui_data_ref[label]);
	return ImGui::CustomTopButton(label);
}

const char *ChaosEngine::api_imgui::_api_input_text(const char *label)
{
	auto &container = this->imgui_data_ref[label];
	ImGui::InputText(label, container, sizeof(container));
	return container;
}

float ChaosEngine::api_imgui::_api_slider_float(const char *label, float min, float max)
{
	auto &container = *reinterpret_cast<float *>(&this->imgui_data_ref[label]);
	ImGui::SliderFloat(label, &container, min, max, "%.1f");
	return container;
}
