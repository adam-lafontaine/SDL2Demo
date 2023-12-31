#include "app.hpp"
#include "../image/image.hpp"


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

        ImageView keyboard;

        MemoryBuffer<Pixel> image_data;
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
        
        mb::destroy_buffer(state_data.image_data);

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


    /*bool load_keyboard_image(app::StateData& state_data)
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
*/

    Image load_keyboard_image()
    {
        Image raw_image{};
        img::read_image_from_file(KEYBOARD_IMAGE_PATH, raw_image);

        return raw_image;
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
            printf("Error: create_state_data()\n");
            return false;
        }

        auto& state_data = *state.data_;

        u32 total_pixels = 0;
        Image raw_keyboard;
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH, raw_keyboard))
        {
            printf("Error: load_keyboard_image()\n");
            return false;
        }
        constexpr u32 keyboard_scale = 2;
        auto const keyboard_width = raw_keyboard.width * keyboard_scale;
        auto const keyboard_height = raw_keyboard.height * keyboard_scale;
        total_pixels += keyboard_width * keyboard_height;


        auto& pixel_data = state_data.image_data;
        pixel_data = img::create_buffer32(total_pixels);

        state_data.keyboard = img::make_view(keyboard_width, keyboard_height, pixel_data);        
        img::scale_up(img::make_view(raw_keyboard), state_data.keyboard, keyboard_scale);
        img::destroy_image(raw_keyboard);

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