#include <stdplus/fd/ops/fcntl.hpp>
#include <stdplus/internal/sys.hpp>
#include <stdplus/util/cexec.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

using internal::Sys;
using util::callCheckErrno;

int fcntl_getfl(const Fd& fd)
{
    return callCheckErrno("fcntl getfl", &Sys::fcntl_getfl, fd.getSys(),
                          fd.getVal());
}

void fcntl_setfl(const Fd& fd, int flags)
{
    callCheckErrno("fcntl setfl", &Sys::fcntl_setfl, fd.getSys(), fd.getVal(),
                   flags);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
