#include "sdl_include.hpp"
#include "../util/stopwatch.hpp"
#include "../app/app.hpp"

#ifndef NDEBUG

#include "../util/qsprintf/qsprintf.hpp"

#endif

#include <thread>
#include <cassert>


constexpr auto WINDOW_TITLE = config::APP_TITLE;

constexpr f64 NANO = 1'000'000'000;

constexpr f64 TARGET_FRAMERATE_HZ = 60.0f;
constexpr f64 TARGET_NS_PER_FRAME = NANO / TARGET_FRAMERATE_HZ;


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

    app::AppState app_state{};
    if (!app::init(app_state))
    {
        print_message("Error: app::init()");
        sdl::close();
        return EXIT_FAILURE;
    }

    assert(app_state.screen_view.width);
    assert(app_state.screen_view.height);

    auto const screen_width = app_state.screen_view.width;
    auto const screen_height = app_state.screen_view.height;

    if (!screen_width || !screen_height)
    {
        print_message("Error: app::init()");
        sdl::close();
        return EXIT_FAILURE;
    }

    Vec2Du32 screen_dim{};
    screen_dim.x = screen_width;
    screen_dim.y = screen_height;

    Vec2Du32 window_dim{};
    window_dim.x = config::WINDOW_SCALE * screen_width;
    window_dim.y = config::WINDOW_SCALE * screen_height;

    sdl::ScreenMemory screen{};
    if(!sdl::create_screen_memory(screen, WINDOW_TITLE, screen_dim, window_dim))
    {
        print_message("Error: sdl::create_screen_memory()");
        sdl::close();
        return EXIT_FAILURE;
    }

    sdl::AudioMemory audio{};
    if (!sdl::create_audio_memory(audio))
    {
        print_message("Error: sdl::create_audio_memory()");
        sdl::close();
        return EXIT_FAILURE;
    }

    app_state.screen_view.matrix_data_ = screen.image.data_;

    input::Input input[2] = {};
    sdl::ControllerInput controller_input = {};
    sdl::open_game_controllers(controller_input, input[0]);
    input[1].num_controllers = input[0].num_controllers;

    auto const cleanup = [&]()
    {
        sdl::pause_audio(audio);
        app::close(app_state);
        sdl::close_game_controllers(controller_input, input[0]);
        sdl::close();
    };

    b32 frame_curr = 0;
    b32 frame_prev = 1;

    Stopwatch sw;
    f64 frame_nano = TARGET_NS_PER_FRAME;    

#ifndef NDEBUG
    f64 dbg_ns_elapsed = 0.0;
    constexpr f64 dbg_title_refresh_ns = NANO * 0.25;
    constexpr int dbg_TITLE_LEN = 50;
    char dbg_title[dbg_TITLE_LEN] = { 0 };
    int dbg_frame_milli = 0;
#endif

    g_running = true;
    sdl::play_audio(audio);

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
        input_curr.dt_frame = (f32)(1.0 / TARGET_FRAMERATE_HZ);

        app::update(app_state, input_curr);

        // track frame rate
        frame_nano = sw.get_time_nano();

#ifndef NDEBUG
        dbg_frame_milli = (int)(frame_nano / 1'000'000 + 0.5);
#endif

        auto sleep_ns = TARGET_NS_PER_FRAME - frame_nano;
        if (sleep_ns > 0)
        { 
            std::this_thread::sleep_for(std::chrono::nanoseconds((i64)(sleep_ns)));
            while (frame_nano < TARGET_NS_PER_FRAME)
            {
                frame_nano = sw.get_time_nano();
            }        
        }

        sw.start();

#ifndef NDEBUG
        dbg_ns_elapsed += frame_nano;
        if(dbg_ns_elapsed >= dbg_title_refresh_ns)
        {
            auto fps = (int)(NANO / frame_nano + 0.5);
            qsnprintf(dbg_title, dbg_TITLE_LEN, "%s (%d fps / %d ms)", WINDOW_TITLE, fps, dbg_frame_milli);
            SDL_SetWindowTitle(screen.window, dbg_title);

            dbg_ns_elapsed = 0.0;
        }
#endif

        sdl::render_screen(screen);
        sdl::update_audio(audio, app_state.audio_volume);

        frame_prev = frame_curr;
        frame_curr = !frame_curr;
    }

    cleanup();

    return EXIT_SUCCESS;
}