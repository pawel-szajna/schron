#pragma once

#include <memory>
#include <sol/sol.hpp>

namespace ui
{
class UI;
}

namespace sdl
{
class Renderer;
}

namespace world
{
class World;
}

namespace scripting
{
class UiBindings;
class WorldBindings;

class Scripting
{
public:

    Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer);
    ~Scripting();

    void run(const std::string& script);
    void runAsCoroutine(const std::string& script);

    void sectorEntry(int x, int y, int z);

private:

    sol::state lua;

    [[maybe_unused]] std::unique_ptr<UiBindings> uiBindings;
    [[maybe_unused]] std::unique_ptr<WorldBindings> worldBindings;

    [[maybe_unused]] ui::UI& ui;
    [[maybe_unused]] world::World& world;

};
}
