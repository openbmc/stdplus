#include <stdplus/hash.hpp>

#include <string>
#include <tuple>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashMulti, Basic)
{
    EXPECT_EQ(0, hashMulti());
    EXPECT_EQ(2654435834, hashMulti(1, 2));
    std::tuple<std::string, int> t = {"s", 8};
    hashMulti(1, std::string("bacon"), nullptr, t);
}

} // namespace stdplus
