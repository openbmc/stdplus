#include <stdplus/str/buf.hpp>

#include <format>

namespace stdplus
{

void prints(std::FILE* stream, std::string_view data);

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
        prints(stream, buf);
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
