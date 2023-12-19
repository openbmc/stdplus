#pragma once
#include <fcntl.h>
#include <netinet/ip.h>

#include <stdplus/fd/dupable.hpp>
#include <stdplus/flags.hpp>
#include <stdplus/zstring.hpp>

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

class OpenFlags : public BitFlags<OpenFlag>
{
  public:
    inline OpenFlags(OpenAccess access) :
        BitFlags<OpenFlag>(std::to_underlying(access))
    {}

    inline OpenFlags(BitFlags<OpenFlag> flags) : BitFlags<OpenFlag>(flags) {}
};

DupableFd open(const_zstring pathname, OpenFlags flags, mode_t mode = 0);

enum class SocketDomain : int
{
    INet = AF_INET,
    INet6 = AF_INET6,
    Netlink = AF_NETLINK,
    Packet = AF_PACKET,
    UNIX = AF_UNIX,
};

enum class SocketType : int
{
    Datagram = SOCK_DGRAM,
    Raw = SOCK_RAW,
    Stream = SOCK_STREAM,
};

enum class SocketProto : int
{
    ICMP = IPPROTO_ICMP,
    IP = IPPROTO_IP,
    Raw = IPPROTO_RAW,
    TCP = IPPROTO_TCP,
    UDP = IPPROTO_UDP,
};

DupableFd socket(SocketDomain domain, SocketType type, SocketProto protocol);

} // namespace fd
} // namespace stdplus
