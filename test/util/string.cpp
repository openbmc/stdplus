#include <gtest/gtest.h>
#include <stdplus/util/string.hpp>
#include <string>
#include <string_view>

namespace stdplus
{
namespace util
{
namespace
{

using namespace std::string_literals;
using namespace std::string_view_literals;

TEST(CStr, Basic)
{
    std::string s1 = "a";
    EXPECT_EQ(s1, cStr(s1));
    const std::string s2 = "b";
    EXPECT_EQ(s2, cStr(s2));
    char s3[] = "c";
    EXPECT_EQ(s3, cStr(s3));
    const char* s4 = "d";
    EXPECT_EQ(s4, cStr(s4));
}

TEST(StrCat, NoStr)
{
    EXPECT_EQ("", strCat());
}

TEST(StrCat, SingleStr)
{
    EXPECT_EQ("func", strCat("func"));
}

TEST(StrCat, Multi)
{
    EXPECT_EQ("func world test", strCat("func", " world"sv, " test"s));
}

TEST(StrCat, MoveStr)
{
    EXPECT_EQ("func", strCat("func"s));
    EXPECT_EQ("func world", strCat("func"s, " world"));
}

} // namespace
} // namespace util
} // namespace stdplus
