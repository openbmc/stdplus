#include <stdplus/str/buf.hpp>
#include <stdplus/str/cat.hpp>

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
    EXPECT_EQ("func world test", strCat("func", " world"sv, " test"s));
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
}

} // namespace stdplus
