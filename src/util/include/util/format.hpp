#pragma once

#ifdef SPDLOG_USE_STD_FORMAT
    #include <format>
#else
    #include <spdlog/fmt/bundled/format.h>

namespace std
{
template<typename... T>
std::string format(fmt::format_string<T...> fmt, T&&... args)
{
    return ::fmt::format(fmt, std::forward<T>(args)...);
}
} // namespace std
#endif
