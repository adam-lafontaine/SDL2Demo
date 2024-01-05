#include "app.hpp"
#include "../util/memory_buffer.hpp"
#include "../util/stb_image/stb_image.h"
#include "../util/qsprintf/qsprintf.hpp"

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

    
    static Rect2Du32 to_rect(u16 x, u16 y, u32 width, u32 height)
    {
        Rect2Du32 range{};
        range.x_begin = x;
        range.x_end = x + width;
        range.y_begin = y;
        range.y_end = y + height;

        return range;
    }


    class StringView
    {
    public:
        char* data_;
        u32 capacity;
        u32 length;
    };
    
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


    template <typename T>
    static MatrixSubView2D<T> sub_view(MatrixSubView2D<T> const& view, Rect2Du32 const& range)
    {
        MatrixSubView2D<T> sub_view{};

        sub_view.matrix_data_ = view.matrix_data_;
        sub_view.matrix_width = view.matrix_width;

        sub_view.x_begin = range.x_begin + view.x_begin;
		sub_view.x_end = range.x_end + view.x_begin;
		sub_view.y_begin = range.y_begin + view.y_begin;
		sub_view.y_end = range.y_end + view.y_begin;

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


    void fill(SubView const& view, Pixel color)
    {
        for (u32 y = 0; y < view.height; y++)
        {
            fill_span(row_begin(view, y), color, view.width);
        }
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
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

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


    void transform(GraySubView const& src, SubView const& dst, std::function<Pixel(u8, Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

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


    void transform(ImageView const& src, GrayView const& dst, std::function<u8(Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

        auto const len = src.width * src.height;

        auto s = src.matrix_data_;
        auto d = dst.matrix_data_;

        for (u32 i = 0; i < len; i++)
        {
            d[i] = func(s[i]);
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
                auto const value = func(src_row[src_x]);

                auto dst_y = src_y * scale;
                for (u32 offset_y = 0; offset_y < scale; offset_y++, dst_y++)
                {
                    auto dst_row = row_begin(dst, dst_y);

                    auto dst_x = src_x * scale;
                    for (u32 offset_x = 0; offset_x < scale; offset_x++, dst_x++)
                    {
                        dst_row[dst_x] = value;
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


/* string_view */

namespace sv
{
    static void zero_view(StringView& view)
    {
        view.length = 0;

        for (u32 i = 0; i < view.capacity; i++)
        {
            view.data_[i] = 0;
        }
    }


    StringView make_view(u32 capacity, Buffer8& buffer)
    {
        StringView view{};

        auto data = mb::push_elements(buffer, capacity);
        if (data)
        {
            view.data_ = (char*)data;
            view.capacity = capacity;
            
            zero_view(view);
        }

        return view;
    }

}


/* image files */

namespace
{
    

    const auto ROOT_DIR = fs::path(config::ROOT)/"SDL2Demo";
    const auto ASSETS_DIR = ROOT_DIR / "assets";

    const auto KEYBOARD_IMAGE_PATH = ASSETS_DIR / "keyboard.png";
    const auto MOUSE_IMAGE_PATH = ASSETS_DIR / "mouse.png";
    const auto CONTROLLER_IMAGE_PATH = ASSETS_DIR / "controller.png";
    const auto ASCII_IMAGE_PATH = ASSETS_DIR / "ascii.png";


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
    

    bool load_ascii_image(Image& image)
    {
        if (!img::read_image_from_file(ASCII_IMAGE_PATH.string().c_str(), image))
        {
            return false;
        }

        return true;
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


    class AsciiFilter
    {
    public:
        GrayView filter;

        static constexpr u32 count = 95;

        SubFilter characters[count];
    };


    static void make_keyboard_filter(KeyboardFilter& keyboard)
    {
        auto& view = keyboard.filter;
        
        keyboard.key_1.view = img::sub_view(view, to_rect(21,  3, 14, 14));
        keyboard.key_2.view = img::sub_view(view, to_rect(39,  3, 14, 14));
        keyboard.key_3.view = img::sub_view(view, to_rect(57,  3, 14, 14));
        keyboard.key_4.view = img::sub_view(view, to_rect(75,  3, 14, 14));
        keyboard.key_w.view = img::sub_view(view, to_rect(48, 21, 14, 14));
        keyboard.key_a.view = img::sub_view(view, to_rect(35, 39, 14, 14));
        keyboard.key_s.view = img::sub_view(view, to_rect(53, 39, 14, 14));
        keyboard.key_d.view = img::sub_view(view, to_rect(71, 39, 14, 14));
        keyboard.key_space.view = img::sub_view(view, to_rect(84, 75, 104, 14));
    }


    static void make_mouse_filter(MouseFilter& mouse)
    {
        auto& view = mouse.filter;

        mouse.btn_left.view   = img::sub_view(view, to_rect( 2, 2, 28, 29));
        mouse.btn_middle.view = img::sub_view(view, to_rect(34, 2, 12, 29));
        mouse.btn_right.view  = img::sub_view(view, to_rect(50, 2, 28, 29));
    }


    static void make_controller_filter(ControllerFilter& controller)
    {
        auto& view = controller.filter;

        controller.btn_dpad_up   .view = img::sub_view(view, to_rect(22, 33,  9, 16));
        controller.btn_dpad_down .view = img::sub_view(view, to_rect(22, 60,  9, 16));
        controller.btn_dpad_left .view = img::sub_view(view, to_rect( 5, 50, 16,  9));
        controller.btn_dpad_right.view = img::sub_view(view, to_rect(32, 50, 16,  9));

        controller.btn_a.view = img::sub_view(view, to_rect(159, 63, 13, 13));
        controller.btn_b.view = img::sub_view(view, to_rect(174, 48, 13, 13));
        controller.btn_x.view = img::sub_view(view, to_rect(144, 48, 13, 13));
        controller.btn_y.view = img::sub_view(view, to_rect(159, 33, 13, 13));

        controller.btn_start.view = img::sub_view(view, to_rect(103, 24, 14, 7));
        controller.btn_back .view = img::sub_view(view, to_rect( 75, 24, 14, 7));

        controller.btn_sh_left .view = img::sub_view(view, to_rect( 18, 22, 17,  7));
        controller.btn_sh_right.view = img::sub_view(view, to_rect(157, 22, 17, 7));
        controller.btn_tr_left .view = img::sub_view(view, to_rect( 18,  5, 17, 13));
        controller.btn_tr_right.view = img::sub_view(view, to_rect(157,  5, 17, 13));

        controller.btn_st_left .view = img::sub_view(view, to_rect( 60, 45, 23, 23));
        controller.btn_st_right.view = img::sub_view(view, to_rect(109, 45, 23, 23));
    }


    static void make_ascii_filter(AsciiFilter& ascii, u32 scale)
    {
        auto& view = ascii.filter;
        auto& characters = ascii.characters;

        u32 char_length[ascii.count] = { 2, 4, 4, 6, 6, 5, 6, 5, 5, 5, 5, 5, 4, 5, 4, 5, 5, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 5, 4, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6, 5, 6, 6, 5, 6, 5, 3, 5, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6, 5, 6, 6, 5, 6, 5, 4, 4, 4, 5 };

        constexpr auto char_length_count = sizeof(char_length) / sizeof(u32);
        static_assert(char_length_count == ascii.count);

        auto const value_at = [&view](u32 x, u32 y){ return *(row_begin(view, y) + x); };

        u32 x = 0;
        for (u32 i = 0; i < ascii.count; i++)
        {
            auto width = char_length[i * scale];
            characters[i].view = img::sub_view(view, to_rect(x, 0, width, view.height));
            characters[i].color_id = 1; // BLACK

            img::fill_if(characters[i].view, characters[i].color_id, filter::can_set_color_id);

            x += width;
        }
    }


    static void write_to_view(AsciiFilter const& filter, StringView const& src, SubView const& dst)
    {
        u32 dst_x = 0;
        u32 dst_y = 0;

        for (u32 i = 0; i < src.length; i++)
        {
            auto character = filter.characters[src.data_[i] - 32];
            auto width = character.view.width;
            auto height = character.view.height;
            auto dst_view = img::sub_view(dst, to_rect(dst_x, dst_y, width, height));

            //img::transform(character.view, dst_view, filter::to_render_color);            

            dst_x += width;
        }

        img::fill(dst, filter::BLACK);
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
        filter::AsciiFilter ascii_filter;

        StringView mouse_coords;
        
        SubView screen_keyboard;
        SubView screen_mouse;
        SubView screen_controller;
        SubView screen_mouse_coords;

        b32 is_init;
        MemoryBuffer<u8> u8_data;
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
        
        mb::destroy_buffer(state_data.u8_data);

        std::free(state.data_);
    }
}


/* init */

namespace
{
    void init_keyboard_filter(filter::KeyboardFilter& filter, Image const& raw_keyboard, Buffer8& buffer)
    {
        auto const width = raw_keyboard.width;
        auto const height = raw_keyboard.height;

        filter.filter = img::make_view(width, height, buffer);
        img::transform(img::make_view(raw_keyboard), filter.filter, filter::to_filter_color_id);

        filter::make_keyboard_filter(filter);
    }


    void init_mouse_filter(filter::MouseFilter& filter, Image const& raw_mouse, Buffer8& buffer)
    {
        auto const width = raw_mouse.width;
        auto const height = raw_mouse.height;

        filter.filter = img::make_view(width, height, buffer);
        img::transform(img::make_view(raw_mouse), filter.filter, filter::to_filter_color_id);

        filter::make_mouse_filter(filter);
    }


    void init_controller_filter(filter::ControllerFilter& filter, Image const& raw_controller, Buffer8& buffer)
    {
        auto const width = raw_controller.width;
        auto const height = raw_controller.height;

        filter.filter = img::make_view(width, height, buffer);
        img::transform(img::make_view(raw_controller), filter.filter, filter::to_filter_color_id);

        filter::make_controller_filter(filter);
    }


    void init_ascii_filter(filter::AsciiFilter& filter, Image const& raw_ascii, u32 scale, Buffer8& buffer)
    {
        auto const width = raw_ascii.width * scale;
        auto const height = raw_ascii.height * scale;

        filter.filter = img::make_view(width, height, buffer);
        img::transform_scale_up(img::make_view(raw_ascii), filter.filter, scale, filter::to_filter_color_id);
    }


    void init_screen_ui(app::AppState& state)
    {
        auto& state_data = *state.data_;
        auto& screen = state.screen_view;

        auto keyboard = to_rect(0, 0, state_data.keyboard_filter.filter.width, state_data.keyboard_filter.filter.height);

        auto controller = to_rect(0, keyboard.y_end, state_data.controller_filter.filter.width, state_data.controller_filter.filter.height);
        
        auto mouse = to_rect(controller.x_end, controller.y_begin, state_data.mouse_filter.filter.width, state_data.mouse_filter.filter.height);
        
        state_data.screen_keyboard = img::sub_view(screen, keyboard);
        state_data.screen_mouse = img::sub_view(screen, mouse);
        state_data.screen_controller = img::sub_view(screen, controller);        

        auto const mouse_width = state_data.screen_mouse.width;
        auto const mouse_height = state_data.screen_mouse.height;
        auto coords = to_rect(mouse_width / 5, mouse_height / 2, mouse_width * 3 / 5, 30);

        state_data.screen_mouse_coords = img::sub_view(state_data.screen_mouse, coords);
    }

}


/* update */

namespace
{
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


    void update_mouse_colors(filter::MouseFilter& buttons, input::Input const& input)
    {
        constexpr auto btn_on = filter::ID_RED;
        constexpr auto btn_off = filter::ID_BLUE;

        buttons.btn_left.color_id = input.mouse.btn_left.is_down ? btn_on : btn_off;
        buttons.btn_right.color_id = input.mouse.btn_right.is_down ? btn_on : btn_off;
        buttons.btn_middle.color_id = (input.mouse.btn_middle. is_down || input.mouse.wheel.y != 0) ? btn_on : btn_off;
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


    void update_mouse_coords(StringView& coords, input::Input const& input)
    {
        auto mouse_pos = input.mouse.window_pos;

        sv::zero_view(coords);

        qsnprintf(coords.data_, coords.capacity, "(%d, %d)", mouse_pos.x, mouse_pos.y);

        coords.length = strlen(coords.data_);
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

        filter::write_to_view(state.ascii_filter, state.mouse_coords, state.screen_mouse_coords);
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
        Image raw_ascii;

        auto const cleanup = [&]()
        {
            img::destroy_image(raw_keyboard);
            img::destroy_image(raw_mouse);
            img::destroy_image(raw_controller);
            img::destroy_image(raw_ascii);
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

        if (!load_ascii_image(raw_ascii))
        {
            printf("Error: load_ascii_image()\n");
            cleanup();
            return false;
        }
        
        auto const keyboard_width = raw_keyboard.width;
        auto const keyboard_height = raw_keyboard.height;
        
        auto const mouse_width = raw_mouse.width;
        auto const mouse_height = raw_mouse.height;

        auto const controller_width = raw_controller.width;
        auto const controller_height = raw_controller.height;

        constexpr u32 ASCII_SCALE = 2;

        auto const ascii_width = raw_ascii.width * ASCII_SCALE;
        auto const ascii_height = raw_ascii.height * ASCII_SCALE;

        u32 screen_width = std::max(keyboard_width, controller_width + mouse_width);
        u32 screen_height = keyboard_height + std::max(mouse_height, controller_height);

        constexpr u32 mouse_coord_capacity = sizeof("(0000, 0000)");

        auto& state_data = *state.data_;
        state_data.is_init = false;

        auto const buffer_bytes = screen_width * screen_height + ascii_width * ascii_height + mouse_coord_capacity;

        auto& u8_buffer = state_data.u8_data;
        u8_buffer = create_buffer8(buffer_bytes);   

        init_keyboard_filter(state_data.keyboard_filter, raw_keyboard, u8_buffer);
        init_mouse_filter(state_data.mouse_filter, raw_mouse, u8_buffer);
        init_controller_filter(state_data.controller_filter, raw_controller, u8_buffer);
        init_ascii_filter(state_data.ascii_filter, raw_ascii, ASCII_SCALE, u8_buffer);
        state_data.mouse_coords = sv::make_view(mouse_coord_capacity, u8_buffer);
        
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
        update_mouse_coords(state_data.mouse_coords, input);

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