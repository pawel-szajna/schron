#pragma once

#include <memory>
#include <sol/sol.hpp>

namespace ui
{
class UI;
}

namespace world
{
class World;
}

namespace scripting
{
class UiBindings;

class Scripting
{
public:

    Scripting(ui::UI& ui, world::World& world);
    ~Scripting();

    void run(const std::string& script);

private:

    sol::state lua;

    std::unique_ptr<UiBindings> uiBindings;

    ui::UI& ui;
    world::World& world;

};
}
