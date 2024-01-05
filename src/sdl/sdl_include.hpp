#pragma once

#include "../input/input.hpp"


#if defined(_WIN32)
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>

#ifndef NDEBUG
#define PRINT_MESSAGES
#endif

#ifdef PRINT_MESSAGES
#include <cstdio>
#endif


static void print_message(const char* msg)
{
#ifdef PRINT_MESSAGES
    printf("%s\n", msg);
#endif
}


//#define SDL2_IMPL_B

namespace sdl
{
    constexpr auto SCREEN_BYTES_PER_PIXEL = sizeof(Pixel);
    constexpr auto MAX_CONTROLLERS = input::MAX_CONTROLLERS;

#ifdef SDL2_WASM

    constexpr auto SDL_OPTIONS = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

#else
    
    constexpr auto SDL_OPTIONS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC;

#endif


    class ControllerInput
    {
    public:
        SDL_GameController* controllers[MAX_CONTROLLERS];
        SDL_Haptic* rumbles[MAX_CONTROLLERS];
    };


    class EventInfo
    {
    public:
        SDL_Event event;
        bool first_in_queue = true;
        bool has_event = false;
    };
}


/* sdl_input.cpp */

namespace input
{    
    void process_controller_input(sdl::ControllerInput const& sdl_controller, Input const& old_input, Input& new_input);

    void process_keyboard_input(sdl::EventInfo const& evt, KeyboardInput const& old_keyboard, KeyboardInput& new_keyboard);

    void process_mouse_input(sdl::EventInfo const& evt, MouseInput const& old_mouse, MouseInput& new_mouse);
}



namespace sdl
{
    static void print_error(const char* msg)
    {
#ifdef PRINT_MESSAGES
        printf("%s\n%s\n", msg, SDL_GetError());
#endif
    }


    static void close()
    {
        SDL_Quit();
    }


    static bool init()
    {    
        if (SDL_Init(SDL_OPTIONS) != 0)
        {
            print_error("SDL_Init failed");
            return false;
        }

        return true;
    }


    static void display_error(const char* msg)
    {
#ifndef SDL2_WASM
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "ERROR", msg, 0);
#endif

        print_error(msg);
    }


    static void handle_window_event(SDL_WindowEvent const& w_event)
    {
#ifndef SDL2_WASM

        auto window = SDL_GetWindowFromID(w_event.windowID);
        //auto renderer = SDL_GetRenderer(window);

        switch(w_event.event)
        {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {

            }break;
            case SDL_WINDOWEVENT_EXPOSED:
            {
                
            } break;
        }

#endif
    }


    static void open_game_controllers(ControllerInput& sdl, input::Input& input)
    {
        int num_joysticks = SDL_NumJoysticks();
        int c = 0;
        for(int j = 0; j < num_joysticks; ++j)
        {
            if (!SDL_IsGameController(j))
            {
                continue;
            }

            print_message("found a controller");

            sdl.controllers[c] = SDL_GameControllerOpen(j);
            auto joystick = SDL_GameControllerGetJoystick(sdl.controllers[c]);
            if(!joystick)
            {
                print_message("no joystick");
            }

            sdl.rumbles[c] = SDL_HapticOpenFromJoystick(joystick);
            if(!sdl.rumbles[c])
            {
                print_message("no rumble from joystick");
            }
            else if(SDL_HapticRumbleInit(sdl.rumbles[c]) != 0)
            {
                print_error("SDL_HapticRumbleInit failed");
                SDL_HapticClose(sdl.rumbles[c]);
                sdl.rumbles[c] = 0;
            }
            else
            {
                print_message("found a rumble");
            }

            ++c;

            if (c >= MAX_CONTROLLERS)
            {
                break;
            }
        }

        input.num_controllers = c;
    }


    static void close_game_controllers(ControllerInput& sdl, input::Input const& input)
    {
        for(u32 c = 0; c < input.num_controllers; ++c)
        {
            if(sdl.rumbles[c])
            {
                SDL_HapticClose(sdl.rumbles[c]);
            }
            SDL_GameControllerClose(sdl.controllers[c]);
        }
    }


    static void set_window_icon(SDL_Window* window)
    {
        // https://blog.gibson.sh/2015/04/13/how-to-integrate-your-sdl2-window-icon-or-any-image-into-your-executable/
   
#include "../resources/icon_64.c" // this will "paste" the struct my_icon into this function

    // these masks are needed to tell SDL_CreateRGBSurface(From)
    // to assume the data it gets is byte-wise RGB(A) data
        Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        int shift = (icon_64.bytes_per_pixel == 3) ? 8 : 0;
        rmask = 0xff000000 >> shift;
        gmask = 0x00ff0000 >> shift;
        bmask = 0x0000ff00 >> shift;
        amask = 0x000000ff >> shift;
#else // little endian, like x86
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = (icon_64.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

        SDL_Surface* icon = SDL_CreateRGBSurfaceFrom(
            (void*)icon_64.pixel_data,
            icon_64.width,
            icon_64.height,
            icon_64.bytes_per_pixel * 8,
            icon_64.bytes_per_pixel * icon_64.width,
            rmask, gmask, bmask, amask);

        SDL_SetWindowIcon(window, icon);

        SDL_FreeSurface(icon);
        
    }


    static void toggle_fullscreen(SDL_Window* window)
    {
        static bool is_fullscreen = false;

        if (is_fullscreen)
        {
            SDL_SetWindowFullscreen(window, 0);
        }
        else
        {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }

        is_fullscreen = !is_fullscreen;
    }
}


#ifndef SDL2_IMPL_B


namespace sdl
{
    class ScreenMemory
    {
    public:

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;

        Image image;
    };


    static void destroy_screen_memory(ScreenMemory& screen)
    {
        if (screen.image.data_)
        {
            free(screen.image.data_);
        }

        if (screen.texture)
        {
            SDL_DestroyTexture(screen.texture);
        }

        if (screen.renderer)
        {
            SDL_DestroyRenderer(screen.renderer);
        }

        if(screen.window)
        {
            SDL_DestroyWindow(screen.window);
        }
    }


    namespace screen
    {
        static bool create_window(ScreenMemory& screen, cstr title, u32 width, u32 height)
        {
            screen.window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                (int)width,
                (int)height,
                SDL_WINDOW_RESIZABLE);

            if(!screen.window)
            {
                display_error("SDL_CreateWindow failed");
                return false;
            }

            set_window_icon(screen.window);

            return true;
        }


        static bool create_renderer(ScreenMemory& screen)
        {
            screen.renderer = SDL_CreateRenderer(screen.window, -1, 0);

            if(!screen.renderer)
            {
                display_error("SDL_CreateRenderer failed");
                return false;
            }

            return true;
        }


        static bool create_texture(ScreenMemory& screen, u32 width, u32 height)
        {
            screen.texture =  SDL_CreateTexture(
                screen.renderer,
                SDL_PIXELFORMAT_ABGR8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height);
            
            if(!screen.texture)
            {
                display_error("SDL_CreateTexture failed");
                return false;
            }

            return true;
        }


        static bool create_image(ScreenMemory& screen, u32 width, u32 height)
        {
            screen.image.data_ = (Pixel*)malloc((size_t)(sizeof(Pixel) * width * height));

            if(!screen.image.data_)
            {
                display_error("Allocating image memory failed");
                return false;
            }

            screen.image.width = width;
            screen.image.height = height;

            return true;
        }
    }


    static bool create_screen_memory(ScreenMemory& screen, cstr title, u32 width, u32 height)
    {
        destroy_screen_memory(screen);

        if (!screen::create_window(screen, title, width, height))
        {
            destroy_screen_memory(screen);
            return false;
        }
        
        if (!screen::create_renderer(screen))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!screen::create_texture(screen, width, height))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!screen::create_image(screen, width, height))
        {
            destroy_screen_memory(screen);
            return false;
        }

        return true;
    }


    static bool create_screen_memory(ScreenMemory& screen, cstr title, Vec2Du32 screen_dim, Vec2Du32 window_dim)
    {
        destroy_screen_memory(screen);

        if (!screen::create_window(screen, title, window_dim.x, window_dim.y))
        {
            destroy_screen_memory(screen);
            return false;
        }
        
        if (!screen::create_renderer(screen))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!screen::create_texture(screen, screen_dim.x, screen_dim.y))
        {
            destroy_screen_memory(screen);
            return false;
        }

        if (!screen::create_image(screen, screen_dim.x, screen_dim.y))
        {
            destroy_screen_memory(screen);
            return false;
        }

        return true;
    }


    static void render_screen(ScreenMemory const& screen)
    {
        auto const pitch = screen.image.width * SCREEN_BYTES_PER_PIXEL;
        auto error = SDL_UpdateTexture(screen.texture, 0, (void*)screen.image.data_, pitch);
        if(error)
        {
            print_error("SDL_UpdateTexture failed");
        }

        error = SDL_RenderCopy(screen.renderer, screen.texture, 0, 0);
        if(error)
        {
            print_error("SDL_RenderCopy failed");
        }
        
        SDL_RenderPresent(screen.renderer);
    }
}

#else

namespace sdl
{
    class ScreenMemory
    {
    public:

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* texture = nullptr;
        SDL_Surface* surface = nullptr;

        void* image_data = nullptr;
        
        int image_width;
        int image_height;

        bool surface_locked = false;
    };


    static void lock_surface(ScreenMemory& screen)
    {
        //assert(screen.surface);

        if (!screen.surface_locked && SDL_MUSTLOCK(screen.surface)) 
        {
            SDL_LockSurface(screen.surface);
            screen.surface_locked = true;
        }
    }


    static void unlock_surface(ScreenMemory& screen)
    {
        //assert(screen.surface);

        if (screen.surface_locked && SDL_MUSTLOCK(screen.surface)) 
        {
            SDL_UnlockSurface(screen.surface);
            screen.surface_locked = false;
        }
    }


    static void destroy_screen_memory(ScreenMemory& screen)
    {
        unlock_surface(screen);

        if (screen.texture)
        {
            SDL_DestroyTexture(screen.texture);
        }

        if (screen.renderer)
        {
            SDL_DestroyRenderer(screen.renderer);
        }

        if(screen.window)
        {
            SDL_DestroyWindow(screen.window);
        }
    }


    static bool create_screen_memory(ScreenMemory& screen, const char* title, int width, int height)
    {
        destroy_screen_memory(screen);

        screen.window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_RESIZABLE);

        if(!screen.window)
        {
            display_error("SDL_CreateWindow failed");
            return false;
        }

        set_window_icon(screen.window);

        screen.renderer = SDL_CreateRenderer(screen.window, -1, 0);

        if(!screen.renderer)
        {
            display_error("SDL_CreateRenderer failed");
            destroy_screen_memory(screen);
            return false;
        }

        screen.surface = SDL_CreateRGBSurface(
            0,
            width,
            height,
            SCREEN_BYTES_PER_PIXEL * 8,
            0, 0, 0, 0);

        if(!screen.surface)
        {
            display_error("SDL_CreateRGBSurface failed");
            destroy_screen_memory(screen);
            return false;
        }

        screen.texture =  SDL_CreateTextureFromSurface(screen.renderer, screen.surface);    
        
        if(!screen.texture)
        {
            display_error("SDL_CreateTextureFromSurface");
            destroy_screen_memory(screen);
            return false;
        }    

        screen.image_data = (void*)(screen.surface->pixels);

        screen.image_width = width;
        screen.image_height = height;

        return true;
    }


    static void render_screen(ScreenMemory& screen)
    {
        unlock_surface(screen);

        auto const pitch = screen.image_width * SCREEN_BYTES_PER_PIXEL;
        auto error = SDL_UpdateTexture(screen.texture, 0, screen.image_data, pitch);
        if(error)
        {
            print_sdl_error("SDL_UpdateTexture failed");
        }

        error = SDL_RenderCopy(screen.renderer, screen.texture, 0, 0);
        if(error)
        {
            print_sdl_error("SDL_RenderCopy failed");
        }
        
        SDL_RenderPresent(screen.renderer);
        
        lock_surface(screen);
    }
}

#endif