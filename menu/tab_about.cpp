#include "tab_about.h"

#include <libs/ImGui/imgui.h>
#include "../git_info.h"

void ChaosEngine::Tabs::tab_about::render_tab()
{
    if (ImGui::BeginChild("About"))
    {
        ImGui::Text(
            "Scripting API and Framework\n"
            "\n"

            "Build type: "
            #if defined(_DEBUG) && !defined(NDEBUG)
            "Debug"
            #elif defined(NDEBUG) && !defined(_DEBUG)
            "Release"
            #else
            "Unknown"
            #endif
            "\n"

            "Build Date: " __DATE__ " " __TIME__ "\n"
        );
        ImGui::EndChild();
    }
}