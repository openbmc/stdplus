#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/ops/open.hpp>
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

} // namespace ops
} // namespace fd
} // namespace stdplus
