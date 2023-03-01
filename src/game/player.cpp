#include "player.hpp"

#include "world/level.hpp"

#include <cmath>

namespace game
{
Player::Player()
{}

Player::~Player() = default;

const Position& Player::getPosition() const
{
    return position;
}

void Player::handleMovement(double walk, double strafe, double rotate, const world::Level& level)
{
    auto overlap = [](double x1, double x2, double y1, double y2)
    {
        return std::min(x1, x2) <= std::max(y1, y2) and
               std::min(y1, y2) <= std::max(x1, x2);
    };
    auto intersect = [&](double box1x1, double box1y1, double box1x2, double box1y2,
                         double box2x1, double box2y1, double box2x2, double box2y2)
    {
        return overlap(box1x1, box1x2, box2x1, box2x2) and overlap(box1y1, box1y2, box2y1, box2y2);
    };
    auto side = [](double px, double py, double x1, double y1, double x2, double y2)
    {
        return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
    };

    const auto& sector = level.sector(position.sector);
    auto targetZ = sector.floor + 0.6;
    if (position.z != targetZ)
    {
        position.z = std::clamp(targetZ, position.z - 0.05, position.z + 0.1);
    }

    if (walk != 0 or strafe != 0)
    {
        auto deltaX = walk * std::cos(position.angle) + strafe * std::sin(position.angle);
        auto deltaY = walk * std::sin(position.angle) - strafe * std::cos(position.angle);

        for (const auto& wall : sector.walls)
        {
            if (intersect(position.x, position.y, position.x + deltaX, position.y + deltaY,
                          wall.xStart, wall.yStart, wall.xEnd, wall.yEnd))
            {
                if (side(position.x + deltaX, position.y + deltaY, wall.xStart, wall.yStart, wall.xEnd, wall.yEnd) < 0)
                {
                    if (wall.portal.has_value())
                    {
                        const auto& portal = *wall.portal;
                        spdlog::debug("Changed sector to {}", portal.sector);
                        position.sector = portal.sector;
                        if (portal.transform.has_value())
                        {
                            spdlog::debug("Portal - old: {} {} {}", position.x, position.y, position.z);
                            position.x += portal.transform->x;
                            position.y += portal.transform->y;
                            position.z += portal.transform->z;
                            spdlog::debug("Portal - new: {} {} {}", position.x, position.y, position.z);
                            position.angle += portal.transform->angle;
                        }
                        break;
                    }
                    else
                    {
                        deltaX = 0;
                        deltaY = 0;
                    }
                }
            }
        }

        position.x += deltaX;
        position.y += deltaY;
    }

    if (rotate != 0)
    {
        position.angle += rotate;
    }
}
}
