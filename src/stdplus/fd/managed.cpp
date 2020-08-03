#include <fcntl.h>
#include <fmt/format.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>
#include <unistd.h>
#include <utility>

namespace stdplus
{
namespace detail
{

void drop(int&& fd)
{
    CHECK_ERRNO(close(fd), "close");
}

void setCloExec(int fd, bool val)
{
    auto flag = CHECK_ERRNO(fcntl(fd, F_GETFD), "fcntl GetFd");
    if (val)
    {
        val |= FD_CLOEXEC;
    }
    else
    {
        val &= ~FD_CLOEXEC;
    }
    CHECK_ERRNO(fcntl(fd, F_SETFD, flag),
                fmt::format("fcntl SetFd {:#x}", flag));
}

} // namespace detail

ManagedFd::ManagedFd(int&& fd) : handle(std::move(fd))
{
    detail::setCloExec(fd, true);
}

ManagedFd::ManagedFd(DupableFd&& other) noexcept :
    handle(static_cast<detail::ManagedFdHandle&&>(other.handle))
{
}

ManagedFd::ManagedFd(const DupableFd& other) : ManagedFd(DupableFd(other))
{
}

ManagedFd& ManagedFd::operator=(DupableFd&& other) noexcept
{
    handle = static_cast<detail::ManagedFdHandle&&>(other.handle);
    return *this;
}

ManagedFd& ManagedFd::operator=(const DupableFd& other)
{
    return *this = DupableFd(other);
}

int ManagedFd::release()
{
    return handle.release();
}

int ManagedFd::get()
{
    return handle.value();
}

} // namespace stdplus
