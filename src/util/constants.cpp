#include "constants.hpp"

#include <sol/sol.hpp>
#include <spdlog/spdlog.h>

namespace
{
template<typename T>
void assign(sol::state& lua, const std::string& name, T& value)
{
    if (lua[name].valid())
    {
        value = lua[name];
        spdlog::debug("Read value from config: {}={}", name, value);
    }
}
}

namespace c
{
bool renderStats = false;
bool frameLimit = true;
double shadowResolution = 16;
int shadowDepth = 4;

void loadConfig()
{
    spdlog::debug("Trying to load configuration file");
    try
    {
        sol::state lua;
        lua.open_libraries(sol::lib::base);
        lua.script_file("config.lua");
        assign(lua, "renderStats", renderStats);
        assign(lua, "frameLimit", frameLimit);
        assign(lua, "shadowResolution", shadowResolution);
        assign(lua, "shadowDepth", shadowDepth);
    }
    catch(std::exception& e)
    {
        spdlog::warn("Could not load config: {}. Falling back to defaults", e.what());
    }
}
}
