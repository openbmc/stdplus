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

} // namespace stdplus
