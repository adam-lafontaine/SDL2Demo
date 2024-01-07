#pragma once

#include "output.hpp"


namespace audio
{
    bool init_audio();

    void close_audio();

    bool load_music_from_file(cstr music_file_path, Music& music);

    bool load_sound_from_file(cstr sound_file_path, Sound& sound);

    f32 set_volume(f32 volume);

    void play_music(Music const& music);

    void toggle_pause_music(Music const& music);

    void play_sound(Sound const& sound);
}