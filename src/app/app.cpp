#include "app.hpp"
#include "../image/image.hpp"

namespace img = image;


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
        auto& screen = state.screen_image;

        img::fill(screen, img::to_pixel(0, 255, 0));
    }

    void close(AppState& state)
    {

    }
}