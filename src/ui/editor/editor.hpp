#pragma once

#include "../object.hpp"

#include <deque>
#include <functional>

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
    template<typename T> using VertexGetter = std::function<std::optional<std::tuple<double, double, double, double>>(const T&)>;
    template<typename T> bool mouseWithin(const T& vertices, VertexGetter<T> vg) const;

    void enqueue(int length, double startValue, double targetValue, DiffApplier applier);
    void updateMouse();
    void processMapUpdates();
    void drawMap(sdl::Renderer& renderer);

    struct Diff
    {
        uint64_t startTime, targetTime;
        double startValue, targetValue;
        DiffApplier applier;
    };

    double mapX{-380}, mapY{-120}, mapScale{32};
    [[maybe_unused]] const double& playerX;
    [[maybe_unused]] const double& playerY;

    int mouseX, mouseY;
    int btnX, btnY;
    bool dragging{false};

    world::Level& level;
    std::deque<Diff> diffs;
    std::optional<int> sectorUnderMouse;
};
}
