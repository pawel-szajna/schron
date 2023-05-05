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
        SPDLOG_DEBUG("Read value from config: {}={}", name, value);
    }
}
} // namespace

namespace c
{
bool renderStats        = false;
bool frameLimit         = true;
double shadowResolution = 16;
int shadowDepth         = 4;

void loadConfig()
{
    SPDLOG_DEBUG("Trying to load configuration file");
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
    catch (std::exception& e)
    {
        SPDLOG_WARN("Could not load config: {}. Falling back to defaults", e.what());
    }
}
} // namespace c
