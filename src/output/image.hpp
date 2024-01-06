#pragma once

#include "output.hpp"
#include "../util/memory_buffer.hpp"


#include <functional>

namespace mb = memory_buffer;

template <class F>
using fn = std::function<F>;

namespace image
{
    using Buffer8 = MemoryBuffer<u8>;


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
 


    inline Buffer8 create_buffer8(u32 n_pixels)
	{
		Buffer8 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}


    void destroy_image(Image& image);
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image);

    GrayView make_view(u32 width, u32 height, Buffer8& buffer);
}


/* sub_view */

namespace image
{
    template <typename T>
    static MatrixSubView2D<T> sub_view(MatrixView2D<T> const& view, Rect2Du32 const& range)
    {
        MatrixSubView2D<T> sub_view{};

        sub_view.matrix_data_ = view.matrix_data_;
        sub_view.matrix_width = view.width;
        sub_view.range = range;
        sub_view.width = range.x_end - range.x_begin;
        sub_view.height = range.y_end - range.y_begin;

        return sub_view;
    }


    template <typename T>
    static MatrixSubView2D<T> sub_view(MatrixSubView2D<T> const& view, Rect2Du32 const& range)
    {
        MatrixSubView2D<T> sub_view{};

        sub_view.matrix_data_ = view.matrix_data_;
        sub_view.matrix_width = view.matrix_width;

        sub_view.x_begin = range.x_begin + view.x_begin;
		sub_view.x_end = range.x_end + view.x_begin;
		sub_view.y_begin = range.y_begin + view.y_begin;
		sub_view.y_end = range.y_end + view.y_begin;

		sub_view.width = range.x_end - range.x_begin;
		sub_view.height = range.y_end - range.y_begin;

        return sub_view;
    }
}


/* fill */

namespace image
{
    void fill(ImageView const& view, Pixel color);

    void fill(SubView const& view, Pixel color);

    void fill_if(GraySubView const& view, u8 gray, fn<bool(u8)> const& pred);
}


/* transform */

namespace image
{
    void transform(GrayView const& src, SubView const& dst, fn<Pixel(u8, Pixel)> const& func);

    void transform(GraySubView const& src, SubView const& dst, fn<Pixel(u8, Pixel)> const& func);

    void transform(ImageView const& src, GrayView const& dst, fn<u8(Pixel)> const& func);

    void transform_scale_up(ImageView const& src, GrayView const& dst, u32 scale, fn<u8(Pixel)> const& func);
}


/* read */

namespace image
{
    bool read_image_from_file(const char* img_path_src, Image& image_dst);
}