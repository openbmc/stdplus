#pragma once
#include <fmt/format.h>
#include <system_error>
#include <utility>

// Forward declare builtins in case they are unsupported
int __builtin_LINE();
const char* __builtin_FILE();
const char* __builtin_FUNCTION();

namespace stdplus
{
namespace exception
{

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
auto ignore(F&& f, const char* file = __builtin_FILE(),
            int line = __builtin_LINE(),
            const char* func = __builtin_FUNCTION())
{
    return [f = std::move(f), file, line, func](auto&&... args) mutable {
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

} // namespace exception
} // namespace stdplus
