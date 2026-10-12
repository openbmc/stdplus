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

    std::tuple<> empty{};
    EXPECT_EQ(0u, stdplus::hash<decltype(empty)>{}(empty));
    EXPECT_EQ(0u, std::hash<decltype(empty)>{}(empty));

    int x = 42;
    std::tuple<int&> ref{x};
    EXPECT_EQ(stdplus::hash<int>{}(x), stdplus::hash<decltype(ref)>{}(ref));
    std::tuple<const int> cval{42};
    EXPECT_EQ(stdplus::hash<int>{}(42), stdplus::hash<decltype(cval)>{}(cval));
}

} // namespace stdplus
