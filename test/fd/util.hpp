#include <sys/mman.h>

#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>

#include <algorithm>
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
        auto end = std::copy_n(reinterpret_cast<const std::byte*>(s.data()),
                               std::min(buf.size(), s.size()), buf.begin());
        return buf.subspan(0, end - buf.begin());
    };
}

} // namespace stdplus::fd
