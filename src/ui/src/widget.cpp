#include "object.hpp"

#include <algorithm>
#include <stdexcept>

#include "sdlwrapper/event_types.hpp"
#include "sdlwrapper/renderer.hpp"

namespace alg
{
template<typename Container>
concept Iterable = requires(Container c) {
    {
        c.begin()
    } -> std::same_as<typename Container::iterator>;
    {
        c.end()
    } -> std::same_as<typename Container::iterator>;
};

template<typename Element, typename Predicate, typename ReturnType, typename... Args>
concept HasMethod = requires(Element e, Predicate p, Args&&... args) {
    {
        std::invoke(p, e, std::forward<Args>(args)...)
    } -> std::same_as<ReturnType>;
};

template<typename Container, typename Element = typename Container::value_type, typename Predicate, typename... Args>
requires Iterable<Container> and HasMethod<Element, Predicate, bool, Args...>
constexpr bool any_of(Container& container, Predicate predicate, Args&&... predicateArguments)
{
    return std::any_of(container.begin(),
                       container.end(),
                       [&](const Element& e)
                       { return std::invoke(predicate, e, std::forward<Args>(predicateArguments)...); });
}

template<typename Container, typename Element = typename Container::value_type, typename Method, typename... Args>
requires Iterable<Container> and HasMethod<Element, Method, std::invoke_result_t<Method, Element, Args...>, Args...>
constexpr void invoke_each(Container& container, Method method, Args&&... methodArguments)
{
    std::for_each(container.begin(),
                  container.end(),
                  [&](const Element& e) { return std::invoke(method, e, std::forward<Args>(methodArguments)...); });
}
} // namespace alg

namespace ui
{
bool Object::event(const sdl::event::Event& event)
{
    return alg::any_of(children, &Object::event, event);
}

void Object::render(sdl::Renderer& target)
{
    alg::invoke_each(children, &Object::render, target);
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

    auto o = std::find_if(parent->children.begin(),
                          parent->children.end(),
                          [this](const auto& widget) { return widget.get() == this; });
    if (o != parent->children.end())
    {
        parent->children.erase(o);
    }

    parent = nullptr;
}
} // namespace ui
