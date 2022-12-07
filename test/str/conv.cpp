#include <fmt/format.h>

#include <stdplus/str/conv.hpp>

#include <algorithm>
#include <string_view>

#include <gtest/gtest.h>

namespace stdplus
{

struct TestValS
{
    static inline constexpr std::string_view value = "test";
    static inline constexpr std::wstring_view wvalue = L"test";
};

template <>
struct ToStr<TestValS, void> :
    ToStrStatic<ToStr<TestValS>, TestValS, TestValS::value.size()>
{
    template <typename CharT>
    constexpr std::basic_string_view<CharT> conv(TestValS) const noexcept
    {
        static_assert(bufSize == 4);
        if constexpr (std::is_same_v<char, CharT>)
        {
            return TestValS::value;
        }
        else
        {
            static_assert(std::is_same_v<wchar_t, CharT>);
            return TestValS::wvalue;
        }
    }
};

static_assert(ToStrIsStatic<ToStr<TestValS>>::value);

struct TestValF : TestValS
{};

template <>
struct ToStr<TestValF, void> :
    ToStrFixed<ToStr<TestValF>, TestValF, TestValF::value.size()>
{
    template <typename CharT>
    constexpr CharT* conv(CharT* buf, TestValF) const noexcept
    {
        static_assert(bufSize == 4);
        return std::copy(TestValF::value.begin(), TestValF::value.end(), buf);
    }
};

static_assert(!ToStrIsStatic<ToStr<TestValF>>::value);

struct TestValD : TestValS
{};

template <>
struct ToStr<TestValD, void> : ToStrDynamic<ToStr<TestValD>, TestValD>
{
    template <typename CharT>
    constexpr void conv(std::basic_string<CharT>& buf, TestValD) const
    {
        static_assert(bufSize == std::numeric_limits<std::size_t>::max());
        std::copy(TestValD::value.begin(), TestValD::value.end(),
                  std::back_inserter(buf));
    }
};

static_assert(!ToStrIsStatic<ToStr<TestValF>>::value);

} // namespace stdplus

template <typename CharT>
struct fmt::formatter<stdplus::TestValS, CharT> :
    stdplus::Format<stdplus::TestValS, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::TestValF, CharT> :
    stdplus::Format<stdplus::TestValF, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::TestValD, CharT> :
    stdplus::Format<stdplus::TestValD, CharT>
{};

namespace stdplus
{

TEST(ToStrStatic, Basic)
{
    constexpr TestValS tv;
    constexpr ToStr<TestValS> ts;
    EXPECT_EQ(tv.value, ts.template operator()<char>(tv));
    std::array<char, tv.value.size()> buf;
    EXPECT_EQ(tv.value, std::string_view(buf.data(), ts(buf.data(), tv)));
    std::string dbuf;
    ts(dbuf, tv);
    EXPECT_EQ(tv.value, dbuf);
}

TEST(ToStrFixed, Basic)
{
    constexpr TestValF tv;
    constexpr ToStr<TestValF> ts;
    // EXPECT_EQ(tv.value, ts.template operator()<char>(tv));
    std::array<char, tv.value.size()> buf;
    EXPECT_EQ(tv.value, std::string_view(buf.data(), ts(buf.data(), tv)));
    std::string dbuf;
    ts(dbuf, tv);
    EXPECT_EQ(tv.value, dbuf);
}

TEST(ToStrDynamic, Basic)
{
    constexpr TestValD tv;
    constexpr ToStr<TestValD> ts;
    // EXPECT_EQ(tv.value, ts.template operator()<char>(tv));
    // std::array<char, tv.value.size()> buf;
    // EXPECT_EQ(tv.value, std::string_view(buf.data(), ts(buf.data(), tv)));
    std::string dbuf;
    ts(dbuf, tv);
    EXPECT_EQ(tv.value, dbuf);
}

TEST(ToStrFinal, Basic)
{
    EXPECT_EQ("test", (ToStrFinal<TestValS>{}({})));
    EXPECT_EQ(L"test", (ToStrFinal<TestValS, wchar_t>{}({})));
    EXPECT_EQ("test", (ToStrFinal<TestValF>{}({})));
    EXPECT_EQ(L"test", (ToStrFinal<TestValF, wchar_t>{}({})));
    EXPECT_EQ("test", (ToStrFinal<TestValD>{}({})));
    EXPECT_EQ(L"test", (ToStrFinal<TestValD, wchar_t>{}({})));
}

TEST(Format, Basic)
{
    EXPECT_EQ("t test", fmt::format("t {}", TestValS{}));
    EXPECT_EQ("t test", fmt::format("t {}", TestValF{}));
    EXPECT_EQ("t test", fmt::format("t {}", TestValD{}));
}

} // namespace stdplus
