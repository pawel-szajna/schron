#include "field.hpp"

namespace world
{
Field::Field(FieldType type) :
    type(type)
{}

FieldType Field::getType() const
{
    return type;
}
}
