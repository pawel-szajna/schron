#pragma once

#include "../object.hpp"

namespace world
{
class Level;
}

namespace ui::editor
{
class Editor : public Object
{
public:

    Editor(world::Level& level, const double& playerX, const double& playerY);
    virtual ~Editor();

    void render(sdl::Renderer& target) override;
    void event(const sdl::event::Event& event) override;

private:

    [[maybe_unused]] double mapX{-256}, mapY{-192};
    [[maybe_unused]] const double& playerX;
    [[maybe_unused]] const double& playerY;

    world::Level& level;
};
}
