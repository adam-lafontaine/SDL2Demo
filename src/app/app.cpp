#include "app.hpp"
#include "../image/image.hpp"


#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


namespace img = image;
namespace fs = std::filesystem;



/* image files */

namespace kb
{
    constexpr auto ROOT = "/home/adam/Repos/SDL2Demo";

    const auto ROOT_DIR = fs::path(ROOT);
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";


    static inline bool load_keyboard_image(Image& image)
    {
        return img::read_image_from_file(KEYBOARD_IMAGE_PATH, image);
    }


    class KeyDim
    {
    public:
        u16 x;
        u16 y;
        u16 width;
        u16 height;
    };


    static constexpr KeyDim to_key_dim(u16 x, u16 y, u16 width, u16 height)
    {
        KeyDim kd{};

        kd.x = x;
        kd.y = y;
        kd.width = width;
        kd.height = height;

        return kd;
    }


    static constexpr KeyDim key_dim_1() { return to_key_dim( 44,   8,  28, 28); }
    static constexpr KeyDim key_dim_2() { return to_key_dim( 80,   8,  28, 28); }
    static constexpr KeyDim key_dim_3() { return to_key_dim(116,   8,  28, 28); }
    static constexpr KeyDim key_dim_4() { return to_key_dim(152,   8,  28, 28); }
    static constexpr KeyDim key_dim_5() { return to_key_dim(188,   8,  28, 28); }
    static constexpr KeyDim key_dim_6() { return to_key_dim(224,   8,  28, 28); }
    static constexpr KeyDim key_dim_7() { return to_key_dim(260,   8,  28, 28); }
    static constexpr KeyDim key_dim_8() { return to_key_dim(296,   8,  28, 28); }
    static constexpr KeyDim key_dim_9() { return to_key_dim(332,   8,  28, 28); }
    static constexpr KeyDim key_dim_0() { return to_key_dim(368,   8,  28, 28); }
    static constexpr KeyDim key_dim_w() { return to_key_dim( 98,  44,  28, 28); }
    static constexpr KeyDim key_dim_a() { return to_key_dim( 72,  80,  28, 28); }
    static constexpr KeyDim key_dim_s() { return to_key_dim(108,  80,  28, 28); }
    static constexpr KeyDim key_dim_d() { return to_key_dim(144,  80,  28, 28); }
    static constexpr KeyDim key_dim_space() { return to_key_dim(170, 152, 208, 28); }


    
}


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
        if (!kb::load_keyboard_image(raw_keyboard))
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