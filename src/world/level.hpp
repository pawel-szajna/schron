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

public:

    Level(int id, std::string name);

    void put(Sector&& sector);
    const Sector& sector(int id) const { return map.at(id); }
    std::string toLua() const;

private:

    std::string name{};
    std::unordered_map<int, Sector> map{};

};
}
