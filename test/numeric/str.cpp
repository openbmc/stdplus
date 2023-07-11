#include <stdplus/numeric/str.hpp>

#include <string_view>

#include <gtest/gtest.h>

using std::literals::string_view_literals::operator""sv;

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

TEST(IntToStr, Int8_10)
{
    IntToStr<10, int8_t> enc;
    static_assert(enc.buf_size == 5);
    char buf[enc.buf_size];
    EXPECT_EQ("42", std::string_view(buf, enc(buf, 42)));
    EXPECT_EQ("-127", std::string_view(buf, enc(buf, -127)));
}

TEST(IntToStr, Uint16_10)
{
    IntToStr<10, uint16_t> enc;
    static_assert(enc.buf_size == 6);
    char buf[enc.buf_size];
    EXPECT_EQ("55255", std::string_view(buf, enc(buf, 55255, 3)));
    ToStrHandle<IntToStr<10, uint16_t>> tsh;
    EXPECT_EQ("55255", tsh(55255));
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
    ToStrHandle<IntToStr<16, uint8_t>> tsh;
    EXPECT_EQ("2a", tsh(0x2a));
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
    EXPECT_EQ("10", stdplus::toStr(size_t{10}));
    EXPECT_EQ(L"10", stdplus::toBasicStr<wchar_t>(size_t{10}));
    EXPECT_EQ("-10", stdplus::toStr(ssize_t{-10}));
    EXPECT_EQ(L"-10", stdplus::toBasicStr<wchar_t>(ssize_t{-10}));
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

TEST(StrToInt, Uint8_10)
{
    StrToInt<10, uint8_t> dec;
    EXPECT_EQ(42, dec("42"sv));
    EXPECT_EQ(255, dec("255"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("-1"sv), std::invalid_argument);
    EXPECT_THROW(dec("a0"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("257"sv), std::overflow_error);
    EXPECT_THROW(dec("300"sv), std::overflow_error);
}

TEST(StrToInt, Uint8_11)
{
    StrToInt<11, uint8_t> dec;
    EXPECT_EQ(112, dec("a2"sv));
    EXPECT_EQ(255, dec("212"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("-1"sv), std::invalid_argument);
    EXPECT_THROW(dec("b0"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("213"sv), std::overflow_error);
    EXPECT_THROW(dec("300"sv), std::overflow_error);
}

TEST(StrToInt, Uint16_16)
{
    StrToInt<16, uint16_t> dec;
    EXPECT_EQ(0x42, dec("42"sv));
    EXPECT_EQ(0xfacf, dec("facf"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("-1"sv), std::invalid_argument);
    EXPECT_THROW(dec("g0"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("10000"sv), std::overflow_error);
}

TEST(StrToInt, Uint16_8)
{
    StrToInt<8, uint16_t> dec;
    EXPECT_EQ(042, dec("42"sv));
    EXPECT_EQ(0177777, dec("177777"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("-1"sv), std::invalid_argument);
    EXPECT_THROW(dec("g0"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("277777"sv), std::overflow_error);
}

TEST(StrToInt, Int8_16)
{
    StrToInt<16, int8_t> dec;
    EXPECT_EQ(-1, dec("-1"sv));
    EXPECT_EQ(0x42, dec("42"sv));
    EXPECT_EQ(-0x7f, dec("-7f"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("--1"sv), std::invalid_argument);
    EXPECT_THROW(dec("g0"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("ff"sv), std::overflow_error);
    EXPECT_THROW(dec("10000"sv), std::overflow_error);
}

TEST(StrToInt, Int8_0)
{
    StrToInt<0, int8_t> dec;
    EXPECT_EQ(-1, dec("-1"sv));
    EXPECT_EQ(42, dec("42"sv));
    EXPECT_EQ(0x42, dec("0x42"sv));
    EXPECT_EQ(-42, dec("-42"sv));
    EXPECT_EQ(-0x42, dec("-0x42"sv));
    EXPECT_THROW(dec(""sv), std::invalid_argument);
    EXPECT_THROW(dec("--1"sv), std::invalid_argument);
    EXPECT_THROW(dec("ac"sv), std::invalid_argument);
    EXPECT_THROW(dec(".0"sv), std::invalid_argument);
    EXPECT_THROW(dec("0xff"sv), std::overflow_error);
    EXPECT_THROW(dec("10000"sv), std::overflow_error);
}

TEST(StrToInt, Perf)
{
    GTEST_SKIP();
    StrToInt<16, size_t> dec;
    for (size_t i = 0; i < 100000000; ++i)
    {
        dec("53036893"sv);
    }
    EXPECT_TRUE(false);
}

} // namespace stdplus
