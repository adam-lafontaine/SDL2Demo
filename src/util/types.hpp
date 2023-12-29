#pragma once

#include <cstdint>
#include <cstddef>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using b32 = u32;

using cstr = const char*;


template <typename T>
class Vec2D
{
public:
	T x;
	T y;
};


template <typename T>
class Rect2D
{
public:
	T x_begin;
	T x_end;
	T y_begin;
	T y_end;
};


using Vec2Di32 = Vec2D<i32>;
using Vec2Df32 = Vec2D<f32>;
using Vec2Du32 = Vec2D<u32>;
using Vec2Df64 = Vec2D<f64>;

using Point2Di32 = Vec2Di32;
using Point2Df32 = Vec2Df32;
using Point2Du32 = Vec2Du32;
using Point2Df64 = Vec2Df64;

using Rect2Di32 = Rect2D<i32>;
using Rect2Df32 = Rect2D<f32>;
using Rect2Du32 = Rect2D<u32>;
using Rect2Df64 = Rect2D<f64>;
