#include "sdl_include.hpp"
#include "../output/audio.hpp"

#include <SDL2/SDL_mixer.h>
#include <cstring>
#include <cassert>

/* helpers */

namespace
{
    static bool has_extension(const char* filename, const char* ext)
    {
        size_t file_length = std::strlen(filename);
        size_t ext_length = std::strlen(ext);

        return !std::strcmp(&filename[file_length - ext_length], ext);
    }


    static bool is_valid_audio_file(const char* filename)
    {
        return 
            has_extension(filename, ".mp3") || 
            has_extension(filename, ".MP3") ||
            has_extension(filename, ".ogg") || 
            has_extension(filename, ".OGG") ||
            has_extension(filename, ".wav") ||
            has_extension(filename, ".WAV");
    }


    static f32 get_volume()
    {
        constexpr int MAX = 128;
        constexpr int MIN = 0;

        auto i_volume = Mix_Volume(-1, -1);

        return (f32)(i_volume - MIN) / (MAX - MIN);
    }
}


/* api */

namespace audio
{
    using music_p = Mix_Music*;
    using sound_p = Mix_Chunk*;


    void destroy_music(Music& music)
    {
        Mix_FreeMusic((music_p)music.data_);
        music.data_ = nullptr;

        music.placeholder = 0;
    }


    void destroy_sound(Sound& sound)
    {
        Mix_FreeChunk((sound_p)sound.data_);
        sound.data_ = nullptr;

        sound.placeholder = 0;
    }


    bool init_audio()
    {
        Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
	    SDL_Init(SDL_INIT_AUDIO);

        int const freq = 44100;
        auto const format = MIX_DEFAULT_FORMAT;
        int const channels = 1;
        int const chunk_size = 2048;

        auto rc = Mix_OpenAudio(freq, format, channels, chunk_size);
        if (rc < 0)
        {
            print_message(Mix_GetError());
            return false;
        }

        set_volume(0.5f);

        return true;
    }


    void close_audio()
    {
        Mix_Quit();
    }


    bool load_music_from_file(cstr music_file_path, Music& music)
    {
        auto is_valid_file = is_valid_audio_file(music_file_path);
        assert(is_valid_file && "invalid music file");

        if (!is_valid_file)
        {
            return false;
        }

        music_p data = Mix_LoadMUS(music_file_path);
        if (!data)
        {
            return false;
        }

        music.data_ = (void*)data;

        music.placeholder = 0;

        return true;
    }


    bool load_sound_from_file(cstr sound_file_path, Sound& sound)
    {
        auto is_valid_file = is_valid_audio_file(sound_file_path);
        assert(is_valid_file && "invalid music file");

        if (!is_valid_file)
        {
            return false;
        }

        sound_p data = Mix_LoadWAV(sound_file_path);
        if (!data)
        {
            return false;
        }

        sound.data_ = (void*)data;

        sound.placeholder = 0;

        return true;
    }


    void set_volume(f32 volume)
    {
        constexpr int MAX = 128;
        constexpr int MIN = 0;

        auto hi = volume > 1.0f;
        auto lo = volume < 0.0f;
        auto ok = !hi && !lo;

        auto i_volume = (int)(hi * MAX + lo * MIN + ok * volume * (MAX - MIN));
        if (i_volume == get_volume())
        {
            return;
        }

        Mix_Volume(-1, i_volume);
    }


    void play_music(Music const& music)
    {
        constexpr int FOREVER = -1;
        Mix_PlayMusic((music_p)music.data_, FOREVER);
    }


    void play_sound(Sound const& sound)
    {
        constexpr int N_REPEATS = 0;
        Mix_PlayChannel(-1, (sound_p)sound.data_, N_REPEATS);
    }
   
}