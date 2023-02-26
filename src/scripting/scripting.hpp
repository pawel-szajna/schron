#pragma once

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
class Scripting
{
public:
    Scripting(ui::UI& ui, world::World& world);

private:
    ui::UI& ui;
    world::World& world;

};
}
