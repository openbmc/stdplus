#include <fmt/format.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>

#include <string_view>

#include <gtest/gtest.h>

using std::literals::string_view_literals::operator""sv;

namespace stdplus
{

TEST(EqualOperator, In4Addr)
{
    EXPECT_EQ((In4Addr{255, 0, 255, 0}), (in_addr{hton(0xff00ff00)}));
    EXPECT_EQ((in_addr{}), (In4Addr{}));
    EXPECT_NE((In4Addr{1}), (In4Addr{in_addr{}}));
    std::hash<In4Addr>{}(In4Addr{});
}

TEST(FromStr, In4Addr)
{
    EXPECT_THROW(fromStr<In4Addr>(""), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("0.0.0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("0.0.0."), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>(".0.0.0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("0.0.0.0.0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("x.0.0.0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("ff.0.0.0"), std::invalid_argument);
    EXPECT_THROW(fromStr<In4Addr>("256.0.0.0"), std::overflow_error);

    EXPECT_EQ((In4Addr{}), fromStr<In4Addr>("0.0.0.0"));
    EXPECT_EQ((In4Addr{192, 168, 1, 1}), fromStr<In4Addr>("192.168.001.1"));

    EXPECT_EQ("192.0.0.0"_ip4, (In4Addr{192}));
}

TEST(ToStr, In4Addr)
{
    ToStrHandle<ToStr<In4Addr>> tsh;
    EXPECT_EQ("255.255.255.255", tsh(in_addr{0xffffffff}));
    EXPECT_EQ("1.15.3.4", tsh(In4Addr{1, 15, 3, 4}));
    EXPECT_EQ("0.0.0.0", tsh(In4Addr{}));
    EXPECT_EQ("a 1.15.3.4 b", fmt::format("a {} b", In4Addr{1, 15, 3, 4}));
}

TEST(EqualOperator, In6Addr)
{
    EXPECT_EQ((In6Addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}),
              (in6_addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ((in6_addr{}), (In6Addr{}));
    EXPECT_NE((In6Addr{1}), (In6Addr{in6_addr{}}));
    std::hash<In6Addr>{}(In6Addr{});
}

TEST(FromStr, In6Addr)
{
    constexpr FromStr<In6Addr> fs;
    EXPECT_THROW(fs(""sv), std::invalid_argument);
    EXPECT_THROW(fs("0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0::0:"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:::"sv), std::invalid_argument);
    EXPECT_THROW(fs(":::0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:::0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0::0::0"sv), std::invalid_argument);
    EXPECT_THROW(fs("1::0.0.0."sv), std::invalid_argument);
    EXPECT_THROW(fs("1::.0.0.0"sv), std::invalid_argument);
    EXPECT_THROW(fs("x::0"sv), std::invalid_argument);
    EXPECT_THROW(fs("g::0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:1:2:3:4::5:6:7"sv), std::invalid_argument);
    EXPECT_THROW(fs("::0:1:2:3:4:5:6:7"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:1:2:3:4:5:6:7::"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:1:2:3:4:5:6:7:8"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:1:2:3:4:5:6:0.0.0.0"sv), std::invalid_argument);
    EXPECT_THROW(fs("0:1:2:3:4:5::0.0.0.0"sv), std::invalid_argument);
    EXPECT_THROW(fs("ffff0::0"sv), std::overflow_error);

    EXPECT_EQ((In6Addr{}), fs("::"sv));
    EXPECT_EQ((In6Addr{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                       255, 255, 255, 255, 255}),
              fs("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"sv));
    EXPECT_EQ((In6Addr{}), fs("0:0:0:0:0:0:0:0"sv));
    EXPECT_EQ((In6Addr{0, 1}), fs("1::"sv));
    EXPECT_EQ((In6Addr{0, 0xff}), fs("ff::"sv));
    EXPECT_EQ((In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}),
              fs("::ff"sv));
    EXPECT_EQ((In6Addr{0, 0, 0, 0, 0, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}),
              fs("0:0:ff::ff"sv));
    EXPECT_EQ((In6Addr{0, 1, 0, 2, 0, 3, 0, 4, 0, 0, 0, 6, 0, 7, 0, 8}),
              fs("1:2:3:4::6:7:8"sv));
    EXPECT_EQ((In6Addr{0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 0}),
              fs("1:2:3:4:5:6:7::"sv));
    EXPECT_EQ((In6Addr{0, 0, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8}),
              fs("::2:3:4:5:6:7:8"sv));
    EXPECT_EQ(
        (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 192, 168, 0, 1}),
        fs("::ffff:192.168.0.1"sv));
    EXPECT_EQ((In6Addr{0, 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 168, 0, 1}),
              fs("ff::255.168.0.1"sv));
    EXPECT_EQ((In6Addr{0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 255, 168, 0, 1}),
              fs("0:1:2:3:4:5:255.168.0.1"sv));

    EXPECT_EQ("ff02::"_ip6, (In6Addr{0xff, 2}));
    EXPECT_EQ("1::"_ip6, (In6Addr{0, 1}));
    EXPECT_EQ("100::"_ip6, (In6Addr{1}));
    EXPECT_EQ("2::"_ip6, (In6Addr{0, 2}));
    EXPECT_EQ("ff::"_ip6, (In6Addr{0, 0xff}));
    EXPECT_EQ("::100"_ip6,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_EQ("::1"_ip6,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_EQ("::2"_ip6,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}));
    EXPECT_EQ("::ff"_ip6,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ("1::1"_ip6,
              (In6Addr{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_EQ("5::b"_ip6,
              (In6Addr{0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xb}));
}

TEST(ToStr, In6Addr)
{
    ToStrHandle<ToStr<In6Addr>> tsh;
    EXPECT_EQ("::", tsh(In6Addr{}));
    EXPECT_EQ("ff::", tsh(In6Addr{0, 0xff}));
    EXPECT_EQ("::ff",
              tsh(In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ("0:0:ff::ff", tsh(In6Addr{0, 0, 0, 0, 0, 0xff, 0, 0, 0, 0, 0, 0,
                                        0, 0, 0, 0xff}));
    EXPECT_EQ("::100:0:ff",
              tsh(In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ("ff00::", tsh(In6Addr{0xff}));
    EXPECT_EQ("1:2:3:4:5:6:7:8",
              tsh(In6Addr{0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8}));
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
              tsh(In6Addr{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                          255, 255, 255, 255, 255}));
    // rfc5952 4.2.2
    EXPECT_EQ("1:2:3:4:0:6:7:8",
              tsh(In6Addr{0, 1, 0, 2, 0, 3, 0, 4, 0, 0, 0, 6, 0, 7, 0, 8}));
    // rfc5952 4.2.3
    EXPECT_EQ("1::4:0:0:7:8",
              tsh(In6Addr{0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 7, 0, 8}));
    // rfc5952 5
    EXPECT_EQ("::ffff:192.168.0.1", tsh(In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0xff, 0xff, 192, 168, 0, 1}));

    EXPECT_EQ("a ff00:: b", fmt::format("a {} b", In6Addr{0xff}));
}

TEST(EqualOperator, InAnyAddr)
{
    EXPECT_EQ(InAnyAddr(In6Addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0xff}),
              (In6Addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_NE(InAnyAddr(In6Addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0xff}),
              (in_addr{}));
    EXPECT_EQ((In6Addr{}), InAnyAddr(In6Addr{}));
    EXPECT_NE((In4Addr{}), InAnyAddr(In6Addr{}));
    EXPECT_EQ((In4Addr{}), InAnyAddr(In4Addr{}));
    EXPECT_NE((In6Addr{}), InAnyAddr(In4Addr{}));
    EXPECT_EQ(InAnyAddr(In6Addr{}), (in6_addr{}));
    EXPECT_EQ(InAnyAddr(In4Addr{}), (in_addr{}));
    EXPECT_NE(InAnyAddr(In6Addr{1}), InAnyAddr(In6Addr{}));
    EXPECT_EQ(InAnyAddr(In6Addr{1}), InAnyAddr(In6Addr{1}));

    std::hash<InAnyAddr>{}(In4Addr{});
    std::hash<InAnyAddr>{}(In6Addr{});
}

TEST(FromStr, InAnyAddr)
{
    constexpr FromStr<InAnyAddr> fs;
    EXPECT_EQ((In4Addr{}), fs("0.0.0.0"sv));
    EXPECT_EQ((In6Addr{}), fs("::"sv));
    EXPECT_EQ(
        (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 192, 168, 0, 1}),
        fs("::ffff:192.168.0.1"sv));

    EXPECT_EQ((In4Addr{}), "0.0.0.0"_ip);
    EXPECT_EQ((In6Addr{}), "::"_ip);
    EXPECT_EQ("ff02::"_ip, (In6Addr{0xff, 2}));
    EXPECT_EQ("1::"_ip, (In6Addr{0, 1}));
    EXPECT_EQ("100::"_ip, (In6Addr{1}));
    EXPECT_EQ("2::"_ip, (In6Addr{0, 2}));
    EXPECT_EQ("ff::"_ip, (In6Addr{0, 0xff}));
    EXPECT_EQ("::100"_ip,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}));
    EXPECT_EQ("::1"_ip,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_EQ("::2"_ip,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2}));
    EXPECT_EQ("::ff"_ip,
              (In6Addr{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ("1::1"_ip,
              (In6Addr{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}));
    EXPECT_EQ("5::b"_ip,
              (In6Addr{0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xb}));
}

TEST(ToStr, InAnyAddr)
{
    ToStrHandle<ToStr<InAnyAddr>> tsh;
    EXPECT_EQ("1.15.3.4", tsh(In4Addr{1, 15, 3, 4}));
    EXPECT_EQ("ff::", tsh(In6Addr{0, 0xff}));

    EXPECT_EQ("a ff00:: b", fmt::format("a {} b", InAnyAddr{In6Addr{0xff}}));
}

TEST(Loopback, In4Addr)
{
    EXPECT_TRUE("127.0.0.0"_ip4.isLoopback());
    EXPECT_TRUE("127.0.0.1"_ip4.isLoopback());
    EXPECT_TRUE("127.0.0.83"_ip4.isLoopback());
    EXPECT_TRUE("127.253.0.0"_ip4.isLoopback());
    EXPECT_TRUE("127.255.255.255"_ip4.isLoopback());

    EXPECT_FALSE("0.0.0.0"_ip4.isLoopback());
    EXPECT_FALSE("0.255.255.255"_ip4.isLoopback());
    EXPECT_FALSE("126.255.255.255"_ip4.isLoopback());
    EXPECT_FALSE("128.0.0.0"_ip4.isLoopback());
    EXPECT_FALSE("224.0.0.0"_ip4.isLoopback());
    EXPECT_FALSE("227.30.10.50"_ip4.isLoopback());
    EXPECT_FALSE("239.255.255.255"_ip4.isLoopback());
    EXPECT_FALSE("255.255.255.255"_ip4.isLoopback());
}

TEST(Loopback, In6Addr)
{
    EXPECT_TRUE("::1"_ip6.isLoopback());

    EXPECT_FALSE("::2"_ip6.isLoopback());
    EXPECT_FALSE("1::"_ip6.isLoopback());
    EXPECT_FALSE("2001:5938::fd98"_ip6.isLoopback());
    EXPECT_FALSE("fe80::1"_ip6.isLoopback());
    EXPECT_FALSE("feff:ffff:ffff:ffff::"_ip6.isLoopback());
}

TEST(Unicast, In4Addr)
{
    EXPECT_TRUE("1.1.1.1"_ip4.isUnicast());
    EXPECT_TRUE("8.8.4.4"_ip4.isUnicast());
    EXPECT_TRUE("10.30.0.1"_ip4.isUnicast());
    EXPECT_TRUE("127.0.0.0"_ip4.isUnicast());
    EXPECT_TRUE("127.0.0.1"_ip4.isUnicast());
    EXPECT_TRUE("127.0.0.83"_ip4.isUnicast());
    EXPECT_TRUE("127.253.0.0"_ip4.isUnicast());
    EXPECT_TRUE("127.255.255.255"_ip4.isUnicast());
    EXPECT_TRUE("169.253.255.255"_ip4.isUnicast());
    EXPECT_TRUE("169.254.0.1"_ip4.isUnicast());
    EXPECT_TRUE("169.254.255.255"_ip4.isUnicast());
    EXPECT_TRUE("169.255.0.0"_ip4.isUnicast());
    EXPECT_TRUE("192.168.1.0"_ip4.isUnicast());
    EXPECT_TRUE("240.0.0.0"_ip4.isUnicast());
    EXPECT_TRUE("255.255.255.1"_ip4.isUnicast());

    EXPECT_FALSE("0.0.0.0"_ip4.isUnicast());
    EXPECT_FALSE("0.255.255.255"_ip4.isUnicast());
    EXPECT_FALSE("224.0.0.0"_ip4.isUnicast());
    EXPECT_FALSE("227.30.10.50"_ip4.isUnicast());
    EXPECT_FALSE("239.255.255.255"_ip4.isUnicast());
    EXPECT_FALSE("255.255.255.255"_ip4.isUnicast());
}

TEST(Unicast, In6Addr)
{
    EXPECT_TRUE("::2"_ip6.isUnicast());
    EXPECT_TRUE("::1"_ip6.isUnicast());
    EXPECT_TRUE("1::"_ip6.isUnicast());
    EXPECT_TRUE("2001:5938::fd98"_ip6.isUnicast());
    EXPECT_TRUE("fe80::1"_ip6.isUnicast());
    EXPECT_TRUE("feff:ffff:ffff:ffff::"_ip6.isUnicast());

    EXPECT_FALSE("::"_ip6.isUnicast());
    EXPECT_FALSE("ff00::"_ip6.isUnicast());
    EXPECT_FALSE("ffff:ffff:ffff:ffff:ffff::"_ip6.isUnicast());
}

} // namespace stdplus
