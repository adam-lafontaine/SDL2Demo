#pragma once

#include "../util/memory_buffer.hpp"

#include <filesystem>


namespace mb = memory_buffer;


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


    using Buffer32 = MemoryBuffer<Pixel>;


	inline Buffer32 create_buffer32(u32 n_pixels)
	{
		Buffer32 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image);

    ImageView make_view(u32 width, u32 height, Buffer32& buffer);
}


/* fill */

namespace image
{
    void fill(ImageView const& image, Pixel color);
}


/* copy */

namespace image
{
    void copy(ImageView const& src, ImageView const& dst);
}


/* blend */

namespace image
{
    void alpha_blend(ImageView const& src, ImageView const& dst);
}


/* scale */

namespace image
{
    void scale_up(ImageView const& src, ImageView const& dst, u32 scale);
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