#pragma once

#include "../input/input.hpp"

namespace app
{
    constexpr auto APP_TITLE = "SDL2 Demo";


    class AppState
    {
    public:
        Image screen_image;
    };


    bool init(AppState& state);

    void update(AppState& state, input::Input const& input);

    void close(AppState& state);

}