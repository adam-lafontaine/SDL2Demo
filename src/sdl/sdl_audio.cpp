#include "sdl_include.hpp"


namespace audio
{
    bool create_audio(AudioState& state)
    {
        state.master_volume = 0.5;
        
        return true;
    }
}