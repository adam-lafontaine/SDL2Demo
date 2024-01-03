#include "sdl_include.hpp"
#include "../input/input_state.hpp"


static f32 normalize_axis_value(Sint16 axis)
{
    constexpr f32 min = -1.0f;
    constexpr f32 max = 1.0f;

	f32 norm = (f32)axis / 32767;

    auto hi = (int)(norm > max);
    auto lo = (int)(norm < min);
    auto ok = (int)(!hi && !lo);

	return (ok * norm) + (hi * max) + (lo * min);
}


static f32 q_rsqrt(f32 number)
{
    long i;
    float x2, y;
    constexpr float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}


static f32 q_hypot(f32 a, f32 b)
{
    auto number = a * a + b * b;

    long i;
    float x2, y;
    constexpr float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration

    return 1.0f / y; // std::hypot?
}


/* keyboard */

namespace input
{
    static void record_keyboard_input(SDL_Keycode key_code, KeyboardInput const& old_keyboard, KeyboardInput& new_keyboard, bool is_down)
    {
        switch (key_code)
        {
#if KEYBOARD_A
        case SDLK_a:
            record_button_input(old_keyboard.kbd_A, new_keyboard.kbd_A, is_down);
            break;
#endif
#if KEYBOARD_B
        case SDLK_b:
            record_button_input(old_keyboard.kbd_B, new_keyboard.kbd_B, is_down);
            break;
#endif
#if KEYBOARD_C
        case SDLK_c:
            record_button_input(old_keyboard.kbd_C, new_keyboard.kbd_C, is_down);
            break;
#endif
#if KEYBOARD_D
        case SDLK_d:
            record_button_input(old_keyboard.kbd_D, new_keyboard.kbd_D, is_down);
            break;
#endif
#if KEYBOARD_E
        case SDLK_e:
            record_button_input(old_keyboard.kbd_E, new_keyboard.kbd_E, is_down);
            break;
#endif
#if KEYBOARD_F
        case SDLK_f:
            record_button_input(old_keyboard.kbd_F, new_keyboard.kbd_F, is_down);
            break;
#endif
#if KEYBOARD_G
        SDLK_g:
            record_button_input(old_keyboard.kbd_G, new_keyboard.kbd_G, is_down);
            break;
#endif
#if KEYBOARD_H
        case SDLK_h:
            record_button_input(old_keyboard.kbd_H, new_keyboard.kbd_H, is_down);
            break;
#endif
#if KEYBOARD_I
        case SDLK_i:
            record_button_input(old_keyboard.kbd_I, new_keyboard.kbd_I, is_down);
            break;
#endif
#if KEYBOARD_J
        case SDLK_j:
            record_button_input(old_keyboard.kbd_J, new_keyboard.kbd_J, is_down);
            break;
#endif
#if KEYBOARD_K
        case SDLK_k:
            record_button_input(old_keyboard.kbd_K, new_keyboard.kbd_K, is_down);
            break;
#endif
#if KEYBOARD_L
        case SDLK_l:
            record_button_input(old_keyboard.kbd_L, new_keyboard.kbd_L, is_down);
            break;
#endif
#if KEYBOARD_M
        case SDLK_m:
            record_button_input(old_keyboard.kbd_M, new_keyboard.kbd_M, is_down);
            break;
#endif
#if KEYBOARD_N
        case SDLK_n:
            record_button_input(old_keyboard.kbd_N, new_keyboard.kbd_N, is_down);
            break;
#endif
#if KEYBOARD_O
        case SDLK_o:
            record_button_input(old_keyboard.kbd_O, new_keyboard.kbd_O, is_down);
            break;
#endif
#if KEYBOARD_P
        case SDLK_p:
            record_button_input(old_keyboard.kbd_P, new_keyboard.kbd_P, is_down);
            break;
#endif
#if KEYBOARD_Q
        case SDLK_q:
            record_button_input(old_keyboard.kbd_Q, new_keyboard.kbd_Q, is_down);
            break;
#endif
#if KEYBOARD_R
        case SDLK_r:
            record_button_input(old_keyboard.kbd_R, new_keyboard.kbd_R, is_down);
            break;
#endif
#if KEYBOARD_S
        case SDLK_s:
            record_button_input(old_keyboard.kbd_S, new_keyboard.kbd_S, is_down);
            break;
#endif
#if KEYBOARD_T
        case SDLK_t:
            record_button_input(old_keyboard.kbd_T, new_keyboard.kbd_T, is_down);
            break;
#endif
#if KEYBOARD_U
        case SDLK_u:
            record_button_input(old_keyboard.kbd_U, new_keyboard.kbd_U, is_down);
            break;
#endif
#if KEYBOARD_V
        case SDLK_v:
            record_button_input(old_keyboard.kbd_V, new_keyboard.kbd_V, is_down);
            break;
#endif
#if KEYBOARD_W
        case SDLK_w:
            record_button_input(old_keyboard.kbd_W, new_keyboard.kbd_W, is_down);
            break;
#endif
#if KEYBOARD_X
        case SDLK_x:
            record_button_input(old_keyboard.kbd_X, new_keyboard.kbd_X, is_down);
            break;
#endif
#if KEYBOARD_Y
        case SDLK_y:
            record_button_input(old_keyboard.kbd_Y, new_keyboard.kbd_Y, is_down);
            break;
#endif
#if KEYBOARD_Z
        case SDLK_z:
            record_button_input(old_keyboard.kbd_Z, new_keyboard.kbd_Z, is_down);
            break;
#endif
#if KEYBOARD_0
        case SDLK_0:
            record_button_input(old_keyboard.kbd_0, new_keyboard.kbd_0, is_down);
            break;
#endif
#if KEYBOARD_1
        case SDLK_1:
            record_button_input(old_keyboard.kbd_1, new_keyboard.kbd_1, is_down);
            break;
#endif
#if KEYBOARD_2
        case SDLK_2:
            record_button_input(old_keyboard.kbd_2, new_keyboard.kbd_2, is_down);
            break;
#endif
#if KEYBOARD_3
        case SDLK_3:
            record_button_input(old_keyboard.kbd_3, new_keyboard.kbd_3, is_down);
            break;
#endif
#if KEYBOARD_4
        case SDLK_4:
            record_button_input(old_keyboard.kbd_4, new_keyboard.kbd_4, is_down);
            break;
#endif
#if KEYBOARD_5
        case SDLK_5:
            record_button_input(old_keyboard.kbd_5, new_keyboard.kbd_5, is_down);
            break;
#endif
#if KEYBOARD_6
        case SDLK_6:
            record_button_input(old_keyboard.kbd_6, new_keyboard.kbd_6, is_down);
            break;
#endif
#if KEYBOARD_7
        case SDLK_7:
            record_button_input(old_keyboard.kbd_7, new_keyboard.kbd_7, is_down);
            break;
#endif
#if KEYBOARD_8
        case SDLK_8:
            record_button_input(old_keyboard.kbd_8, new_keyboard.kbd_8, is_down);
            break;
#endif
#if KEYBOARD_9
        case SDLK_9:
            record_button_input(old_keyboard.kbd_9, new_keyboard.kbd_9, is_down);
            break;
#endif
#if KEYBOARD_UP
        case SDLK_UP:
            record_button_input(old_keyboard.kbd_up, new_keyboard.kbd_up, is_down);
            break;
#endif
#if KEYBOARD_DOWN
        case SDLK_DOWN:
            record_button_input(old_keyboard.kbd_down, new_keyboard.kbd_down, is_down);
            break;
#endif
#if KEYBOARD_LEFT
        case SDLK_LEFT:
            record_button_input(old_keyboard.kbd_left, new_keyboard.kbd_left, is_down);
            break;
#endif
#if KEYBOARD_RIGHT
        case SDLK_RIGHT:
            record_button_input(old_keyboard.kbd_right, new_keyboard.kbd_right, is_down);
            break;
#endif
#if KEYBOARD_RETURN
        case SDLK_RETURN:
            record_button_input(old_keyboard.kbd_return, new_keyboard.kbd_return, is_down);
            break;
#endif
#if KEYBOARD_ESCAPE
        case SDLK_ESCAPE:
            record_button_input(old_keyboard.kbd_escape, new_keyboard.kbd_escape, is_down);
            break;
#endif
#if KEYBOARD_SPACE
        case SDLK_SPACE:
            record_button_input(old_keyboard.kbd_space, new_keyboard.kbd_space, is_down);
            break;
#endif
#if KEYBOARD_LSHIFT
        case SDLK_LSHIFT:
            record_button_input(old_keyboard.kbd_left_shift, new_keyboard.kbd_left_shift, is_down);
            break;
#endif
#if KEYBOARD_RSHIFT
        case SDLK_RSHIFT:
            record_button_input(old_keyboard.kbd_right_shift, new_keyboard.kbd_right_shift, is_down);
            break;
#endif
#if KEYBOARD_NUMPAD_0
            case SDLK_KP_0:
                record_button_input(old_keyboard.npd_0, new_keyboard.npd_0, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_1
            case SDLK_KP_1:
                record_button_input(old_keyboard.npd_1, new_keyboard.npd_1, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_2
            case SDLK_KP_2:
                record_button_input(old_keyboard.npd_2, new_keyboard.npd_2, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_3
            case SDLK_KP_3:
                record_button_input(old_keyboard.npd_3, new_keyboard.npd_3, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_4
            case SDLK_KP_4:
                record_button_input(old_keyboard.npd_4, new_keyboard.npd_4, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_5
            case SDLK_KP_5:
                record_button_input(old_keyboard.npd_5, new_keyboard.npd_5, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_6
            case SDLK_KP_6:
                record_button_input(old_keyboard.npd_6, new_keyboard.npd_6, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_7
            case SDLK_KP_7:
                record_button_input(old_keyboard.npd_7, new_keyboard.npd_7, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_8
            case SDLK_KP_8:
                record_button_input(old_keyboard.npd_8, new_keyboard.npd_8, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_9
            case SDLK_KP_9:
                record_button_input(old_keyboard.npd_9, new_keyboard.npd_9, is_down);
                break;
#endif
#if KEYBOARD_NUMPAD_PLUS
        case SDLK_KP_PLUS:
            record_button_input(old_keyboard.npd_plus, new_keyboard.npd_plus, is_down);
            break;
#endif
#if KEYBOARD_NUMPAD_MINUS
        case SDLK_KP_MINUS:
            record_button_input(old_keyboard.npd_minus, new_keyboard.npd_minus, is_down);
            break;
#endif
#if KEYBOARD_NUMPAD_MULTIPLY
        case SDLK_KP_MULTIPLY:
            record_button_input(old_keyboard.npd_mult, new_keyboard.npd_mult, is_down);
            break;
#endif
#if KEYBOARD_NUMPAD_DIVIDE
        case SDLK_KP_DIVIDE:
            record_button_input(old_keyboard.npd_div, new_keyboard.npd_div, is_down);
            break;
#endif

        
        default:
            break;
        }
    }
}


/* mouse */
namespace input
{
    static void record_mouse_button_input(MouseInput const& old_mouse, MouseInput& new_mouse, Uint8 button_code, bool is_down)
    {
        switch(button_code)
        {		
#if MOUSE_LEFT
            case SDL_BUTTON_LEFT:
            {
                record_button_input(old_mouse.btn_left, new_mouse.btn_left, is_down);
            } break;
#endif
#if MOUSE_RIGHT
            case SDL_BUTTON_RIGHT:
            {
                record_button_input(old_mouse.btn_right, new_mouse.btn_right, is_down);
            } break;
#endif
#if MOUSE_MIDDLE
            case SDL_BUTTON_MIDDLE:
            {
                record_button_input(old_mouse.btn_middle, new_mouse.btn_middle, is_down);
            } break;
#endif
#if MOUSE_X1
            case SDL_BUTTON_X1:
            {
                record_button_input(old_mouse.btn_x1, new_mouse.btn_x1, is_down);
            } break;
#endif
#if MOUSE_X2
            case SDL_BUTTON_X2:
            {
                record_button_input(old_mouse.btn_x1, new_mouse.btn_x1, is_down);
            } break;
#endif
        }
    }


    static void record_mouse_position_input(MouseInput& mouse, SDL_MouseMotionEvent const& motion)
    {
#if MOUSE_POSITION
        mouse.window_pos.x = motion.x;
        mouse.window_pos.y = motion.y;
#endif
    }


    static void record_mouse_wheel_input(MouseInput& mouse, SDL_MouseWheelEvent const& wheel)
    {
#if MOUSE_WHEEL
        mouse.wheel.x = wheel.x;
        mouse.wheel.y = wheel.y;
#endif
    }
}


/* controller */

namespace input
{
    static f32 get_controller_axis(SDL_GameController* sdl_controller, SDL_GameControllerAxis axis_key)
    {
        auto axis = SDL_GameControllerGetAxis(sdl_controller, axis_key);
        return normalize_axis_value(axis);
    }


    static void record_controller_button(SDL_GameController* sdl_controller, SDL_GameControllerButton btn_key, ButtonState const& old_btn, ButtonState& new_btn)
    {
        auto is_down = SDL_GameControllerGetButton(sdl_controller, btn_key);
        record_button_input(old_btn, new_btn, is_down);
    }


    static void record_controller_stick_button(SDL_GameController* sdl_controller, SDL_GameControllerButton btn_key, ButtonState const& old_btn, ButtonState& new_btn, VectorState<f32> const& stick)
    {
        // ignore button press if stick is used for direction
        auto is_down = SDL_GameControllerGetButton(sdl_controller, btn_key) && stick.magnitude < 0.3;
        record_button_input(old_btn, new_btn, is_down);
    }


    static void record_controller_button_input(SDL_GameController* sdl_controller, ControllerInput const& old_controller, ControllerInput& new_controller)
    {

#if CONTROLLER_BTN_DPAD_UP
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_DPAD_UP, old_controller.btn_dpad_up, new_controller.btn_dpad_up);
#endif
#if CONTROLLER_BTN_DPAD_DOWN
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN, old_controller.btn_dpad_down, new_controller.btn_dpad_down);
#endif
#if CONTROLLER_BTN_DPAD_LEFT
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT, old_controller.btn_dpad_left, new_controller.btn_dpad_left);
#endif
#if CONTROLLER_BTN_DPAD_RIGHT
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, old_controller.btn_dpad_right, new_controller.btn_dpad_right);
#endif
#if CONTROLLER_BTN_START
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_START, old_controller.btn_start, new_controller.btn_start);
#endif
#if CONTROLLER_BTN_BACK
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_BACK, old_controller.btn_back, new_controller.btn_back);
#endif
#if CONTROLLER_BTN_A
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_A, old_controller.btn_a, new_controller.btn_a);
#endif
#if CONTROLLER_BTN_B
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_B, old_controller.btn_b, new_controller.btn_b);
#endif
#if CONTROLLER_BTN_X
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_X, old_controller.btn_x, new_controller.btn_x);
#endif
#if CONTROLLER_BTN_Y
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_Y, old_controller.btn_y, new_controller.btn_y);
#endif
#if CONTROLLER_BTN_SHOULDER_LEFT
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, old_controller.btn_shoulder_left, new_controller.btn_shoulder_left);
#endif
#if CONTROLLER_BTN_SHOULDER_RIGHT
        record_controller_button(sdl_controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, old_controller.btn_shoulder_right, new_controller.btn_shoulder_right);
#endif
#if CONTROLLER_BTN_STICK_LEFT
        record_controller_stick_button(sdl_controller, SDL_CONTROLLER_BUTTON_LEFTSTICK, old_controller.btn_stick_left, new_controller.btn_stick_left, new_controller.stick_left);
#endif
#if CONTROLLER_BTN_STICK_RIGHT
        record_controller_stick_button(sdl_controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK, old_controller.btn_stick_right, new_controller.btn_stick_right, new_controller.stick_right);        
#endif

    }


    static void record_controller_axis_input(SDL_GameController* sdl_controller, ControllerInput& controller)
    {

#if CONTROLLER_AXIS_STICK_LEFT
        auto& left = controller.stick_left;
        left.vec.x = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_LEFTX);
        left.vec.y = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_LEFTY);

        left.magnitude = q_hypot(left.vec.x, left.vec.y);
        left.unit_direction.x = left.vec.x / left.magnitude;
        left.unit_direction.y = left.vec.y / left.magnitude;

#endif
#if CONTROLLER_AXIS_STICK_RIGHT
        auto& right = controller.stick_right;
        right.vec.x = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_RIGHTX);
        right.vec.y = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_RIGHTY);

        right.magnitude = q_hypot(right.vec.x, right.vec.y);
        right.unit_direction.x = right.vec.x / right.magnitude;
        right.unit_direction.y = right.vec.y / right.magnitude;
#endif

    }


    static void record_controller_trigger_input(SDL_GameController* sdl_controller, ControllerInput& controller)
    {

#if CONTROLLER_TRIGGER_LEFT
        controller.trigger_left = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
#endif
#if CONTROLLER_TRIGGER_RIGHT
        controller.trigger_right = get_controller_axis(sdl_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
#endif

    }


    static void record_controller_dpad_vector(ControllerInput& controller)
    {
#if CONTROLLER_BTN_DPAD_ALL

        auto& vec = controller.vec_dpad;

        vec.vec.x = (i32)controller.btn_dpad_right.is_down - (i32)controller.btn_dpad_left.is_down;
        vec.vec.y = (i32)controller.btn_dpad_down.is_down - (i32)controller.btn_dpad_up.is_down;

        auto const vec_x = (f32)vec.vec.x;
        auto const vec_y = (f32)vec.vec.y;

        constexpr f32 hypot = 1.4142135f;
        
        if (vec_x || vec_y)
        {
            vec.magnitude = (vec_x && vec_y) ? hypot : 1.0f;

            vec.unit_direction.x = vec_x / vec.magnitude;
            vec.unit_direction.y = vec_y / vec.magnitude;
        }
        else
        {
            vec.magnitude = 0.0f;      
            vec.unit_direction.x = 0.0f;
            vec.unit_direction.y = 0.0f;
        }

#endif
    }


    static void record_controller_input(SDL_GameController* sdl_controller, ControllerInput const& old_controller, ControllerInput& new_controller)
    {
        if(!sdl_controller || !SDL_GameControllerGetAttached(sdl_controller))
        {
            return;
        }

        record_controller_axis_input(sdl_controller, new_controller);

        record_controller_button_input(sdl_controller, old_controller, new_controller);
        
        record_controller_trigger_input(sdl_controller, new_controller);
        record_controller_dpad_vector(new_controller);
    }
}



/* api */

namespace input
{
    void process_keyboard_input(sdl::EventInfo const& evt, KeyboardInput const& old_keyboard, KeyboardInput& new_keyboard)
    {
        if (evt.first_in_queue)
        {
            copy_keyboard_state(old_keyboard, new_keyboard);
        }

        if (!evt.has_event)
        {
            return;
        }

        auto event = evt.event;

        switch (event.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            if (event.key.repeat)
            {
                return;
            }

            bool is_down = event.type == SDL_KEYDOWN; //event.key.state == SDL_PRESSED;

            auto key_code = event.key.keysym.sym;
            record_keyboard_input(key_code, old_keyboard, new_keyboard, is_down);
        } break;
        }
    }


    void process_mouse_input(sdl::EventInfo const& evt, MouseInput const& old_mouse, MouseInput& new_mouse)
    {
        if (evt.first_in_queue)
        {
            copy_mouse_state(old_mouse, new_mouse);
        }

        if (!evt.has_event)
        {
            return;
        }

        auto event = evt.event;

        auto& mouse = new_mouse;

        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            bool is_down = event.type == SDL_MOUSEBUTTONDOWN;
            auto button_code = event.button.button;

            record_mouse_button_input(old_mouse, mouse, button_code, is_down);
        } break;

#if MOUSE_POSITION

        case SDL_MOUSEMOTION:
        {
            record_mouse_position_input(mouse, event.motion);
        } break;

#endif

#if MOUSE_WHEEL

        case SDL_MOUSEWHEEL:
        {
            record_mouse_wheel_input(mouse, event.wheel);
        } break;
#endif
        }
    }


    void process_controller_input(sdl::ControllerInput const& sdl_controller, Input const& old_input, Input& new_input)
    {
        for(u32 c = 0; c < new_input.num_controllers; ++c)
        {
            input::copy_controller_state(old_input.controllers[c], new_input.controllers[c]);
            record_controller_input(sdl_controller.controllers[c], old_input.controllers[c], new_input.controllers[c]);
        }
    }
}
