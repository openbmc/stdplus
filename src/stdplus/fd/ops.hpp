#pragma once
#include <stdplus/fd/intf.hpp>
#include <stdplus/raw.hpp>
#include <stdplus/types.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

void readAll(Fd& fd, span<std::byte> data);
void recvAll(Fd& fd, span<std::byte> data, int flags);
void writeAll(Fd& fd, span<const std::byte> data);
void sendAll(Fd& fd, span<const std::byte> data, int flags);

span<std::byte> readAligned(Fd& fd, size_t align, span<std::byte> buf);
span<std::byte> recvAligned(Fd& fd, size_t align, span<std::byte> buf,
                            int flags);
span<const std::byte> writeAligned(Fd& fd, size_t align,
                                   span<const std::byte> data);
span<const std::byte> sendAligned(Fd& fd, size_t align,
                                  span<const std::byte> data, int flags);

template <typename Fun, typename Container, typename... Args>
auto alignedOp(Fun&& fun, Fd& fd, Container&& c, Args&&... args)
{
    auto ret = fun(fd, sizeof(*std::data(c)), raw::asSpan<std::byte>(c),
                   std::forward<Args>(args)...);
    return span<decltype(*std::data(c))>(c, ret.size() / sizeof(*std::data(c)));
}

} // namespace detail

template <typename Container>
inline auto read(Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::readAligned, fd,
                             std::forward<Container>(c));
}

template <typename Container>
inline auto recv(Fd& fd, Container&& c, int flags)
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
inline auto send(Fd& fd, Container&& c, int flags)
{
    return detail::alignedOp(detail::sendAligned, fd,
                             std::forward<Container>(c), flags);
}

template <typename T>
inline void readAll(Fd& fd, T&& t)
{
    detail::readAll(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline void recvAll(Fd& fd, T&& t, int flags)
{
    detail::recvAll(fd, raw::asSpan<std::byte>(t), flags);
}

template <typename T>
inline void writeAll(Fd& fd, T&& t)
{
    detail::writeAll(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline void sendAll(Fd& fd, T&& t, int flags)
{
    detail::sendAll(fd, raw::asSpan<std::byte>(t), flags);
}

inline size_t lseek(Fd& fd, off_t offset, int whence)
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

template <typename Opt>
inline void setsockopt(Fd& fd, int level, int optname, Opt&& opt)
{
    return fd.setsockopt(level, optname, raw::asSpan<std::byte>(opt));
}

inline int ioctl(Fd& fd, unsigned long id, void* data)
{
    return fd.ioctl(id, data);
}

} // namespace fd
} // namespace stdplus
