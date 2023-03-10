#include "scripting.hpp"

#include "ui/ui.hpp"
#include "ui_bindings.hpp"
#include "world_bindings.hpp"
#include "world/world.hpp"

#include <spdlog/spdlog.h>

namespace scripting
{
Scripting::Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer) :
    ui(ui),
    world(world)
{
    lua.open_libraries(sol::lib::base);

    uiBindings = std::make_unique<UiBindings>(lua, ui, renderer);
    worldBindings = std::make_unique<WorldBindings>(lua, world);
}

Scripting::~Scripting() = default;

void Scripting::run(const std::string& script)
try
{
    spdlog::info("Running script: {}", script);
    lua.script_file(script);
}
catch (std::runtime_error& exception)
{
    spdlog::error("LUA error: {}", exception.what());
}

void Scripting::sectorEntry(int x, int y)
{
    auto sectorFunction = std::format("sector_{}_{}", x, y);
    if (lua[sectorFunction].valid())
    {
        spdlog::debug("Calling LUA function {} due to sector entry", sectorFunction);
        lua[sectorFunction]();
    }
}
}
