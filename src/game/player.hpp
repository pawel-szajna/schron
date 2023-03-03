#pragma once

namespace world
{
class Level;
}

namespace game
{
struct Position
{
    int sector{1};
    double x{0.5}, y{0.5}, z{0.6}, angle;
};

class Player
{
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

    void move(Direction direction);
    void rotate(Rotation rotation);

    void frame(const world::Level& level, double frameTime);
    [[nodiscard]] const Position& getPosition() const;

private:

    Position position;
    Position target;

    int moving{0};
    int strafing{0};
    int rotating{0};
};
}