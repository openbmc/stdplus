#include <unistd.h>

#include <stdplus/fd/create.hpp>
#include <stdplus/fd/impl.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/net/addr/sock.hpp>

#include <array>
#include <cstddef>
#include <format>
#include <system_error>

#include <gtest/gtest.h>

namespace stdplus::fd
{

TEST(FdImpl, RecvfromAndAcceptClampAddrLen)
{
    auto sender =
        socket(SocketDomain::UNIX, SocketType::Datagram, SocketProto::IP);
    auto receiver =
        socket(SocketDomain::UNIX, SocketType::Datagram, SocketProto::IP);
    SockUAddr sender_addr(std::format("@stdplus-fd-impl-{}-s", getpid()));
    SockUAddr receiver_addr(std::format("@stdplus-fd-impl-{}-r", getpid()));
    bind(sender, sender_addr);
    bind(receiver, receiver_addr);

    auto rbuf = receiver_addr.buf();
    std::array<std::byte, 4> payload{};
    sender.sendto(payload, SendFlags(0),
                  {reinterpret_cast<const std::byte*>(&rbuf), rbuf.len});

    std::array<std::byte, 4> recv_buf{};
    std::array<std::byte, 4> small_addr{};
    EXPECT_THROW(receiver.recvfrom(recv_buf, RecvFlags(0), small_addr),
                 std::system_error);

    sender.sendto(payload, SendFlags(0),
                  {reinterpret_cast<const std::byte*>(&rbuf), rbuf.len});
    auto [data, addr] =
        receiver.recvfrom(recv_buf, RecvFlags(0), std::span<std::byte>{});
    EXPECT_EQ(data.size(), payload.size());
    EXPECT_TRUE(addr.empty());

    sender.sendto(payload, SendFlags(0),
                  {reinterpret_cast<const std::byte*>(&rbuf), rbuf.len});
    std::array<std::byte, sizeof(sockaddr_un)> full_addr{};
    auto [data2, addr2] = receiver.recvfrom(recv_buf, RecvFlags(0), full_addr);
    EXPECT_EQ(data2.size(), payload.size());
    EXPECT_EQ(addr2.size(), sender_addr.buf().len);

    auto server =
        socket(SocketDomain::UNIX, SocketType::Stream, SocketProto::IP);
    auto client =
        socket(SocketDomain::UNIX, SocketType::Stream, SocketProto::IP);
    SockUAddr srv_addr(std::format("@stdplus-fd-impl-{}-srv", getpid()));
    SockUAddr cli_addr(std::format("@stdplus-fd-impl-{}-cli", getpid()));
    bind(server, srv_addr);
    listen(server, 1);
    bind(client, cli_addr);
    connect(client, srv_addr);

    auto acc = server.accept(small_addr);
    ASSERT_TRUE(acc.has_value());
    DupableFd accepted(std::move(std::get<0>(*acc)));
    EXPECT_EQ(std::get<1>(*acc).size(), small_addr.size());
}

} // namespace stdplus::fd
