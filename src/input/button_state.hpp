#pragma once

#include "../util/types.hpp"


namespace input
{
	constexpr u32 N_STATES = 3;
	

	union ButtonState
	{
		b32 states[N_STATES];
		struct
		{
			b32 pressed;
			b32 is_down;
			b32 raised;
		};


	};
}


