#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/ops/create.hpp>
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

DupableFd open(const char* pathname, int flags, const Sys* sys)
{
    return DupableFd(callCheckErrno("open", &Sys::open, sys, pathname, flags),
                     sys);
}

DupableFd socket(int domain, int type, int protocol, const Sys* sys)
{
    return DupableFd(
        callCheckErrno("socket", &Sys::socket, sys, domain, type, protocol),
        sys);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
