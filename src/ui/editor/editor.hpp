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

    void render(sdl::Surface &target, SDL_Rect coords) override;

private:

    double mapX{-256}, mapY{-192};
    const double& playerX;
    const double& playerY;

    world::Level& level;
    sdl::Surface surface;
};
}
}
