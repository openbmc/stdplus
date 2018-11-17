#include <fcntl.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/sys.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

void drop(int&& fd, const Sys*& sys)
{
    util::callCheckErrno("close", &Sys::close, sys, fd);
}

} // namespace detail

ManagedFd::ManagedFd(int&& fd, const Sys* sys) : handle(std::move(fd), sys)
{
    util::callCheckErrno("fcntl_setfd", &Sys::fcntl_setfd, sys, *handle,
                         FD_CLOEXEC);
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

} // namespace fd
} // namespace stdplus
