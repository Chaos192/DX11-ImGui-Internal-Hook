#pragma once

#include "i_api.h"
#include <libs/sol/sol.hpp>

namespace ChaosEngine
{
	class api_win : public i_api
	{
	protected:
		api_win() {}
		bool setup_api(sol::state &slua);

	private:
		sol::object _api_find_module(std::wstring module_name);
		sol::table _api_get_all_modules();
	};
}