#include <stdplus/net/addr/ether.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(EqualOperator, EtherAddr)
{
    EXPECT_EQ((EtherAddr{1}), (ether_addr{1}));
    EXPECT_EQ((EtherAddr{}), (EtherAddr{}));
    EXPECT_NE((EtherAddr{1}), (EtherAddr{ether_addr{}}));
    std::hash<EtherAddr>{}(EtherAddr{});
}

TEST(FromStr, EtherAddr)
{
    EXPECT_THROW(fromStr<EtherAddr>("0x:00:00:00:00:00"),
                 std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00:00:00:00:00"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00:00:00:00:00:"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00:00:00:00::00"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>(":00:00:00:00:00"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00::00:00:00:00"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>(":::::"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00:0:0:0:0"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("00:00:00:00:00:00:00"),
                 std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>(""), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("123456789XYZ"), std::invalid_argument);
    EXPECT_THROW(fromStr<EtherAddr>("123456789AB"), std::overflow_error);
    EXPECT_THROW(fromStr<EtherAddr>("123456789ABCD"), std::overflow_error);

    EXPECT_EQ((EtherAddr{}), fromStr<EtherAddr>("00:00:00:00:00:00"));
    EXPECT_EQ((EtherAddr{0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa}),
              fromStr<EtherAddr>("FF:EE:DD:cc:bb:aa"));
    EXPECT_EQ((EtherAddr{0x00, 0x01, 0x02, 0x03, 0x04, 0x05}),
              fromStr<EtherAddr>("0:1:2:3:4:5"));
    EXPECT_EQ((EtherAddr{0x01, 0x23, 0x45, 0x67, 0x89, 0xab}),
              fromStr<EtherAddr>("0123456789AB"));
    EXPECT_EQ((EtherAddr{0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa}),
              fromStr<EtherAddr>("FFEEDDccbbaa"));
}

} // namespace stdplus
