#include "image.hpp"
#include "../util/stb_image/stb_image.h"

#include <cstring>

namespace image
{
    bool create_image(Image& image, u32 width, u32 height)
    {
        auto data = (Pixel*)std::malloc((size_t)(sizeof(Pixel) * width * height));
        if (!data)
        {
            return false;
        }

        image.data_ = data;
        image.width = width;
        image.height = height;

        return true;
    }

    
    void destroy_image(Image& image)
    {
        if (image.data_)
		{
			std::free((void*)image.data_);
			image.data_ = nullptr;
		}

		image.width = 0;
		image.height = 0;
    }
}


/* row_begin */

namespace image
{
    template <typename T>
	static inline SpanView<T> row_begin(MatrixView2D<T> const& view, u32 y)
	{
        SpanView<T> span{};

        span.begin = view.matrix_data_ + (u64)(y * view.width);
        span.length = view.width;

        return span;
	}


    template <typename T>
    static inline SpanView<T> row_begin(MatrixSubView2D<T> const& view, u32 y)
    {
        SpanView<T> span{};

        span.begin = view.matrix_data_ + (u64)((view.y_begin + y) * view.matrix_width + view.x_begin);
        span.length = view.width;

        return span;
    }


    template <typename T>
    static inline SpanView<T> to_span(MatrixView2D<T> const& view)
    {
        SpanView<T> span{};

        span.begin = view.matrix_data_;
        span.length = view.width * view.height;

        return span;
    }
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image)
    {
        ImageView view{};

        view.width = image.width;
        view.height = image.height;
        view.matrix_data_ = image.data_;

        return view;
    }


    GrayView make_view(u32 width, u32 height, Buffer8& buffer)
    {
        GrayView view{};

        view.matrix_data_ = mb::push_elements(buffer, width * height);
        if (view.matrix_data_)
        {
            view.width = width;
            view.height = height;
        }

        return view;
    }
}


/* fill */

namespace image
{
    template <typename T>
	static inline void fill_span(SpanView<T> const& dst, T value)
	{
		for (u32 i = 0; i < dst.length; ++i)
		{
			dst.begin[i] = value;
		}
	}


    template <typename T>
    static void fill_span_if(SpanView<T> const& dst, u8 value, fn<bool(T)> const& pred)
    {
        for (u32 i = 0; i < dst.length; ++i)
		{
			dst.begin[i] = pred(dst.begin[i]) ? value : dst.begin[i];
		}
    }


    void fill(ImageView const& view, Pixel color)
    {
        fill_span(to_span(view), color);
    }


    void fill(SubView const& view, Pixel color)
    {
        for (u32 y = 0; y < view.height; y++)
        {
            fill_span(row_begin(view, y), color);
        }
    }


    void fill_if(GraySubView const& view, u8 gray, fn<bool(u8)> const& pred)
    {
        for (u32 y = 0; y < view.height; y++)
        {
            fill_span_if(row_begin(view, y), gray, pred);
        }
    }
}


/* transform */

namespace image
{
    static void transform_span(SpanView<u8> const& src, SpanView<Pixel> const& dst, fn<Pixel(u8, Pixel)> const& func)
    {
        auto s = src.begin;
        auto d = dst.begin;

        for (u32 i = 0; i < src.length; i++)
        {
            d[i] = func(s[i], d[i]);
        }
    }


    static void transform_span(SpanView<Pixel> const& src, SpanView<u8> const& dst, fn<u8(Pixel)> const& func)
    {
        auto s = src.begin;
        auto d = dst.begin;

        for (u32 i = 0; i < src.length; i++)
        {
            d[i] = func(s[i]);
        }
    }


    void transform(GrayView const& src, SubView const& dst, fn<Pixel(u8, Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

        for (u32 y = 0; y < src.height; y++)
        {
            transform_span(row_begin(src, y), row_begin(dst, y), func);
        }
    }


    void transform(GraySubView const& src, SubView const& dst, fn<Pixel(u8, Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

        for (u32 y = 0; y < src.height; y++)
        {
            transform_span(row_begin(src, y), row_begin(dst, y), func);
        }
    }


    void transform(ImageView const& src, GrayView const& dst, fn<u8(Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width);
        assert(dst.height == src.height);

        transform_span(to_span(src), to_span(dst), func);
    }


    void transform_scale_up(ImageView const& src, GrayView const& dst, u32 scale, fn<u8(Pixel)> const& func)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width * scale);
        assert(dst.height == src.height * scale);

        for (u32 src_y = 0; src_y < src.height; src_y++)
        {
            auto src_row = row_begin(src, src_y);
            for (u32 src_x = 0; src_x < src.width; src_x++)
            {
                auto const value = func(src_row.begin[src_x]);

                auto dst_y = src_y * scale;
                for (u32 offset_y = 0; offset_y < scale; offset_y++, dst_y++)
                {
                    auto dst_row = row_begin(dst, dst_y);

                    auto dst_x = src_x * scale;
                    for (u32 offset_x = 0; offset_x < scale; offset_x++, dst_x++)
                    {
                        dst_row.begin[dst_x] = value;
                    }
                }
            }
        }
    }
}


/* read */

namespace image
{
    static bool has_extension(const char* filename, const char* ext)
    {
        size_t file_length = std::strlen(filename);
        size_t ext_length = std::strlen(ext);

        return !std::strcmp(&filename[file_length - ext_length], ext);
    }


    static bool is_valid_image_file(const char* filename)
    {
        return 
            has_extension(filename, ".bmp") || 
            has_extension(filename, ".BMP") ||
            has_extension(filename, ".png")||
            has_extension(filename, ".PNG");
    }


    bool read_image_from_file(const char* img_path_src, Image& image_dst)
	{
        auto is_valid_file = is_valid_image_file(img_path_src);
        assert(is_valid_file && "invalid image file");

        if (!is_valid_file)
        {
            return false;
        }

		int width = 0;
		int height = 0;
		int image_channels = 0;
		int desired_channels = 4;

		auto data = (Pixel*)stbi_load(img_path_src, &width, &height, &image_channels, desired_channels);

		assert(data && "stbi_load() - no image data");
		assert(width && "stbi_load() - no image width");
		assert(height && "stbi_load() - no image height");

		if (!data)
		{
			return false;
		}

		image_dst.data_ = data;
		image_dst.width = width;
		image_dst.height = height;

		return true;
	}
}