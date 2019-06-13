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

DupableFd open(const char* pathname, int flags, const internal::Sys* sys)
{
    return DupableFd(util::callCheckErrno("open", &internal::Sys::open, sys,
                                          pathname, flags),
                     sys);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
