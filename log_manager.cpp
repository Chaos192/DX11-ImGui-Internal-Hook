#include "log_manager.h"

static std::deque<ChaosEngine::log_manager::log_cont> logs;

const std::deque<ChaosEngine::log_manager::log_cont> &ChaosEngine::log_manager::get_logs()
{
	return logs;
}

void ChaosEngine::log_manager::push_log(std::string txt, log_type_t type)
{
	logs.emplace_back(std::move(txt), type);
}

void ChaosEngine::log_manager::clear_log()
{
	logs.clear();
}
