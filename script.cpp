#pragma once

#include "script.h"

#include <Windows.h>
#include <utils/macro.h>
#include "log_manager.h"
#include "callback_manager.h"
#include "api/api_mem.h"

/// <summary>
/// RAII implementation of applying script state value
/// Reasoning: im not gonna type [this->state_value = ChaosEngine::script::state::UNLOADED;] for every [return false;] in ChaosEngine::script::load()
/// </summary>
class _load_state_trigger
{
public:
	_load_state_trigger(ChaosEngine::script::state &state_value_)
		: state_value(state_value_)
	{
		this->state_value = ChaosEngine::script::state::LOADING;
	}

	~_load_state_trigger()
	{
		if (this->state_value == ChaosEngine::script::state::LOADING)
			this->state_value = ChaosEngine::script::state::UNLOADED;
	}

	void set_state_loaded()
	{
		this->state_value = ChaosEngine::script::state::LOADED;
	}

private:
	ChaosEngine::script::state &state_value;
};

ChaosEngine::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
	DEBUG_COUT("\nConstructed ChaosEngine::script object # " << this->filepath << " # " << filepath_);
	this->filename = this->filepath.substr(this->filepath.find_last_of("/\\") + 1);
	this->load_mconfig();
}

bool ChaosEngine::script::load()
{
	if (!this->script_file_exists())
		return false;

	_load_state_trigger state_trigger(this->current_state);

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	temp_lua_state->open_libraries(sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::base, sol::lib::bit32, sol::lib::coroutine, sol::lib::io, sol::lib::os, sol::lib::package);

	if (auto load_res = temp_lua_state->script_file(this->filepath); !load_res.valid())
	{
		sol::error err_load_res = load_res;
		ChaosEngine::log_manager::push_log(err_load_res.what(), ChaosEngine::log_manager::log_type::LUA);
		return false;
	}

	sol::function callback_onload;
	{
		if (auto get_onload_res = temp_lua_state->operator[]("on_load"); get_onload_res.valid())
		{
			callback_onload = get_onload_res;
		}
		else
		{
			ChaosEngine::log_manager::push_log("Callback \"on_load\" not found.", ChaosEngine::log_manager::log_type::ChaosEngine);
			return false;
		}
	}

	if (!ChaosEngine::script_apis::setup_all_apis(*temp_lua_state.get()))
	{
		ChaosEngine::log_manager::push_log("Failed to load API in lua state", ChaosEngine::log_manager::log_type::ChaosEngine);
		return false;
	}

	// upon success we can move the unique_ptr from this scope to the class
	this->lua_state = std::move(temp_lua_state);
	state_trigger.set_state_loaded();
	++ChaosEngine::script::count_loaded_scripts;

	DEBUG_COUT("\nLoaded lua: " << this->filepath);
	ChaosEngine::log_manager::push_log("Script Loaded: " + this->filename, ChaosEngine::log_manager::log_type::ChaosEngine);

	// Load configs
	this->load_mconfig();

	// Run the script
	callback_onload();
	return true;
}

bool ChaosEngine::script::unload()
{
	if (!this->lua_state.operator bool())
		return false;

	this->current_state = script::state::UNLOADING;

	ChaosEngine::callback_manager::get_callbacks().on_unload.dispatch_filtered(this); // this is inefficient ik
	ChaosEngine::callback_manager::disable_api_callbacks_for_script(this);

	this->lua_state.reset();
	this->current_state = ChaosEngine::script::state::UNLOADED;
	--ChaosEngine::script::count_loaded_scripts;

	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	ChaosEngine::log_manager::push_log("Script Unloaded: " + this->filename, ChaosEngine::log_manager::log_type::ChaosEngine);
	return true;
}

bool ChaosEngine::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

ChaosEngine::script::operator bool() const
{
	return this->lua_state.operator bool();
}

void ChaosEngine::script::load_mconfig()
{
	sol::state temp_state;
	sol::state *sel_state = this->lua_state.get(); // load the lua state of the instance
	// check if there's a lua state, if there's not one yet then use a temporary state;
	if (!this->lua_state)
	{
		temp_state.script_file(this->filepath);
		sel_state = &temp_state;
	}
	sol::state &_l = *sel_state;

	#define _ChaosEngine_LOAD_CONFIG_CTMDEF(name, def_val) this->config. ## name ## = _l["_mconfig"][#name].get_or(def_val)
	#define _ChaosEngine_LOAD_CONFIG(name) _ChaosEngine_LOAD_CONFIG_CTMDEF(name, this->config.##name##)

	_ChaosEngine_LOAD_CONFIG_CTMDEF(name, this->get_filename());
	_ChaosEngine_LOAD_CONFIG(description);

	#undef _ChaosEngine_LOAD_CONFIG
	#undef _ChaosEngine_LOAD_CONFIG_CTMDEF
}

const std::string_view ChaosEngine::script::get_filepath() const
{
	return this->filepath;
}

const std::string_view ChaosEngine::script::get_filename() const
{
	return this->filename;
}

const ChaosEngine::script::state ChaosEngine::script::get_current_state() const
{
	return this->current_state;
}

const ChaosEngine::script_config &ChaosEngine::script::get_config() const
{
	return this->config;
}

sol::state &ChaosEngine::script::get_lua_state()
{
	return *this->lua_state.get();
}

void ChaosEngine::script::script_push_log(std::string msg)
{
	api_ChaosEngine::script_push_log(std::move(msg));
}

#define _CASE_STATE_TO_RET_STR(state_) case ChaosEngine::script::state:: ## state_ ## : return #state_
const char *ChaosEngine::script::state_to_cstr(script::state state_)
{
	switch (state_)
	{
		_CASE_STATE_TO_RET_STR(UNLOADING);
		_CASE_STATE_TO_RET_STR(UNLOADED);
		_CASE_STATE_TO_RET_STR(LOADING);
		_CASE_STATE_TO_RET_STR(LOADED);
	}

	return nullptr;
}
#undef _CASE_STATE_TO_RET_STR