#include "types.hpp"

#include <cassert>


template <typename T>
class RingBuffer
{
public:
    u32 capacity = 0;
    u32 read = 0;
    u32 write = 0;

    T* data_ = nullptr;
};


namespace ring_buffer
{
    template <typename T>
    bool create_buffer(RingBuffer<T>& buffer, u32 n_elements)
    {
        assert(n_elements);
		assert(!buffer.data_);

		if (n_elements == 0 || buffer.data_)
		{
			return false;
		}

		buffer.data_ = (T*)std::malloc(n_elements * sizeof(T));
		assert(buffer.data_);

		if (!buffer.data_)
		{
			return false;
		}

		buffer.capacity = n_elements;
		buffer.read = 0;
        buffer.write = 0;

		return true;
    }


    template <typename T>
	void destroy_buffer(RingBuffer<T>& buffer)
	{
		if (buffer.data_)
		{
			std::free(buffer.data_);
		}		

		buffer.data_ = nullptr;
		buffer.capacity = 0;
		buffer.read = 0;
        buffer.write = 0;
	}    


    template <typename T>
    void copy(T* src, T* dst, u32 len)
    {
        constexpr auto sz64 = sizeof(u64);
        auto const len8 = sizeof(T) * len;        
        auto const len64 = len8 / sz64;
        auto src64 = (u64*)src;
        auto dst64 = (u64*)dst;
        auto src8 = (u8*)src;
        auto dst8 = (u8*)dst;

        for (u32 i = 0; i < len64; i++)
        {
            dst64[i] = src64[i];
        }

        for (u32 i = len64 * sz64; i < len8; i++)
        {
            dst8[i] = src8[i];
        }
    }


    template <typename T>
    void write_data(RingBuffer<T>& buffer, T* src, u32 length)
    {    
        u32 len1 = length;
        u32 len2 = 0;
        u32 new_write = buffer.write + length;

        u32 to_end = buffer.capacity - buffer.write;

        if (length > to_end)
        {
            len1 = to_end;
            len2 = length - len1;
            new_write = len2;
        }

        T* src1 = src;
        T* dst1 = buffer.data_ + buffer.write;

        T* src2 = src + len1;
        T* dst2 = buffer.data_;

        copy(src1, dst1, len1);
        copy(src2, dst2, len2);

        buffer.write = new_write;
    }


    template <typename T>
    void read_data(RingBuffer<T>& buffer, T* dst, u32 length)
    {
        u32 len1 = length;
        u32 len2 = 0;
        u32 new_read = buffer.read + length;

        u32 to_end = buffer.capacity - buffer.read;

        if (length > to_end)
        {
            len1 = to_end;
            len2 = length - len1;
            new_read = len2;
        }

        T* src1 = buffer.data_ + buffer.read;
        T* dst1 = dst;

        T* src2 = buffer.data_;
        T* dst2 = dst + len1;

        copy(src1, dst1, len1);
        copy(src2, dst2, len2);

        buffer.read = new_read;
    }
}