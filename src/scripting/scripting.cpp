#include "scripting.hpp"

#include "ui/ui.hpp"
#include "world/world.hpp"

namespace scripting
{
Scripting::Scripting(ui::UI& ui, world::World& world) :
    ui(ui),
    world(world)
{}
}
