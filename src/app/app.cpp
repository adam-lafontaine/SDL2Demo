#include "app.hpp"


namespace app
{
    bool init(AppState& state)
    {
        auto& screen = state.screen_image;

        screen.height = 450;
        screen.width = 800;

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {

    }

    void close(AppState& state)
    {

    }
}