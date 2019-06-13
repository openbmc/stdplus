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

SysImpl sys_impl;

} // namespace internal
} // namespace stdplus
