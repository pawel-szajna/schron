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
    double x, y, z{0.6}, angle;
};

class Player
{
public:

    Player();
    ~Player();

    void handleMovement(double walk, double strafe, double rotate, const world::Level& level);
    const Position& getPosition() const;

private:

    Position position;

};
}