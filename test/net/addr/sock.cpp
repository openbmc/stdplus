#include <stdplus/net/addr/sock.hpp>

#include <string_view>

#include <gtest/gtest.h>

using std::literals::string_view_literals::operator""sv;

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

TEST(SockUAddr, Basic)
{
    // Non-abstract Requires null-terminator
    EXPECT_THROW(SockUAddr(std::string(SockUAddr::maxLen, 'a')),
                 std::invalid_argument);
    EXPECT_NO_THROW(SockUAddr(std::string(SockUAddr::maxLen - 1, 'a')));

    // Abstract can use the full space
    EXPECT_THROW(SockUAddr(std::string(SockUAddr::maxLen + 1, '\0')),
                 std::invalid_argument);
    EXPECT_NO_THROW(SockUAddr(std::string(SockUAddr::maxLen, '\0')));

    EXPECT_THROW(SockUAddr("hi\0o"sv), std::invalid_argument);

    constexpr SockUAddr addr1{"@hi"sv};
    constexpr SockUAddr addr2{"\0hi"sv};
    constexpr SockUAddr addr3{"/nope"sv};
    constexpr SockUAddr addr4{""sv};

    EXPECT_EQ(addr1, addr1);
    EXPECT_EQ(addr1, addr2);
    EXPECT_NE(addr3, addr1);
    EXPECT_NE(addr4, addr1);
    EXPECT_NE(addr4, addr3);

    EXPECT_EQ(addr1.path(), "@hi"sv);
    EXPECT_EQ(addr2.path(), "@hi"sv);
    EXPECT_EQ(addr3.path(), "/nope"sv);
    EXPECT_EQ(addr4.path(), ""sv);

    auto addr = addr1.sockaddr();
    EXPECT_EQ(AF_UNIX, addr.sun_family);
    EXPECT_EQ("\0hi"sv, std::string_view(addr.sun_path, 3));
    auto buf = addr2.buf();
    auto ptr = reinterpret_cast<std::uint8_t*>(&buf);
    EXPECT_TRUE(
        std::equal(ptr, ptr + buf.len, reinterpret_cast<std::uint8_t*>(&addr)));
    addr = addr3;
    EXPECT_EQ("/nope\0"sv, std::string_view(addr.sun_path, 6));
    EXPECT_EQ(addr1.sockaddrLen(), sizeof(addr.sun_family) + 3);
    EXPECT_EQ(addr2.sockaddrLen(), sizeof(addr.sun_family) + 3);
    EXPECT_EQ(addr3.sockaddrLen(), sizeof(addr.sun_family) + 6);
    EXPECT_EQ(addr4.sockaddrLen(), sizeof(addr.sun_family));
}

TEST(SockAnyAddr, Basic)
{
    constexpr SockAnyAddr addr1(std::in_place_type<Sock4Addr>, In4Addr{255},
                                3959);
    constexpr SockAnyAddr addr2(std::in_place_type<SockUAddr>, "/hi"sv);
    EXPECT_NE(addr1, addr2);
    EXPECT_EQ(addr1, addr1);
    auto buf = addr1.buf();
    EXPECT_EQ(buf.len, sizeof(sockaddr_in));
    EXPECT_EQ(addr1.sockaddrLen(), sizeof(sockaddr_in));
    EXPECT_EQ(addr2.sockaddrLen(), sizeof(sa_family_t) + 4);
}

} // namespace stdplus
