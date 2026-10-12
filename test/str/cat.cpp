#include <stdplus/str/buf.hpp>
#include <stdplus/str/cat.hpp>
#include <stdplus/zstring_view.hpp>

#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace stdplus
{

using namespace std::string_literals;
using namespace std::string_view_literals;

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
    EXPECT_EQ("func world test ff",
              strCat("func", " world"sv, " test"s, " ff"_zsv));
    static_assert(strCat("func", " world"sv, " test"s, " ff"_zsv) ==
                  "func world test ff");
}

TEST(StrCat, MoveStr)
{
    EXPECT_EQ("func", strCat("func"s));
    EXPECT_EQ("func world", strCat("func"s, " world"));
}

TEST(StrAppend, Buf)
{
    StrBuf buf;
    strAppend(buf, "a ");
    strAppend(buf);
    strAppend(buf, "b"s, " c"sv);
    EXPECT_EQ("a b c", buf);

    // Self-append across inline-to-dynamic and dynamic-to-dynamic reallocations
    std::string expected = "a b c";
    for (int i = 0; i < 6; ++i)
    {
        strAppend(buf, buf, std::string_view(buf).substr(0, 2));
        expected += expected + expected.substr(0, 2);
        EXPECT_EQ(expected, std::string_view(buf));
    }

    std::string str = "hello";
    std::string expected_str = "hello";
    for (int i = 0; i < 6; ++i)
    {
        strAppend(str, str, std::string_view(str).substr(1, 3));
        expected_str += expected_str + expected_str.substr(1, 3);
        EXPECT_EQ(expected_str, str);
    }

    // Views of another character type are copied without rebasing
    WStrBuf wide;
    strAppend(wide, "xy"sv);
    EXPECT_EQ(L"xy", std::wstring_view(wide));
}

} // namespace stdplus
