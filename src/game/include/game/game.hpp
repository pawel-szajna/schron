#pragma once

#include "engine/noise.hpp"
#include "game_mode.hpp"
#include "mode_executor.hpp"
#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/surface.hpp"
#include "sdlwrapper/texture.hpp"
#include "sdlwrapper/window.hpp"
#include "util/constants.hpp"

#include <memory>
#include <unordered_map>

namespace scripting
{
class Scripting;
}

namespace ui
{
class UI;
}

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

    sdl::Window window{"Schron", c::windowWidth, c::windowHeight, 0};
    sdl::Renderer renderer{window};

    std::unique_ptr<world::World> world;
    std::unique_ptr<ui::UI> ui;
    std::unique_ptr<scripting::Scripting> scripting;

    ModesMap modes{};

    int noiseLevel{13};
    engine::Noise noise{renderer, noiseLevel};
};
} // namespace game
