#pragma once

#include "sector.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace engine
{
class Engine;
}

namespace scripting
{
class WorldBindings;
}

namespace ui::editor
{
class Editor;
}

namespace world
{
class Level
{
    friend class engine::Engine;
    friend class ui::editor::Editor;
    friend class scripting::WorldBindings;

    struct Interaction { int sector; double x, y; std::string script; };
    using SectorsMap = std::unordered_map<int, Sector>;

public:

    Level(int id, std::string name);

    void put(Sector sector);
    [[nodiscard]] const Sector& sector(int id) const { return map.at(id); }
    [[nodiscard]] const SectorsMap& sectors() const { return map; }

    void interaction(int sector, double x, double y, const std::string& script);
    std::optional<std::string> checkScript(int sector, double x, double y) const;

    [[nodiscard]] std::string toLua() const;

private:

    std::string name{};
    std::unordered_set<std::string> additionalTextures{};
    SectorsMap map{};
    std::vector<Interaction> interactions{};

};
}
