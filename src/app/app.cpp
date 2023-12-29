#include "app.hpp"
#include "../image/image.hpp"

namespace img = image;


namespace app
{
    bool init(AppState& state)
    {
        auto& screen = state.screen_view;

        screen.height = 450;
        screen.width = 800;

        state.screen_color = img::to_pixel(0, 128, 0);

        return true;
    }


    void update(AppState& state, input::Input const& input)
    {
        auto dir = input.mouse.wheel.y;
        auto red = (int)state.screen_color.red;
        auto blue = (int)state.screen_color.blue;
        auto delta = 10;
        auto max = 250;
        if (dir > 0)
        {
            if (blue > 0)
            {
                blue -= delta;
            }
            else if (red < max)
            {
                red += delta;
            }            
        }
        
        if (dir < 0)
        {
            if (red > 0)
            {
                red -= delta;
            }
            else if (blue < max)
            {
                blue += delta;
            }
        }

        auto& screen = state.screen_view;

        state.screen_color.red = (u8)red;
        state.screen_color.blue = (u8)blue;

        img::fill(screen, state.screen_color);
    }


    void close(AppState& state)
    {

    }
}