#include <stdplus/net/addr/sock.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(Sock4Addr, Basic)
{
    constexpr Sock4Addr addr1{.addr = In4Addr{255}, .port = 3959};
    Sock4Addr addr2{.addr = In4Addr{255}, .port = 2000};
    Sock4Addr addr3{.addr = In4Addr{0}, .port = 3959};

    EXPECT_EQ(addr1, addr1);
    EXPECT_NE(addr1, addr2);
    EXPECT_NE(addr3, addr1);

    auto addr = addr1.sockaddr();
    EXPECT_EQ(AF_INET, addr.sin_family);
    EXPECT_EQ(In4Addr{255}, addr.sin_addr);
    EXPECT_EQ(3959, stdplus::ntoh(addr.sin_port));
    EXPECT_EQ(sizeof(sockaddr_in), addr1.sockaddrLen());
}

TEST(Sock6Addr, Basic)
{
    constexpr Sock6Addr addr1{.addr = In6Addr{255}, .port = 3959, .scope = 0};
    Sock6Addr addr2{.addr = In6Addr{255}, .port = 2000, .scope = 0};
    Sock6Addr addr3{.addr = In6Addr{0}, .port = 3959, .scope = 0};
    Sock6Addr addr4{.addr = In6Addr{255}, .port = 3959, .scope = 1};

    EXPECT_EQ(addr1, addr1);
    EXPECT_NE(addr1, addr2);
    EXPECT_NE(addr3, addr1);
    EXPECT_NE(addr4, addr1);

    auto addr = addr1.sockaddr();
    EXPECT_EQ(AF_INET6, addr.sin6_family);
    EXPECT_EQ(In6Addr{255}, addr.sin6_addr);
    EXPECT_EQ(3959, stdplus::ntoh(addr.sin6_port));
    EXPECT_EQ(sizeof(sockaddr_in6), addr1.sockaddrLen());
}

TEST(SockAnyAddr, Basic)
{
    constexpr SockAnyAddr addr1(std::in_place_type<Sock4Addr>, In4Addr{255},
                                3959);
    EXPECT_EQ(addr1, addr1);
    auto buf = addr1.buf();
    EXPECT_EQ(buf.len, sizeof(sockaddr_in));
    EXPECT_EQ(addr1.sockaddrLen(), sizeof(sockaddr_in));
}

} // namespace stdplus
