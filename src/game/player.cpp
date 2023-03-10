#include "player.hpp"

#include "world/level.hpp"

#include <cmath>
#include <numbers>

namespace game
{
namespace
{
constexpr auto rotationSpeed{2.4};
constexpr auto movementSpeed{1.8};

bool overlap(double x1, double x2, double y1, double y2)
{
    return std::min(x1, x2) <= std::max(y1, y2) and
           std::min(y1, y2) <= std::max(x1, x2);
}

bool intersect(double box1x1, double box1y1, double box1x2, double box1y2,
               double box2x1, double box2y1, double box2x2, double box2y2)
{
    return overlap(box1x1, box1x2, box2x1, box2x2) and
           overlap(box1y1, box1y2, box2y1, box2y2);
}

double side(double px, double py, double x1, double y1, double x2, double y2)
{
    return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
}
}

Player::Player() = default;

Player::~Player() = default;

const Position& Player::getPosition() const
{
    return position;
}

void Player::move(Direction direction)
{
    if (moving or rotating)
    {
        return;
    }

    target = position;

    switch (direction)
    {
    case Direction::Forward:
        target.x += std::cos(position.angle);
        target.y += std::sin(position.angle);
        moving = 1;
        break;
    case Direction::Backward:
        target.x -= std::cos(position.angle);
        target.y -= std::sin(position.angle);
        moving = -1;
        break;
    case Direction::Left:
    case Direction::Right:
        break;
    }
}

void Player::rotate(Rotation rotation)
{
    if (moving or rotating)
    {
        return;
    }

    target = position;

    switch(rotation)
    {
    case Rotation::Left:
        target.angle -= std::numbers::pi / 2;
        rotating = -1;
        break;
    case Rotation::Right:
        target.angle += std::numbers::pi / 2;
        rotating = +1;
        break;
    }
}

void Player::frame(const world::Level& level, double frameTime)
{
    const auto& sector = level.sector(position.sector);
    auto targetZ = sector.floor + 0.6;
    if (position.z != targetZ)
    {
        position.z = std::clamp(targetZ, position.z - frameTime, position.z + frameTime * 2);
    }

    if (moving or strafing)
    {
        auto deltaX = frameTime * moving * movementSpeed * std::cos(position.angle) + frameTime * strafing * movementSpeed * std::sin(position.angle);
        auto deltaY = frameTime * moving * movementSpeed * std::sin(position.angle) - frameTime * strafing * movementSpeed * std::cos(position.angle);

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
                            target.x += portal.transform->x;
                            target.y += portal.transform->y;
                            spdlog::debug("Portal - new: {} {} {}", position.x, position.y, position.z);
                            position.angle += portal.transform->angle;
                        }
                        break;
                    }
                }
            }
        }

        position.x += deltaX;
        position.y += deltaY;

        if ((target.x - position.x) * (moving * std::cos(position.angle)) <= 1e-4 and
            (target.y - position.y) * (moving * std::sin(position.angle)) <= 1e-4)
        {
            position.x = target.x;
            position.y = target.y;
            moving = 0;
        }
    }

    if (rotating != 0)
    {
        position.angle += rotating * frameTime * rotationSpeed;

        if ((target.angle - position.angle) * rotating < 0)
        {
            position.angle = target.angle;
            rotating = 0;
        }
    }
}
}
