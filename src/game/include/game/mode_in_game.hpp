#pragma once

#include "mode_executor.hpp"
#include "player.hpp"
#include "sub_mode.hpp"

#include <functional>
#include <memory>
#include <queue>
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
class Text;
class UI;

namespace editor
{
class Editor;
}
} // namespace ui

namespace world
{
class Level;
class World;
} // namespace world

namespace game
{
class ModeInGame : public ModeExecutor
{
    using TimedFunction = std::function<void(void)>;

    struct TimedExecution
    {
        uint64_t time;
        TimedFunction function;

        bool operator<(const TimedExecution& other) const { return time < other.time; }
    };

public:

    ModeInGame(
        ui::UI& ui, world::World& world, sdl::Renderer& renderer, scripting::Scripting& scripting, int& noiseLevel);
    virtual ~ModeInGame();

    void entry() override;
    void exit() override;
    std::optional<GameMode> frame(double frameTime) override;
    void event(const sdl::event::Event& event) override;

private:

    void notify(uint64_t timeout, const std::string& message);
    void enqueue(uint64_t timeout, TimedFunction fn);

    void startDialogue();
    void endDialogue();

    void save(const std::string& filename);
    void save(std::ostream& os) const;

    std::unique_ptr<engine::Engine> engine;

    ui::UI& ui;
    world::World& world;
    sdl::Renderer& renderer;
    scripting::Scripting& scripting;

    Player player;

    int lastX, lastY, lastZ;
    double lastFrameX, lastFrameY, lastFrameZ, lastFrameAngle;

    bool shouldSave{false};

    std::optional<std::pair<std::shared_ptr<ui::Text>, std::string>> tooltipWidget;

    std::priority_queue<TimedExecution> timers;

    std::unique_ptr<SubMode> subMode;
};
} // namespace game
