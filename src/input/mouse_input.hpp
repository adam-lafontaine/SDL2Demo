#pragma once

#include "button_state.hpp"


// activate buttons to accept input
#define MOUSE_LEFT 1
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 1
#define MOUSE_X1 0
#define MOUSE_X2 0

// track mouse position
#define MOUSE_POSITION 1

#define MOUSE_WHEEL 1


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



