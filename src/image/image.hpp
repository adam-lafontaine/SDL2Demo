#pragma once

#include "../util/memory_buffer.hpp"

#include <filesystem>
#include <functional>


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


/* gray */

namespace image
{
    using ImageGray = Matrix2D<u8>;
    using ImageViewGray = MatrixView2D<u8>;
}


/* create destroy */

namespace image
{
    bool create_image(Image& image, u32 width, u32 height);

    void destroy_image(Image& image);


    using Buffer32 = MemoryBuffer<Pixel>;
    using Buffer8 = MemoryBuffer<u8>;


	inline Buffer32 create_buffer32(u32 n_pixels)
	{
		Buffer32 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}


    inline Buffer8 create_buffer8(u32 n_pixels)
	{
		Buffer8 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image);

    ImageView make_view(u32 width, u32 height, Buffer32& buffer);

    ImageViewGray make_view(u32 width, u32 height, Buffer8& buffer);
}


/* sub_view */

namespace image
{
    template <typename T>
    class MatrixSubView2D
    {
    public:
        T*  matrix_data_;
        u32 matrix_width;

		u32 width;
		u32 height;

        union
        {
            Rect2Du32 range;

            struct
			{
				u32 x_begin;
				u32 x_end;
				u32 y_begin;
				u32 y_end;
			};
        };
    };


    using ImageSubView = MatrixSubView2D<Pixel>;
    
    using ImageSubViewGray = MatrixSubView2D<u8>;


    ImageSubView sub_view(ImageView const& image, Rect2Du32 const& range);

    ImageSubViewGray sub_view(ImageViewGray const& view, Rect2Du32 const& range);
}


/* fill */

namespace image
{
    void fill(ImageView const& view, Pixel color);

    void fill(ImageSubView const& view, Pixel color);

    void fill_if(ImageSubView const& view, Pixel color, std::function<bool(Pixel)> const& pred);
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

    void alpha_blend(ImageView const& src, ImageSubView const& dst);

    void alpha_blend(ImageView const& view, Pixel color);

    void alpha_blend(ImageSubView const& view, Pixel color);
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