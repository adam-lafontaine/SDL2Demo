#pragma once

#include "../input/input.hpp"
#include "../output/output.hpp"


namespace app
{
    using ImageView = image::ImageView;
    using AudioState = audio::AudioState;


    class StateData;


    class AppState
    {
    public:
        ImageView screen;

        AudioState audio;

        StateData* data_ = nullptr;
    };


    bool init(AppState& state);

    void update(AppState& state, input::Input const& input);

    void close(AppState& state);

}


namespace config
{
    constexpr auto APP_TITLE = "SDL2 Demo";

    constexpr u32 WINDOW_SCALE = 2;


#ifdef _WIN32
    constexpr auto ROOT = "C:/D_Data/Repos";
#else
    constexpr auto ROOT = "/home/adam/Repos";
#endif
}