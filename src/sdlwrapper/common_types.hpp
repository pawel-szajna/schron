#pragma once

#include <cstdint>

namespace sdl
{
using Pixel = uint32_t;

struct Rectangle
{
    int x, y;
    int width, height;
};

struct Color
{
    uint8_t r, g, b, a;
};

struct Vertex
{
    float x, y;
    uint8_t r, g, b, a;
    float texX, texY;
};

enum class BlendMode
{
    None,
    Blend,
    Add,
    Mod,
};
}
