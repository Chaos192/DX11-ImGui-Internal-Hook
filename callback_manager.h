#pragma once

#include <utils/hash.h>
#include <libs/sol/sol.hpp>
#include <vector>
#include <script.h>
#include <mutex>

// For performance and lazy reasons
// it would be better to just keep the registered callbacks in the vector
// and just check their parent scripts if they're still loaded and active
// having to safely remove a registered callback in a vector would be too much
// effort when we can just keep it there just incase the script gets loaded again later
// not only will this eliminate the need of mutex locks and checks when clearing lua callbacks

namespace ChaosEngine::callback_manager
{
	struct api_callback
	{
		struct lua_callback
		{
			ChaosEngine::script *parent = nullptr;
			bool active = false;
			sol::function lua_func = sol::nil;

			bool valid()
			{
				return parent && parent->get_current_state() == ChaosEngine::script::state::LOADED && active;
			}
		};

		api_callback(const utils::fnv1a64_t _hashed_name)
			: hashed_name(_hashed_name) {}

		const utils::fnv1a64_t    hashed_name;
		std::vector<lua_callback> lua_callbacks;
		int                       active_callbacks_count = 0;

		template <typename... vargs_t>
		void dispatch(vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				cb.lua_func(args...);

				if (++total_dispatched == this->active_callbacks_count)
					return;
			}
		}

		template <typename... vargs_t>
		void dispatch_filtered(ChaosEngine::script *parent, vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return;

			for (auto &cb : this->lua_callbacks)
			{
				if (cb.parent != parent || !cb.active)
					continue;

				cb.lua_func(args...);
			}
		}

		template <typename... vargs_t>
		bool dispatch_cancelable(vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return false;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				if (sol::protected_function_result cb_res = cb.lua_func(args...); cb_res.valid())
				{
					sol::optional<bool> res = cb_res;
					if (res && *res)
						return true;
				}

				if (++total_dispatched == this->active_callbacks_count)
					return false;
			}

			return false;
		}

		using delegate_t = bool(*)(const ChaosEngine::script &, const sol::function &);
		bool dispatch_delegate(delegate_t delegate) const
		{
			if (!this->active_callbacks_count)
				return false;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				if (delegate(*cb.parent, cb.lua_func))
					return true;

				if (++total_dispatched == this->active_callbacks_count)
					return false;
			}
		}

		template <typename return_t, typename... vargs_t>
		bool dispatch_returnable(return_t &out_return, vargs_t... args) const
		{
			if (!this->active_callbacks_count)
				return false;

			int total_dispatched = 0;
			for (auto &cb : this->lua_callbacks)
			{
				if (!cb.active)
					continue;

				if (sol::protected_function_result cb_res = cb.lua_func(args...); cb_res.valid())
				{
					sol::optional<return_t> res = cb_res;
					if (res)
						out_return = *res;
					return true;
				}

				if (++total_dispatched == this->active_callbacks_count)
					return false;
			}

			return false;
		}
	};

	#define _ChaosEngine_DECLARE_API_CALLBACK(name) api_callback name = api_callback(utils::hash_fnv1a(#name));
	struct _api_callback_container
	{
		_ChaosEngine_DECLARE_API_CALLBACK(on_imgui_draw)
		_ChaosEngine_DECLARE_API_CALLBACK(dx_draw)
		_ChaosEngine_DECLARE_API_CALLBACK(dx_drawindexed)
		_ChaosEngine_DECLARE_API_CALLBACK(on_unload)
		_ChaosEngine_DECLARE_API_CALLBACK(menu_imgui_tab)
		_ChaosEngine_DECLARE_API_CALLBACK(on_key)
		//_ChaosEngine_DECLARE_API_CALLBACK(on_animator_speed)
	};
	#undef _ChaosEngine_DECLARE_API_CALLBACK

	bool register_luafn(utils::fnv1a64_t hashed_name, void *parent, sol::function fn);
	void disable_api_callbacks_for_script(ChaosEngine::script *parent);
	const _api_callback_container &get_callbacks();

	inline bool       use_mut_on_imgui_draw = true;
	inline std::mutex mut_on_imgui_draw;
}