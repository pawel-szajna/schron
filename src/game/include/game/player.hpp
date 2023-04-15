#pragma once

#include "util/constants.hpp"

#include <cstdint>
#include <numbers>
#include <optional>
#include <queue>
#include <string>

namespace scripting
{
class Scripting;
}

namespace world
{
class Level;
}

namespace game
{
class Position
{
public:

    int sector{1};
    double x{0.5}, y{0.5}, z{0.6}, angle{std::numbers::pi / 2};
    double fovH = c::renderHeight * 0.65;
    double fovV = c::renderWidth * 0.22;
};

class Player
{
    struct MoveTarget
    {
        double x, y, z;
        int factor;
    };

    struct FovAnimation
    {
    private:

        template<typename T>
        struct Change
        {
            T old, target;
        };

    public:

        Change<double> fovH, fovV;
        Change<uint64_t> time;
    };

    struct Item
    {
        int id;
        std::string name;
        std::string description;
    };

public:

    enum class Direction
    {
        Forward,
        Backward,
        Left,
        Right,
    };
    enum class Rotation
    {
        Left,
        Right,
    };

    Player(scripting::Scripting& scripting, int& noiseLevel);
    ~Player();

    void move(const world::Level& level, Direction direction);
    void rotate(Rotation rotation);

    void frame(const world::Level& level, double frameTime);
    void animateFov(double fovH, double fovV, uint64_t length);
    [[nodiscard]] const Position& getPosition() const;

    [[nodiscard]] int getSanity() const;
    void setSanity(int sanity);
    void modSanity(int delta);

    int addItem(std::string name, std::string description);
    [[nodiscard]] int checkItem(const std::string& name) const;
    void removeItem(int id);

    void save(std::ostream& os) const;

    bool standing() const;

private:

    void acquireMove();
    double enterable(const world::Level& level, double x, double y) const;

    void place(int sector, double x, double y, double z, double a, double fovH, double fovV);

    Position position;
    Position target;

    std::queue<MoveTarget> moves{};
    std::optional<FovAnimation> fovAnimation{};

    std::vector<Item> inventory{};
    int itemCounter{0};

    scripting::Scripting& scripting;
    int& noiseLevel;

    int moving{0};
    int rotating{0};

    int sanity{100};
};
} // namespace game
