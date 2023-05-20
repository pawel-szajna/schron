#include "object.hpp"

#include <algorithm>
#include <stdexcept>

namespace ui
{
bool Object::event(const sdl::event::Event& event)
{
    return std::any_of(children.begin(), children.end(), [&](const auto& child) { return child->event(event); });
}

void Object::render(sdl::Renderer& target)
{
    for (const auto& child : children)
    {
        child->render(target);
    }
}

void Object::attach(Widget child)
{
    if (child.get() == this)
    {
        throw std::runtime_error{"Cannot attach a widget to itself!"};
    }
    if (child->parent != nullptr)
    {
        child->detach();
    }
    child->parent = this;
    children.emplace_back(std::move(child));
}

void Object::detachAll()
{
    while (not children.empty())
    {
        children.front()->detach();
    }
}

void Object::detach()
{
    if (parent == nullptr)
    {
        return;
    }

    auto o = std::find_if(parent->children.begin(), parent->children.end(), [this](const auto& widget) { return widget.get() == this; });
    if (o != parent->children.end())
    {
        parent->children.erase(o);
    }

    parent = nullptr;
}
}
