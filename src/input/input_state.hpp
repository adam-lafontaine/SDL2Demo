#pragma once

#include "input.hpp"


namespace input
{
	inline void record_button_input(ButtonState const& old_state, ButtonState& new_state, b32 is_down)
	{
		new_state.pressed = !old_state.is_down && is_down;
		new_state.is_down = is_down;
		new_state.raised = old_state.is_down && !is_down;
	}


	inline void reset_button_state(ButtonState& state)
	{
		for (u32 i = 0; i < N_STATES; ++i)
		{
			state.states[i] = 0;
		}
	}


	inline void copy_button_state(ButtonState const& src, ButtonState& dst)
	{
		dst.is_down = src.is_down;
		dst.pressed = 0;
		dst.raised = 0;
	}


	template <typename T>
	inline void copy_vec_2d(Vec2D<T> const& src, Vec2D<T>& dst)
	{
		dst.x = src.x;
		dst.y = src.y;
	}


	inline void copy_controller_state(ControllerInput const& src, ControllerInput& dst)
	{
		for (u32 i = 0; i < CONTROLLER_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}

#if CONTROLLER_STICK_LEFT
		copy_vec_2d(src.stick_left, dst.stick_left);
#endif

#if CONTROLLER_STICK_RIGHT
		copy_vec_2d(src.stick_right, dst.stick_right);
#endif

#if CONTROLLER_TRIGGER_LEFT
		dst.trigger_left = src.trigger_left;
#endif

#if CONTROLLER_TRIGGER_RIGHT
		dst.trigger_right = src.trigger_right;
#endif
	}


	inline void copy_keyboard_state(KeyboardInput const& src, KeyboardInput& dst)
	{
		for (u32 i = 0; i < KEYBOARD_KEYS; ++i)
		{
			copy_button_state(src.keys[i], dst.keys[i]);
		}
	}


	inline void copy_mouse_state(MouseInput const& src, MouseInput& dst)
	{
		for (u32 i = 0; i < MOUSE_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}

#if MOUSE_POSITION
		dst.window_pos.x = src.window_pos.x;
		dst.window_pos.y = src.window_pos.y;
#endif

#if MOUSE_WHEEL
		dst.wheel.x = src.wheel.x;
		dst.wheel.y = src.wheel.y;
#endif
	}


	inline void reset_mouse(MouseInput& mouse)
	{

		for (u32 i = 0; i < MOUSE_BUTTONS; ++i)
		{
			reset_button_state(mouse.buttons[i]);
		}

#if MOUSE_POSITION
		mouse.window_pos.x = 0;
		mouse.window_pos.y = 0;
#endif

#if MOUSE_WHEEL
		mouse.wheel.x = 0;
		mouse.wheel.y = 0;
#endif
	}
}

