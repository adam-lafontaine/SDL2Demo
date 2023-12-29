#pragma once

#include "../input/input.hpp"

namespace app
{
    constexpr auto APP_TITLE = "SDL2 Demo";


    class  ScreenBuffer
	{
	public:
		void* memory;
		u32 width;
		u32 height;
		u32 bytes_per_pixel;
	};

}