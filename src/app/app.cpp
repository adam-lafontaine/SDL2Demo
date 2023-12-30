#include "app.hpp"
#include "../image/image.hpp"

#include <cassert>

#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


namespace img = image;
namespace fs = std::filesystem;


namespace app
{
    class StateData
    {
    public:

        Pixel screen_color;

        Image keyboard;
    };


    static bool create_state_data(AppState& state)
    {
        auto data = (StateData*)std::malloc(sizeof(StateData));
        if (!data)
        {
            return false;
        }

        state.data_ = data;

        return true;
    }


    static void destroy_state_data(AppState& state)
    {
        auto& state_data = *state.data_;
        img::destroy_image(state_data.keyboard);

        std::free(state.data_);
    }
}


/* image files */

namespace
{
    constexpr auto ROOT = "/home/adam/Repos/SDL2Demo";

    const auto ROOT_DIR = fs::path(ROOT);
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";


    bool load_keyboard_image(app::StateData& state_data)
    {
        Image raw_image{};
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH, raw_image))
        {
            return false;
        }

        Image keyboard{};
        constexpr u32 scale = 2;

        if (!img::create_image(keyboard, raw_image.width * scale, raw_image.height * scale))
        {
            img::destroy_image(raw_image);
            return false;
        }

        img::scale_up(img::make_view(raw_image), img::make_view(keyboard), scale);

        state_data.keyboard = keyboard;

        img::destroy_image(raw_image);

        return true;
    }
}


/* render */

namespace
{
    void render_keyboard(app::StateData const& state, ImageView const& screen)
    {
        img::alpha_blend(img::make_view(state.keyboard), screen);
    }
}


/* api */

namespace app
{
    bool init(AppState& state)
    {
        if (!create_state_data(state))
        {
            return false;
        }

        auto& state_data = *state.data_;
        
        if (!load_keyboard_image(state_data))
        {
            return false;
        }

        u32 screen_width = state_data.keyboard.width;
        u32 screen_height = state_data.keyboard.height;
        
        auto& screen = state.screen_view;

        screen.width = screen_width;
        screen.height = screen_height;

        state_data.screen_color = img::to_pixel(0, 128, 0);

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {
        auto& screen = state.screen_view;
        auto& state_data = *state.data_;

        img::fill(screen, state_data.screen_color);
        render_keyboard(state_data, screen);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }
}