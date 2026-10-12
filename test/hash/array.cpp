#include <stdplus/hash/array.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashArray, Basic)
{
    std::array<std::string, 2> a{"bacon", "sound"};
    const stdplus::hash<decltype(a)> h;
    EXPECT_EQ(h(a), std::hash<decltype(a)>{}(a));

    int c[2] = {1, 2};
    const stdplus::hash<decltype(c)> hc;
    EXPECT_EQ(hc(c), std::hash<decltype(c)>{}(c));
}

} // namespace stdplus
