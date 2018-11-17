#include <fcntl.h>
#include <stdplus/fd/sys.hpp>
#include <unistd.h>

namespace stdplus
{
namespace fd
{

int SysImpl::close(int fd) const
{
    return ::close(fd);
}

int SysImpl::fcntl_dupfd_cloexec(int fd) const
{
    return ::fcntl(fd, F_DUPFD_CLOEXEC, 0);
}

int SysImpl::fcntl_setfd(int fd, int flags) const
{
    return ::fcntl(fd, F_SETFD, flags);
}

SysImpl sys_impl;

} // namespace fd
} // namespace stdplus
