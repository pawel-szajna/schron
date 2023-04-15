#pragma once

#include "util/format.hpp"

#include <stdexcept>
#include <string_view>
#include <utility>

namespace sdl
{
constexpr int Success{0};

/**
 * @class Wrapped
 * @brief Wraps a C-style SDL type.
 * @tparam T Name of wrapped type.
 *
 * This class template is intended to be used a base class for concrete wrappers
 * for SDL structures, as it does not itself provide any functionality.
 */
template<typename T>
class Wrapped
{
public:

    Wrapped()                                = default;
    Wrapped(const Wrapped<T>&)               = delete;
    Wrapped<T>& operator=(const Wrapped<T>&) = delete;

    Wrapped(Wrapped<T>&& other) noexcept { std::swap(wrapped, other.wrapped); }

    Wrapped<T>& operator=(Wrapped<T>&& other) noexcept
    {
        std::swap(wrapped, other.wrapped);
        return *this;
    }

    T* operator*() { return wrapped; }

    T* operator->() { return wrapped; }

    const T* operator*() const { return wrapped; }

    const T* operator->() const { return wrapped; }

protected:

    /**
     * @brief Assigns the wrapped element.
     *
     * Intended to be called in the derived class constructor.
     *
     * @param element Pointer to the created element.
     * @param name Human-readable name of the wrapped type.
     */
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
} // namespace sdl
