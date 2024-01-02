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
    const auto MOUSE_IMAGE_PATH = ASSETS_DIR / "mouse.png";


    static inline bool load_keyboard_image(Image& image)
    {
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }


    static inline bool load_mouse_image(Image& image)
    {
        if (!img::read_image_from_file(MOUSE_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }
}


namespace ui
{
    constexpr auto WHITE = img::to_pixel(255, 255, 255);
    constexpr auto KEY_BLUE = img::to_pixel(0, 0, 200);
    constexpr auto KEY_RED = img::to_pixel(200, 0, 0);


    class UILocation
    {
    public:
        u16 x;
        u16 y;
        u16 width;
        u16 height;
    };


    static constexpr UILocation to_ui_loc(u16 x, u16 y, u16 width, u16 height)
    {
        UILocation kd{};

        kd.x = x;
        kd.y = y;
        kd.width = width;
        kd.height = height;

        return kd;
    }


    static ImageSubView get_sub_view(ImageView const& view, UILocation const& loc)
    {
        Rect2Du32 range{};
        range.x_begin = loc.x;
        range.x_end = loc.x + loc.width;
        range.y_begin = loc.y;
        range.y_end = loc.y + loc.height;

        return img::sub_view(view, range);
    }


    class KeyboardViews
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
            Pixel key_colors[count];

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


    class MouseViews
    {
    public:
        static constexpr u32 count = 3;

        union 
        {
            ImageSubView buttons[count];

            struct
            {
                ImageSubView btn_left;
                ImageSubView btn_middle;
                ImageSubView btn_right;
            };            
        };

        union
        {
            Pixel btn_colors[count];

            struct 
            {
                Pixel color_left;
                Pixel color_middle;
                Pixel color_right;
            };
        };



    };


    KeyboardViews make_ui_keyboard_views(ImageView const& keyboard_view)
    {
        KeyboardViews kv{};

        kv.key_1 = get_sub_view(keyboard_view, to_ui_loc( 42,   6,  28, 28));
        kv.key_2 = get_sub_view(keyboard_view, to_ui_loc( 78,   6,  28, 28));
        kv.key_3 = get_sub_view(keyboard_view, to_ui_loc(114,   6,  28, 28));
        kv.key_4 = get_sub_view(keyboard_view, to_ui_loc(150,   6,  28, 28));
        kv.key_w = get_sub_view(keyboard_view, to_ui_loc( 96,  42,  28, 28));
        kv.key_a = get_sub_view(keyboard_view, to_ui_loc( 70,  78,  28, 28));
        kv.key_s = get_sub_view(keyboard_view, to_ui_loc(106,  78,  28, 28));
        kv.key_d = get_sub_view(keyboard_view, to_ui_loc(142,  78,  28, 28));
        kv.key_space = get_sub_view(keyboard_view, to_ui_loc(168, 150, 208, 28));

        return kv;
    }


    MouseViews make_ui_mouse_views(ImageView const& mouse_view)
    {
        MouseViews mv{};

        mv.btn_left = get_sub_view(mouse_view, to_ui_loc(4, 4, 56, 58));
        mv.btn_middle = get_sub_view(mouse_view, to_ui_loc(68, 4, 24, 58));
        mv.btn_right = get_sub_view(mouse_view, to_ui_loc(100, 4, 56, 58));

        return mv;
    }
}


namespace app
{
    class StateData
    {
    public:

        Pixel background_color;

        ImageView ui_keyboard;
        ui::KeyboardViews keyboard_views;

        ImageView ui_mouse;
        ui::MouseViews mouse_views;

        b32 is_init;
        ImageSubView screen_keyboard;
        ImageSubView screen_mouse;

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
    void init_keyboard(app::StateData& state, Image const& raw_keyboard, u32 up_scale, img::Buffer32& buffer)
    {
        auto width = raw_keyboard.width * up_scale;
        auto height = raw_keyboard.height * up_scale;

        state.ui_keyboard = img::make_view(width, height, buffer);        
        img::scale_up(img::make_view(raw_keyboard), state.ui_keyboard, up_scale);
        
        state.keyboard_views = ui::make_ui_keyboard_views(state.ui_keyboard);

        auto const no_alpha = [](Pixel p){ return p.alpha == 0; };

        auto& keys = state.keyboard_views;
        for (u32 i = 0; i < keys.count; i++)
        {
            keys.key_colors[i] = ui::WHITE;
            img::fill_if(keys.keys[i], keys.key_colors[i], no_alpha);
        }
    }


    void update_key_colors(ui::KeyboardViews& keys, input::Input const& input)
    {
        constexpr auto key_on = ui::KEY_RED;
        constexpr auto key_off = ui::KEY_BLUE;

        keys.color_1 = input.keyboard.kbd_1.is_down ? key_on : key_off;
        keys.color_2 = input.keyboard.kbd_2.is_down ? key_on : key_off;
        keys.color_3 = input.keyboard.kbd_3.is_down ? key_on : key_off;
        keys.color_4 = input.keyboard.kbd_4.is_down ? key_on : key_off;
        keys.color_w = input.keyboard.kbd_W.is_down ? key_on : key_off;
        keys.color_a = input.keyboard.kbd_A.is_down ? key_on : key_off;
        keys.color_s = input.keyboard.kbd_S.is_down ? key_on : key_off;
        keys.color_d = input.keyboard.kbd_D.is_down ? key_on : key_off;
        keys.color_space = input.keyboard.kbd_space.is_down ? key_on : key_off;
    }
}


/* mouse */

namespace
{
    void init_mouse(app::StateData& state, Image const& raw_mouse, u32 up_scale, img::Buffer32& buffer)
    {
        auto width = raw_mouse.width * up_scale;
        auto height = raw_mouse.height * up_scale;

        state.ui_mouse = img::make_view(width, height, buffer);        
        img::scale_up(img::make_view(raw_mouse), state.ui_mouse, up_scale);
        
        state.mouse_views = ui::make_ui_mouse_views(state.ui_mouse);

        auto const has_color = [](Pixel p)
        {
            return p.alpha > 0 && (p.red > 0 || p.green > 0 || p.blue > 0);
        };

        auto& buttons = state.mouse_views;
        for (u32 i = 0; i < buttons.count; i++)
        {
            buttons.btn_colors[i] = ui::WHITE;
            img::fill_if(buttons.buttons[i], buttons.btn_colors[i], has_color);
        }
    }


    void update_mouse_colors(ui::MouseViews& buttons, input::Input const& input)
    {
        constexpr auto btn_on = ui::KEY_RED;
        constexpr auto btn_off = ui::KEY_BLUE;

        buttons.color_left = input.mouse.btn_left.is_down ? btn_on : btn_off;
        buttons.color_right = input.mouse.btn_right.is_down ? btn_on : btn_off;
        buttons.color_middle = (input.mouse.btn_middle. is_down || input.mouse.wheel.y != 0) ? btn_on : btn_off;
    }
}


/* render */

namespace
{
    void render_keyboard(app::StateData const& state)
    {
        auto const not_black = [](Pixel p)
        {
            return p.red > 0 || p.green > 0 || p.blue > 0;
        };

        auto& keys = state.keyboard_views;
        for (u32 i = 0; i < keys.count; i++)
        {
            auto color = keys.key_colors[i];
            auto view = keys.keys[i];

            img::fill_if(view, color, not_black);
        }

        img::alpha_blend(state.ui_keyboard, state.screen_keyboard);
    }


    void render_mouse(app::StateData const& state)
    {
        auto const has_color = [](Pixel p)
        {
            return p.alpha > 0 && (p.red > 0 || p.green > 0 || p.blue > 0);
        };

        auto& buttons = state.mouse_views;
        for (u32 i = 0; i < buttons.count; i++)
        {
            auto color = buttons.btn_colors[i];
            auto view = buttons.buttons[i];
            img::fill_if(view, color, has_color);
        }

        img::alpha_blend(state.ui_mouse, state.screen_mouse);
    }
}


namespace
{
    static void init_screen_ui(app::AppState& state)
    {
        auto& state_data = *state.data_;
        auto& screen = state.screen_view;

        Rect2Du32 kbd{};
        kbd.x_begin = 0;
        kbd.x_end = kbd.x_begin + state_data.ui_keyboard.width;
        kbd.y_begin = 0;
        kbd.y_end = kbd.y_begin + state_data.ui_keyboard.height;

        Rect2Du32 mouse{};
        mouse.x_begin = kbd.x_end;
        mouse.x_end = mouse.x_begin + state_data.ui_mouse.width;
        mouse.y_begin = kbd.y_begin;
        mouse.y_end = mouse.y_begin + state_data.ui_mouse.height;

        state_data.screen_keyboard = img::sub_view(screen, kbd);
        state_data.screen_mouse = img::sub_view(screen, mouse);
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
        
        Image raw_keyboard;
        Image raw_mouse;

        constexpr u32 keyboard_scale = 2;
        constexpr u32 mouse_scale = 2;

        auto const cleanup = [&]()
        {
            img::destroy_image(raw_keyboard);
            img::destroy_image(raw_mouse);
        };

        if (!load_keyboard_image(raw_keyboard))
        {
            printf("Error: load_keyboard_image()\n");
            cleanup();
            return false;
        }
        
        if (!load_mouse_image(raw_mouse))
        {
            printf("Error: load_mouse_image()\n");
            cleanup();
            return false;
        }
        
        auto const keyboard_width = raw_keyboard.width * keyboard_scale;
        auto const keyboard_height = raw_keyboard.height * keyboard_scale;
        
        auto const mouse_width = raw_mouse.width * mouse_scale;
        auto const mouse_height = raw_mouse.height * mouse_scale;

        u32 screen_width = keyboard_width + mouse_width;
        u32 screen_height = std::max(keyboard_height, mouse_height);

        auto& state_data = *state.data_;
        state_data.is_init = false;

        auto& pixel_buffer = state_data.image_data;
        pixel_buffer = img::create_buffer32(screen_width * screen_height);        

        init_keyboard(state_data, raw_keyboard, keyboard_scale, pixel_buffer);
        init_mouse(state_data, raw_mouse, mouse_scale, pixel_buffer);        
        
        auto& screen = state.screen_view;

        screen.width = screen_width;
        screen.height = screen_height;

        state_data.background_color = img::to_pixel(128, 128, 128);

        cleanup();

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {
        auto& screen = state.screen_view;
        auto& state_data = *state.data_;

        if (!state_data.is_init)
        {
            init_screen_ui(state);
            state_data.is_init = true;
        }

        update_key_colors(state_data.keyboard_views, input);
        update_mouse_colors(state_data.mouse_views, input);

        img::fill(screen, state_data.background_color);
        render_keyboard(state_data);
        render_mouse(state_data);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }
}