#pragma once
#include <fmt/format.h>
#include <functional>
#include <stdplus/fd/intf.hpp>

namespace stdplus
{
namespace fd
{

class FormatBuffer
{
  public:
    explicit FormatBuffer(stdplus::Fd& fd, size_t max = 4096);
    ~FormatBuffer() noexcept(false);
    FormatBuffer(const FormatBuffer&) = delete;
    FormatBuffer(FormatBuffer&&) = default;
    FormatBuffer& operator=(const FormatBuffer&) = delete;
    FormatBuffer& operator=(FormatBuffer&&) = default;

    template <typename... Args>
    void append(fmt::format_string<Args...> fmt, Args&&... args)
    {
        fmt::format_to(std::back_inserter(buf), fmt,
                       std::forward<Args>(args)...);
        writeIfNeeded();
    }

    void flush();

  private:
    std::reference_wrapper<stdplus::Fd> fd;
    fmt::memory_buffer buf;
    size_t max;

    void writeIfNeeded();
};

} // namespace fd
} // namespace stdplus
