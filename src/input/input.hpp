#pragma once

#include "keyboard_input.hpp"
#include "mouse_input.hpp"
#include "controller_input.hpp"


namespace input
{
	constexpr u32 MAX_CONTROLLERS = 1;


	class Input
	{
	public:
		KeyboardInput keyboard;
		MouseInput mouse;

		ControllerInput controllers[MAX_CONTROLLERS];
		u32 num_controllers;

		f32 dt_frame;
	};
}



