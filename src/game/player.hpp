#pragma once

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
    int sector{1};
    double x{0.5}, y{0.5}, z{0.6}, angle{std::numbers::pi / 2};
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