#include <gtest/gtest.h>
#include <iostream>
#include <stdplus/zstring.hpp>
#include <string>
#include <string_view>

using std::literals::string_literals::operator""s;
using std::literals::string_view_literals::operator""sv;

namespace stdplus
{

TEST(Zstring, Construct)
{
    auto str = "a\0"s;
    EXPECT_EQ("a", zstring(str));
    EXPECT_EQ("a", const_zstring(str));
    const auto cstr = str;
    EXPECT_EQ("a", const_zstring(cstr));
    EXPECT_EQ("b", const_zstring("b\0"));
    char as[] = "c";
    EXPECT_EQ("c", zstring(as));
    EXPECT_EQ("c", const_zstring(as));

    std::cerr << const_zstring(as);
}

TEST(Zstring, NoTypeCoercion)
{
    char empty[] = "";
    auto zs = zstring(empty);
    auto czs = const_zstring("");

    EXPECT_NE(zs, "\0"sv);
    EXPECT_NE("\0"sv, zs);
    EXPECT_LT(zs, "\0"sv);
    EXPECT_GT("\0"sv, zs);
    EXPECT_NE(czs, "\0"sv);
    EXPECT_NE("\0"sv, czs);
    EXPECT_LT(czs, "\0"sv);
    EXPECT_GT("\0"sv, czs);

    auto str = "\0"s;
    EXPECT_NE(zs, str);
    EXPECT_NE(str, zs);
    EXPECT_LT(zs, str);
    EXPECT_GT(str, zs);
    EXPECT_NE(czs, str);
    EXPECT_NE(str, czs);
    EXPECT_LT(czs, str);
    EXPECT_GT(str, czs);
}

TEST(Zstring, Comparison)
{
    char mut[] = "ac";
    auto zs = zstring(mut);
    auto czs = const_zstring("ac");

#define test(cmp, rcmp, str)                                                   \
    EXPECT_##cmp(zs, const_zstring(str));                                      \
    EXPECT_##rcmp(const_zstring(str), zs);                                     \
    EXPECT_##cmp(czs, const_zstring(str));                                     \
    EXPECT_##rcmp(const_zstring(str), czs);                                    \
    EXPECT_##cmp(zs, str);                                                     \
    EXPECT_##rcmp(str, zs);                                                    \
    EXPECT_##cmp(czs, str);                                                    \
    EXPECT_##rcmp(str, czs);                                                   \
    EXPECT_##cmp(zs, str##sv);                                                 \
    EXPECT_##rcmp(str##sv, zs);                                                \
    EXPECT_##cmp(czs, str##sv);                                                \
    EXPECT_##rcmp(str##sv, czs);                                               \
    EXPECT_##cmp(zs, str##s);                                                  \
    EXPECT_##rcmp(str##s, zs);                                                 \
    EXPECT_##cmp(czs, str##s);                                                 \
    EXPECT_##rcmp(str##s, czs);                                                \
    EXPECT_##cmp(zs, reinterpret_cast<const char*>(str));                      \
    EXPECT_##rcmp(reinterpret_cast<const char*>(str), zs);                     \
    EXPECT_##cmp(czs, reinterpret_cast<const char*>(str));                     \
    EXPECT_##rcmp(reinterpret_cast<const char*>(str), czs)
    test(EQ, EQ, "ac");
    test(GT, LT, "a");
    test(LT, GT, "acb");
    test(LT, GT, "ad");
    test(GT, LT, "ab");
#undef test
}

} // namespace stdplus
