#include <stdplus/strconv.hpp>

#include <algorithm>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string_view>

namespace stdplus
{

struct TestVal
{
};

template <typename CharT>
struct ToStr<TestVal, CharT, true>
{
    static inline constexpr std::string_view val = "test";
    static inline constexpr size_t buf_size = val.size();

    constexpr CharT* operator()(CharT* buf, TestVal) const noexcept
    {
        return std::copy(val.begin(), val.end(), buf);
    }
};

} // namespace stdplus

template <typename CharT>
struct fmt::formatter<stdplus::TestVal, CharT>
    : stdplus::Format<stdplus::TestVal, CharT>
{
};

namespace stdplus
{

TEST(ToStrBuf, Basic)
{
    EXPECT_EQ("test", (ToStrBuf<TestVal>{}({})));
    EXPECT_EQ(L"test", (ToStrBuf<TestVal, wchar_t>{}({})));
}

TEST(Format, Basic)
{
    EXPECT_EQ("t test", fmt::format("t {}", TestVal{}));
}

} // namespace stdplus
