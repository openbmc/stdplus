#include <stdplus/net/addr/sock.hpp>

#include <format>
#include <string>
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

TEST(Sock4Addr, FromStr)
{
    constexpr FromStr<Sock4Addr> fs;
    EXPECT_THROW(fs("10"sv), std::invalid_argument);
    EXPECT_THROW(fs(":3959"sv), std::invalid_argument);
    EXPECT_THROW(fs("0.0.0.0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0.0.0.0:"sv), std::invalid_argument);
    EXPECT_THROW(fs(":::80"sv), std::invalid_argument);
    EXPECT_EQ((Sock4Addr{In4Addr{}, 30}), fs("0.0.0.0:30"sv));
}

TEST(Sock4Addr, ToStr)
{
    ToStrHandle<ToStr<Sock4Addr>> tsh;
    EXPECT_EQ("0.0.0.0:3959", tsh(Sock4Addr({}, 3959)));
    EXPECT_EQ("255.0.255.255:28",
              tsh(Sock4Addr(In4Addr{255, 0, 255, 255}, 28)));
    EXPECT_EQ("a 1.2.3.4:32 b",
              std::format("a {} b", Sock4Addr(In4Addr{1, 2, 3, 4}, 32)));
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

TEST(Sock6Addr, FromStr)
{
    constexpr FromStr<Sock6Addr> fs;
    EXPECT_THROW(fs("10"sv), std::invalid_argument);
    EXPECT_THROW(fs(":10"sv), std::invalid_argument);
    EXPECT_THROW(fs("ff::"sv), std::invalid_argument);
    EXPECT_THROW(fs("[ff::]"sv), std::invalid_argument);
    EXPECT_THROW(fs("[::]:"sv), std::invalid_argument);
    EXPECT_THROW(fs("0.0.0.0:0"sv), std::invalid_argument);
    EXPECT_EQ((Sock6Addr{In6Addr{}, 80, 0}), fs("[::]:80"sv));
}

TEST(Sock6Addr, ToStr)
{
    ToStrHandle<ToStr<Sock6Addr>> tsh;
    EXPECT_EQ("[::]:0", tsh(Sock6Addr({}, 0, 0)));
    EXPECT_EQ("[ff00::]:128", tsh(Sock6Addr(In6Addr{0xff}, 128, 0)));
    EXPECT_EQ("a [102:304::]:32 b",
              std::format("a {} b", Sock6Addr(In6Addr{1, 2, 3, 4}, 32, 0)));
}

TEST(SockUAddr, Basic)
{
    std::string as(sizeof(sockaddr_un), 'a');
    EXPECT_THROW((SockUAddr(as)), std::invalid_argument);
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
    auto buf = addr2.sockbuf();
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

TEST(SockUAddr, FromStr)
{
    constexpr FromStr<SockUAddr> fs;
    std::string as(sizeof(sockaddr_un), 'a');
    EXPECT_THROW(fs(std::string_view(as)), std::invalid_argument);
    EXPECT_THROW(fs("a\0"sv), std::invalid_argument);
    EXPECT_EQ((SockUAddr{"/nope"sv}), fs("unix:/nope"sv));
    EXPECT_EQ((SockUAddr{"@hi"sv}), fs("@hi"sv));
}

TEST(SockUAddr, ToStr)
{
    ToStrHandle<ToStr<SockUAddr>> tsh;
    EXPECT_EQ("unix:/nope", tsh(SockUAddr("/nope"sv)));
    EXPECT_EQ("unix:@hi", tsh(SockUAddr("\0hi"sv)));
    EXPECT_EQ("a unix:a b", std::format("a {} b", SockUAddr("a"sv)));
}

TEST(SockAnyAddr, Basic)
{
    constexpr SockAnyAddr addr1(std::in_place_type<Sock4Addr>, In4Addr{255},
                                3959);
    constexpr SockAnyAddr addr2(std::in_place_type<SockUAddr>, "/hi"sv);
    EXPECT_NE(addr1, addr2);
    EXPECT_EQ(addr1, addr1);
    auto buf = addr1.sockbuf();
    EXPECT_EQ(buf.len, sizeof(sockaddr_in));
    EXPECT_EQ(addr1.sockaddrLen(), sizeof(sockaddr_in));
    EXPECT_EQ(addr2.sockaddrLen(), sizeof(sa_family_t) + 4);
}

TEST(SockAnyAddr, FromStr)
{
    constexpr FromStr<SockAnyAddr> fs;
    EXPECT_THROW(fs("abcd"sv), std::invalid_argument);
    EXPECT_THROW(fs("/nope"sv), std::invalid_argument);
    EXPECT_EQ((Sock4Addr{In4Addr{}, 30}), fs("0.0.0.0:30"sv));
    EXPECT_EQ((Sock6Addr{In6Addr{}, 80, 0}), fs("[::]:80"sv));
    EXPECT_EQ((SockUAddr{"/nope"sv}), fs("unix:/nope"sv));
}

TEST(SockAnyAddr, ToStr)
{
    ToStrHandle<ToStr<SockAnyAddr>> tsh;
    EXPECT_EQ("unix:/nope", tsh(SockUAddr("/nope"sv)));
    EXPECT_EQ("0.0.0.0:3949", tsh(Sock4Addr(In4Addr{}, 3949)));
    EXPECT_EQ("a [::]:356 b",
              std::format("a {} b", Sock6Addr(In6Addr{}, 356, 0)));
}

} // namespace stdplus
