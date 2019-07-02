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

void setFdCloexec(const Fd& fd, bool value)
{
    util::callCheckErrno("fcntl_setfd", &Sys::fcntl_setfd, fd.getSys(),
                         fd.getValue(), value ? FD_CLOEXEC : 0);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
