#pragma once

#include "../object.hpp"

#include <deque>

namespace world
{
class Level;
}

namespace ui::editor
{
class Editor : public Object
{
public:

    Editor(world::Level& level, const double& playerX, const double& playerY);
    virtual ~Editor();

    void render(sdl::Renderer& target) override;
    void event(const sdl::event::Event& event) override;

private:

    using DiffApplier = std::function<void(double)>;

    void enqueue(int length, double startValue, double targetValue, DiffApplier applier);

    struct Diff
    {
        uint64_t startTime, targetTime;
        double startValue, targetValue;
        DiffApplier applier;
    };

    double mapX, mapY, mapScale{32};
    [[maybe_unused]] const double& playerX;
    [[maybe_unused]] const double& playerY;

    int mouseX, mouseY;
    int btnX, btnY;
    bool dragging{false};

    world::Level& level;
    std::deque<Diff> diffs;
};
}
