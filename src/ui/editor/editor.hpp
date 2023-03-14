#pragma once

#include "../object.hpp"

#include "sdlwrapper/cursor.hpp"

#include <deque>
#include <functional>
#include <optional>
#include <set>
#include <vector>

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
    void resizeSector(int id, double left, double right, double top, double bottom);
    void resizeSingleSector(int id, double left, double right, double top, double bottom, bool recurse);

    struct Diff
    {
        uint64_t startTime, targetTime;
        double startValue, targetValue;
        DiffApplier applier;
    };

    double mapX{-380}, mapY{-120}, mapScale{32};

    enum class Direction
    {
        Top,
        Bottom,
        Left,
        Right,
    };

    int mouseX, mouseY;
    int btnX, btnY;
    int dragX, dragY;
    bool dragging{false};
    bool clicked{false};
    bool dragged{false};
    std::optional<Direction> draggedWall;

    world::Level& level;
    std::deque<Diff> diffs;
    std::optional<int> sectorUnderMouse;
    std::optional<int> selectedSector;
    std::set<int> resized;

    sdl::Font& font;
    sdl::Cursor horizontal{7};
    sdl::Cursor vertical{8};
};
}
