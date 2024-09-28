#include "script_manager.h"

#include <Windows.h>
#include <libs/ImGui/imgui.h>
#include <cstddef>
#include <cstdint>
#include <utils/macro.h>

static std::vector<ChaosEngine::script> script_instances;

const std::vector<ChaosEngine::script> &ChaosEngine::script_manager::get_scripts()
{
    return script_instances;
}

bool ChaosEngine::script_manager::script_autoexec(std::string_view file_path)
{
    if (!std::filesystem::exists(file_path))
        return false;

    sol::state autoexec_script;

    auto namespace_ChaosEngine = autoexec_script["ChaosEngine"].get_or_create<sol::table>();
    namespace_ChaosEngine.set_function("import", [](const char *file_path) -> bool
    {
        return ChaosEngine::script_manager::script_import(file_path);
    });

    namespace_ChaosEngine.set_function("load", [](const char *file_path) -> bool
    {
        if (ChaosEngine::script *script = nullptr; ChaosEngine::script_manager::script_import(file_path, &script))
            return script->load();

        return false;
    });

    autoexec_script.script_file(file_path.data());

    return true;
}

bool ChaosEngine::script_manager::script_import(std::string_view file_path, ChaosEngine::script **script_instance_out)
{
    if (!std::filesystem::exists(file_path))
        return false;

    for (auto &script : script_instances)
        if (script.get_filepath() == file_path)
            return false;

    auto &loaded_script = script_instances.emplace_back(file_path);
    if (script_instance_out)
        *script_instance_out = &loaded_script;

    return true;
}

void ChaosEngine::script_manager::unload_all_scripts()
{
    for (auto &script : script_instances)
        script.unload();
}
