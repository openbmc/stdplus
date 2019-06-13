#include <fcntl.h>
#include <stdplus/fd/sys.hpp>
#include <sys/socket.h>
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

int SysImpl::open(const char* pathname, int flags) const
{
    return ::open(pathname, flags);
}

int SysImpl::socket(int domain, int type, int protocol) const
{
    return ::socket(domain, type, protocol);
}

SysImpl sys_impl;

} // namespace fd
} // namespace stdplus
