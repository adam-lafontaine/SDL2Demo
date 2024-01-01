#pragma once

#include "input.hpp"


/* helpers */

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


	template <typename T>
	inline void copy_vector_state(VectorState<T> const& src, VectorState<T>& dst)
	{
		copy_vec_2d(src.vec, dst.vec);
		copy_vec_2d(src.unit_direction, dst.unit_direction);
		dst.angle_rad = src.angle_rad;
		dst.magnitude = src.magnitude;
	}



}


/* keyboard */

namespace input
{
	inline void copy_keyboard_state(KeyboardInput const& src, KeyboardInput& dst)
	{
		for (u32 i = 0; i < N_KEYBOARD_KEYS; ++i)
		{
			copy_button_state(src.keys[i], dst.keys[i]);
		}
	}
}


/* mouse */

namespace input
{
	
	inline void copy_mouse_position(MouseInput const& src, MouseInput& dst)
	{
#if MOUSE_POSITION
		copy_vec_2d(src.window_pos, dst.window_pos);
#endif
	}


	inline void copy_mouse_wheel(MouseInput const& src, MouseInput& dst)
	{
#if MOUSE_WHEEL
		copy_vec_2d(src.wheel, dst.wheel);
#endif
	}	


	inline void copy_mouse_state(MouseInput const& src, MouseInput& dst)
	{
		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}

		copy_mouse_position(src, dst);
		copy_mouse_wheel(src, dst);
	}	


	inline void reset_mouse(MouseInput& mouse)
	{
		for (u32 i = 0; i < N_MOUSE_BUTTONS; ++i)
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


/* controller */

namespace input
{
	inline void copy_controller_buttons(ControllerInput const& src, ControllerInput& dst)
	{
		for (u32 i = 0; i < N_CONTROLLER_BUTTONS; ++i)
		{
			copy_button_state(src.buttons[i], dst.buttons[i]);
		}
	}


	inline void copy_controller_axes(ControllerInput const& src, ControllerInput& dst)
	{
#if CONTROLLER_AXIS_STICK_LEFT
		copy_vector_state(src.stick_left, dst.stick_left);
#endif

#if CONTROLLER_AXIS_STICK_RIGHT
		copy_vector_state(src.stick_right, dst.stick_right);
#endif
	}


	inline void copy_controller_triggers(ControllerInput const& src, ControllerInput& dst)
	{
#if CONTROLLER_TRIGGER_LEFT
		dst.trigger_left = src.trigger_left;
#endif

#if CONTROLLER_TRIGGER_RIGHT
		dst.trigger_right = src.trigger_right;
#endif
	}


	inline void copy_controller_state(ControllerInput const& src, ControllerInput& dst)
	{
		copy_controller_buttons(src, dst);
		copy_controller_axes(src, dst);
		copy_controller_triggers(src, dst);
	}
}

