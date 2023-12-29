#include "sdl_include.hpp"
#include "../util/stopwatch.hpp"

#include <thread>


constexpr auto WINDOW_TITLE = "Window Title";
constexpr int WINDOW_HEIGHT = 480;
constexpr int WINDOW_WIDTH = 600;

constexpr f64 TARGET_FRAMERATE_HZ = 60.0f;
constexpr f64 TARGET_NS_PER_FRAME = 1'000'000.0 / TARGET_FRAMERATE_HZ;


static bool g_running = false;


static void end_program()
{
    g_running = false;
}


static void handle_sdl_event(SDL_Event const& event, SDL_Window* window)
{
    switch(event.type)
    {
    case SDL_WINDOWEVENT:
        sdl::handle_window_event(event.window);
        break;

    case SDL_QUIT:
        print_message("SDL_QUIT");
        end_program();
        break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        auto key_code = event.key.keysym.sym;
        auto alt = event.key.keysym.mod & KMOD_ALT;

        if (alt)
        {
            switch (key_code)
            {
            case SDLK_F4:
                print_message("ALT F4");
                end_program();
                break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                print_message("ALT ENTER");
                sdl::toggle_fullscreen(window);
                break;
            
            default:
                break;
            }
        }
        else
        {
            switch (key_code)
            {
            case SDLK_ESCAPE:
                print_message("ESC");
                end_program();
                break;

            default:
                break;
            }
        }            

    } break;
        
    }
}


int main(int argc, char *argv[])
{
    if(!sdl::init())
    {        
        return EXIT_FAILURE;
    }

    sdl::ScreenMemory screen{};
    if(!create_screen_memory(screen, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT))
    {
        return EXIT_FAILURE;
    }

    input::Input input[2] = {};
    sdl::ControllerInput controller_input = {};

    auto const cleanup = [&]()
    {
        sdl::close_game_controllers(controller_input, input[0]);
        sdl::close();
    };

    open_game_controllers(controller_input, input[0]);
    input[1].num_controllers = input[0].num_controllers;

    b32 frame_curr = 0;
    b32 frame_prev = 1;
    Stopwatch sw;
    f64 frame_ns_elapsed = TARGET_NS_PER_FRAME;
    char dbg_title[50] = { 0 };
    f64 ns_elapsed = 0.0;
    f64 title_refresh_ns = 0.5;

    auto const wait_for_framerate = [&]()
    {
        frame_ns_elapsed = sw.get_time_nano();
/*
#ifndef NDEBUG
        if(ns_elapsed >= title_refresh_ns)
        {
            ns_elapsed = 0.0;
            
            snprintf(dbg_title, 50, "%s (%d)", WINDOW_TITLE, (int)(frame_ns_elapsed / 1000));
            SDL_SetWindowTitle(screen.window, dbg_title);            
        }
#endif
*/
        auto sleep_ns = (u32)(TARGET_NS_PER_FRAME - frame_ns_elapsed);
        if (sleep_ns > 0)
        { 
            std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns));
            while (frame_ns_elapsed < TARGET_NS_PER_FRAME)
            {
                frame_ns_elapsed = sw.get_time_nano();
            }        
        }

        ns_elapsed += frame_ns_elapsed;        

        sw.start();
    };

    g_running = true;

    sw.start();
    while(g_running)
    {
        sdl::EventInfo evt{};
        evt.first_in_queue = true;
        evt.has_event = false;

        auto& input_curr = input[frame_curr];
        auto& input_prev = input[frame_prev];

        while (SDL_PollEvent(&evt.event))
        {
            evt.has_event = true;
            handle_sdl_event(evt.event, screen.window);
            input::process_keyboard_input(evt, input_prev.keyboard, input_curr.keyboard);
            input::process_mouse_input(evt, input_prev.mouse, input_curr.mouse);
            evt.first_in_queue = false;
        }

        if (!evt.has_event)
        {
            input::process_keyboard_input(evt, input_prev.keyboard, input_curr.keyboard);
            input::process_mouse_input(evt, input_prev.mouse, input_curr.mouse);
        }

        input::process_controller_input(controller_input, input_prev, input_curr);

        // does not miss frames but slows animation
        input_curr.dt_frame = (f32)(TARGET_NS_PER_FRAME / 1'000'000);

        wait_for_framerate();
        sdl::render_screen(screen);

        frame_prev = frame_curr;
        frame_curr = !frame_curr;
    }

    cleanup();

    return EXIT_SUCCESS;
}