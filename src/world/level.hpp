#pragma once

#include "field.hpp"

#include <string>
#include <vector>

namespace world
{
class Level
{
public:
    Level(int width, int height, std::string name);

    Field& at(int x, int y) { return map.at(width * x + y); }
    const Field& at(int x, int y) const { return map.at(width * x + y); }

private:
    int width;
    int height;

    std::string name{};
    std::vector<Field> map{};

};
}
