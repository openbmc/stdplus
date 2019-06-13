#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/ops/create.hpp>
#include <stdplus/fd/sys.hpp>
#include <stdplus/util/cexec.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

DupableFd open(const char* pathname, int flags, const Sys* sys)
{
    return DupableFd(
        util::callCheckErrno("open", &Sys::open, sys, pathname, flags), sys);
}

DupableFd open(const std::string& pathname, int flags, const Sys* sys)
{
    return open(pathname.c_str(), flags, sys);
}

DupableFd socket(int domain, int type, int protocol, const Sys* sys)
{
    return DupableFd(util::callCheckErrno("socket", &Sys::socket, sys, domain,
                                          type, protocol),
                     sys);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
