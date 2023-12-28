#pragma once

#include "button_state.hpp"


#define CONTROLLER_UP 1
#define CONTROLLER_DOWN 1
#define CONTROLLER_LEFT 1
#define CONTROLLER_RIGHT 1
#define CONTROLLER_START 1
#define CONTROLLER_BACK 1
#define CONTROLLER_LEFT_SHOULDER 1
#define CONTROLLER_RIGHT_SHOULDER 1
#define CONTROLLER_A 1
#define CONTROLLER_B 1
#define CONTROLLER_X 1
#define CONTROLLER_Y 1
#define CONTROLLER_STICK_LEFT 1
#define CONTROLLER_STICK_RIGHT 1
#define CONTROLLER_TRIGGER_LEFT 1
#define CONTROLLER_TRIGGER_RIGHT 1


namespace input
{
    constexpr size_t CONTROLLER_BUTTONS = 
    CONTROLLER_UP +
    CONTROLLER_DOWN +
    CONTROLLER_LEFT +
    CONTROLLER_RIGHT +
    CONTROLLER_START +
    CONTROLLER_BACK +
    CONTROLLER_LEFT_SHOULDER +
    CONTROLLER_RIGHT_SHOULDER +
    CONTROLLER_A +
    CONTROLLER_B +
    CONTROLLER_X +
    CONTROLLER_Y;
}


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
#if CONTROLLER_UP
                ButtonState dpad_up;
#endif
#if CONTROLLER_DOWN
                ButtonState dpad_down;
#endif
#if CONTROLLER_LEFT
                ButtonState dpad_left;
#endif
#if CONTROLLER_RIGHT
                ButtonState dpad_right;
#endif
#if CONTROLLER_START
                ButtonState button_start;
#endif
#if CONTROLLER_BACK
                ButtonState button_back;
#endif
#if CONTROLLER_LEFT_SHOULDER
                ButtonState shoulder_left;
#endif
#if CONTROLLER_RIGHT_SHOULDER
                ButtonState shoulder_right;
#endif
#if CONTROLLER_A
                ButtonState button_a;
#endif
#if CONTROLLER_B
                ButtonState button_b;
#endif
#if CONTROLLER_X
                ButtonState button_x;
#endif
#if CONTROLLER_Y
                ButtonState  button_y;
#endif
            };            
        };

#if CONTROLLER_STICK_LEFT
            Vec2Df32 stick_left;
#endif
#if CONTROLLER_STICK_RIGHT
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
