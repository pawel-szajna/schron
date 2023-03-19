#pragma once

#include "mode_executor.hpp"
#include "player.hpp"

#include <memory>
#include <string>

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
    enum class State
    {
        Default,
        Choice,
        Speech,
    };

public:

    ModeInGame(ui::UI& ui, world::World& world, sdl::Renderer& renderer, scripting::Scripting& scripting);
    virtual ~ModeInGame();

    void entry() override;
    void exit() override;
    std::optional<GameMode> frame(double frameTime) override;
    void event(const sdl::event::Event& event) override;

private:

    void choice(std::string caption, std::vector<std::string> choices);
    void text(std::string caption);
    void speech(std::string person, std::string caption);

    Player player{};

    std::unique_ptr<engine::Engine> engine;

    ui::UI& ui;
    world::World& world;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;

    int lastX, lastY, lastZ;
    double lastFrameX, lastFrameY, lastFrameZ, lastFrameAngle;

    State state{State::Default};

    std::optional<std::pair<int, std::string>> tooltipWidget;
    int choiceWidget;
};
}
