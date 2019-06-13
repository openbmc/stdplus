#include <fcntl.h>
#include <stdplus/fd/ops/fcntl.hpp>
#include <stdplus/fd/sys.hpp>
#include <stdplus/util/cexec.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{
namespace
{

int fcntl_getfl(const Fd& fd)
{
    return util::callCheckErrno("fcntl getfl", &Sys::fcntl_getfl, fd.getSys(),
                                fd.getValue());
}

void fcntl_setfl(const Fd& fd, int flags)
{
    util::callCheckErrno("fcntl setfl", &Sys::fcntl_setfl, fd.getSys(),
                         fd.getValue(), flags);
}

} // namespace

void setFdCloexec(const Fd& fd, bool value)
{
    util::callCheckErrno("fcntl_setfd", &Sys::fcntl_setfd, fd.getSys(),
                         fd.getValue(), value ? FD_CLOEXEC : 0);
}

void setFileNonblock(const Fd& fd, bool value)
{
    fcntl_setfl(fd, (fcntl_getfl(fd) & ~O_NONBLOCK) | (value ? O_NONBLOCK : 0));
}

} // namespace ops
} // namespace fd
} // namespace stdplus
