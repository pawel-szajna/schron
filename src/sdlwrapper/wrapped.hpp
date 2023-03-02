#pragma once

#include "util/format.hpp"

#include <stdexcept>
#include <string_view>
#include <utility>

namespace sdl
{
constexpr int Success{0};

template<typename T>
class Wrapped
{
public:

    Wrapped() = default;
    Wrapped(const Wrapped<T>&) = delete;
    Wrapped<T>& operator=(const Wrapped<T>&) = delete;

    Wrapped(Wrapped<T>&& other) noexcept { std::swap(wrapped, other.wrapped); }
    Wrapped<T>& operator=(Wrapped<T>&& other) noexcept { std::swap(wrapped, other.wrapped); return *this; }

    T* operator*() { return wrapped; }
    T* operator->() { return wrapped; }
    const T* operator*() const { return wrapped; }
    const T* operator->() const { return wrapped; }

protected:

    void assign(T* element, std::string_view name = "wrapped element")
    {
        if (element == nullptr)
        {
            throw std::runtime_error{std::format("could not construct {}", name)};
        }
        wrapped = element;
    }

    T* wrapped{nullptr};
};
}
