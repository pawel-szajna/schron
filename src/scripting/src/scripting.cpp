#include "scripting.hpp"

#include "ui/ui.hpp"
#include "ui_bindings.hpp"
#include "world/world.hpp"
#include "world_bindings.hpp"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace
{
void debug(const std::string& msg)
{
    SPDLOG_DEBUG(msg);
}

void info(const std::string& msg)
{
    SPDLOG_INFO(msg);
}

void warning(const std::string& msg)
{
    SPDLOG_WARN(msg);
}
} // namespace

namespace scripting
{
Scripting::Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer)
    : ui(ui)
    , world(world)
{
    lua.open_libraries(sol::lib::base, sol::lib::table);

    bind("debug", debug);
    bind("info", info);
    bind("warn", warning);

    uiBindings    = std::make_unique<UiBindings>(lua, ui, renderer);
    worldBindings = std::make_unique<WorldBindings>(lua, world);
}

Scripting::~Scripting() = default;

void Scripting::run(const std::string& script)
try
{
    SPDLOG_INFO("Running script: {}", script);
    lua.script_file(script);
}
catch (std::runtime_error& exception)
{
    SPDLOG_ERROR("LUA error: {}", exception.what());
}

void Scripting::runAsCoroutine(const std::string& script)
try
{
    SPDLOG_INFO("Running interactive script: {}", script);
    sol::thread thread       = sol::thread::create(lua);
    sol::coroutine coroutine = thread.state().load_file(script);
    coroutines.emplace(std::move(thread), std::move(coroutine));
    resume();
}
catch (std::runtime_error& exception)
{
    SPDLOG_ERROR("LUA error: {}", exception.what());
}

void Scripting::resume()
{
    if (coroutines.empty())
    {
        SPDLOG_WARN("Tried to resume not existing execution");
        return;
    }

    SPDLOG_DEBUG("Resume called on current script");

    auto result = std::get<sol::coroutine>(coroutines.top())();
    if (result.valid())
    {
        if (result.status() != sol::call_status::yielded)
        {
            SPDLOG_DEBUG("Popping coroutine due to execution result: {}", std::to_underlying(result.status()));
            coroutines.pop();
        }
    }
    else
    {
        sol::error err = result;
        SPDLOG_WARN("LUA error: {}", err.what());
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
                throw sol::error(result.get<std::string>());
            }
        }
        catch (std::exception& e)
        {
            SPDLOG_WARN("Failed script execution: {}", e.what());
        }
    }
}

void Scripting::save(std::ostream& os) const
{
    for (auto [name, value] : lua)
    {
        switch (value.get_type())
        {
        case sol::type::boolean:
            os << std::format("{}={}\n", name.as<std::string>(), value.as<bool>());
            break;
        case sol::type::number:
            os << std::format("{}={}\n", name.as<std::string>(), value.as<double>());
            break;
        case sol::type::string:
            os << std::format("{}=\"{}\"\n", name.as<std::string>(), value.as<std::string>());
            break;
        default:
            break;
        }
    }
}

void Scripting::unbind(const std::string& name)
{
    lua[name] = sol::lua_nil;
}

} // namespace scripting
