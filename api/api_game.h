#pragma once

#include "i_api.h"

namespace ChaosEngine
{
	class api_game : public i_api
	{
	protected:
		api_game() {}
		bool setup_api(sol::state &slua);

	private:
		//sol::table _api_get_map_coords();
	};
}