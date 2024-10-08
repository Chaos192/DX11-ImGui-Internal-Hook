#include "api_mem.h"
#include <utils/mem.h>
#include <Windows.h>
#include "../log_manager.h"
#include <libs/sol/sol.hpp>

bool ChaosEngine::api_mem::setup_api(sol::state &slua)
{
	auto namespace_mem = slua["mem"].get_or_create<sol::table>();
	namespace_mem.set_function("ida_scan", &ChaosEngine::api_mem::_api_ida_scan, this);
	namespace_mem.set_function("patch", &ChaosEngine::api_mem::_api_patch, this);
	namespace_mem.set_function("read_uint", &ChaosEngine::api_mem::_api_read_uint, this);
	namespace_mem.set_function("write_uint", &ChaosEngine::api_mem::_api_write_uint, this);
	namespace_mem.set_function("calc_rel2abs32", &ChaosEngine::api_mem::_api_calc_rel2abs32, this);

	return true;
}

sol::object ChaosEngine::api_mem::_api_ida_scan(std::uintptr_t start_adr, std::size_t size, std::string ida_pattern)
{
	auto result = utils::ida_scan(reinterpret_cast<void *>(start_adr), size, ida_pattern.c_str());

	if (!result)
		return sol::nil;

	return sol::make_object(this->get_lua_state(), reinterpret_cast<std::uintptr_t>(result));
}

int ChaosEngine::api_mem::_api_patch(std::uintptr_t addr, std::vector<std::uint8_t> byte_array)
{
	// TODO: lua enum this
	enum result_e
	{
		SUCCESSFUL = 0,
		ALREADY_PATCHED = 1,
		PROT_CHANGE_XRW_FAILED = 2,
		PROT_CHANGE_RESTORE_FAILED = 3
	};

	// TODO: store in a global static list all the patched memory and check for collisions
	// TODO: use win api helper

	void *addr_void = reinterpret_cast<void *>(addr);
	std::size_t byte_arr_size = byte_array.size();
	DWORD old_prot = 0;

	if (!VirtualProtect(addr_void, byte_arr_size, PAGE_EXECUTE_READWRITE, &old_prot))
		return result_e::PROT_CHANGE_XRW_FAILED;

	std::memcpy(addr_void, byte_array.data(), byte_arr_size);

	if (!VirtualProtect(addr_void, byte_arr_size, old_prot, &old_prot))
		return result_e::PROT_CHANGE_RESTORE_FAILED;

	return result_e::SUCCESSFUL;
}

std::uint64_t ChaosEngine::api_mem::_api_read_uint(std::uintptr_t addr, std::size_t prim_t_size)
{
	std::memcpy(&prim_t_size, reinterpret_cast<void *>(addr), prim_t_size); // optimization!
	return prim_t_size;
}

void ChaosEngine::api_mem::_api_write_uint(std::uintptr_t addr, std::size_t prim_t_size, std::uint64_t value)
{
	std::memcpy(reinterpret_cast<void *>(addr), &value, prim_t_size);
}

std::uintptr_t ChaosEngine::api_mem::_api_calc_rel2abs32(std::uintptr_t inst_addr, std::size_t inst_size)
{
	return reinterpret_cast<std::uintptr_t>(utils::calc_rel2abs32(reinterpret_cast<void *>(inst_addr), inst_size));
}
