#pragma once

#include <libs/sol/sol.hpp>
#include <string>

namespace ChaosEngine
{
	class i_api
	{
	public:
		virtual sol::state &get_lua_state() = 0;
		virtual void script_push_log(std::string msg) = 0;
		virtual const std::string_view get_filepath() const = 0;
	};
}