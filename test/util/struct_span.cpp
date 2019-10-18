#include <gtest/gtest.h>
#include <stdplus/util/struct.hpp>
#include <vector>

namespace stdplus
{
namespace util
{
namespace
{

TEST(Extract, TooSmall)
{
    const std::vector<char> v = {'c'};
    span<const char> s = v;
    EXPECT_THROW(extract<int>(s), std::runtime_error);
    EXPECT_EQ(1, s.size());
}

TEST(Extract, Basic)
{
    const std::vector<int> v = {4};
    span<const int> s = v;
    EXPECT_EQ(4, extract<int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(Extract, Larger)
{
    const std::vector<int> v{3, 4, 5};
    span<const int> s = v;
    EXPECT_EQ(3, extract<int>(s));
    EXPECT_EQ(2, s.size());
}

} // namespace
} // namespace util
} // namespace stdplus
