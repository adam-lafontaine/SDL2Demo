#include "app.hpp"
#include "../util/memory_buffer.hpp"
#include "../util/stb_image/stb_image.h"

#include <filesystem>
#include <functional>
#include <array>
#include <cassert>
#include <cstring>


#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif

namespace mb = memory_buffer;


/* types */

namespace
{
    template <typename T>
    class MatrixSubView2D
    {
    public:
        T*  matrix_data_;
        u32 matrix_width;

        u32 width;
        u32 height;

        union
        {
            Rect2Du32 range;

            struct
            {
                u32 x_begin;
                u32 x_end;
                u32 y_begin;
                u32 y_end;
            };
        };
    };


    using SubView = MatrixSubView2D<Pixel>;    
    using GraySubView = MatrixSubView2D<u8>;
    using ImageGray = Matrix2D<u8>;
    using GrayView = MatrixView2D<u8>;
    using Buffer8 = MemoryBuffer<u8>;
 


    inline Buffer8 create_buffer8(u32 n_pixels)
	{
		Buffer8 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}
}


/* row_begin */

namespace
{
    template <typename T>
	static inline T* row_begin(MatrixView2D<T> const& view, u32 y)
	{
		return view.matrix_data_ + (u64)(y * view.width);
	}


    template <typename T>
    static inline T* row_begin(MatrixSubView2D<T> const& view, u32 y)
    {
        return view.matrix_data_ + (u64)((view.y_begin + y) * view.matrix_width + view.x_begin);
    }
}


namespace image
{
    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue, u8 alpha)
    {
        Pixel p{};
        p.red = red;
        p.green = green;
        p.blue = blue;
        p.alpha = alpha;

        return p;
    }


    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue)
    {
        return to_pixel(red, green, blue, 255);
    }

    
    void destroy_image(Image& image)
    {
        if (image.data_)
		{
			std::free((void*)image.data_);
			image.data_ = nullptr;
		}

		image.width = 0;
		image.height = 0;
    }
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image)
    {
        ImageView view{};

        view.width = image.width;
        view.height = image.height;
        view.matrix_data_ = image.data_;

        return view;
    }


    GrayView make_view(u32 width, u32 height, Buffer8& buffer)
    {
        GrayView view{};

        view.matrix_data_ = mb::push_elements(buffer, width * height);
        if (view.matrix_data_)
        {
            view.width = width;
            view.height = height;
        }

        return view;
    }
}


/* sub_view */

namespace image
{
    template <typename T>
    static MatrixSubView2D<T> sub_view(MatrixView2D<T> const& view, Rect2Du32 const& range)
    {
        MatrixSubView2D<T> sub_view{};

        sub_view.matrix_data_ = view.matrix_data_;
        sub_view.matrix_width = view.width;
        sub_view.range = range;
        sub_view.width = range.x_end - range.x_begin;
        sub_view.height = range.y_end - range.y_begin;

        return sub_view;
    }
}


/* fill */

namespace image
{
    template <typename T>
	static inline void fill_span(T* dst, T value, u32 len)
	{
		for (u32 i = 0; i < len; ++i)
		{
			dst[i] = value;
		}
	}


    void fill(ImageView const& view, Pixel color)
    {
        fill_span(view.matrix_data_, color, view.width * view.height);
    }


    void fill_if(GraySubView const& view, u8 gray, std::function<bool(u8)> const& pred)
    {
        for (u32 y = 0; y < view.height; y++)
        {
            auto row = row_begin(view, y);
            for (u32 x = 0; x < view.width; x++)
            {
                if (pred(row[x]))
                {
                    row[x] = gray;
                }
            }
        }
    }
}


/* transform */

namespace image
{
    void transform(GrayView const& src, SubView const& dst, std::function<Pixel(u8, Pixel)> const& func)
    {
        for (u32 y = 0; y < src.height; y++)
        {
            auto s = row_begin(src, y);
            auto d = row_begin(dst, y);
            for (u32 x = 0; x < src.width; x++)
            {
                d[x] = func(s[x], d[x]);
            }
        }
    }


    void transform_scale_up(ImageView const& src, GrayView const& dst, u32 scale, std::function<u8(Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width * scale);
        assert(dst.height == src.height * scale);

        for (u32 src_y = 0; src_y < src.height; src_y++)
        {
            auto src_row = row_begin(src, src_y);
            for (u32 src_x = 0; src_x < src.width; src_x++)
            {
                auto const s = src_row[src_x];

                auto dst_y = src_y * scale;
                for (u32 offset_y = 0; offset_y < scale; offset_y++, dst_y++)
                {
                    auto dst_row = row_begin(dst, dst_y);

                    auto dst_x = src_x * scale;
                    for (u32 offset_x = 0; offset_x < scale; offset_x++, dst_x++)
                    {
                        dst_row[dst_x] = func(s);
                    }
                }
            }
        }
    }
}


/* read */

namespace image
{
    static bool has_extension(const char* filename, const char* ext)
    {
        size_t file_length = std::strlen(filename);
        size_t ext_length = std::strlen(ext);

        return !std::strcmp(&filename[file_length - ext_length], ext);
    }


    static bool is_valid_image_file(const char* filename)
    {
        return 
            has_extension(filename, ".bmp") || 
            has_extension(filename, ".BMP") ||
            has_extension(filename, ".png")||
            has_extension(filename, ".PNG");
    }


    bool read_image_from_file(const char* img_path_src, Image& image_dst)
	{
        auto is_valid_file = is_valid_image_file(img_path_src);
        assert(is_valid_file && "invalid image file");

        if (!is_valid_file)
        {
            return false;
        }

		int width = 0;
		int height = 0;
		int image_channels = 0;
		int desired_channels = 4;

		auto data = (Pixel*)stbi_load(img_path_src, &width, &height, &image_channels, desired_channels);

		assert(data && "stbi_load() - no image data");
		assert(width && "stbi_load() - no image width");
		assert(height && "stbi_load() - no image height");

		if (!data)
		{
			return false;
		}

		image_dst.data_ = data;
		image_dst.width = width;
		image_dst.height = height;

		return true;
	}
}


namespace img = image;
namespace fs = std::filesystem;

/* image files */

namespace
{
#ifdef _WIN32
    constexpr auto ROOT = "C:/D_Data/Repos";
#else
    constexpr auto ROOT = "/home/adam/Repos";
#endif    

    const auto ROOT_DIR = fs::path(ROOT)/"SDL2Demo";
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";
    const auto MOUSE_IMAGE_PATH = ASSETS_DIR / "mouse.png";
    const auto CONTROLLER_IMAGE_PATH = ASSETS_DIR / "controller.png";


    bool load_keyboard_image(Image& image)
    {
        if (!img::read_image_from_file(KEYBOARD_IMAGE_PATH.string().c_str(), image))
        {
            return false;
        }

        return true;
    }


    bool load_mouse_image(Image& image)
    {
        if (!img::read_image_from_file(MOUSE_IMAGE_PATH.string().c_str(), image))
        {
            return false;
        }

        return true;
    }


    bool load_controller_image(Image& image)
    {
        if (!img::read_image_from_file(CONTROLLER_IMAGE_PATH.string().c_str(), image))
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


/* color table */

namespace filter
{
    constexpr auto WHITE = img::to_pixel(255, 255, 255);
    constexpr auto BLACK = img::to_pixel(0, 0, 0);
    constexpr auto TRANSPARENT = img::to_pixel(0, 0, 0, 0);
    constexpr auto BTN_BLUE = img::to_pixel(0, 0, 200);
    constexpr auto BTN_RED = img::to_pixel(200, 0, 0);


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


    u8 to_filter_color_id(Pixel p)
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

}


namespace filter
{
    class SubFilter
    {
    public:
        GraySubView view;
        u8 color_id;
    };

    
    class KeyboardFilter
    {
    public:
        GrayView filter;

        static constexpr u32 count = 9;
        
        union 
        {
            SubFilter keys[count];

            struct
            {
                SubFilter key_1;
                SubFilter key_2;
                SubFilter key_3;
                SubFilter key_4;
                SubFilter key_w;
                SubFilter key_a;
                SubFilter key_s;
                SubFilter key_d;
                SubFilter key_space;
            };            
        };

    };


    class MouseFilter
    {
    public:
        GrayView filter;


        static constexpr u32 count = 3;

        union 
        {
            SubFilter buttons[count];

            struct
            {
                SubFilter btn_left;
                SubFilter btn_middle;
                SubFilter btn_right;
            };
        };
    };


    class ControllerFilter
    {
    public:
        GrayView filter;


        static constexpr u32 count = 16;

        union
        {
            SubFilter buttons[count];

            struct
            {
                SubFilter btn_dpad_up;
                SubFilter btn_dpad_down;
                SubFilter btn_dpad_left;
                SubFilter btn_dpad_right;
                SubFilter btn_a;
                SubFilter btn_b;
                SubFilter btn_x;
                SubFilter btn_y;
                SubFilter btn_start;
                SubFilter btn_back;
                SubFilter btn_sh_left;
                SubFilter btn_sh_right;
                SubFilter btn_tr_left;
                SubFilter btn_tr_right;
                SubFilter btn_st_left;
                SubFilter btn_st_right;
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


    static void make_keyboard_filter(KeyboardFilter& keyboard)
    {
        auto& view = keyboard.filter;
        
        keyboard.key_1.view = img::sub_view(view, to_rect( 42,   6,  28, 28));
        keyboard.key_2.view = img::sub_view(view, to_rect( 78,   6,  28, 28));
        keyboard.key_3.view = img::sub_view(view, to_rect(114,   6,  28, 28));
        keyboard.key_4.view = img::sub_view(view, to_rect(150,   6,  28, 28));
        keyboard.key_w.view = img::sub_view(view, to_rect( 96,  42,  28, 28));
        keyboard.key_a.view = img::sub_view(view, to_rect( 70,  78,  28, 28));
        keyboard.key_s.view = img::sub_view(view, to_rect(106,  78,  28, 28));
        keyboard.key_d.view = img::sub_view(view, to_rect(142,  78,  28, 28));
        keyboard.key_space.view = img::sub_view(view, to_rect(168, 150, 208, 28));
    }


    static void make_mouse_filter(MouseFilter& mouse)
    {
        auto& view = mouse.filter;

        mouse.btn_left.view = img::sub_view(view, to_rect(4, 4, 56, 58));
        mouse.btn_middle.view = img::sub_view(view, to_rect(68, 4, 24, 58));
        mouse.btn_right.view = img::sub_view(view, to_rect(100, 4, 56, 58));
    }


    static void make_controller_filter(ControllerFilter& controller)
    {
        auto& view = controller.filter;

        controller.btn_dpad_up   .view = img::sub_view(view, to_rect(44,  66, 18, 32));
        controller.btn_dpad_down .view = img::sub_view(view, to_rect(44, 120, 18, 32));
        controller.btn_dpad_left .view = img::sub_view(view, to_rect(10, 100, 32, 18));
        controller.btn_dpad_right.view = img::sub_view(view, to_rect(64, 100, 32, 18));

        controller.btn_a.view = img::sub_view(view, to_rect(318, 126, 26, 26));
        controller.btn_b.view = img::sub_view(view, to_rect(348,  96, 26, 26));
        controller.btn_x.view = img::sub_view(view, to_rect(288,  96, 26, 26));
        controller.btn_y.view = img::sub_view(view, to_rect(318,  66, 26, 26));

        controller.btn_start.view = img::sub_view(view, to_rect(206, 48, 28, 14));
        controller.btn_back .view = img::sub_view(view, to_rect(150, 48, 28, 14));

        controller.btn_sh_left .view = img::sub_view(view, to_rect( 36, 44, 34, 14));
        controller.btn_sh_right.view = img::sub_view(view, to_rect(314, 44, 34, 14));
        controller.btn_tr_left .view = img::sub_view(view, to_rect( 36, 10, 34, 26));
        controller.btn_tr_right.view = img::sub_view(view, to_rect(314, 10, 34, 26));

        controller.btn_st_left .view = img::sub_view(view, to_rect(120, 90, 46, 46));
        controller.btn_st_right.view = img::sub_view(view, to_rect(218, 90, 46, 46));
    }
}


namespace app
{
    class StateData
    {
    public:

        Pixel background_color;
        
        filter::KeyboardFilter keyboard_filter;        
        filter::MouseFilter mouse_filter;
        filter::ControllerFilter controller_filter;

        b32 is_init;
        SubView screen_keyboard;
        SubView screen_mouse;
        SubView screen_controller;
        
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
    void init_keyboard_filter(filter::KeyboardFilter& filter, Image const& raw_keyboard, u32 up_scale, Buffer8& buffer)
    {
        auto width = raw_keyboard.width * up_scale;
        auto height = raw_keyboard.height * up_scale;

        filter.filter = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_keyboard), filter.filter, up_scale, filter::to_filter_color_id);

        filter::make_keyboard_filter(filter);
    }


    void update_key_colors(filter::KeyboardFilter& keyboard, input::Input const& input)
    {
        constexpr auto key_on = filter::ID_RED;
        constexpr auto key_off = filter::ID_BLUE;

        keyboard.key_1.color_id = input.keyboard.kbd_1.is_down ? key_on : key_off;
        keyboard.key_2.color_id = input.keyboard.kbd_2.is_down ? key_on : key_off;
        keyboard.key_3.color_id = input.keyboard.kbd_3.is_down ? key_on : key_off;
        keyboard.key_4.color_id = input.keyboard.kbd_4.is_down ? key_on : key_off;
        keyboard.key_w.color_id = input.keyboard.kbd_W.is_down ? key_on : key_off;
        keyboard.key_a.color_id = input.keyboard.kbd_A.is_down ? key_on : key_off;
        keyboard.key_s.color_id = input.keyboard.kbd_S.is_down ? key_on : key_off;
        keyboard.key_d.color_id = input.keyboard.kbd_D.is_down ? key_on : key_off;
        keyboard.key_space.color_id = input.keyboard.kbd_space.is_down ? key_on : key_off;
    }
}


/* mouse */

namespace
{
    void init_mouse_filter(filter::MouseFilter& filter, Image const& raw_mouse, u32 up_scale, Buffer8& buffer)
    {
        auto width = raw_mouse.width * up_scale;
        auto height = raw_mouse.height * up_scale;

        filter.filter = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_mouse), filter.filter, up_scale, filter::to_filter_color_id);

        filter::make_mouse_filter(filter);
    }


    void update_mouse_colors(filter::MouseFilter& buttons, input::Input const& input)
    {
        constexpr auto btn_on = filter::ID_RED;
        constexpr auto btn_off = filter::ID_BLUE;

        buttons.btn_left.color_id = input.mouse.btn_left.is_down ? btn_on : btn_off;
        buttons.btn_right.color_id = input.mouse.btn_right.is_down ? btn_on : btn_off;
        buttons.btn_middle.color_id = (input.mouse.btn_middle. is_down || input.mouse.wheel.y != 0) ? btn_on : btn_off;
    }
}


/* controller */

namespace
{
    void init_controller_filter(filter::ControllerFilter& filter, Image const& raw_controller, u32 up_scale, Buffer8& buffer)
    {
        auto width = raw_controller.width * up_scale;
        auto height = raw_controller.height * up_scale;

        filter.filter = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_controller), filter.filter, up_scale, filter::to_filter_color_id);

        filter::make_controller_filter(filter);
    }


    void update_controller_colors(filter::ControllerFilter& buttons, input::Input const& input)
    {
        constexpr auto btn_on = filter::ID_RED;
        constexpr auto btn_off = filter::ID_BLUE;

        buttons.btn_dpad_up.color_id = input.controller.btn_dpad_up.is_down ? btn_on : btn_off;
        buttons.btn_dpad_down.color_id = input.controller.btn_dpad_down.is_down ? btn_on : btn_off;
        buttons.btn_dpad_left.color_id = input.controller.btn_dpad_left.is_down ? btn_on : btn_off;
        buttons.btn_dpad_right.color_id = input.controller.btn_dpad_right.is_down ? btn_on : btn_off;

        buttons.btn_a.color_id = input.controller.btn_a.is_down ? btn_on : btn_off;
        buttons.btn_b.color_id = input.controller.btn_b.is_down ? btn_on : btn_off;
        buttons.btn_x.color_id = input.controller.btn_x.is_down ? btn_on : btn_off;
        buttons.btn_y.color_id = input.controller.btn_y.is_down ? btn_on : btn_off;

        buttons.btn_start.color_id = input.controller.btn_start.is_down ? btn_on : btn_off;
        buttons.btn_back.color_id = input.controller.btn_back.is_down ? btn_on : btn_off;

        buttons.btn_sh_left.color_id = input.controller.btn_shoulder_left.is_down ? btn_on : btn_off;
        buttons.btn_sh_right.color_id = input.controller.btn_shoulder_right.is_down ? btn_on : btn_off;

        buttons.btn_tr_left.color_id = input.controller.trigger_left > 0.0f ? btn_on : btn_off;
        buttons.btn_tr_right.color_id = input.controller.trigger_right > 0.0f ? btn_on : btn_off;

        buttons.btn_st_left.color_id = 
            input.controller.stick_left.magnitude > 0.3f ||
            input.controller.btn_stick_left.is_down            
            ? btn_on : btn_off;
        
        buttons.btn_st_right.color_id = 
            input.controller.stick_right.magnitude > 0.3f ||
            input.controller.btn_stick_right.is_down            
            ? btn_on : btn_off;
    }
}


/* render */

namespace
{
    void render_keyboard(app::StateData const& state)
    {
        auto& filter = state.keyboard_filter;
        auto& keys = filter.keys;
        
        for (u32 i = 0; i < filter.count; i++)
        {
            img::fill_if(keys[i].view, keys[i].color_id, filter::can_set_color_id);
        }

        img::transform(filter.filter, state.screen_keyboard, filter::to_render_color);
    }


    void render_mouse(app::StateData const& state)
    {
        auto& filter = state.mouse_filter;
        auto& buttons = filter.buttons;

        for (u32 i = 0; i < filter.count; i++)
        {
            img::fill_if(buttons[i].view, buttons[i].color_id, filter::can_set_color_id);
        }

        img::transform(filter.filter, state.screen_mouse, filter::to_render_color);
    }


    void render_controller(app::StateData const& state)
    {
        auto& filter = state.controller_filter;
        auto& buttons = filter.buttons;

        for (u32 i = 0; i < filter.count; i++)
        {
            img::fill_if(buttons[i].view, buttons[i].color_id, filter::can_set_color_id);
        }

        img::transform(filter.filter, state.screen_controller, filter::to_render_color);
    }
}


/* init */

namespace
{
    void init_screen_ui(app::AppState& state)
    {
        auto& state_data = *state.data_;
        auto& screen = state.screen_view;

        Rect2Du32 keyboard{};
        keyboard.x_begin = 0;
        keyboard.x_end = keyboard.x_begin + state_data.keyboard_filter.filter.width;
        keyboard.y_begin = 0;
        keyboard.y_end = keyboard.y_begin + state_data.keyboard_filter.filter.height;

        Rect2Du32 controller{};
        controller.x_begin = 0;
        controller.x_end = controller.x_begin + state_data.controller_filter.filter.width;
        controller.y_begin = keyboard.y_end;
        controller.y_end = controller.y_begin + state_data.controller_filter.filter.height;

        Rect2Du32 mouse{};
        mouse.x_begin = controller.x_end;
        mouse.x_end = mouse.x_begin + state_data.mouse_filter.filter.width;
        mouse.y_begin = controller.y_begin;
        mouse.y_end = mouse.y_begin + state_data.mouse_filter.filter.height;

        state_data.screen_keyboard = img::sub_view(screen, keyboard);
        state_data.screen_mouse = img::sub_view(screen, mouse);
        state_data.screen_controller = img::sub_view(screen, controller);
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
        Image raw_controller;

        constexpr u32 keyboard_scale = 2;
        constexpr u32 mouse_scale = 2;
        constexpr u32 controller_scale = 2;

        auto const cleanup = [&]()
        {
            img::destroy_image(raw_keyboard);
            img::destroy_image(raw_mouse);
            img::destroy_image(raw_controller);
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

        if (!load_controller_image(raw_controller))
        {
            printf("Error: load_controller_image()\n");
            cleanup();
            return false;
        }
        
        auto const keyboard_width = raw_keyboard.width * keyboard_scale;
        auto const keyboard_height = raw_keyboard.height * keyboard_scale;
        
        auto const mouse_width = raw_mouse.width * mouse_scale;
        auto const mouse_height = raw_mouse.height * mouse_scale;

        auto const controller_width = raw_controller.width * controller_scale;
        auto const controller_height = raw_controller.height * controller_scale;

        u32 screen_width = std::max(keyboard_width, controller_width + mouse_width);
        u32 screen_height = keyboard_height + std::max(mouse_height, controller_height);

        auto& state_data = *state.data_;
        state_data.is_init = false;

        auto& mask_buffer = state_data.mask_data;
        mask_buffer = create_buffer8(screen_width * screen_height);   

        init_keyboard_filter(state_data.keyboard_filter, raw_keyboard, keyboard_scale, mask_buffer);
        init_mouse_filter(state_data.mouse_filter, raw_mouse, mouse_scale, mask_buffer);
        init_controller_filter(state_data.controller_filter, raw_controller, controller_scale, mask_buffer);
        
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

        update_key_colors(state_data.keyboard_filter, input);
        update_mouse_colors(state_data.mouse_filter, input);
        update_controller_colors(state_data.controller_filter, input);

        img::fill(screen, state_data.background_color);
        render_keyboard(state_data);
        render_mouse(state_data);
        render_controller(state_data);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
    }
}