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
}