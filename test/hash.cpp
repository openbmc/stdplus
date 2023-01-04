#include <stdplus/hash.hpp>

#include <array>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashMulti, Basic)
{
    EXPECT_EQ(0, hashMulti());
    EXPECT_EQ(2654435834, hashMulti(1, 2));
    int s[4] = {1, 2, 3, 4};
    std::array<uint16_t, 2> a = {3, 4};
    std::tuple<std::string, int> t = {"s", 8};
    hashMulti(1, std::string("bacon"), nullptr, s, a, t);
}

} // namespace stdplus
