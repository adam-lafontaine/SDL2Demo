#pragma once

#include "../util/types.hpp"



namespace image
{
    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue, u8 alpha)
    {
        Pixel p{};
        p.red = red;
        p.green = green;
        p.blue = blue;
        p.alpha = alpha;

        return p;
    }


    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue)
    {
        return to_pixel(red, green, blue, 255);
    }
}


/* fill */

namespace image
{
    void fill(Image const& image, Pixel color);
}


/* read */

namespace image
{
    bool read_image_from_file(const char* img_path_src, Image& image_dst);
}