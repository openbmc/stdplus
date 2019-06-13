#include <fcntl.h>
#include <stdplus/internal/sys.hpp>
#include <sys/socket.h>
#include <unistd.h>

namespace stdplus
{
namespace internal
{

int SysImpl::dup(int oldfd) const
{
    return ::dup(oldfd);
}

int SysImpl::close(int fd) const
{
    return ::close(fd);
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

} // namespace internal
} // namespace stdplus
