#include <fcntl.h>
#include <stdplus/internal/sys.hpp>
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

int SysImpl::read(int fd, void* buf, size_t count) const
{
    return ::read(fd, buf, count);
}

int SysImpl::fcntl_setfl(int fd, int flags) const
{
    return ::fcntl(fd, F_SETFL, flags);
}

int SysImpl::fcntl_getfl(int fd) const
{
    return ::fcntl(fd, F_GETFL);
}

SysImpl sys_impl;

} // namespace internal
} // namespace stdplus
