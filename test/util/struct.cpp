#include <gtest/gtest.h>
#include <stdplus/util/struct.hpp>

namespace stdplus
{
namespace util
{
namespace
{

TEST(CopyFrom, Empty)
{
    const std::string_view s;
    EXPECT_THROW(copyFrom<int>(s), std::runtime_error);
    EXPECT_TRUE(s.empty());
}

TEST(CopyFrom, Basic)
{
    int a = 4;
    const std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, copyFrom<int>(s));
}

TEST(CopyFrom, Larger)
{
    const std::string_view s("abc");
    EXPECT_EQ('a', copyFrom<char>(s));
}

TEST(Extract, TooSmall)
{
    std::string_view s("a");
    EXPECT_THROW(extract<int>(s), std::runtime_error);
    EXPECT_EQ(1, s.size());
}

TEST(Extract, Basic)
{
    int a = 4;
    std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, extract<int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(Extract, Larger)
{
    std::string_view s("abc");
    EXPECT_EQ('a', extract<char>(s));
    EXPECT_EQ(2, s.size());
}

} // namespace
} // namespace util
} // namespace stdplus
