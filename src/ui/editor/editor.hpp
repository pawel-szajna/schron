#pragma once

#include "../object.hpp"

namespace world
{
class Level;
}

namespace ui
{
namespace editor
{
class Editor : public Object
{
public:

    Editor(world::Level& level, const double& playerX, const double& playerY);
    virtual ~Editor();

    void render(sdl::Renderer& target) override;

private:

    [[maybe_unused]] double mapX{-256}, mapY{-192};
    [[maybe_unused]] const double& playerX;
    [[maybe_unused]] const double& playerY;

    world::Level& level;
};
}
}
