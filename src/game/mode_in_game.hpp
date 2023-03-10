#pragma once

#include "mode_executor.hpp"
#include "player.hpp"

#include <memory>

namespace engine
{
class Engine;
}

namespace scripting
{
class Scripting;
}

namespace sdl
{
class Renderer;
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

    ModeInGame(ui::UI& ui, world::World& world, sdl::Renderer& renderer, scripting::Scripting& scripting);
    virtual ~ModeInGame();

    void entry() override;
    void exit() override;
    std::optional<GameMode> frame(double frameTime) override;

private:

    Player player{};

    std::unique_ptr<engine::Engine> engine;

    ui::UI& ui;
    world::World& world;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;

    int lastX, lastY, lastZ;
};
}
