#pragma once

#define SIMAGE_NO_WRITE
#define SIMAGE_NO_RESIZE


#define STBI_NO_GIF
#define STBI_NO_PSD
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_HDR
#define STBI_NO_TGA
#define STBI_NO_JPEG

//#define STBI_NO_PNG
//#define STBI_NO_BMP
//#define STBI_NO_SIMD
//#define STBI_NEON

/*
#include "../../src/util/mem.hpp"
#define STBI_MALLOC mem::v_malloc
#define STBI_REALLOC mem::v_realloc
#define STBI_FREE mem::v_free
*/

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include "stb_image_resize.h"
