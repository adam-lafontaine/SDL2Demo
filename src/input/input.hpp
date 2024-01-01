#pragma once

#include "button_state.hpp"

#include "keyboard_input.hpp"
#include "mouse_input.hpp"
#include "controller_input.hpp"


/* keyboard */

namespace input
{
	union KeyboardInput
	{
		ButtonState keys[KEYBOARD_KEYS];
		
		struct
		{

#if KEYBOARD_A
			ButtonState a_key;
#endif
#if KEYBOARD_B
			ButtonState b_key;
#endif
#if KEYBOARD_C
			ButtonState c_key;
#endif
#if KEYBOARD_D
			ButtonState d_key;
#endif
#if KEYBOARD_E
			ButtonState e_key;
#endif
#if KEYBOARD_F
			ButtonState f_key;
#endif
#if KEYBOARD_G
			ButtonState g_key;
#endif
#if KEYBOARD_H
			ButtonState h_key;
#endif
#if KEYBOARD_I
			ButtonState i_key;
#endif
#if KEYBOARD_J
			ButtonState j_key;
#endif
#if KEYBOARD_K
			ButtonState k_key;
#endif
#if KEYBOARD_L
			ButtonState l_key;
#endif
#if KEYBOARD_M
			ButtonState m_key;
#endif
#if KEYBOARD_N
			ButtonState n_key;
#endif
#if KEYBOARD_O
			ButtonState o_key;
#endif
#if KEYBOARD_P
			ButtonState p_key;
#endif
#if KEYBOARD_Q
			ButtonState q_key;
#endif
#if KEYBOARD_R
			ButtonState r_key;
#endif
#if KEYBOARD_S
			ButtonState s_key;
#endif
#if KEYBOARD_T
			ButtonState t_key;
#endif
#if KEYBOARD_U
			ButtonState u_key;
#endif
#if KEYBOARD_V
			ButtonState v_key;
#endif
#if KEYBOARD_W
			ButtonState w_key;
#endif
#if KEYBOARD_X
			ButtonState x_key;
#endif
#if KEYBOARD_Y
			ButtonState y_key;
#endif
#if KEYBOARD_Z
			ButtonState z_key;
#endif
#if KEYBOARD_0
			ButtonState zero_key;
#endif
#if KEYBOARD_1
			ButtonState one_key;
#endif
#if KEYBOARD_2
			ButtonState two_key;
#endif
#if KEYBOARD_3
			ButtonState three_key;
#endif
#if KEYBOARD_4
			ButtonState four_key;
#endif
#if KEYBOARD_5
			ButtonState five_key;
#endif
#if KEYBOARD_6
			ButtonState six_key;
#endif
#if KEYBOARD_7
			ButtonState seven_key;
#endif
#if KEYBOARD_8
			ButtonState eight_key;
#endif
#if KEYBOARD_9
			ButtonState nine_key;
#endif
#if KEYBOARD_UP
			ButtonState up_key;
#endif
#if KEYBOARD_DOWN
			ButtonState down_key;
#endif
#if KEYBOARD_LEFT
			ButtonState left_key;
#endif
#if KEYBOARD_RIGHT
			ButtonState right_key;
#endif
#if KEYBOARD_RETURN
			ButtonState return_key;
#endif
#if KEYBOARD_ESCAPE
			ButtonState escape_key;
#endif
#if KEYBOARD_SPACE
			ButtonState space_key;
#endif
#if KEYBOARD_PLUS
			ButtonState plus_key;
#endif
#if KEYBOARD_MINUS
			ButtonState minus_key;
#endif
#if KEYBOARD_MULTIPLY
			ButtonState mult_key;
#endif
#if KEYBOARD_DIVIDE
			ButtonState div_key;
#endif

		};

	};
}


/* mouse */

namespace input
{
	constexpr size_t MOUSE_BUTTONS =
	MOUSE_LEFT
	+ MOUSE_RIGHT
	+ MOUSE_MIDDLE
	+ MOUSE_X1
	+ MOUSE_X2
	;


	class MouseInput
	{
	public:

#if MOUSE_POSITION

		Point2Di32 window_pos;

#endif

#if MOUSE_WHEEL

		Vec2Di32 wheel;

#endif

		union
		{
			ButtonState buttons[MOUSE_BUTTONS];
			struct
			{
#if MOUSE_LEFT
				ButtonState button_left;
#endif
#if MOUSE_RIGHT
				ButtonState button_right;
#endif
#if MOUSE_MIDDLE
				ButtonState button_middle;
#endif
#if MOUSE_X1
				ButtonState button_x1;
#endif
#if MOUSE_X2
				ButtonState button_x2;
#endif
			};
		};

	};
}


/* controller */

namespace input
{
    class ControllerInput
    {
    public:

        union
        {
            ButtonState buttons[CONTROLLER_BUTTONS];

            struct
            {
#if CONTROLLER_BTN_DPAD_UP
                ButtonState btn_dpad_up;
#endif
#if CONTROLLER_BTN_DPAD_DOWN
                ButtonState btn_dpad_down;
#endif
#if CONTROLLER_BTN_DPAD_LEFT
                ButtonState btn_dpad_left;
#endif
#if CONTROLLER_BTN_DPAD_RIGHT
                ButtonState btn_dpad_right;
#endif
#if CONTROLLER_BTN_START
                ButtonState btn_start;
#endif
#if CONTROLLER_BTN_BACK
                ButtonState btn_back;
#endif
#if CONTROLLER_BTN_A
                ButtonState btn_a;
#endif
#if CONTROLLER_BTN_B
                ButtonState btn_b;
#endif
#if CONTROLLER_BTN_X
                ButtonState btn_x;
#endif
#if CONTROLLER_BTN_Y
                ButtonState btn_y;
#endif
#if CONTROLLER_BTN_SHOULDER_LEFT
                ButtonState btn_shoulder_left;
#endif
#if CONTROLLER_BTN_SHOULDER_RIGHT
                ButtonState btn_shoulder_right;
#endif
#if CONTROLLER_BTN_STICK_LEFT
                ButtonState btn_stick_left;
#endif
#if CONTROLLER_BTN_STICK_RIGHT
                ButtonState btn_stick_right;
#endif
            };            
        };

#if CONTROLLER_AXIS_STICK_LEFT
            Vec2Df32 stick_left;
#endif
#if CONTROLLER_AXIS_STICK_RIGHT
            Vec2Df32 stick_right;
#endif
#if CONTROLLER_TRIGGER_LEFT
            f32 trigger_left;
#endif
#if CONTROLLER_TRIGGER_RIGHT
            f32 trigger_right;
#endif        

    };
}


/* input */

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



