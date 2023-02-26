#pragma once

#include <memory>

namespace world
{
class World;
}

namespace game
{
class Game
{
public:
    Game();
    ~Game();

    void start();

private:
    std::unique_ptr<world::World> world{};
};
}
