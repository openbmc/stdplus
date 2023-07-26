#include <fcntl.h>
#include <sys/socket.h>

#include <stdplus/fd/create.hpp>
#include <stdplus/util/cexec.hpp>

#include <format>

namespace stdplus
{
namespace fd
{

DupableFd open(const_zstring pathname, OpenFlags flags, mode_t mode)
{
    return DupableFd(
        CHECK_ERRNO(::open(pathname.c_str(), static_cast<int>(flags), mode),
                    std::format("open `{}`", pathname.c_str())));
}

DupableFd socket(SocketDomain domain, SocketType type, SocketProto protocol)
{
    return DupableFd(
        CHECK_ERRNO(::socket(static_cast<int>(domain), static_cast<int>(type),
                             static_cast<int>(protocol)),
                    "socket"));
}

} // namespace fd
} // namespace stdplus
