#pragma once

#include "ui/object.hpp"

#include "sdlwrapper/renderer.hpp"
#include "sdlwrapper/surface.hpp"
#include "sdlwrapper/texture.hpp"

namespace game
{
class Player;
}

namespace world
{
class Level;
}

namespace ui
{
class MiniMap : public Object
{
public:

    MiniMap(sdl::Renderer& renderer, const world::Level& level, const game::Player& player);
    virtual ~MiniMap();

    void render(sdl::Renderer& target) override;
    bool event(const sdl::event::Event& event) override;

private:

    const world::Level& level;
    const game::Player& player;

    sdl::Surface surface;
    sdl::Renderer surfaceRenderer;
    sdl::Texture texture;
};

using MiniMapWidget = std::shared_ptr<MiniMap>;
} // namespace ui
