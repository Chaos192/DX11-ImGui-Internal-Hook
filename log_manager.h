#pragma once

#include <string>
#include <deque>

namespace ChaosEngine::log_manager
{
	using log_type_t = unsigned char;

	struct log_type
	{
		enum : unsigned char
		{
			GAME,
			SCRIPTS,
			LUA,
			ChaosEngine,
			UNSPEC,
			_SIZE
		};
	};

	struct log_cont
	{
		std::string txt;
		log_type_t type;
	};

	const std::deque<ChaosEngine::log_manager::log_cont> &get_logs();
	void push_log(std::string txt, log_type_t type = log_type::UNSPEC);
	void clear_log();
}