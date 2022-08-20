#pragma once
#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <stdplus/fd/intf.hpp>
#include <stdplus/fd/managed.hpp>
#include <string_view>
#include <type_traits>

namespace fmt
{
namespace detail
{
template <typename T>
struct is_compiled_string;
}
} // namespace fmt

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
    inline void append(fmt::format_string<Args...> fmt, Args&&... args)
    {
        fmt::format_to(std::back_inserter(buf), fmt,
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

    void flush();

  private:
    std::reference_wrapper<Fd> fd;
    fmt::memory_buffer buf;
    size_t max;

    void writeIfNeeded();
};

class FormatToFile
{
  public:
    explicit FormatToFile(std::string_view tmpl = "/tmp/stdplus.XXXXXX");
    ~FormatToFile();
    FormatToFile(const FormatToFile&) = delete;
    FormatToFile(FormatToFile&&) = delete;
    FormatToFile& operator=(const FormatToFile&) = delete;
    FormatToFile& operator=(FormatToFile&&) = delete;

    template <typename... Args>
    inline void append(fmt::format_string<Args...> fmt, Args&&... args)
    {
        buf.append(fmt, std::forward<Args>(args)...);
    }
    template <typename T, typename... Args,
              std::enable_if_t<fmt::detail::is_compiled_string<T>::value,
                               bool> = true>
    inline void append(const T& t, Args&&... args)
    {
        buf.append(t, std::forward<Args>(args)...);
    }

    void commit(const std::filesystem::path& out, int mode = 0644);

    inline const std::string& getTmpname() const
    {
        return tmpname;
    }

  private:
    std::string tmpname;
    ManagedFd fd;
    FormatBuffer buf;
};

} // namespace fd
} // namespace stdplus
