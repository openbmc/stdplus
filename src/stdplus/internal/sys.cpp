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

SysImpl sys_impl;

} // namespace internal
} // namespace stdplus
