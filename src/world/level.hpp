#pragma once

#include "sector.hpp"

#include <string>
#include <unordered_map>

namespace ui::editor
{
class Editor;
}

namespace world
{
class Level
{
    friend class ui::editor::Editor;
    using SectorsMap = std::unordered_map<int, Sector>;

public:

    Level(int id, std::string name);

    void put(Sector&& sector);
    [[nodiscard]] const Sector& sector(int id) const { return map.at(id); }
    [[nodiscard]] const SectorsMap& sectors() const { return map; }

    [[nodiscard]] std::string toLua() const;

private:

    std::string name{};
    SectorsMap map{};

};
}
