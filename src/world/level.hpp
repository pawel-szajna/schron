#pragma once

#include "sector.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ui::editor
{
class Editor;
}

namespace world
{
class Level
{
    friend class ui::editor::Editor;
    struct Interaction { int sector; double x, y; std::string script; };
    using SectorsMap = std::unordered_map<int, Sector>;

public:

    Level(int id, std::string name);

    void put(Sector&& sector);
    [[nodiscard]] const Sector& sector(int id) const { return map.at(id); }
    [[nodiscard]] const SectorsMap& sectors() const { return map; }

    void interaction(int sector, double x, double y, const std::string& script);
    std::optional<std::string> checkScript(int sector, double x, double y) const;

    [[nodiscard]] std::string toLua() const;

private:

    std::string name{};
    SectorsMap map{};
    std::vector<Interaction> interactions{};

};
}
