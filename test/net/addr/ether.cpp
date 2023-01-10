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

} // namespace stdplus
