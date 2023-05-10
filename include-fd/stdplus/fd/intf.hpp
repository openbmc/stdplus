#pragma once
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <stdplus/flags.hpp>

#include <cstddef>
#include <optional>
#include <span>
#include <tuple>

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

enum class ProtFlag : int
{
    Exec = PROT_EXEC,
    Read = PROT_READ,
    Write = PROT_WRITE,
};
using ProtFlags = BitFlags<int, ProtFlag>;

enum class MMapAccess : int
{
    Shared = MAP_SHARED,
    Private = MAP_PRIVATE,
};

enum class MMapFlag : int
{
};

class MMapFlags : public BitFlags<int, MMapFlag>
{
  public:
    inline MMapFlags(MMapAccess access) :
        BitFlags<int, MMapFlag>(static_cast<int>(access))
    {}

    inline MMapFlags(BitFlags<int, MMapFlag> flags) :
        BitFlags<int, MMapFlag>(flags)
    {}
};

class MMap;

class Fd
{
  public:
    virtual ~Fd() = default;

    virtual std::span<std::byte> read(std::span<std::byte> buf) = 0;
    virtual std::span<std::byte> recv(std::span<std::byte> buf,
                                      RecvFlags flags) = 0;
    virtual std::span<const std::byte>
        write(std::span<const std::byte> data) = 0;
    virtual std::span<const std::byte> send(std::span<const std::byte> data,
                                            SendFlags flags) = 0;
    virtual size_t lseek(off_t offset, Whence whence) = 0;
    virtual void truncate(off_t size) = 0;
    virtual void bind(std::span<const std::byte> sockaddr) = 0;
    virtual void listen(int backlog) = 0;
    virtual std::tuple<std::optional<int>, std::span<std::byte>>
        accept(std::span<std::byte> sockaddr) = 0;
    virtual void setsockopt(SockLevel level, SockOpt optname,
                            std::span<const std::byte> opt) = 0;
    virtual int ioctl(unsigned long id, void* data) = 0;
    virtual int constIoctl(unsigned long id, void* data) const = 0;
    virtual void fcntlSetfd(FdFlags flags) = 0;
    virtual FdFlags fcntlGetfd() const = 0;
    virtual void fcntlSetfl(FileFlags flags) = 0;
    virtual FileFlags fcntlGetfl() const = 0;

  protected:
    virtual std::span<std::byte> mmap(std::span<std::byte> window,
                                      ProtFlags prot, MMapFlags flags,
                                      off_t offset) = 0;
    virtual void munmap(std::span<std::byte> window) = 0;
    friend class MMap;
};

} // namespace fd

using fd::Fd;

} // namespace stdplus
