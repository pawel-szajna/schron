#include "scripting.hpp"

#include "ui/ui.hpp"
#include "ui_bindings.hpp"
#include "world_bindings.hpp"
#include "world/world.hpp"

#include <spdlog/spdlog.h>

namespace
{
void debug(const std::string& msg) { spdlog::debug(msg); }
void info(const std::string& msg) { spdlog::info(msg); }
void warning(const std::string& msg) { spdlog::warn(msg); }
}

namespace scripting
{
Scripting::Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer) :
    ui(ui),
    world(world)
{
    lua.open_libraries(sol::lib::base, sol::lib::table);

    bind("debug", debug);
    bind("info", info);
    bind("warn", warning);

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

void Scripting::runAsCoroutine(const std::string& script)
try
{
    spdlog::info("Running interactive script: {}", script);
    sol::thread thread = sol::thread::create(lua);
    sol::coroutine coroutine = thread.state().load_file(script);
    coroutines.emplace(std::move(thread), std::move(coroutine));
    resume();
}
catch (std::runtime_error& exception)
{
    spdlog::error("LUA error: {}", exception.what());
}

void Scripting::resume()
{
    if (coroutines.empty())
    {
        spdlog::warn("Tried to resume not existing execution");
        return;
    }

    spdlog::debug("Resume called on current script");

    auto result = std::get<sol::coroutine>(coroutines.top())();
    if (result.valid())
    {
        if (result.status() != sol::call_status::yielded)
        {
            spdlog::debug("Popping coroutine due to execution result: {}", std::to_underlying(result.status()));
            coroutines.pop();
        }
    }
    else
    {
        sol::error err = result;
        spdlog::warn("LUA error: {}", err.what());
        coroutines.pop();
    }
}

void Scripting::sectorEntry(int x, int y, int z)
{
    runFunctionIfExists(std::format("enter_{}_{}_{}", x, y, z));
}

void Scripting::sanityChange()
{
    runFunctionIfExists("sanity_change");
}

void Scripting::runFunctionIfExists(const std::string& function)
{
    if (lua[function].valid())
    {
        try
        {
            auto result = lua[function]();
            if (not result.valid())
            {
                throw sol::error(result);
            }
        }
        catch (std::exception& e)
        {
            spdlog::warn("Failed script execution: {}", e.what());
        }
    }
}
}
