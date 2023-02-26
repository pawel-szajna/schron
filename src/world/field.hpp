#pragma once

namespace world
{
enum class FieldType
{
    Empty,
    Wall,
};

class Field
{
public:
    Field(FieldType type);

    [[nodiscard]] FieldType getType() const;

private:
    FieldType type;

};
}
