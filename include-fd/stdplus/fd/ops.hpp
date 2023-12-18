#pragma once
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/intf.hpp>
#include <stdplus/raw.hpp>

#include <span>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

void readExact(Fd& fd, std::span<std::byte> data);
void recvExact(Fd& fd, std::span<std::byte> data, RecvFlags flags);
void writeExact(Fd& fd, std::span<const std::byte> data);
void sendExact(Fd& fd, std::span<const std::byte> data, SendFlags flags);

std::span<std::byte> readAligned(Fd& fd, size_t align,
                                 std::span<std::byte> buf);
std::span<std::byte> recvAligned(Fd& fd, size_t align, std::span<std::byte> buf,
                                 RecvFlags flags);
std::span<const std::byte> writeAligned(Fd& fd, size_t align,
                                        std::span<const std::byte> data);
std::span<const std::byte> sendAligned(Fd& fd, size_t align,
                                       std::span<const std::byte> data,
                                       SendFlags flags);

template <typename Fun, typename Container, typename... Args>
auto alignedOp(Fun&& fun, Fd& fd, Container&& c, Args&&... args)
{
    using Data = raw::detail::dataType<Container>;
    auto ret = fun(fd, sizeof(Data), raw::asSpan<std::byte>(c),
                   std::forward<Args>(args)...);
    return std::span<Data>(std::begin(c), ret.size() / sizeof(Data));
}

} // namespace detail

template <typename Container>
inline auto read(Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::readAligned, fd,
                             std::forward<Container>(c));
}

template <typename Container>
inline auto recv(Fd& fd, Container&& c, RecvFlags flags)
{
    return detail::alignedOp(detail::recvAligned, fd,
                             std::forward<Container>(c), flags);
}

template <typename Container>
inline auto write(Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::writeAligned, fd,
                             std::forward<Container>(c));
}

template <typename Container>
inline auto send(Fd& fd, Container&& c, SendFlags flags)
{
    return detail::alignedOp(detail::sendAligned, fd,
                             std::forward<Container>(c), flags);
}

template <typename T>
inline void readExact(Fd& fd, T&& t)
{
    detail::readExact(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline void recvExact(Fd& fd, T&& t, RecvFlags flags)
{
    detail::recvExact(fd, raw::asSpan<std::byte>(t), flags);
}

template <typename T>
inline void writeExact(Fd& fd, T&& t)
{
    detail::writeExact(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline void sendExact(Fd& fd, T&& t, SendFlags flags)
{
    detail::sendExact(fd, raw::asSpan<std::byte>(t), flags);
}

inline size_t lseek(Fd& fd, off_t offset, Whence whence)
{
    return fd.lseek(offset, whence);
}

inline void truncate(Fd& fd, off_t size)
{
    return fd.truncate(size);
}

template <typename SockAddr>
inline void bind(Fd& fd, SockAddr&& sockaddr)
{
    return fd.bind(raw::asSpan<std::byte>(sockaddr));
}

template <typename SockAddr>
inline void connect(Fd& fd, SockAddr&& sockaddr)
{
    return fd.connect(raw::asSpan<std::byte>(sockaddr));
}

inline void listen(Fd& fd, int backlog)
{
    return fd.listen(backlog);
}

inline std::optional<DupableFd> accept(Fd& fd)
{
    auto ret = fd.accept(std::span<std::byte>{});
    if (!ret)
    {
        return std::nullopt;
    }
    return DupableFd(std::move(std::get<0>(*ret)));
}

template <typename Opt>
inline void setsockopt(Fd& fd, SockLevel level, SockOpt optname, Opt&& opt)
{
    return fd.setsockopt(level, optname, raw::asSpan<std::byte>(opt));
}

template <typename Data>
inline int constIoctl(const Fd& fd, unsigned long id, Data&& data)
{
    return fd.constIoctl(id, raw::asSpan<std::byte>(data).data());
}

template <typename Data>
inline int ioctl(Fd& fd, unsigned long id, Data&& data)
{
    return fd.ioctl(id, raw::asSpan<std::byte>(data).data());
}

inline FdFlags getFdFlags(const Fd& fd)
{
    return fd.fcntlGetfd();
}

inline void setFdFlags(Fd& fd, FdFlags flags)
{
    return fd.fcntlSetfd(flags);
}

inline FileFlags getFileFlags(const Fd& fd)
{
    return fd.fcntlGetfl();
}

inline void setFileFlags(Fd& fd, FileFlags flags)
{
    return fd.fcntlSetfl(flags);
}

} // namespace fd
} // namespace stdplus
