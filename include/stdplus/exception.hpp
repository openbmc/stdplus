#pragma once
#include <stdplus/print.hpp>

#include <source_location>
#include <system_error>
#include <utility>

namespace stdplus
{
namespace exception
{

struct Incomplete : public std::system_error
{
    Incomplete(const char* what);
    Incomplete(const std::string& what);
};

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
                stdplus::print(stderr, "Ignoring({}:{} {}): {}\n", file, line,
                               func, e.what());
            }
            catch (...)
            {
                stdplus::print(stderr, "Ignoring({}:{} {}): Invalid Error\n",
                               file, line, func);
            }
            using Ret = std::invoke_result_t<decltype(f), decltype(args)...>;
            if constexpr (!std::is_same_v<void, Ret>)
            {
                return Ret();
            }
        };
}

template <typename F>
auto ignore(F&& f, const std::source_location location =
                       std::source_location::current()) noexcept
{
    return ignore(std::forward<F>(f), location.file_name(), location.line(),
                  location.function_name());
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
        {}
        using Ret = std::invoke_result_t<decltype(f), decltype(args)...>;
        if constexpr (!std::is_same_v<void, Ret>)
        {
            return Ret();
        }
    };
}

} // namespace exception
} // namespace stdplus
