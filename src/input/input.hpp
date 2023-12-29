#pragma once

#include "keyboard_input.hpp"
#include "mouse_input.hpp"
#include "controller_input.hpp"


namespace input
{
#ifdef SINGLE_CONTROLLER
	constexpr u32 MAX_CONTROLLERS = 1;
#else
	constexpr u32 MAX_CONTROLLERS = 400;
#endif


	class Input
	{
	public:
		KeyboardInput keyboard;
		MouseInput mouse;
		
		u32 num_controllers;

		f32 dt_frame;

#ifdef SINGLE_CONTROLLER

		union
		{
			ControllerInput controller;
			ControllerInput controllers[MAX_CONTROLLERS];
		};		
		
#else
		ControllerInput controllers[MAX_CONTROLLERS];
		
#endif
	};
}



