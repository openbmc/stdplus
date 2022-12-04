#include <gtest/gtest.h>
#include <stdplus/numeric/string.hpp>
#include <string_view>

namespace stdplus
{

TEST(IntToStr, uint8_10)
{
    IntToStr<uint8_t, 10> e;
    static_assert(e.buf_size == 3);
    char buf[e.buf_size];
    EXPECT_EQ("0", std::string_view(buf, e(buf, 0)));
    EXPECT_EQ("42", std::string_view(buf, e(buf, 42)));
    EXPECT_EQ("255", std::string_view(buf, e(buf, 255)));
    EXPECT_EQ("000", std::string_view(buf, e(buf, 0, 3)));
    EXPECT_EQ("255", std::string_view(buf, e(buf, 255, 3)));
}

TEST(IntToStr, uint8_16)
{
    IntToStr<uint8_t, 16> e;
    static_assert(e.buf_size == 2);
    char buf[e.buf_size];
    EXPECT_EQ("0", std::string_view(buf, e(buf, 0)));
    EXPECT_EQ("2a", std::string_view(buf, e(buf, 42)));
    EXPECT_EQ("ff", std::string_view(buf, e(buf, 255)));
    EXPECT_EQ("00", std::string_view(buf, e(buf, 0, 2)));
    EXPECT_EQ("02", std::string_view(buf, e(buf, 2, 2)));
    EXPECT_EQ("ff", std::string_view(buf, e(buf, 255, 2)));
}

} // namespace stdplus
