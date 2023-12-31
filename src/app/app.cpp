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

namespace
{
    constexpr auto ROOT = "/home/adam/Repos/SDL2Demo";

    const auto ROOT_DIR = fs::path(ROOT);
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";


    static inline bool load_keyboard_image(Image& image)
    {
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }
}


namespace kb
{
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

        static constexpr u32 count = 9;
        
        union 
        {
            ImageSubView keys[count];

            struct
            {
                ImageSubView key_1;
                ImageSubView key_2;
                ImageSubView key_3;
                ImageSubView key_4;
                ImageSubView key_w;
                ImageSubView key_a;
                ImageSubView key_s;
                ImageSubView key_d;
                ImageSubView key_space;
            };            
        };

        union
        {
            Pixel colors[count];

            struct 
            {
                Pixel color_1;
                Pixel color_2;
                Pixel color_3;
                Pixel color_4;
                Pixel color_w;
                Pixel color_a;
                Pixel color_s;
                Pixel color_d;
                Pixel color_space;
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

        Pixel background_color;

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


/* keyboard */

namespace
{
    constexpr auto KEY_BLUE = img::to_pixel(0, 0, 200);
    constexpr auto KEY_RED = img::to_pixel(200, 0, 0);





    void init_keyboard(app::StateData& state, Image const& raw_keyboard, u32 up_scale, img::Buffer32& buffer)
    {
        auto width = raw_keyboard.width * up_scale;
        auto height = raw_keyboard.height * up_scale;

        state.keyboard = img::make_view(width, height, buffer);        
        img::scale_up(img::make_view(raw_keyboard), state.keyboard, up_scale);

        auto& keys = state.keyboard_views;
        keys = kb::make_key_views(state.keyboard);

        for (u32 i = 0; i < keys.count; i++)
        {
            keys.colors[i] = KEY_BLUE;
        }
    }


    void update_key_colors(kb::KeyViews& keys, input::Input const& input)
    {
        for (u32 i = 0; i < keys.count; i++)
        {
            keys.colors[i] = KEY_BLUE;
        }

        if (input.keyboard.one_key.is_down)
        {
            keys.color_1 = KEY_RED;
        }

        if (input.keyboard.two_key.is_down)
        {
            keys.color_2 = KEY_RED;
        }

        if (input.keyboard.three_key.is_down)
        {
            keys.color_3 = KEY_RED;
        }

        if (input.keyboard.four_key.is_down)
        {
            keys.color_4 = KEY_RED;
        }

        if (input.keyboard.w_key.is_down)
        {
            keys.color_w = KEY_RED;
        }

        if (input.keyboard.a_key.is_down)
        {
            keys.color_a = KEY_RED;
        }

        if (input.keyboard.s_key.is_down)
        {
            keys.color_s = KEY_RED;
        }

        if (input.keyboard.d_key.is_down)
        {
            keys.color_d = KEY_RED;
        }

        if (input.keyboard.space_key.is_down)
        {
            keys.color_space = KEY_RED;
        }
    }
}


/* render */

namespace
{
    void render_keyboard(app::StateData const& state, ImageView const& screen)
    {
        auto const not_black = [](Pixel p)
        {
            return p.red > 0 || p.green > 0 || p.blue > 0 || p.alpha == 0;
        };

        auto& keys = state.keyboard_views;
        for (u32 i = 0; i < keys.count; i++)
        {
            auto color = keys.colors[i];
            auto view = keys.keys[i];

            img::fill_if(view, color, not_black);
        }

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
        if (!load_keyboard_image(raw_keyboard))
        {
            printf("Error: load_keyboard_image()\n");
            return false;
        }

        constexpr u32 keyboard_scale = 2;
        auto const keyboard_width = raw_keyboard.width * keyboard_scale;
        auto const keyboard_height = raw_keyboard.height * keyboard_scale;
        total_pixels += keyboard_width * keyboard_height;

        auto& pixel_buffer = state_data.image_data;
        pixel_buffer = img::create_buffer32(total_pixels);        

        init_keyboard(state_data, raw_keyboard, keyboard_scale, pixel_buffer);
        img::destroy_image(raw_keyboard);        

        u32 screen_width = state_data.keyboard.width;
        u32 screen_height = state_data.keyboard.height;
        
        auto& screen = state.screen_view;

        screen.width = screen_width;
        screen.height = screen_height;

        state_data.background_color = img::to_pixel(128, 128, 128);

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {
        auto& screen = state.screen_view;
        auto& state_data = *state.data_;

        update_key_colors(state_data.keyboard_views, input);

        img::fill(screen, state_data.background_color);
        render_keyboard(state_data, screen);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }
}