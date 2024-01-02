#include "app.hpp"
#include "../image/image.hpp"

#include <array>


#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


namespace img = image;
namespace fs = std::filesystem;

using ImageSubView = img::ImageSubView;
using MaskView = img::ImageGraySubView;


constexpr auto WHITE = img::to_pixel(255, 255, 255);
constexpr auto BLACK = img::to_pixel(0, 0, 0);
constexpr auto TRANSPARENT = img::to_pixel(0, 0, 0, 0);
constexpr auto BTN_BLUE = img::to_pixel(0, 0, 200);
constexpr auto BTN_RED = img::to_pixel(200, 0, 0);


/* image files */

namespace
{
    constexpr auto ROOT = "/home/adam/Repos/SDL2Demo";

    const auto ROOT_DIR = fs::path(ROOT);
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";
    const auto MOUSE_IMAGE_PATH = ASSETS_DIR / "mouse.png";
    const auto CONTROLLER_IMAGE_PATH = ASSETS_DIR / "controller.png";


    bool load_keyboard_image(Image& image)
    {
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }


    bool load_mouse_image(Image& image)
    {
        if (!img::read_image_from_file(MOUSE_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }


    bool load_controller_image(Image& image)
    {
        if (!img::read_image_from_file(CONTROLLER_IMAGE_PATH, image))
        {
            return false;
        }

        return true;
    }


    bool has_color(Pixel p)
    {
        return p.alpha > 0 && (p.red > 0 || p.green > 0 || p.blue > 0);
    }
}


namespace mask
{
    static constexpr std::array<Pixel, 5> COLOR_TABLE = 
    {
        TRANSPARENT,
        BLACK,
        WHITE,        
        BTN_BLUE,
        BTN_RED
    };


    constexpr u8 ID_BLUE = 3;
    constexpr u8 ID_RED = 4;    


    u8 to_mask_color_id(Pixel p)
    {
        if (p.alpha == 0)
        {
            return 0; // TRANSPARENT
        }

        if (p.red == 0 && p.green == 0 && p.blue == 0)
        {
            return 1; // BLACK
        }

        return 2; // WHITE
    }


    bool can_set_color_id(u8 current_id)
    {
        return current_id > 1;
    }


    Pixel to_render_color(u8 mask, Pixel color)
    {
        if (mask == 0)
        {
            return color;
        }

        return COLOR_TABLE[mask];
    }

    
    class KeyboardMasks
    {
    public:
        img::BinaryView mask_view;

        static constexpr u32 count = 9;
        
        union 
        {
            MaskView keys[count];

            struct
            {
                MaskView key_1;
                MaskView key_2;
                MaskView key_3;
                MaskView key_4;
                MaskView key_w;
                MaskView key_a;
                MaskView key_s;
                MaskView key_d;
                MaskView key_space;
            };            
        };

        union
        {
            u8 color_ids[count];

            struct 
            {
                u8 color_1;
                u8 color_2;
                u8 color_3;
                u8 color_4;
                u8 color_w;
                u8 color_a;
                u8 color_s;
                u8 color_d;
                u8 color_space;
                
            };
        };
    };


    class MouseMasks
    {
    public:
        img::BinaryView mask_view;


        static constexpr u32 count = 3;

        union 
        {
            MaskView buttons[count];

            struct
            {
                MaskView btn_left;
                MaskView btn_middle;
                MaskView btn_right;
            };
        };

        union
        {
            u8 color_ids[count];

            struct
            {
                u8 color_left;
                u8 color_middle;
                u8 color_right;
            };
        };
    };


    
    static Rect2Du32 to_rect(u16 x, u16 y, u16 width, u16 height)
    {
        Rect2Du32 range{};
        range.x_begin = x;
        range.x_end = x + width;
        range.y_begin = y;
        range.y_end = y + height;

        return range;
    }


    static void make_keyboard_masks(KeyboardMasks& masks)
    {
        auto& keyboard_view = masks.mask_view;
        
        masks.key_1 = img::sub_view(keyboard_view, to_rect( 42,   6,  28, 28));
        masks.key_2 = img::sub_view(keyboard_view, to_rect( 78,   6,  28, 28));
        masks.key_3 = img::sub_view(keyboard_view, to_rect(114,   6,  28, 28));
        masks.key_4 = img::sub_view(keyboard_view, to_rect(150,   6,  28, 28));
        masks.key_w = img::sub_view(keyboard_view, to_rect( 96,  42,  28, 28));
        masks.key_a = img::sub_view(keyboard_view, to_rect( 70,  78,  28, 28));
        masks.key_s = img::sub_view(keyboard_view, to_rect(106,  78,  28, 28));
        masks.key_d = img::sub_view(keyboard_view, to_rect(142,  78,  28, 28));
        masks.key_space = img::sub_view(keyboard_view, to_rect(168, 150, 208, 28));
    }


    static void make_mouse_masks(MouseMasks& masks)
    {
        auto& mouse_view = masks.mask_view;

        masks.btn_left = img::sub_view(mouse_view, to_rect(4, 4, 56, 58));
        masks.btn_middle = img::sub_view(mouse_view, to_rect(68, 4, 24, 58));
        masks.btn_right = img::sub_view(mouse_view, to_rect(100, 4, 56, 58));
    }
}


namespace app
{
    class StateData
    {
    public:

        Pixel background_color;
        
        mask::KeyboardMasks keyboard_mask;        
        mask::MouseMasks mouse_mask;

        b32 is_init;
        ImageSubView screen_keyboard;
        ImageSubView screen_mouse;
        
        MemoryBuffer<u8> mask_data;
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
        
        mb::destroy_buffer(state_data.mask_data);

        std::free(state.data_);
    }
}


/* keyboard */

namespace
{
    void init_keyboard_mask(mask::KeyboardMasks& masks, Image const& raw_keyboard, u32 up_scale, img::Buffer8& buffer)
    {
        auto width = raw_keyboard.width * up_scale;
        auto height = raw_keyboard.height * up_scale;

        masks.mask_view = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_keyboard), masks.mask_view, up_scale, mask::to_mask_color_id);

        mask::make_keyboard_masks(masks);
    }


    void update_key_colors(mask::KeyboardMasks& keys, input::Input const& input)
    {
        constexpr auto key_on = mask::ID_RED;
        constexpr auto key_off = mask::ID_BLUE;

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
    void init_mouse_mask(mask::MouseMasks& masks, Image const& raw_mouse, u32 up_scale, img::Buffer8& buffer)
    {
        auto width = raw_mouse.width * up_scale;
        auto height = raw_mouse.height * up_scale;

        masks.mask_view = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_mouse), masks.mask_view, up_scale, mask::to_mask_color_id);

        mask::make_mouse_masks(masks);
    }


    void update_mouse_colors(mask::MouseMasks& buttons, input::Input const& input)
    {
        constexpr auto btn_on = mask::ID_RED;
        constexpr auto btn_off = mask::ID_BLUE;

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
        auto& keys = state.keyboard_mask;
        
        for (u32 i = 0; i < keys.count; i++)
        {
            auto color_id = keys.color_ids[i];
            auto mask = keys.keys[i];

            img::fill_if(mask, color_id, mask::can_set_color_id);
        }

        img::transform(keys.mask_view, state.screen_keyboard, mask::to_render_color);
    }


    void render_mouse(app::StateData const& state)
    {
        auto& buttons = state.mouse_mask;

        for (u32 i = 0; i < buttons.count; i++)
        {
            auto color_id = buttons.color_ids[i];
            auto mask = buttons.buttons[i];
            
            img::fill_if(mask, color_id, mask::can_set_color_id);
        }

        img::transform(buttons.mask_view, state.screen_mouse, mask::to_render_color);
    }
}


/* init */

namespace
{
    void init_screen_ui(app::AppState& state)
    {
        auto& state_data = *state.data_;
        auto& screen = state.screen_view;

        Rect2Du32 kbd{};
        kbd.x_begin = 0;
        kbd.x_end = kbd.x_begin + state_data.keyboard_mask.mask_view.width;
        kbd.y_begin = 0;
        kbd.y_end = kbd.y_begin + state_data.keyboard_mask.mask_view.height;

        Rect2Du32 mouse{};
        mouse.x_begin = 0;
        mouse.x_end = mouse.x_begin + state_data.mouse_mask.mask_view.width;
        mouse.y_begin = kbd.y_end;
        mouse.y_end = mouse.y_begin + state_data.mouse_mask.mask_view.height;

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

        u32 screen_width = keyboard_width;
        u32 screen_height = keyboard_height + mouse_height;

        auto& state_data = *state.data_;
        state_data.is_init = false;

        auto& mask_buffer = state_data.mask_data;
        mask_buffer = img::create_buffer8(screen_width * screen_height);   

        init_keyboard_mask(state_data.keyboard_mask, raw_keyboard, keyboard_scale, mask_buffer);
        init_mouse_mask(state_data.mouse_mask, raw_mouse, mouse_scale, mask_buffer);        
        
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

        update_key_colors(state_data.keyboard_mask, input);
        update_mouse_colors(state_data.mouse_mask, input);

        img::fill(screen, state_data.background_color);
        render_keyboard(state_data);
        render_mouse(state_data);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }
}