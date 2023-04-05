#pragma once

#include <utility>

namespace engine
{
constexpr auto cross(double x1, double y1, double x2, double y2) { return x1 * y2 - x2 * y1; }
constexpr auto intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    auto c1 = cross(x1, y1, x2, y2);
    auto c2 = cross(x3, y3, x4, y4);
    auto m = cross(x1 - x2, y1 - y2, x3 - x4, y3 - y4);
    return std::make_pair(cross(c1, x1 - x2, c2, x3 - x4) / m, cross(c1, y1 - y2, c2, y3 - y4) / m);
}
}
