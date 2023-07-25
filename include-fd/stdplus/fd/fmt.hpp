#pragma once
#include <fmt/core.h>

#include <stdplus/fd/intf.hpp>
#include <stdplus/str/buf.hpp>

#include <format>
#include <functional>
#include <type_traits>
#include <utility>

namespace fmt::detail
{
template <typename T>
struct is_compiled_string;
} // namespace fmt::detail

namespace stdplus
{
namespace fd
{

class FormatBuffer
{
  public:
    explicit FormatBuffer(Fd& fd, size_t max = 4096);
    ~FormatBuffer() noexcept(false);
    FormatBuffer(const FormatBuffer&) = delete;
    FormatBuffer(FormatBuffer&&) = default;
    FormatBuffer& operator=(const FormatBuffer&) = delete;
    FormatBuffer& operator=(FormatBuffer&&) = default;

    template <typename... Args>
    inline void append(std::format_string<Args...> fmt, Args&&... args)
    {
        std::format_to(std::back_inserter(buf), fmt,
                       std::forward<Args>(args)...);
        writeIfNeeded();
    }

    template <typename T, typename... Args,
              std::enable_if_t<fmt::detail::is_compiled_string<T>::value,
                               bool> = true>
    inline void append(const T& t, Args&&... args)
    {
        fmt::format_to(std::back_inserter(buf), t, std::forward<Args>(args)...);
        writeIfNeeded();
    }

    inline FormatBuffer& appendsv(std::string_view sv)
    {
        std::copy(sv.begin(), sv.end(), buf.append(sv.size()));
        writeIfNeeded();
        return *this;
    }

    inline void appends(const auto&... s)
    {
        (appendsv(s), ...);
    }

    void flush();

  private:
    std::reference_wrapper<Fd> fd;
    stdplus::StrBuf buf;
    size_t max;

    void writeIfNeeded();
};

} // namespace fd
} // namespace stdplus
