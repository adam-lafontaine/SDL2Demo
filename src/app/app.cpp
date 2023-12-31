#include "app.hpp"
#include "../image/image.hpp"


#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


namespace img = image;
namespace fs = std::filesystem;

using ImageSubView = img::ImageSubView;



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


    class KeyLocation
    {
    public:
        u16 x;
        u16 y;
        u16 width;
        u16 height;
    };


    class KeyViews
    {
    public:

        static constexpr u32 count = 15;
        
        union 
        {
            ImageSubView keys[count];

            struct
            {
                ImageSubView key_1;
                ImageSubView key_2;
                ImageSubView key_3;
                ImageSubView key_4;
                ImageSubView key_5;
                ImageSubView key_6;
                ImageSubView key_7;
                ImageSubView key_8;
                ImageSubView key_9;
                ImageSubView key_0;
                ImageSubView key_w;
                ImageSubView key_a;
                ImageSubView key_s;
                ImageSubView key_d;
                ImageSubView key_space;
            };
            
        };
        
    };


    static constexpr KeyLocation to_key_loc(u16 x, u16 y, u16 width, u16 height)
    {
        KeyLocation kd{};

        kd.x = x;
        kd.y = y;
        kd.width = width;
        kd.height = height;

        return kd;
    }


    static ImageSubView get_sub_view(ImageView const& view, KeyLocation const& loc)
    {
        Rect2Du32 range{};
        range.x_begin = loc.x;
        range.x_end = loc.x + loc.width;
        range.y_begin = loc.y;
        range.y_end = loc.y + loc.height;

        return img::sub_view(view, range);
    }


    static constexpr KeyLocation key_loc_1() { return to_key_loc( 42,   6,  28, 28); }
    static constexpr KeyLocation key_loc_2() { return to_key_loc( 78,   6,  28, 28); }
    static constexpr KeyLocation key_loc_3() { return to_key_loc(114,   6,  28, 28); }
    static constexpr KeyLocation key_loc_4() { return to_key_loc(150,   6,  28, 28); }
    static constexpr KeyLocation key_loc_5() { return to_key_loc(186,   6,  28, 28); }
    static constexpr KeyLocation key_loc_6() { return to_key_loc(222,   6,  28, 28); }
    static constexpr KeyLocation key_loc_7() { return to_key_loc(258,   6,  28, 28); }
    static constexpr KeyLocation key_loc_8() { return to_key_loc(294,   6,  28, 28); }
    static constexpr KeyLocation key_loc_9() { return to_key_loc(330,   6,  28, 28); }
    static constexpr KeyLocation key_loc_0() { return to_key_loc(366,   6,  28, 28); }
    static constexpr KeyLocation key_loc_w() { return to_key_loc( 96,  42,  28, 28); }
    static constexpr KeyLocation key_loc_a() { return to_key_loc( 70,  78,  28, 28); }
    static constexpr KeyLocation key_loc_s() { return to_key_loc(106,  78,  28, 28); }
    static constexpr KeyLocation key_loc_d() { return to_key_loc(142,  78,  28, 28); }
    static constexpr KeyLocation key_loc_space() { return to_key_loc(168, 150, 208, 28); }


    KeyViews make_key_views(ImageView const& keyboard_view)
    {
        KeyViews kv{};

        kv.key_1 = get_sub_view(keyboard_view, key_loc_1());
        kv.key_2 = get_sub_view(keyboard_view, key_loc_2());
        kv.key_3 = get_sub_view(keyboard_view, key_loc_3());
        kv.key_4 = get_sub_view(keyboard_view, key_loc_4());
        kv.key_5 = get_sub_view(keyboard_view, key_loc_5());
        kv.key_6 = get_sub_view(keyboard_view, key_loc_6());
        kv.key_7 = get_sub_view(keyboard_view, key_loc_7());
        kv.key_8 = get_sub_view(keyboard_view, key_loc_8());
        kv.key_9 = get_sub_view(keyboard_view, key_loc_9());
        kv.key_0 = get_sub_view(keyboard_view, key_loc_0());
        kv.key_w = get_sub_view(keyboard_view, key_loc_w());
        kv.key_a = get_sub_view(keyboard_view, key_loc_a());
        kv.key_s = get_sub_view(keyboard_view, key_loc_s());
        kv.key_d = get_sub_view(keyboard_view, key_loc_d());
        kv.key_space = get_sub_view(keyboard_view, key_loc_space());

        return kv;
    }
}


namespace app
{
    class StateData
    {
    public:

        Pixel screen_color;

        ImageView keyboard;
        kb::KeyViews keyboard_views;

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

        state_data.keyboard_views = kb::make_key_views(state_data.keyboard);

        auto& keys = state_data.keyboard_views;
        auto const blue = img::to_pixel(0, 0, 255);
        for (u32 i = 0; i < keys.count; i++)
        {
            img::fill(keys.keys[i], blue);
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