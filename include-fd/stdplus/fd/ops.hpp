#pragma once
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/intf.hpp>
#include <stdplus/function_view.hpp>
#include <stdplus/net/addr/sock.hpp>
#include <stdplus/raw.hpp>

#include <span>
#include <utility>
#include <vector>

namespace stdplus
{
namespace fd
{
namespace detail
{

void readExact(const Fd& fd, std::span<std::byte> data);
void recvExact(const Fd& fd, std::span<std::byte> data, RecvFlags flags);
void writeExact(const Fd& fd, std::span<const std::byte> data);
void sendExact(const Fd& fd, std::span<const std::byte> data, SendFlags flags);
void sendtoExact(const Fd& fd, std::span<const std::byte> data, SendFlags flags,
                 std::span<const std::byte> addr);

std::span<std::byte> readAligned(const Fd& fd, size_t align,
                                 std::span<std::byte> buf);
void readAll(const Fd& fd,
             function_view<std::span<std::byte>(size_t req)> resize);
std::span<std::byte> readAllFixed(const Fd& fd, size_t align,
                                  std::span<std::byte> buf);
std::span<std::byte> recvAligned(const Fd& fd, size_t align,
                                 std::span<std::byte> buf, RecvFlags flags);
std::span<const std::byte> writeAligned(const Fd& fd, size_t align,
                                        std::span<const std::byte> data);
std::span<const std::byte> sendAligned(const Fd& fd, size_t align,
                                       std::span<const std::byte> data,
                                       SendFlags flags);

void verifyExact(size_t expected, size_t actual);

template <typename Fun, typename Container, typename... Args>
auto alignedOp(Fun&& fun, const Fd& fd, Container&& c, Args&&... args)
{
    using Data = raw::detail::dataType<Container>;
    auto ret = fun(fd, sizeof(Data), raw::asSpan<std::byte>(c),
                   std::forward<Args>(args)...);
    return std::span<Data>(std::begin(c), ret.size() / sizeof(Data));
}

} // namespace detail

template <typename Container>
inline auto read(const Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::readAligned, fd,
                             std::forward<Container>(c));
}

template <typename Container = std::vector<std::byte>>
Container readAll(const Fd& fd)
{
    using Data = raw::detail::dataType<Container>;
    Container ret;
    auto resize = [&](size_t req) {
        ret.resize((req + sizeof(Data) - 1) / sizeof(Data));
        return std::span(reinterpret_cast<std::byte*>(ret.data()),
                         ret.size() * sizeof(Data));
    };
    detail::readAll(fd, resize);
    return ret;
}

template <typename Container>
inline auto readAllFixed(const Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::readAllFixed, fd,
                             std::forward<Container>(c));
}

template <typename Container>
inline auto recv(const Fd& fd, Container&& c, RecvFlags flags = {})
{
    return detail::alignedOp(detail::recvAligned, fd,
                             std::forward<Container>(c), flags);
}

template <typename Container>
auto recvfrom(const Fd& fd, Container&& c, SockAddrBuf& addr,
              RecvFlags flags = {})
{
    using Data = raw::detail::dataType<Container>;
    auto ret = fd.recvfrom(
        raw::asSpan<std::byte>(c), flags,
        std::span(reinterpret_cast<std::byte*>(&addr), addr.maxLen));
    addr.len = std::get<1>(ret).size();
    return std::span<Data>(std::begin(c),
                           std::get<0>(ret).size() / sizeof(Data));
}

template <typename Container>
inline auto write(const Fd& fd, Container&& c)
{
    return detail::alignedOp(detail::writeAligned, fd,
                             std::forward<Container>(c));
}

template <typename Container>
inline auto send(const Fd& fd, Container&& c, SendFlags flags = {})
{
    return detail::alignedOp(detail::sendAligned, fd,
                             std::forward<Container>(c), flags);
}

template <typename Container>
auto sendto(const Fd& fd, Container&& c, const SockAddrBuf& addr,
            SendFlags flags = {})
{
    using Data = raw::detail::dataType<Container>;
    auto ret = fd.sendto(
        raw::asSpan<std::byte>(c), flags,
        std::span(reinterpret_cast<const std::byte*>(&addr), addr.len));
    return std::span<Data>(std::begin(c), ret.size() / sizeof(Data));
}

template <typename T>
inline void readExact(const Fd& fd, T&& t)
{
    detail::readExact(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline auto readAllExact(const Fd& fd, T&& t)
{
    auto s = raw::asSpan<std::byte>(t);
    detail::verifyExact(s.size(), detail::readAllFixed(fd, 1, s).size());
}

template <typename T>
inline void recvExact(const Fd& fd, T&& t, RecvFlags flags = {})
{
    detail::recvExact(fd, raw::asSpan<std::byte>(t), flags);
}

template <typename T>
inline void writeExact(const Fd& fd, T&& t)
{
    detail::writeExact(fd, raw::asSpan<std::byte>(t));
}

template <typename T>
inline void sendExact(const Fd& fd, T&& t, SendFlags flags = {})
{
    detail::sendExact(fd, raw::asSpan<std::byte>(t), flags);
}

template <typename Container>
inline void sendtoExact(const Fd& fd, Container&& c, const SockAddrBuf& addr,
                        SendFlags flags = {})
{
    detail::sendtoExact(
        fd, raw::asSpan<std::byte>(c), flags,
        std::span(reinterpret_cast<const std::byte*>(&addr), addr.len));
}

inline size_t lseek(const Fd& fd, off_t offset, Whence whence)
{
    return fd.lseek(offset, whence);
}

inline void truncate(const Fd& fd, off_t size)
{
    return fd.truncate(size);
}

inline void bind(const Fd& fd, const SockAddrBuf& addr)
{
    return fd.bind({reinterpret_cast<const std::byte*>(&addr), addr.len});
}

inline void bind(const Fd& fd, const SockAddr auto& addr)
{
    return bind(fd, addr.buf());
}

inline void bind(const Fd& fd, const auto& sockaddr)
{
    return fd.bind(raw::asSpan<std::byte>(sockaddr));
}

inline void connect(const Fd& fd, const SockAddrBuf& addr)
{
    return fd.connect({reinterpret_cast<const std::byte*>(&addr), addr.len});
}

inline void connect(const Fd& fd, const SockAddr auto& addr)
{
    return connect(fd, addr.buf());
}

inline void connect(const Fd& fd, const auto& sockaddr)
{
    return fd.connect(raw::asSpan<std::byte>(sockaddr));
}

inline void listen(const Fd& fd, int backlog)
{
    return fd.listen(backlog);
}

inline std::optional<DupableFd> accept(const Fd& fd)
{
    auto ret = fd.accept(std::span<std::byte>{});
    if (!ret)
    {
        return std::nullopt;
    }
    return DupableFd(std::move(std::get<0>(*ret)));
}

inline std::optional<DupableFd> accept(const Fd& fd, SockAddrBuf& addr)
{
    auto ret =
        fd.accept(std::span(reinterpret_cast<std::byte*>(&addr), addr.maxLen));
    if (!ret)
    {
        return std::nullopt;
    }
    addr.len = std::get<1>(*ret).size();
    return DupableFd(std::move(std::get<0>(*ret)));
}

template <typename Opt>
inline void setsockopt(const Fd& fd, SockLevel level, SockOpt optname,
                       Opt&& opt)
{
    return fd.setsockopt(level, optname, raw::asSpan<std::byte>(opt));
}

template <typename Data>
inline int constIoctl(const Fd& fd, unsigned long id, Data&& data)
{
    return fd.constIoctl(id, raw::asSpan<std::byte>(data).data());
}

template <typename Data>
inline int ioctl(const Fd& fd, unsigned long id, Data&& data)
{
    return fd.ioctl(id, raw::asSpan<std::byte>(data).data());
}

inline FdFlags getFdFlags(const Fd& fd)
{
    return fd.fcntlGetfd();
}

inline void setFdFlags(const Fd& fd, FdFlags flags)
{
    return fd.fcntlSetfd(flags);
}

inline FileFlags getFileFlags(const Fd& fd)
{
    return fd.fcntlGetfl();
}

inline void setFileFlags(const Fd& fd, FileFlags flags)
{
    return fd.fcntlSetfl(flags);
}

} // namespace fd
} // namespace stdplus
