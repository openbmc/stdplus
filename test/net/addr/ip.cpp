#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(EqualOperator, In4Addr)
{
    EXPECT_EQ((In4Addr{255, 0, 255, 0}), (in_addr{hton(0xff00ff00)}));
    EXPECT_EQ((in_addr{}), (In4Addr{}));
    EXPECT_NE((In4Addr{1}), (In4Addr{in_addr{}}));
    std::hash<In4Addr>{}(In4Addr{});
}

TEST(EqualOperator, In6Addr)
{
    EXPECT_EQ((In6Addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}),
              (in6_addr{0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff}));
    EXPECT_EQ((in6_addr{}), (In6Addr{}));
    EXPECT_NE((In6Addr{1}), (In6Addr{in6_addr{}}));
    std::hash<In6Addr>{}(In6Addr{});
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
