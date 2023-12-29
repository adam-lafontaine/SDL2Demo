#include "image.hpp"


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


    void fill(Image const& image, Pixel color)
    {
        fill_span(image.data_, color, image.width * image.height);
    }
}