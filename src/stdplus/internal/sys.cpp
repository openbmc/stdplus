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

SysImpl sys_impl;

} // namespace internal
} // namespace stdplus
