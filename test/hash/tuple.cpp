#include <stdplus/hash/tuple.hpp>

#include <string>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(HashTuple, Basic)
{
    std::tuple<std::string, int> a{"bacon", 2};
    const stdplus::hash<decltype(a)> h;
    EXPECT_EQ(h(a), std::hash<decltype(a)>{}(a));
}

} // namespace stdplus
