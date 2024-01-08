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


    static f32 get_music_volume()
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;
        
        auto i_volume = Mix_VolumeMusic(-1);        

        return (f32)(i_volume - MIN) / (MAX - MIN);
    }


    static f32 get_sound_volume()
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        auto i_volume = Mix_Volume(-1, -1);        

        return (f32)(i_volume - MIN) / (MAX - MIN);
    }


    template <typename T>
    static constexpr T clamp(T value, T min, T max)
    {
        const T t = value < min ? min : value;
        return t > max ? max : t;
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

        music.is_on = false;
        music.is_paused = false;
    }


    void destroy_sound(Sound& sound)
    {
        Mix_FreeChunk((sound_p)sound.data_);
        sound.data_ = nullptr;

        sound.is_on = false;
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

        set_master_volume(0.5f);

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

        music.is_on = false;
        music.is_paused = false;

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

        sound.is_on = false;

        return true;
    }


    f32 set_music_volume(f32 volume)
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        volume = clamp(volume, 0.0f, 1.0f);

        auto i_volume = (int)(volume * (MAX - MIN));
        if (i_volume == Mix_VolumeMusic(-1))
        {
            return get_music_volume();
        }
        
        Mix_VolumeMusic(i_volume);
        return get_music_volume();
    }


    f32 set_sound_volume(f32 volume)
    {
        constexpr int MAX = MIX_MAX_VOLUME;
        constexpr int MIN = 0;

        volume = clamp(volume, 0.0f, 1.0f);

        auto i_volume = (int)(volume * (MAX - MIN));
        if (i_volume == Mix_Volume(-1, -1))
        {
            return get_sound_volume();
        }

        Mix_Volume(-1, i_volume);
        return get_sound_volume();
    }


    void play_music(Music& music)
    {
        constexpr int FOREVER = -1;
        Mix_PlayMusic((music_p)music.data_, FOREVER);
        music.is_on = true;
    }


    void toggle_pause_music(Music& music)
    {
        if (!music.is_on)
        {
            return;
        }

        if (Mix_PausedMusic() == 1)
        {
            Mix_ResumeMusic();
            music.is_paused = false;
        }
        else
        {
            Mix_PauseMusic();
            music.is_paused = false;
        }
    }


    static Sound* sound_list[10] = { 0 };

    static void sound_finished(int channel)
    {
        if (sound_list[channel])
        {
            sound_list[channel]->is_on = false;
        }
    }


    void play_sound(Sound& sound)
    {
        constexpr int N_REPEATS = 0;
        auto channel = Mix_PlayChannel(-1, (sound_p)sound.data_, N_REPEATS);

        if (!sound_list[channel])
        {
            sound_list[channel] = &sound;
            Mix_ChannelFinished(sound_finished);
        }        

        sound.is_on = true;
    }
   
}