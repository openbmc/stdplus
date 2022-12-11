#include <stdplus/str/cexpr.hpp>
#include <stdplus/zstring_view.hpp>

#include <string>
#include <string_view>

#include <gtest/gtest.h>

using namespace std::string_view_literals;
using namespace std::string_literals;

namespace stdplus
{

TEST(Constexpr, SvFromSv)
{
    EXPECT_EQ("abc"sv, cexprZsv<[]() { return "abc"sv; }>);
    EXPECT_EQ("abc"sv, cexprSv<[]() { return "abc"sv; }>);
}

TEST(Constexpr, SvFromSmallStr)
{
    EXPECT_EQ("abc"sv, cexprSv<[]() { return "abc"s; }>);
    EXPECT_EQ("abc"sv, cexprZsv<[]() { return "abc"s; }>);
}

TEST(Constexpr, SvFromAllocStr)
{
    constexpr auto cb = []() {
        return "abcdefg"s.append("hijklmnopqrstuvwxyz"sv);
    };
    EXPECT_EQ("abcdefghijklmnopqrstuvwxyz"sv, cexprSv<cb>);
    EXPECT_EQ("abcdefghijklmnopqrstuvwxyz"sv, cexprZsv<cb>);
}

} // namespace stdplus
