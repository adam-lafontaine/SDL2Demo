#include "app.hpp"
#include "../output/image.hpp"
#include "../output/audio.hpp"
#include "../util/qsprintf/qsprintf.hpp"

#include <filesystem>
#include <array>
#include <cassert>
#include <cstring>


#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif

namespace img = image;
namespace fs = std::filesystem;

using Buffer8 = img::Buffer8;
using Image = img::Image;
using Pixel = img::Pixel;
using GrayView = img::GrayView;
using GraySubView = img::GraySubView;
using SubView = img::SubView;
using Sound = audio::Sound;
using Music = audio::Music;


static Rect2Du32 to_rect(u16 x, u16 y, u32 width, u32 height)
{
    Rect2Du32 range{};
    range.x_begin = x;
    range.x_end = x + width;
    range.y_begin = y;
    range.y_end = y + height;

    return range;
}


/* string_view */

namespace sv
{
    class StringView
    {
    public:
        char* data_;
        u32 capacity;
        u32 length;
    };


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


    inline bool load_image(fs::path const& path, Image& image)
    {
        return img::read_image_from_file(path.string().c_str(), image);
    }


    bool load_keyboard_image(Image& image)
    {        
        return load_image(KEYBOARD_IMAGE_PATH, image);
    }


    bool load_mouse_image(Image& image)
    {
        return load_image(MOUSE_IMAGE_PATH, image);
    }


    bool load_controller_image(Image& image)
    {
        return load_image(CONTROLLER_IMAGE_PATH, image);
    }
    

    bool load_ascii_image(Image& image)
    {
        return load_image(ASCII_IMAGE_PATH, image);
    }
}


/* audio files */

namespace
{
    // https://www.kenney.nl/
    // https://pixabay.com/music/

    const auto LASER_SOUND_PATH = ASSETS_DIR / "laserLarge_000.ogg";
    const auto RETRO_SOUND_PATH = ASSETS_DIR / "laserRetro_003.ogg";
    const auto DOOR_SOUND_PATH = ASSETS_DIR / "doorOpen_000.ogg";
    const auto FORCE_FIELD_SOUND_PATH = ASSETS_DIR / "forceField_000.ogg";
    const auto MELLOW_MUSIC_PATH = ASSETS_DIR / "mellow-future-bass-bounce-on-it-184234.mp3";


    inline bool load_sound(fs::path const& path, Sound& sound)
    {
        return audio::load_sound_from_file(path.string().c_str(), sound);
    }


    inline bool load_music(fs::path const& path, Music& music)
    {
        return audio::load_music_from_file(path.string().c_str(), music);
    }


    bool load_laser_sound(Sound& sound)
    {
        return load_sound(LASER_SOUND_PATH, sound);
    }


    bool load_retro_sound(Sound& sound)
    {
        return load_sound(RETRO_SOUND_PATH, sound);
    }


    bool load_door_sound(Sound& sound)
    {
        return load_sound(DOOR_SOUND_PATH, sound);
    }


    bool load_force_field_sound(Sound& sound)
    {
        return load_sound(FORCE_FIELD_SOUND_PATH, sound);
    }


    bool load_mellow_music(Music& music)
    {
        return load_music(MELLOW_MUSIC_PATH, music);
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

        u32 x = 0;
        for (u32 i = 0; i < ascii.count; i++)
        {
            auto width = char_length[i] * scale;
            characters[i].view = img::sub_view(view, to_rect(x, 0, width, view.height));
            characters[i].color_id = 1; // BLACK

            img::fill_if(characters[i].view, characters[i].color_id, filter::can_set_color_id);

            x += width;
        }
    }


    static void write_to_view(AsciiFilter const& filter, sv::StringView const& src, SubView const& dst)
    {
        u32 const height = std::min(filter.filter.height, dst.height);
        
        auto dst_rect = to_rect(0, 0, 0, height);

        for (u32 i = 0; i < src.length && dst_rect.x_end < dst.width; i++)
        {
            auto character = filter.characters[src.data_[i] - ' '].view;

            auto char_width = character.width;

            if (dst_rect.x_end + char_width > dst.width)
            {
                char_width = dst.width - dst_rect.x_end;
            }

            dst_rect.x_end += char_width;

            auto char_view = img::sub_view(character, to_rect(0, 0, char_width, height));
            
            auto dst_view = img::sub_view(dst, dst_rect);

            img::transform(char_view, dst_view, filter::to_render_color);            

            dst_rect.x_begin = dst_rect.x_end;
        }
    }
}


namespace sound
{
    class State
    {
    public:
        Sound sound;
        b32 is_on;
    };

    class SoundState
    {
    public:
        static constexpr u32 count = 4;

        union 
        {
            State list[count];

            struct 
            {
                State laser;
                State retro;
                State door;
                State force_field;
            };
        };

    };
}


namespace music
{
    class State
    {
    public:
        Music music;
        b32 is_on;
    };


    class MusicState
    {
    public:
        static constexpr u32 count = 1;

        union 
        {
            State list[count];

            struct 
            {
                State song;
            };
        };
    };
}


namespace app
{
    

    class AudioState
    {
    public:
        f32 master_volume;

        sound::SoundState sounds;
        music::MusicState music;        
    };


    void destroy_audio_state(AudioState& audio)
    {
        for (u32 i = 0; i < audio.sounds.count; i++)
        {
            audio::destroy_sound(audio.sounds.list[i].sound);
        }

        for (u32 i = 0; i < audio.music.count; i++)
        {
            audio::destroy_music(audio.music.list[i].music);
        }
    }


    class StateData
    {
    public:

        Pixel background_color;
        
        filter::KeyboardFilter keyboard_filter;        
        filter::MouseFilter mouse_filter;
        filter::ControllerFilter controller_filter;
        filter::AsciiFilter ascii_filter;

        sv::StringView mouse_coords;
        
        SubView screen_keyboard;
        SubView screen_mouse;
        SubView screen_controller;
        SubView screen_mouse_coords;

        AudioState audio;

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

        destroy_audio_state(state_data.audio);
        
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

        filter::make_ascii_filter(filter, scale);
    }


    bool init_screen_filters(app::AppState& state)
    {
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

        constexpr u32 ASCII_SCALE = 1;

        auto const ascii_width = raw_ascii.width * ASCII_SCALE;
        auto const ascii_height = raw_ascii.height * ASCII_SCALE;

        u32 screen_width = std::max(keyboard_width, controller_width + mouse_width);
        u32 screen_height = keyboard_height + std::max(mouse_height, controller_height);

        constexpr u32 mouse_coord_capacity = sizeof("(0000, 0000)");

        auto& state_data = *state.data_;

        auto const buffer_bytes = screen_width * screen_height + ascii_width * ascii_height + mouse_coord_capacity;

        auto& u8_buffer = state_data.u8_data;
        u8_buffer = img::create_buffer8(buffer_bytes);   

        init_keyboard_filter(state_data.keyboard_filter, raw_keyboard, u8_buffer);
        init_mouse_filter(state_data.mouse_filter, raw_mouse, u8_buffer);
        init_controller_filter(state_data.controller_filter, raw_controller, u8_buffer);
        init_ascii_filter(state_data.ascii_filter, raw_ascii, ASCII_SCALE, u8_buffer);
        state_data.mouse_coords = sv::make_view(mouse_coord_capacity, u8_buffer);

        auto& screen = state.screen;

        screen.width = screen_width;
        screen.height = screen_height;        

        state_data.background_color = img::to_pixel(128, 128, 128);

        cleanup();

        return true;
    }


    bool init_audio(app::AudioState& audio)
    {
        if (!audio::init_audio())
        {
            return false;
        }

        audio.master_volume = 0.5f;

        auto& sounds = audio.sounds;

        if (!load_laser_sound(sounds.laser.sound))
        {
            return false;
        }

        if (!load_retro_sound(sounds.retro.sound))
        {
            return false;
        }

        if (!load_door_sound(sounds.door.sound))
        {
            return false;
        }

        if (!load_force_field_sound(sounds.force_field.sound))
        {
            return false;
        }

        auto& music = audio.music;

        if (!load_mellow_music(music.song.music))
        {
            return false;
        }
        

        for (u32 i = 0; i < sounds.count; i++)
        {
            sounds.list[i].is_on = false;
        }

        for (u32 i = 0; i < music.count; i++)
        {
            music.list[i].is_on = false;
        }

        return true;
    }


    void init_screen_ui(app::AppState& state)
    {
        auto& state_data = *state.data_;
        auto& screen = state.screen;

        auto keyboard = to_rect(0, 0, state_data.keyboard_filter.filter.width, state_data.keyboard_filter.filter.height);

        auto controller = to_rect(0, keyboard.y_end, state_data.controller_filter.filter.width, state_data.controller_filter.filter.height);
        
        auto mouse = to_rect(controller.x_end, controller.y_begin, state_data.mouse_filter.filter.width, state_data.mouse_filter.filter.height);
        
        state_data.screen_keyboard = img::sub_view(screen, keyboard);
        state_data.screen_mouse = img::sub_view(screen, mouse);
        state_data.screen_controller = img::sub_view(screen, controller);        

        auto const mouse_width = state_data.screen_mouse.width;
        auto const mouse_height = state_data.screen_mouse.height;

        auto const coord_x = mouse_width / 6;
        auto const coord_y = mouse_height / 2;
        auto const coord_width = mouse_width * 2 / 3;
        auto const coord_height = state_data.ascii_filter.filter.height;
        auto coords = to_rect(coord_x, coord_y, coord_width, coord_height);

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


    void update_mouse_coords(sv::StringView& coords, input::Input const& input)
    {
        auto mouse_pos = input.mouse.window_pos;

        sv::zero_view(coords);

        qsnprintf(coords.data_, coords.capacity, "(%d, %d)", mouse_pos.x, mouse_pos.y);

        coords.length = std::strlen(coords.data_);
    }


    void update_audio_volume(app::AudioState& state, input::Input const& input)
    {
        constexpr f32 delta = 0.1;

        if (input.mouse.wheel.y == 0)
        {
            return;
        }

        state.master_volume += (input.mouse.wheel.y) * delta;
    }


    void update_sounds(sound::SoundState& sounds, input::Input const& input)
    {
        auto const map_sound_input = [](auto const& btn, auto& sound)
        {
            if (btn.is_down && !sound.is_on)
            {
                sound.is_on = true;
            }
        };

        map_sound_input(input.keyboard.kbd_1, sounds.laser);
        map_sound_input(input.keyboard.kbd_2, sounds.retro);
        map_sound_input(input.keyboard.kbd_3, sounds.door);
        map_sound_input(input.keyboard.kbd_4, sounds.force_field);
    }


    void update_music(music::MusicState& music, input::Input const& input)
    {
        if (input.keyboard.kbd_space.is_down)
        {
            if (music.song.is_on)
            {
                audio::toggle_pause_music(music.song.music);
            }
            else
            {
                audio::play_music(music.song.music);
                music.song.is_on = true;
            }
        }
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

        //img::fill(state.screen_mouse_coords, img::to_pixel(0, 128, 0));
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


    void play_audio(app::AudioState& audio)
    {
        audio.master_volume = audio::set_volume(audio.master_volume);


        auto& sounds = audio.sounds;
        for (u32 i = 0; i < sounds.count; i++)
        {
            auto& sound = sounds.list[i];
            if (sound.is_on)
            {
                audio::play_sound(sound.sound);
                sound.is_on = false;
            }
        }
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
            close(state);
            return false;
        }

        if (!init_screen_filters(state))
        {
            printf("Error: create_state_data()\n");
            close(state);
            return false;
        }

        auto& state_data = *state.data_;

        if (!init_audio(state_data.audio))
        {
            printf("Error: init_audio()\n");
            close(state);
            return false;
        }

        state_data.is_init = false;       

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {
        auto& screen = state.screen;
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

        update_audio_volume(state_data.audio, input);
        update_sounds(state_data.audio.sounds, input);
        update_music(state_data.audio.music, input);

        img::fill(screen, state_data.background_color);
        render_keyboard(state_data);
        render_mouse(state_data);
        render_controller(state_data);

        play_audio(state_data.audio);
    }


    void close(AppState& state)
    {
        destroy_state_data(state);
        audio::close_audio();
    }
}