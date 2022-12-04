#include <gtest/gtest.h>
#include <stdplus/numeric/str.hpp>
#include <string_view>

namespace stdplus
{

TEST(IntToStr, Uint8_10)
{
    IntToStr<10, uint8_t> enc;
    static_assert(enc.buf_size == 4);
    char buf[enc.buf_size];
    EXPECT_EQ("0", std::string_view(buf, enc(buf, 0)));
    EXPECT_EQ("42", std::string_view(buf, enc(buf, 42)));
    EXPECT_EQ("255", std::string_view(buf, enc(buf, 255)));
    EXPECT_EQ("000", std::string_view(buf, enc(buf, 0, 3)));
    EXPECT_EQ("255", std::string_view(buf, enc(buf, 255, 3)));
}

TEST(IntToStr, Uint16_10)
{
    IntToStr<10, uint16_t> enc;
    static_assert(enc.buf_size == 6);
    char buf[enc.buf_size];
    EXPECT_EQ("55255", std::string_view(buf, enc(buf, 55255, 3)));
}

TEST(IntToStr, Uint32_10)
{
    IntToStr<10, uint32_t> enc;
    static_assert(enc.buf_size == 10);
    char buf[enc.buf_size];
    EXPECT_EQ("55255", std::string_view(buf, enc(buf, 55255, 3)));
    EXPECT_EQ("055255", std::string_view(buf, enc(buf, 55255, 6)));
    EXPECT_EQ("255255", std::string_view(buf, enc(buf, 255255, 3)));
}

TEST(IntToStr, Uint8_16)
{
    IntToStr<16, uint8_t> enc;
    static_assert(enc.buf_size == 2);
    char buf[enc.buf_size];
    EXPECT_EQ("0", std::string_view(buf, enc(buf, 0)));
    EXPECT_EQ("2a", std::string_view(buf, enc(buf, 42)));
    EXPECT_EQ("ff", std::string_view(buf, enc(buf, 255)));
    EXPECT_EQ("00", std::string_view(buf, enc(buf, 0, 2)));
    EXPECT_EQ("02", std::string_view(buf, enc(buf, 2, 2)));
    EXPECT_EQ("ff", std::string_view(buf, enc(buf, 255, 2)));
}

TEST(IntToStr, Uint8_8)
{
    IntToStr<8, uint8_t> enc;
    static_assert(enc.buf_size == 3);
    char buf[enc.buf_size];
    EXPECT_EQ("0", std::string_view(buf, enc(buf, 0)));
    EXPECT_EQ("7", std::string_view(buf, enc(buf, 7)));
    EXPECT_EQ("10", std::string_view(buf, enc(buf, 8)));
    EXPECT_EQ("377", std::string_view(buf, enc(buf, 255)));
}

TEST(IntToStr, Uint8_11)
{
    IntToStr<11, uint8_t> enc;
    static_assert(enc.buf_size == 3);
    char buf[enc.buf_size];
    EXPECT_EQ("0", std::string_view(buf, enc(buf, 0)));
    EXPECT_EQ("39", std::string_view(buf, enc(buf, 42)));
    EXPECT_EQ("212", std::string_view(buf, enc(buf, 255)));
}

TEST(ToString, Int)
{
    EXPECT_EQ("10", stdplus::to_string(size_t{10}));
    EXPECT_EQ(L"10", stdplus::to_basic_string<wchar_t>(size_t{10}));
    EXPECT_EQ("-10", stdplus::to_string(ssize_t{-10}));
    EXPECT_EQ(L"-10", stdplus::to_basic_string<wchar_t>(ssize_t{-10}));
}

TEST(ToString, perf)
{
    GTEST_SKIP();
    IntToStr<16, size_t> enc;
    char buf[enc.buf_size];
    for (size_t i = 0; i < 100000000; ++i)
    {
        enc(buf, i);
    }
    EXPECT_TRUE(false);
}

} // namespace stdplus
