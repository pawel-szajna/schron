#include "object.hpp"

namespace ui
{
Object::Object(int id, int x, int y) :
    id(id),
    x(x),
    y(y),
    surface(sdl::make_surface(0, 0))
{}
}
