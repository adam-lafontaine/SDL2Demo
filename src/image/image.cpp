#include "image.hpp"
#include "stb_include.hpp"

#include <cassert>
#include <cstring>

#ifndef NDEBUG
#include <cstdio>
#else
#define printf(fmt, ...)
#endif


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
			std::free(image.data_);
			image.data_ = nullptr;
		}

		image.width = 0;
		image.height = 0;
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



    void copy(Image const& src, ImageView const& dst)
    {
        assert(src.data_);
        assert(dst.matrix_data_);
        assert(src.width == dst.width);
        assert(src.height == dst.height);

        copy_span(src.data_, dst.matrix_data_, src.width * src.height);
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