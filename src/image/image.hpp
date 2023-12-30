#pragma once

#include "../util/types.hpp"

#include <filesystem>



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


/* create destroy */

namespace image
{
    bool create_image(Image& image, u32 width, u32 height);

    void destroy_image(Image& image);
}


/* fill */

namespace image
{
    void fill(ImageView const& image, Pixel color);
}


/* copy */

namespace image
{
    void copy(Image const& src, ImageView const& dst);
}


/* read */

namespace image
{
    using path = std::filesystem::path;


    bool read_image_from_file(const char* img_path_src, Image& image_dst);


    inline bool read_image_from_file(path const& img_path_src, Image& image_dst)
	{
		return read_image_from_file(img_path_src.string().c_str(), image_dst);
	}
}