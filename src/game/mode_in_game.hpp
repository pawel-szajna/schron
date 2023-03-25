#pragma once

#include "mode_executor.hpp"
#include "player.hpp"
#include "sub_mode.hpp"

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
public:

    ModeInGame(ui::UI& ui,
               world::World& world,
               sdl::Renderer& renderer,
               scripting::Scripting& scripting,
               int& noiseLevel);
    virtual ~ModeInGame();

    void entry() override;
    void exit() override;
    std::optional<GameMode> frame(double frameTime) override;
    void event(const sdl::event::Event& event) override;

private:

    void startDialogue();
    void endDialogue();

    void save(const std::string& filename) const;
    void save(std::ostream& os) const;

    std::unique_ptr<engine::Engine> engine;

    ui::UI& ui;
    world::World& world;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;

    Player player;

    int lastX, lastY, lastZ;
    double lastFrameX, lastFrameY, lastFrameZ, lastFrameAngle;

    std::optional<std::pair<int, std::string>> tooltipWidget;

    std::unique_ptr<SubMode> subMode;
};
}
