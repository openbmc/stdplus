#pragma once
#include <stdplus/fd/intf.hpp>
#include <stdplus/raw.hpp>
#include <stdplus/types.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{

void readAll(Fd& fd, stdplus::span<std::byte> data);
void recvAll(Fd& fd, stdplus::span<std::byte> data, int flags);
void writeAll(Fd& fd, stdplus::span<const std::byte> data);
void sendAll(Fd& fd, stdplus::span<const std::byte> data, int flags);

stdplus::span<std::byte> readAligned(Fd& fd, size_t align,
                                     stdplus::span<std::byte> buf);
stdplus::span<std::byte> recvAligned(Fd& fd, size_t align,
                                     stdplus::span<std::byte> buf, int flags);
stdplus::span<const std::byte>
    writeAligned(Fd& fd, size_t align, stdplus::span<const std::byte> data);
stdplus::span<const std::byte> sendAligned(Fd& fd, size_t align,
                                           stdplus::span<const std::byte> data,
                                           int flags);

namespace detail
{
template <typename Fun, typename Container, typename... Args>
auto containerOp(Fun&& fun, Fd& fd, Container&& c, Args&&... args)
{
    auto ret =
        fun(fd, sizeof(*std::data(c)), stdplus::raw::asSpan<std::byte>(c),
            std::forward<Args>(args)...);
    return stdplus::span<decltype(*std::data(c))>(c, ret.size() /
                                                         sizeof(*std::data(c)));
}
} // namespace detail

template <typename... Args>
auto read(Args&&... args)
{
    return containerOp(readAligned, std::forward<Args>(args)...);
}

template <typename... Args>
auto recv(Args&&... args)
{
    return containerOp(recvAligned, std::forward<Args>(args)...);
}

template <typename... Args>
auto write(Args&&... args)
{
    return containerOp(writeAligned, std::forward<Args>(args)...);
}

template <typename... Args>
auto send(Args&&... args)
{
    return containerOp(sendAligned, std::forward<Args>(args)...);
}

} // namespace fd
} // namespace stdplus
