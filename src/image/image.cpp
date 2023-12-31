#include "image.hpp"
#include "stb_include.hpp"

#include <cassert>
#include <cstring>

#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


/* row_begin */

namespace image
{
    template <typename T>
	static inline T* row_begin(MatrixView2D<T> const& view, u32 y)
	{
		return view.matrix_data_ + (u64)(y * view.width);
	}


    static inline Pixel* row_begin(ImageSubView const& view, u32 y)
    {
        return view.matrix_data_ + (u64)((view.y_begin + y) * view.matrix_width + view.x_begin);
    }
}


/* create destroy */

namespace image
{
    bool create_image(Image& image, u32 width, u32 height)
	{
		assert(width);
		assert(height);

		image.data_ = (Pixel*)std::malloc(width * height * sizeof(Pixel));
		if (!image.data_)
		{
			return false;
		}

		assert(image.data_ && "create_image()");

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


    ImageView make_view(u32 width, u32 height, Buffer32& buffer)
    {
        ImageView view{};

        view.matrix_data_ = mb::push_elements(buffer, width * height);
        if (view.matrix_data_)
        {
            view.width = width;
            view.height = height;
        }

        return view;
    }
}


/* sub_view */

namespace image
{
    ImageSubView sub_view(ImageView const& view, Rect2Du32 const& range)
    {
        ImageSubView sub_view{};

        sub_view.matrix_data_ = view.matrix_data_;
        sub_view.matrix_width = view.width;
        sub_view.range = range;
        sub_view.width = range.x_end - range.x_begin;
        sub_view.height = range.y_end - range.y_begin;

        return sub_view;
    }
}


/* fill */

namespace image
{
    template <typename T>
	static inline void fill_span(T* dst, T value, u32 len)
	{
		for (u32 i = 0; i < len; ++i)
		{
			dst[i] = value;
		}
	}


    void fill(ImageView const& view, Pixel color)
    {
        fill_span(view.matrix_data_, color, view.width * view.height);
    }


    void fill(ImageSubView const& view, Pixel color)
    {
        Pixel* dst = 0;

        for (u32 y = 0; y < view.height; y++)
        {
            fill_span(row_begin(view, y), color, view.width);
        }
    }
}


/* copy */

namespace image
{
    template <typename T>
    static inline void copy_span(T* src, T* dst, u32 len)
    {
        for (u32 i = 0; i < len; ++i)
		{
			dst[i] = src[i];
		}
    }



    void copy(ImageView const& src, ImageView const& dst)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(src.width == dst.width);
        assert(src.height == dst.height);

        copy_span(src.matrix_data_, dst.matrix_data_, src.width * src.height);
    }
}


/* blend */

namespace image
{
    static inline u8 blend_linear(u8 s, u8 c, f32 a)
	{
		auto blended = a * s + (1.0f - a) * c;
		return (u8)(blended + 0.5);
	}


    static inline void alpha_blend_span(Pixel* src, Pixel* dst, u32 len)
    {
        for (u32 i = 0; i < len; ++i)
        {
            auto const s = src[i];
            auto& d = dst[i];

            auto a = s.alpha / 255.0f;

            d.red = blend_linear(s.red, d.red, a);
            d.green = blend_linear(s.green, d.green, a);
            d.blue = blend_linear(s.blue, d.blue, a);
        }
    }


    void alpha_blend(ImageView const& src, ImageView const& dst)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(src.width == dst.width);
        assert(src.height == dst.height);

        alpha_blend_span(src.matrix_data_, dst.matrix_data_, src.width * src.height);
    }
}


/* scale */

namespace image
{
    void scale_up(ImageView const& src, ImageView const& dst, u32 scale)
    {
        assert(src.matrix_data_);
        assert(dst.matrix_data_);
        assert(dst.width == src.width * scale);
        assert(dst.height == src.height * scale);

        u32 i = 0;

        for (u32 src_y = 0; src_y < src.height; src_y++)
        {
            auto src_row = row_begin(src, src_y);
            for (u32 src_x = 0; src_x < src.width; src_x++)
            {
                auto const s = src_row[src_x];

                auto dst_y = src_y * scale;
                for (u32 offset_y = 0; offset_y < scale; offset_y++, dst_y++)
                {
                    auto dst_row = row_begin(dst, dst_y);

                    auto dst_x = src_x * scale;
                    for (u32 offset_x = 0; offset_x < scale; offset_x++, dst_x++)
                    {
                        dst_row[dst_x] = s;
                        i++;
                    }
                }
            }
        }

        assert(i == dst.width * dst.height);
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