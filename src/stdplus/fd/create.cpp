#include <fcntl.h>
#include <fmt/format.h>
#include <stdplus/fd/create.hpp>
#include <stdplus/util/cexec.hpp>
#include <sys/socket.h>

namespace stdplus
{
namespace fd
{

DupableFd open(const char* pathname, OpenFlags flags, mode_t mode)
{
    return DupableFd(
        CHECK_ERRNO(::open(pathname, static_cast<int>(flags), mode),
                    fmt::format("open `{}`", pathname)));
}

DupableFd socket(int domain, int type, int protocol)
{
    return DupableFd(CHECK_ERRNO(::socket(domain, type, protocol), "socket"));
}

} // namespace fd
} // namespace stdplus
