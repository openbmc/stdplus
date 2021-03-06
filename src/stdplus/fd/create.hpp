#pragma once
#include <fcntl.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/flags.hpp>
#include <string>

namespace stdplus
{
namespace fd
{

enum class OpenAccess : int
{
    ReadOnly = O_RDONLY,
    WriteOnly = O_WRONLY,
    ReadWrite = O_RDWR,
};

enum class OpenFlag : int
{
    Append = O_APPEND,
    Async = O_ASYNC,
    CloseOnExec = O_CLOEXEC,
    Create = O_CREAT,
    Direct = O_DIRECT,
    Directory = O_DIRECTORY,
    Dsync = O_DSYNC,
    EnsureCreate = O_EXCL,
    LargeFile = O_LARGEFILE,
    NoAtime = O_NOATIME,
    NoCtty = O_NOCTTY,
    NoFollow = O_NOFOLLOW,
    NonBlock = O_NONBLOCK,
    Path = O_PATH,
    Sync = O_SYNC,
    TmpFile = O_TMPFILE,
    Trunc = O_TRUNC,
};

class OpenFlags : public BitFlags<int, OpenFlag>
{
  public:
    inline OpenFlags(OpenAccess access) :
        BitFlags<int, OpenFlag>(static_cast<int>(access))
    {
    }

    inline OpenFlags(BitFlags<int, OpenFlag> flags) :
        BitFlags<int, OpenFlag>(flags)
    {
    }
};

DupableFd open(const char* pathname, OpenFlags flags, mode_t mode = 0);
inline DupableFd open(const std::string& pathname, OpenFlags flags,
                      mode_t mode = 0)
{
    return open(pathname.c_str(), flags, mode);
}

enum class SocketDomain : int
{
};

enum class SocketType : int
{
};

enum class SocketProto : int
{
};

DupableFd socket(SocketDomain domain, SocketType type, SocketProto protocol);

} // namespace fd
} // namespace stdplus
