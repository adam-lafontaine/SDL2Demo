#pragma once

#include "button_state.hpp"


#define CONTROLLER_BTN_DPAD_UP 1
#define CONTROLLER_BTN_DPAD_DOWN 1
#define CONTROLLER_BTN_DPAD_LEFT 1
#define CONTROLLER_BTN_DPAD_RIGHT 1
#define CONTROLLER_BTN_START 1
#define CONTROLLER_BTN_BACK 1
#define CONTROLLER_BTN_A 1
#define CONTROLLER_BTN_B 1
#define CONTROLLER_BTN_X 1
#define CONTROLLER_BTN_Y 1
#define CONTROLLER_BTN_SHOULDER_LEFT 1
#define CONTROLLER_BTN_SHOULDER_RIGHT 1
#define CONTROLLER_BTN_STICK_LEFT 1
#define CONTROLLER_BTN_STICK_RIGHT 1
#define CONTROLLER_AXIS_STICK_LEFT 1
#define CONTROLLER_AXIS_STICK_RIGHT 1
#define CONTROLLER_TRIGGER_LEFT 1
#define CONTROLLER_TRIGGER_RIGHT 1


namespace input
{
    constexpr size_t CONTROLLER_BUTTONS = 
    CONTROLLER_BTN_DPAD_UP +
    CONTROLLER_BTN_DPAD_DOWN +
    CONTROLLER_BTN_DPAD_LEFT +
    CONTROLLER_BTN_DPAD_RIGHT +
    CONTROLLER_BTN_START +
    CONTROLLER_BTN_BACK +    
    CONTROLLER_BTN_A +
    CONTROLLER_BTN_B +
    CONTROLLER_BTN_X +
    CONTROLLER_BTN_Y +
    CONTROLLER_BTN_SHOULDER_LEFT +
    CONTROLLER_BTN_SHOULDER_RIGHT +
    CONTROLLER_BTN_STICK_LEFT +
    CONTROLLER_BTN_STICK_RIGHT
    ;
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
