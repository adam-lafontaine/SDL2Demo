#pragma once

#include "../util/types.hpp"


/*  image basic */

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

    using Pixel = RGBAu8;
    using Image = Matrix2D<Pixel>;
    using ImageView = MatrixView2D<Pixel>;    
    using ImageGray = Matrix2D<u8>;
    using GrayView = MatrixView2D<u8>;


    bool create_image(Image& image, u32 width, u32 height);

    void destroy_image(Image& image);
}


/* audio basic */

namespace audio
{
    class Music
    {
    public:
        void* data_;

        int placeholder;
    };


    class Sound
    {
    public:
        void* data_;

        int placeholder;
    };


    void destroy_music(Music& music);

    void destroy_sound(Sound& sound);

}