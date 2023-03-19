#include "level.hpp"

#include <spdlog/spdlog.h>

namespace world
{
Level::Level(int id,
             std::string name) :
     name(std::move(name))
{
    spdlog::info("Loading level #{}", id);
}

void Level::put(Sector&& sector)
{
    if (map.contains(sector.id))
    {
        spdlog::warn("Duplicate sector {}", sector.id);
    }
    map.emplace(sector.id, std::move(sector));
}

std::string Level::toLua() const
{
    std::string output{};
    for (const auto& [_, sector] : map)
    {
        output += sector.toLua();
    }
    return output;
}

void Level::interaction(int sector, double x, double y, const std::string& script)
{
    interactions.erase(std::remove_if(interactions.begin(), interactions.end(),
                                      [sector, x, y] (const auto& i)
                                      {
                                          return i.sector == sector and std::abs(i.x - x) < 0.1 and std::abs(i.y - y) < 0.1;
                                      }),
                       interactions.end());
    interactions.emplace_back(Interaction{sector, x, y, script});
}

std::optional<std::string> Level::checkScript(int sector, double x, double y) const
{
    auto maybeScript = std::find_if(interactions.begin(), interactions.end(),
                                    [sector, x, y] (const auto& i)
                                    {
                                        return i.sector == sector and std::abs(i.x - x) < 0.1 and std::abs(i.y - y) < 0.1;
                                    });

    if (maybeScript == interactions.end())
    {
        return std::nullopt;
    }

    return maybeScript->script;
}
}
