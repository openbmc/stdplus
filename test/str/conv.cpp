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
struct ToStr<TestValS>
{
    using type = TestValS;
    static inline constexpr std::size_t buf_size = TestValS::value.size();

    template <typename CharT>
    constexpr std::basic_string_view<CharT> operator()(TestValS) const noexcept
    {
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

static_assert(!detail::ToStrFixed<ToStr<TestValS>>);
static_assert(detail::ToStrStatic<ToStr<TestValS>>);

struct TestValF : TestValS
{};

template <>
struct ToStr<TestValF>
{
    using type = TestValF;
    static inline constexpr std::size_t buf_size = TestValF::value.size();

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, TestValF) const noexcept
    {
        return std::copy(TestValF::value.begin(), TestValF::value.end(), buf);
    }
};

static_assert(detail::ToStrFixed<ToStr<TestValF>>);
static_assert(!detail::ToStrStatic<ToStr<TestValF>>);

struct TestValD : TestValS
{};

template <>
struct ToStr<TestValD>
{
    using type = TestValD;

    template <typename CharT>
    constexpr void operator()(stdplus::BasicStrBuf<CharT>& buf, TestValD) const
    {
        auto ptr = buf.append(TestValD::value.size());
        std::copy(TestValD::value.begin(), TestValD::value.end(), ptr);
    }
};

static_assert(!detail::ToStrFixed<ToStr<TestValD>>);
static_assert(!detail::ToStrStatic<ToStr<TestValD>>);

} // namespace stdplus

template <typename CharT>
struct fmt::formatter<stdplus::TestValS, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::TestValS>, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::TestValF, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::TestValF>, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::TestValD, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::TestValD>, CharT>
{};

namespace stdplus
{

TEST(ToStrAdapter, Static)
{
    StrBuf buf;
    ToStrAdap<ToStr<TestValS>>{}(buf, TestValS{});
    EXPECT_EQ("test", buf);
    buf.reset();
    auto ptr = buf.append(4);
    EXPECT_EQ(4, ToStrAdap<ToStr<TestValS>>{}(ptr, TestValS{}) - ptr);
    EXPECT_EQ("test", std::string_view(ptr, 4));
}

TEST(ToStrAdapter, Fixed)
{
    StrBuf buf;
    ToStrAdap<ToStr<TestValF>>{}(buf, TestValF{});
    EXPECT_EQ("test", buf);
}

TEST(ToStrAdapter, Dynamic)
{
    StrBuf buf;
    ToStrAdap<ToStr<TestValD>>{}(buf, TestValD{});
    EXPECT_EQ("test", buf);
}

TEST(ToStrHandle, Basic)
{
    EXPECT_EQ("test", (ToStrHandle<ToStr<TestValS>>{}({})));
    EXPECT_EQ(L"test", (ToStrHandle<ToStr<TestValS>, wchar_t>{}({})));
    EXPECT_EQ("test", (ToStrHandle<ToStr<TestValF>>{}({})));
    EXPECT_EQ(L"test", (ToStrHandle<ToStr<TestValF>, wchar_t>{}({})));
    EXPECT_EQ("test", (ToStrHandle<ToStr<TestValD>>{}({})));
    EXPECT_EQ(L"test", (ToStrHandle<ToStr<TestValD>, wchar_t>{}({})));
}

TEST(Format, Basic)
{
    EXPECT_EQ("t test", fmt::format("t {}", TestValS{}));
    EXPECT_EQ("t test", fmt::format("t {}", TestValF{}));
    EXPECT_EQ("t test", fmt::format("t {}", TestValD{}));
}

template <>
struct FromStr<TestValS>
{
    constexpr TestValS operator()(const auto& str) const
    {
        const std::basic_string_view sv{str};
        if (sv == TestValS::value)
        {
            return TestValS{};
        }
        throw std::runtime_error("Invalid TestValS");
    }
};

TEST(FromStr, Basic)
{
    EXPECT_NO_THROW(FromStr<TestValS>{}(std::string_view{"test"}));
    EXPECT_THROW(FromStr<TestValS>{}(std::string_view{"hi"}),
                 std::runtime_error);
}

} // namespace stdplus
