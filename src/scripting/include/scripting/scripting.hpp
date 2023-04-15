#pragma once

#include <memory>
#include <sol/sol.hpp>
#include <stack>

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

/**
 * @class Scripting
 * @brief Bridge between the game and Lua scripts.
 *
 * This class acts as the main unit connecting the C++ game code to the Lua scripting
 * framework. It provides an interface to the sol2 library it uses underneath.
 *
 * This class also automatically registers some C++-Lua bindings.
 */
class Scripting
{
public:

    Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer);
    ~Scripting();

    /**
     * @brief Starts a script.
     * @param script Script filename
     *
     * Launches a Lua script in the current context.
     */
    void run(const std::string& script);

    /**
     * @brief Starts a coroutine.
     * @param script Script filename
     *
     * Launches a Lua script as a coroutine. Scripts which pause execution,
     * waiting for results from C++, have to be launched as coroutines.
     */
    void runAsCoroutine(const std::string& script);

    /**
     * @brief Resumes the current coroutine.
     *
     * This function does nothing (apart from printing a warning message) if
     * there is no current coroutine.
     */
    void resume();

    /**
     * @brief Removes a value or function binding from the current Lua context.
     * @param name Binding name
     */
    void unbind(const std::string& name);

    void sectorEntry(int x, int y, int z);
    void sanityChange();

    void save(std::ostream& os) const;

    /**
     * @brief Binds a function.
     * @param name Function name, as seen in Lua environment
     * @param args The function to bind, optionally with argument values to bind
     */
    template<typename... Args>
    void bind(const std::string& name, Args&&... args)
    {
        lua.set_function(name, std::forward<Args>(args)...);
    }

    /**
     * @brief Binds a function which yields execution.
     * @param name Function name, as seen in Lua environment
     * @param fn The function to bind
     * @param args Optionally, argument values to bind
     *
     * This function is similar to Scripting::bind, however the Lua script execution
     * will be paused after the function is called and has to be manually resumed by
     * calling the Scripting::resume function.
     */
    template<typename Fn, typename... Args>
    void bindYielding(const std::string& name, Fn&& fn, Args&&... args)
    {
        bind(name, sol::yielding(fn), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void unbind(const std::string& name, Args&&... args)
    {
        unbind(name);
        unbind(std::forward<Args>(args)...);
    }

    template<typename T>
    void set(const std::string& name, const T& t)
    {
        lua[name] = t;
    }

private:

    void runFunctionIfExists(const std::string& function);

    sol::state lua;

    [[maybe_unused]] std::unique_ptr<UiBindings> uiBindings;
    [[maybe_unused]] std::unique_ptr<WorldBindings> worldBindings;

    std::stack<std::pair<sol::thread, sol::coroutine>> coroutines;

    [[maybe_unused]] ui::UI& ui;
    [[maybe_unused]] world::World& world;
};
} // namespace scripting
