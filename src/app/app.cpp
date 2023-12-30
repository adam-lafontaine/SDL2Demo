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

    DataResult<Image> load_keyboard_image()
    {
        DataResult<Image> result;
        result.success = img::read_image_from_file(KEYBOARD_IMAGE_PATH, result.data);

        assert(result.success && "Error load_keyboard_image()");

        return result;
    }
}


/* render */

namespace
{
    void render_keyboard(app::StateData const& state, ImageView const& screen)
    {
        img::alpha_blend(state.keyboard, screen);
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

        auto keyboard_result = load_keyboard_image();
        if (!keyboard_result.success)
        {
            return false;
        }

        auto& raw_keyboard_image = keyboard_result.data;

        state_data.keyboard = keyboard_result.data;

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