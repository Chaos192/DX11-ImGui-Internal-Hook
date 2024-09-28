#pragma once

#include <vector>
#include <utility>
#include "script.h"

namespace ChaosEngine::script_manager
{
	const std::vector<ChaosEngine::script> &get_scripts();
	bool script_autoexec(std::string_view file_path);
	bool script_import(std::string_view file_path, ChaosEngine::script **script_instance_out = nullptr);

	void unload_all_scripts();
}