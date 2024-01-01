#pragma once

#include "../input/input.hpp"

namespace app
{
    constexpr auto APP_TITLE = "SDL2 Demo";


    class StateData;


    class AppState
    {
    public:
        ImageView screen_view;

        StateData* data_ = nullptr;
    };


    bool init(AppState& state);

    void update(AppState& state, input::Input const& input);

    void close(AppState& state);

}