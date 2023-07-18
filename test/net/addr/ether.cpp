#include <fmt/format.h>

#include <stdplus/net/addr/ether.hpp>

#include <format>

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

TEST(Query, IsMulticast)
{
    EXPECT_TRUE((EtherAddr{255, 255, 255, 255, 255, 255}.isMulticast()));
    EXPECT_TRUE((EtherAddr{1}.isMulticast()));
    EXPECT_FALSE((EtherAddr{0, 1, 2, 3, 4, 5}.isMulticast()));
    EXPECT_FALSE((EtherAddr{0xfe, 255, 255, 255, 255, 255}.isMulticast()));
}

TEST(Query, IsUnicast)
{
    EXPECT_TRUE((EtherAddr{0, 1, 2, 3, 4, 5}.isUnicast()));
    EXPECT_TRUE((EtherAddr{0xfe, 255, 255, 255, 255, 255}.isUnicast()));
    EXPECT_FALSE((EtherAddr{}.isUnicast()));
    EXPECT_FALSE((EtherAddr{1}.isUnicast()));
    EXPECT_FALSE((EtherAddr{255, 255, 255, 255, 255, 255}.isUnicast()));
}

TEST(ToStr, EthAddr)
{
    ToStrHandle<ToStr<EtherAddr>> tsh;
    EXPECT_EQ("11:22:33:44:55:66",
              tsh(EtherAddr{0x11, 0x22, 0x33, 0x44, 0x55, 0x66}));
    EXPECT_EQ("01:02:03:04:05:67",
              tsh(EtherAddr{0x01, 0x02, 0x03, 0x04, 0x05, 0x67}));
    EXPECT_EQ("00:00:00:00:00:00",
              tsh(ether_addr{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
    EXPECT_EQ(
        "a 01:02:03:04:05:67 b",
        fmt::format("a {} b", EtherAddr{0x01, 0x02, 0x03, 0x04, 0x05, 0x67}));
    EXPECT_EQ(
        "a 01:02:03:04:05:67 b",
        std::format("a {} b", EtherAddr{0x01, 0x02, 0x03, 0x04, 0x05, 0x67}));
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
