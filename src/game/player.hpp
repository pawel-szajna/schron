#pragma once

#include "util/constants.hpp"

#include <numbers>
#include <queue>

namespace world
{
class Level;
}

namespace game
{
class Position
{
public:
    int sector{15};
    double x{8.5}, y{7.5}, z{0.6}, angle{-std::numbers::pi / 2};
    double fovH = c::renderHeight * 0.65;
    double fovV = c::renderWidth * 0.22;
};

class Player
{
    struct MoveTarget
    {
        double x, y, z;
        int factor;
    };

public:

    enum class Direction
    {
        Forward,
        Backward,
        Left,
        Right,
    };
    enum class Rotation
    {
        Left,
        Right,
    };

    Player();
    ~Player();

    void move(const world::Level& level, Direction direction);
    void rotate(Rotation rotation);

    void frame(const world::Level& level, double frameTime);
    [[nodiscard]] const Position& getPosition() const;

private:

    void acquireMove();
    double enterable(const world::Level& level, double x, double y) const;

    Position position;
    Position target;

    std::queue<MoveTarget> moves{};

    int moving{0};
    int rotating{0};
};
}