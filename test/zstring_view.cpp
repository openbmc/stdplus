#include <gtest/gtest.h>
#include <iostream>
#include <set>
#include <stdplus/zstring_view.hpp>
#include <string>
#include <unordered_set>

namespace stdplus
{

using std::literals::string_literals::operator""s;
using zstring_view_literals::operator""_zsv;

TEST(ZstringView, Basic)
{
    auto s1 = zstring_view("ac");
    auto s = "b"s;
    auto s2 = zstring_view(s);
    std::string_view sv = s1;

    EXPECT_NE(sv, s2);
    EXPECT_NE(s1, s2);

    EXPECT_EQ("ac", s1);
    EXPECT_EQ(s1, "ac");
    EXPECT_EQ(s, s2);
    EXPECT_EQ(s2, s);
    EXPECT_EQ(s1, sv);
    EXPECT_EQ(sv, s1);
    EXPECT_LT("ab", s1);
    EXPECT_GT("ad", s1);
    EXPECT_LE(s1, "ac");
    EXPECT_LE(s, s2);
    EXPECT_LE(s2, s);
    EXPECT_LE(s1, sv);
    EXPECT_LE(sv, s1);

    std::cerr << s1;

    std::unordered_set<zstring_view> uset{s1, s2};
    EXPECT_EQ(1, uset.count("ac"));
    EXPECT_EQ(1, uset.count("b"));
    std::set<zstring_view> set{s2, s2};
    EXPECT_EQ(0, set.count("ac"));
    EXPECT_EQ(1, set.count("b"));
}

TEST(ZstringView, ConstructError)
{
    auto s = "hi\0"s;
    EXPECT_THROW((zstring_view(s)), std::invalid_argument);
    "hi"_zsv;
}

TEST(ZstringView, Suffix)
{
    auto s1 = zstring_view("ac");

    EXPECT_EQ("ac", s1.suffix(0));
    EXPECT_EQ("c", s1.suffix(1));
    EXPECT_EQ("", s1.suffix(2));
    EXPECT_THROW(s1.suffix(3), std::out_of_range);
}

} // namespace stdplus
