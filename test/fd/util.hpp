#include <sys/mman.h>

#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>

#include <string_view>

namespace stdplus::fd
{

inline ManagedFd makeMemfd(std::string_view contents)
{
    auto fd = ManagedFd(CHECK_ERRNO(memfd_create("test", 0), "memfd_create"));
    write(fd, contents);
    lseek(fd, 0, Whence::Set);
    return fd;
}

constexpr auto readSv(std::string_view s)
{
    return [s](std::span<std::byte> buf) {
        if (s.size())
        {
            memcpy(buf.data(), s.data(), s.size());
        }
        return buf.subspan(0, s.size());
    };
}

} // namespace stdplus::fd
