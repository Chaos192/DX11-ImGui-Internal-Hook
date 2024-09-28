#include "api_ChaosEngine.h"
#include <utils/macro.h>
#include "../log_manager.h"
#include <filesystem>
#include "../callback_manager.h"

bool ChaosEngine::api_ChaosEngine::setup_api(sol::state &slua)
{
	auto namespace_ChaosEngine = slua["ChaosEngine"].get_or_create<sol::table>();
	namespace_ChaosEngine.set_function("log", &api_ChaosEngine::_api_log, this);
	namespace_ChaosEngine.set_function("register_callback", &api_ChaosEngine::_api_register_callback, this);
	namespace_ChaosEngine.set_function("get_script_dir", &api_ChaosEngine::_api_get_script_dir, this);
	namespace_ChaosEngine.set_function("create_detached_thread", &api_ChaosEngine::_api_create_detached_thread, this);

	return true;
}

void ChaosEngine::api_ChaosEngine::script_push_log(std::string msg)
{
	DEBUG_COUT("\n[SCRIPT] " << msg);
	ChaosEngine::log_manager::push_log(std::move(msg), ChaosEngine::log_manager::log_type::SCRIPTS);
}

void ChaosEngine::api_ChaosEngine::_api_log(std::string txt)
{
	this->script_push_log(std::move(txt));
}

bool ChaosEngine::api_ChaosEngine::_api_register_callback(std::string id, sol::function callback)
{
	return ChaosEngine::callback_manager::register_luafn(utils::hash_fnv1a(id.c_str()), this, callback);
}

std::string ChaosEngine::api_ChaosEngine::_api_get_script_dir()
{
	auto abs = std::filesystem::canonical(this->get_filepath()).string();
	return abs.substr(0, abs.find_last_of("/\\") + 1);
}

void ChaosEngine::api_ChaosEngine::_api_create_detached_thread(sol::function fn)
{
	return std::thread([](sol::function fn)
	{
		fn();
	}, std::move(fn)).detach();
}
