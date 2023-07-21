#include <stdplus/str/buf.hpp>

#include <cstdio>
#include <format>
#include <system_error>

namespace stdplus
{

template <bool ln>
struct Printer
{
    template <typename... Args>
    static void print(std::FILE* stream, std::format_string<Args...> fmt,
                      Args&&... args)
    {
        stdplus::StrBuf buf;
        std::format_to(std::back_inserter(buf), fmt,
                       std::forward<Args>(args)...);
        if constexpr (ln)
        {
            buf.push_back('\n');
        }
        int r = std::fwrite(buf.data(), sizeof(char), buf.size(), stream);
        if (r < 0)
        {
            throw std::system_error(errno, std::generic_category());
        }
    }
};

template <typename... Args>
void print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
{
    Printer<false>::print(stream, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void print(std::format_string<Args...> fmt, Args&&... args)
{
    Printer<false>::print(stdout, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void println(std::FILE* stream, std::format_string<Args...> fmt,
                    Args&&... args)
{
    Printer<true>::print(stream, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void println(std::format_string<Args...> fmt, Args&&... args)
{
    Printer<true>::print(stdout, fmt, std::forward<Args>(args)...);
}

} // namespace stdplus
