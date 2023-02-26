#include "level.hpp"

#include <spdlog/spdlog.h>

namespace world
{
Level::Level(int width,
             int height,
             std::string name) :
     width(width),
     height(height),
     name(std::move(name)),
     map(width * height, Field{FieldType::Empty})
{
    spdlog::debug("Creating level {}", this->name);
}
}
