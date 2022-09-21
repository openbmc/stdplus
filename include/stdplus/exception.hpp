#pragma once
#include <fmt/format.h>
#include <source_location>
#include <system_error>
#include <utility>

#if __has_builtin(__builtin_source_location)
#include <source_location>
#else
#include <experimental/source_location>
#endif

namespace stdplus
{
namespace exception
{
namespace detail
{
#if __has_builtin(__builtin_source_location)
using source_location = std::source_location;
#else
using source_location = std::experimental::source_location;
#endif
} // namespace detail

struct WouldBlock : public std::system_error
{
    WouldBlock(const char* what);
    WouldBlock(const std::string& what);
};

struct Eof : public std::system_error
{
    Eof(const char* what);
    Eof(const std::string& what);
};

template <typename F>
auto ignore(F&& f, const detail::source_location location =
                       detail::source_location::current())
{
    return ignore(std::forward<F>(f), location.file_name(), location.line(),
                  location.function_name());
}

template <typename F>
auto ignore(F&& f, const char* file, int line, const char* func) noexcept
{
    return
        [f = std::move(f), file, line, func](auto&&... args) mutable noexcept {
            try
            {
                return f(std::forward<decltype(args)>(args)...);
            }
            catch (const std::exception& e)
            {
                fmt::print(stderr, "Ignoring({}:{} {}): {}\n", file, line, func,
                           e.what());
            }
            catch (...)
            {
                fmt::print(stderr, "Ignoring({}:{} {}): Invalid Error\n", file,
                           line, func);
            }
            using Ret = std::invoke_result_t<decltype(f), decltype(args)...>;
            if constexpr (!std::is_same_v<void, Ret>)
            {
                return Ret();
            }
        };
}

template <typename F>
auto ignoreQuiet(F&& f) noexcept
{
    return [f = std::move(f)](auto&&... args) mutable noexcept {
        try
        {
            return f(std::forward<decltype(args)>(args)...);
        }
        catch (...)
        {
        }
        using Ret = std::invoke_result_t<decltype(f), decltype(args)...>;
        if constexpr (!std::is_same_v<void, Ret>)
        {
            return Ret();
        }
    };
}

} // namespace exception
} // namespace stdplus
