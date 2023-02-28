#pragma once

#include "mode_executor.hpp"

#include <memory>

namespace engine
{
class Engine;
}

namespace sdl
{
class Surface;
}

namespace ui
{
class UI;
namespace editor
{
class Editor;
}
}

namespace world
{
class Level;
class World;
}

namespace game
{
class ModeInGame : public ModeExecutor
{
public:

    ModeInGame(ui::UI& ui, world::World& world, sdl::Surface& target);
    virtual ~ModeInGame();

    void entry() override;
    void exit() override;
    std::optional<GameMode> frame(double frameTime) override;

private:

    double playerX = 0.0;
    double playerY = 0.0;
    double playerZ = 0.5;
    double playerAngle = 0.0;

    std::unique_ptr<engine::Engine> engine{};
    std::unique_ptr<ui::editor::Editor> editor{};

    ui::UI& ui;
    world::World& world;

    sdl::Surface& target;
};
}
