#include <fmt/format.h>
#include <gtest/gtest.h>
#include <iostream>
#include <set>
#include <stdplus/zstring_view.hpp>
#include <string>
#include <unordered_set>

namespace stdplus
{

using std::literals::string_literals::operator""s;
using std::literals::string_view_literals::operator""sv;
using zstring_view_literals::operator""_zsv;

TEST(ZstringView, IsString)
{
    EXPECT_TRUE((
        detail::same_string<std::string, char, std::char_traits<char>>::value));
    EXPECT_FALSE((detail::same_string<std::string_view, char,
                                      std::char_traits<char>>::value));
}

TEST(ZstringView, Basic)
{
    auto s1 = zstring_view("ac");
    auto s = "b"s;
    auto s2 = zstring_view(s);
    std::string_view sv = s1;
    char mut[] = "aa";
    auto s3 = zstring_view(mut);

    EXPECT_NE(sv, s2);
    EXPECT_NE(s1, s2);
    EXPECT_EQ(s3, "aa");

    EXPECT_EQ("ac"_zsv, s1);
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
    EXPECT_EQ("bac", fmt::format("b{}", s1));

    std::unordered_set<zstring_view> uset{s1, s2};
    EXPECT_EQ(1, uset.count("ac"));
    EXPECT_EQ(1, uset.count("b"));
    std::set<zstring_view> set{s2, s2};
    EXPECT_EQ(0, set.count("ac"));
    EXPECT_EQ(1, set.count("b"));

    auto from_str = [&](zstring_view cs) { EXPECT_EQ(cs, "ac"); };
    from_str("ac"s);
}

TEST(ZstringView, ConstructError)
{
    auto s = "hi\0"s;
#ifdef NDEBUG
    EXPECT_EQ("hi", zstring_view(s));
#else
    EXPECT_THROW((zstring_view(s)), std::invalid_argument);
#endif

    char mut1[] = "aa\0";
    char mut2[] = {'a', 'a'};
#ifdef NDEBUG
    EXPECT_EQ("aa\0", zstring_view(mut1));
    EXPECT_EQ("a", zstring_view(mut2));
#else
    EXPECT_THROW((zstring_view(mut1)), std::invalid_argument);
    EXPECT_THROW((zstring_view(mut2)), std::invalid_argument);
#endif
}

TEST(ZstringView, Suffix)
{
    auto s1 = zstring_view("ac");

    EXPECT_EQ("ac", s1.suffix(0));
    EXPECT_EQ("c", s1.suffix(1));
    EXPECT_EQ("", s1.suffix(2));
    EXPECT_THROW(s1.suffix(3), std::out_of_range);
}

TEST(ZstringView, NoTypeCoercion)
{
    EXPECT_NE(""_zsv, "\0");
    EXPECT_NE("\0", ""_zsv);
    EXPECT_NE(""_zsv, "\0"sv);
    EXPECT_NE("\0"sv, ""_zsv);
    EXPECT_LT(""_zsv, "\0"sv);
    EXPECT_GT("\0"sv, ""_zsv);
    auto str = "\0"s;
    EXPECT_NE(""_zsv, str);
    EXPECT_NE(str, ""_zsv);
    EXPECT_LT(""_zsv, str);
    EXPECT_GT(str, ""_zsv);
}

} // namespace stdplus
