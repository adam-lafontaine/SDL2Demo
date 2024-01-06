#pragma once

#include "../util/types.hpp"


/*  image types */

namespace image
{
    class RGBAu8
    {
    public:
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    };


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


    using Pixel = RGBAu8;
    using Image = Matrix2D<Pixel>;
    using ImageView = MatrixView2D<Pixel>;
    using SubView = MatrixSubView2D<Pixel>;    
    using GraySubView = MatrixSubView2D<u8>;
    using ImageGray = Matrix2D<u8>;
    using GrayView = MatrixView2D<u8>;    
}