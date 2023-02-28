#pragma once

namespace util
{
struct Position
{
    double x, y;
    double dirX, dirY;
    double planeX, planeY;
};

template<typename T>
struct Vector
{
    // Vector(const std::pair<T, T>& pair) : x(pair.first), y(pair.second) {}
    T x, y;
};

using IntVec = Vector<int>;
using DoubleVec = Vector<double>;

}
