#pragma once
#include <stdplus/fd/intf.hpp>
#include <stdplus/str/buf.hpp>

#include <format>
#include <functional>
#include <type_traits>
#include <utility>

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

    void flush();

  private:
    std::reference_wrapper<Fd> fd;
    stdplus::StrBuf buf;
    size_t max;

    void writeIfNeeded();
};

} // namespace fd
} // namespace stdplus
