#pragma once
#include <cstddef>
#include <fcntl.h>
#include <stdplus/flags.hpp>
#include <stdplus/types.hpp>
#include <sys/socket.h>

namespace stdplus
{
namespace fd
{

enum class RecvFlag : int
{
    DontWait = MSG_DONTWAIT,
    ErrQueue = MSG_ERRQUEUE,
    OutOfBounds = MSG_OOB,
    Peek = MSG_PEEK,
    Trunc = MSG_TRUNC,
    WaitAll = MSG_WAITALL,
};
using RecvFlags = BitFlags<int, RecvFlag>;

enum class SendFlag : int
{
    Confirm = MSG_CONFIRM,
    DontRoute = MSG_DONTROUTE,
    DontWait = MSG_DONTWAIT,
    EndOfRecord = MSG_EOR,
    More = MSG_MORE,
    NoSignal = MSG_NOSIGNAL,
    OutOfBounds = MSG_OOB,
};
using SendFlags = BitFlags<int, SendFlag>;

enum class Whence : int
{
    Set = SEEK_SET,
    Cur = SEEK_CUR,
    End = SEEK_END,
};

enum class SockLevel : int
{
    Socket = SOL_SOCKET,
};

enum class SockOpt : int
{
    Debug = SO_DEBUG,
    Broadcast = SO_BROADCAST,
    ReuseAddr = SO_REUSEADDR,
    KeepAlive = SO_KEEPALIVE,
    Linger = SO_LINGER,
    OOBInline = SO_OOBINLINE,
    SendBuf = SO_SNDBUF,
    RecvBuf = SO_RCVBUF,
    DontRoute = SO_DONTROUTE,
    RecvLowWait = SO_RCVLOWAT,
    RecvTimeout = SO_RCVTIMEO,
    SendLowWait = SO_SNDLOWAT,
    SendTimeout = SO_SNDTIMEO,
};

enum class FdFlag : int
{
    CloseOnExec = FD_CLOEXEC,
};
using FdFlags = BitFlags<int, FdFlag>;

enum class FileFlag : int
{
    Append = O_APPEND,
    Async = O_ASYNC,
    Direct = O_DIRECT,
    NoAtime = O_NOATIME,
    NonBlock = O_NONBLOCK,
};
using FileFlags = BitFlags<int, FileFlag>;

class Fd
{
  public:
    virtual ~Fd() = default;

    virtual span<std::byte> read(span<std::byte> buf) = 0;
    virtual span<std::byte> recv(span<std::byte> buf, RecvFlags flags) = 0;
    virtual span<const std::byte> write(span<const std::byte> data) = 0;
    virtual span<const std::byte> send(span<const std::byte> data,
                                       SendFlags flags) = 0;
    virtual size_t lseek(off_t offset, Whence whence) = 0;
    virtual void truncate(off_t size) = 0;
    virtual void bind(span<const std::byte> sockaddr) = 0;
    virtual void setsockopt(SockLevel level, SockOpt optname,
                            span<const std::byte> opt) = 0;
    virtual int ioctl(unsigned long id, void* data) = 0;
    virtual int constIoctl(unsigned long id, void* data) const = 0;
    virtual void fcntlSetfd(FdFlags flags) = 0;
    virtual FdFlags fcntlGetfd() const = 0;
    virtual void fcntlSetfl(FileFlags flags) = 0;
    virtual FileFlags fcntlGetfl() const = 0;
};

} // namespace fd

using fd::Fd;

} // namespace stdplus
