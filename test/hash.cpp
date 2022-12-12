#include <gtest/gtest.h>
#include <stdplus/hash.hpp>
#include <string>

namespace stdplus
{

TEST(HashMulti, Basic)
{
    EXPECT_EQ(0, hashMulti());
    EXPECT_EQ(2654435834, hashMulti(1, 2));
    hashMulti(1, std::string("bacon"), nullptr);
}

} // namespace stdplus
