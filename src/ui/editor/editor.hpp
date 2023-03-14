#pragma once

#include "../object.hpp"

#include <deque>
#include <functional>

namespace sdl
{
class Font;
}

namespace world
{
class Level;
}

namespace ui::editor
{
class Editor : public Object
{
public:

    Editor(world::Level& level, sdl::Font& font);
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
    void drawSelectedSector(sdl::Renderer& renderer);
    void processMapClicks();

    struct Diff
    {
        uint64_t startTime, targetTime;
        double startValue, targetValue;
        DiffApplier applier;
    };

    double mapX{-380}, mapY{-120}, mapScale{32};

    int mouseX, mouseY;
    int btnX, btnY;
    int dragX, dragY;
    bool dragging{false};
    bool clicked{false};

    world::Level& level;
    std::deque<Diff> diffs;
    std::optional<int> sectorUnderMouse;
    std::optional<int> selectedSector;

    sdl::Font& font;
};
}
