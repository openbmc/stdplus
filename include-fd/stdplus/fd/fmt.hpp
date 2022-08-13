#pragma once
#include <filesystem>
#include <fmt/format.h>
#include <functional>
#include <stdplus/fd/intf.hpp>
#include <stdplus/fd/managed.hpp>

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

class FormatToFile
{
  public:
    explicit FormatToFile(const char* tmpl = "/tmp/stdplus.XXXXXX");
    ~FormatToFile();
    FormatToFile(const FormatToFile&) = delete;
    FormatToFile(FormatToFile&&) = delete;
    FormatToFile& operator=(const FormatToFile&) = delete;
    FormatToFile& operator=(FormatToFile&&) = delete;

    template <typename... Args>
    void append(fmt::format_string<Args...> fmt, Args&&... args)
    {
        buf.append(fmt, std::forward<Args>(args)...);
    }

    void commit(const std::filesystem::path& out, int mode = 0644);

    inline const std::string& getTmpname() const
    {
        return tmpname;
    }

  private:
    std::string tmpname;
    stdplus::ManagedFd fd;
    FormatBuffer buf;
};

} // namespace fd
} // namespace stdplus
