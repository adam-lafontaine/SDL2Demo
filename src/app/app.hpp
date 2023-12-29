#pragma once

#include "../input/input.hpp"

namespace app
{
    constexpr auto APP_TITLE = "SDL2 Demo";


    class AppState
    {
    public:
        ImageView screen_view;

        Pixel screen_color;
    };


    bool init(AppState& state);

    void update(AppState& state, input::Input const& input);

    void close(AppState& state);

}