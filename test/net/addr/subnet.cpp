#include <fmt/format.h>

#include <stdplus/net/addr/subnet.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

auto addr4Full = In4Addr{255, 255, 255, 255};
auto addr6Full = In6Addr{255, 255, 255, 255, 255, 255, 255, 255,
                         255, 255, 255, 255, 255, 255, 255, 255};

TEST(Subnet4, Basic)
{
    EXPECT_NO_THROW(Subnet4(in_addr{0xffffffff}, 32));
    EXPECT_NO_THROW(Subnet4(addr4Full, 0));
    EXPECT_NO_THROW(Subnet4(in_addr{}, 10));
    EXPECT_THROW(Subnet4(in_addr{0xffffffff}, 33), std::invalid_argument);
    EXPECT_THROW(Subnet4(in_addr{0xffffffff}, 64), std::invalid_argument);

    EXPECT_NE(Subnet4(in_addr{0xff}, 32), Subnet4(in_addr{}, 32));
    EXPECT_NE(Subnet4(in_addr{0xff}, 26), Subnet4(in_addr{0xff}, 27));
    EXPECT_EQ(Subnet4(in_addr{0xff}, 32), Subnet4(in_addr{0xff}, 32));
    EXPECT_EQ(Subnet4(in_addr{}, 1), Subnet4(in_addr{}, 1));
}

TEST(Subnet4, Network)
{
    EXPECT_EQ((In4Addr{}), Subnet4(In4Addr{}, 32).network());
    EXPECT_EQ(addr4Full, Subnet4(addr4Full, 32).network());
    EXPECT_EQ((In4Addr{255, 255, 128, 0}), Subnet4(addr4Full, 17).network());
    EXPECT_EQ((In4Addr{255, 255, 0, 0}), Subnet4(addr4Full, 16).network());
    EXPECT_EQ((In4Addr{255, 254, 0, 0}), Subnet4(addr4Full, 15).network());
    EXPECT_EQ((In4Addr{}), Subnet4(addr4Full, 0).network());
    EXPECT_EQ((In4Addr{}), Subnet4(In4Addr{}, 0).network());
}

TEST(Subnet4, Contains)
{
    EXPECT_TRUE(Subnet4(addr4Full, 32).contains(addr4Full));
    EXPECT_FALSE(Subnet4(addr4Full, 32).contains(In4Addr{255, 255, 255, 254}));
    EXPECT_FALSE(Subnet4(addr4Full, 32).contains(In4Addr{}));
    EXPECT_TRUE(
        Subnet4(addr4Full, 17).contains(static_cast<in_addr>(addr4Full)));
    EXPECT_TRUE(Subnet4(addr4Full, 17).contains(In4Addr{255, 255, 128, 134}));
    EXPECT_FALSE(Subnet4(addr4Full, 17).contains(In4Addr{255, 255, 127, 132}));
    EXPECT_TRUE(Subnet4(addr4Full, 14).contains(addr4Full));
    EXPECT_TRUE(Subnet4(addr4Full, 0).contains(addr4Full));
    EXPECT_TRUE(Subnet4(In4Addr{}, 0).contains(addr4Full));
    EXPECT_TRUE(Subnet4(addr4Full, 0).contains(In4Addr{}));
}

TEST(Subnet6, Basic)
{
    EXPECT_NO_THROW(Subnet6(in6_addr{0xff}, 128));
    EXPECT_NO_THROW(Subnet6(addr6Full, 0));
    EXPECT_NO_THROW(Subnet6(in6_addr{}, 65));
    EXPECT_THROW(Subnet6(in6_addr{0xff}, 129), std::invalid_argument);
    EXPECT_THROW(Subnet6(in6_addr{0xff}, 150), std::invalid_argument);

    EXPECT_NE(Subnet6(in6_addr{0xff}, 32), Subnet6(in6_addr{}, 32));
    EXPECT_NE(Subnet6(in6_addr{0xff}, 26), Subnet6(in6_addr{0xff}, 27));
    EXPECT_EQ(Subnet6(in6_addr{0xff}, 32), Subnet6(in6_addr{0xff}, 32));
    EXPECT_EQ(Subnet6(in6_addr{}, 1), Subnet6(in6_addr{}, 1));
}

TEST(Subnet6, Network)
{
    EXPECT_EQ(In6Addr(), Subnet6(In6Addr(), 128).network());
    EXPECT_EQ(addr6Full, Subnet6(addr6Full, 128).network());
    EXPECT_EQ((In6Addr{255, 255, 255, 255, 224}),
              Subnet6(addr6Full, 35).network());
    EXPECT_EQ((In6Addr{255, 255, 255, 255}), Subnet6(addr6Full, 32).network());
    EXPECT_EQ((In6Addr{255, 255, 128, 0}), Subnet6(addr6Full, 17).network());
    EXPECT_EQ((In6Addr{255, 255, 0, 0}), Subnet6(addr6Full, 16).network());
    EXPECT_EQ((In6Addr{255, 254, 0, 0}), Subnet6(addr6Full, 15).network());
    EXPECT_EQ((In6Addr{}), Subnet6(addr6Full, 0).network());
    EXPECT_EQ((In6Addr{}), Subnet6(In6Addr{}, 0).network());
}

TEST(Subnet6, Contains)
{
    auto addr6NFull = addr6Full;
    addr6NFull.s6_addr[15] = 254;
    EXPECT_TRUE(Subnet6(addr6Full, 128).contains(addr6Full));
    EXPECT_FALSE(Subnet6(addr6Full, 128).contains(addr6NFull));
    EXPECT_FALSE(Subnet6(addr6Full, 128).contains(In6Addr{}));
    EXPECT_TRUE(
        Subnet6(addr6Full, 127).contains(static_cast<in6_addr>(addr6Full)));
    EXPECT_TRUE(Subnet6(addr6Full, 127).contains(addr6NFull));
    EXPECT_TRUE(
        Subnet6(addr6Full, 33).contains(In6Addr{255, 255, 255, 255, 128, 255}));
    EXPECT_FALSE(
        Subnet6(addr6Full, 33).contains(In6Addr{255, 255, 255, 255, 127}));
    EXPECT_TRUE(Subnet6(In6Addr{}, 33).contains(In6Addr{0, 0, 0, 0, 127}));
    EXPECT_TRUE(
        Subnet6(addr6Full, 14).contains(In6Addr{255, 255, 0, 0, 0, 0, 0, 145}));
    EXPECT_FALSE(Subnet6(addr6Full, 14).contains(In6Addr{255, 127, 1}));
    EXPECT_TRUE(Subnet6(addr6Full, 0).contains(addr6Full));
    EXPECT_TRUE(Subnet6(In6Addr{}, 0).contains(addr6Full));
    EXPECT_TRUE(Subnet6(addr6Full, 0).contains(In6Addr{}));
}

TEST(SubnetAny, Basic)
{
    EXPECT_NO_THROW(SubnetAny(in_addr{0xffffffff}, 32));
    EXPECT_NO_THROW(SubnetAny(addr4Full, 0));
    EXPECT_NO_THROW(SubnetAny(InAnyAddr{addr4Full}, 0));
    EXPECT_NO_THROW(SubnetAny(in_addr{}, 10));
    EXPECT_THROW(SubnetAny(in_addr{0xffffffff}, 33), std::invalid_argument);
    EXPECT_THROW(SubnetAny(InAnyAddr{in_addr{0xffffffff}}, 33),
                 std::invalid_argument);
    EXPECT_THROW(SubnetAny(in_addr{0xffffffff}, 64), std::invalid_argument);
    EXPECT_NO_THROW(SubnetAny(in6_addr{0xff}, 128));
    EXPECT_NO_THROW(SubnetAny(addr6Full, 0));
    EXPECT_NO_THROW(SubnetAny(InAnyAddr{addr6Full}, 0));
    EXPECT_NO_THROW(SubnetAny(in6_addr{}, 65));
    EXPECT_THROW(SubnetAny(in6_addr{0xff}, 129), std::invalid_argument);
    EXPECT_THROW(SubnetAny(InAnyAddr{in6_addr{0xff}}, 129),
                 std::invalid_argument);
    EXPECT_THROW(SubnetAny(in6_addr{0xff}, 150), std::invalid_argument);

    EXPECT_NO_THROW(SubnetAny(Subnet4(in_addr{}, 32)));
    EXPECT_NO_THROW(SubnetAny(Subnet6(in6_addr{0xff}, 128)));

    EXPECT_NE(SubnetAny(in6_addr{0xff}, 32), Subnet6(in6_addr{}, 32));
    EXPECT_NE(Subnet6(in6_addr{0xff}, 26), SubnetAny(in6_addr{0xff}, 27));
    EXPECT_EQ(SubnetAny(in6_addr{0xff}, 32), Subnet6(in6_addr{0xff}, 32));
    EXPECT_EQ(SubnetAny(in6_addr{0xff}, 32), SubnetAny(in6_addr{0xff}, 32));
    EXPECT_NE(SubnetAny(in6_addr{0xff}, 32), Subnet4(in_addr{0xff}, 32));
    EXPECT_NE(SubnetAny(in_addr{0xff}, 32), Subnet4(in_addr{}, 32));
    EXPECT_NE(Subnet4(in_addr{0xff}, 26), SubnetAny(in_addr{0xff}, 27));
    EXPECT_EQ(SubnetAny(in_addr{0xff}, 32), Subnet4(in_addr{0xff}, 32));
    EXPECT_EQ(SubnetAny(in_addr{0xff}, 32), SubnetAny(in_addr{0xff}, 32));
}

TEST(SubnetAny, Network)
{
    EXPECT_EQ(In6Addr(), SubnetAny(In6Addr(), 128).network());
    EXPECT_EQ(addr6Full, SubnetAny(addr6Full, 128).network());
    EXPECT_EQ(In6Addr(), SubnetAny(addr6Full, 0).network());
    EXPECT_EQ(In4Addr(), SubnetAny(In4Addr(), 32).network());
    EXPECT_EQ(addr4Full, SubnetAny(addr4Full, 32).network());
    EXPECT_EQ(In4Addr(), SubnetAny(addr4Full, 0).network());
}

TEST(SubnetAny, Contains)
{
    EXPECT_TRUE(SubnetAny(addr6Full, 128).contains(addr6Full));
    EXPECT_TRUE(SubnetAny(addr6Full, 128).contains(InAnyAddr{addr6Full}));
    EXPECT_FALSE(SubnetAny(addr6Full, 128).contains(in6_addr{}));
    EXPECT_FALSE(SubnetAny(addr6Full, 128).contains(InAnyAddr(in6_addr{})));
    EXPECT_TRUE(SubnetAny(addr4Full, 32).contains(addr4Full));
    EXPECT_TRUE(SubnetAny(addr4Full, 32).contains(InAnyAddr{addr4Full}));
    EXPECT_FALSE(SubnetAny(addr4Full, 32).contains(in_addr{}));
    EXPECT_FALSE(SubnetAny(addr4Full, 32).contains(InAnyAddr{In4Addr{}}));
}

} // namespace stdplus
