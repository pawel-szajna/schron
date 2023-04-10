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

class Scripting
{
public:

    Scripting(ui::UI& ui, world::World& world, sdl::Renderer& renderer);
    ~Scripting();

    void run(const std::string& script);
    void runAsCoroutine(const std::string& script);
    void resume();

    void unbind(const std::string& name);

    void sectorEntry(int x, int y, int z);
    void sanityChange();

    void save(std::ostream& os) const;

    template<typename... Args>
    void bind(const std::string& name, Args&& ...args)
    {
        lua.set_function(name, std::forward<Args>(args)...);
    }

    template<typename Fn, typename... Args>
    void bindYielding(const std::string& name, Fn&& fn, Args&& ...args)
    {
        bind(name, sol::yielding(fn), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void unbind(const std::string& name, Args&& ...args)
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
}