#pragma once

#include "util/position.hpp"

#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

namespace world
{
class Wall
{
public:
    struct Portal
    {
        struct Transformation
        {
            double x;
            double y;
            double z;
            double angle;
        };
        int sector;
        std::optional<Transformation> transform{std::nullopt};
    };

    double xStart, yStart, xEnd, yEnd;
    std::optional<Portal> portal{std::nullopt};
};

class Sector
{
public:
    int id{};
    std::vector<Wall> walls{};
    double ceiling{1.0};
    double floor{0.0};
};
}
