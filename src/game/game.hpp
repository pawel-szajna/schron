#pragma once

#include "game_mode.hpp"
#include "mode_executor.hpp"
#include "sdlwrapper/sdlwrapper.hpp"

#include <memory>
#include <unordered_map>

namespace world
{
class World;
}

namespace game
{

class Game
{
    using ModesMap = std::unordered_map<GameMode, std::unique_ptr<ModeExecutor>>;
public:

    Game();
    ~Game();

    void start();

private:

    void mainLoop();
    void switchMode(GameMode target);

    GameMode mode{GameMode::Init};

    ModesMap modes{};

    sdl::Surface mainWindow;
    sdl::Surface screen;

    std::unique_ptr<world::World> world{};
};
}
