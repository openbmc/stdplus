#include <endian.h>

#include <stdplus/raw.hpp>

#include <array>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace stdplus
{
namespace raw
{
namespace
{

TEST(Equal, Equal)
{
    int a = 4;
    unsigned b = 4;
    EXPECT_TRUE(equal(a, b));
    b = 5;
    EXPECT_FALSE(equal(a, b));
}

TEST(CopyFrom, Empty)
{
    const std::string_view cs;
    EXPECT_THROW(copyFrom<int>(cs), std::runtime_error);
    std::string_view s;
    EXPECT_THROW(copyFrom<int>(s), std::runtime_error);
    EXPECT_THROW(copyFromStrict<char>(s), std::runtime_error);
}

TEST(CopyFrom, Basic)
{
    int a = 4;
    const std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, copyFrom<int>(s));
    EXPECT_EQ(a, copyFromStrict<int>(s));
}

TEST(CopyFrom, Partial)
{
    const std::vector<char> s = {'a', 'b', 'c'};
    EXPECT_EQ('a', copyFrom<char>(s));
    EXPECT_THROW(copyFromStrict<char>(s), std::runtime_error);
    const char s2[] = "def";
    EXPECT_EQ('d', copyFrom<char>(s2));
}

struct Int
{
    uint8_t data[sizeof(int)];

    inline bool operator==(const Int& other) const
    {
        return memcmp(data, other.data, sizeof(data)) == 0;
    }
};

TEST(RefFrom, Empty)
{
    const std::string_view cs;
    EXPECT_THROW(refFrom<Int>(cs), std::runtime_error);
    std::string_view s;
    EXPECT_THROW(refFrom<Int>(s), std::runtime_error);
    EXPECT_THROW(refFromStrict<Int>(s), std::runtime_error);
}

TEST(RefFrom, Basic)
{
    Int a = {4, 0, 0, 4};
    const std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, refFrom<Int>(s));
    EXPECT_EQ(a, refFromStrict<Int>(s));
}

TEST(RefFrom, Partial)
{
    const std::vector<char> s = {'a', 'b', 'c'};
    EXPECT_EQ('a', refFrom<char>(s));
    EXPECT_THROW(refFromStrict<char>(s), std::runtime_error);
    const char s2[] = "def";
    EXPECT_EQ('d', refFrom<char>(s2));
}

TEST(Extract, TooSmall)
{
    std::string str = "aaaa";
    str.resize(1);
    std::string_view s(str);
    EXPECT_THROW(extract<int>(s), std::runtime_error);
    EXPECT_EQ("a", s);
}

TEST(Extract, Basic)
{
    int a = 4;
    std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, extract<int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(Extract, Partial)
{
    std::string_view s("abc");
    EXPECT_EQ('a', extract<char>(s));
    EXPECT_EQ("bc", s);
}

TEST(ExtractRef, TooSmall)
{
    std::string_view s("a");
    EXPECT_THROW(extractRef<Int>(s), std::runtime_error);
    EXPECT_EQ("a", s);
}

TEST(ExtractRef, Basic)
{
    Int a = {4, 0, 0, 4};
    std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    EXPECT_EQ(a, extractRef<Int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(ExtractRef, Partial)
{
    std::string_view s("abc");
    EXPECT_EQ('a', extractRef<char>(s));
    EXPECT_EQ("bc", s);
}

TEST(AsView, Byte)
{
    int32_t a = 4;
    auto s = asView<uint8_t>(a);
    EXPECT_EQ(a, copyFrom<int>(s));
}

TEST(AsView, Int)
{
    int32_t a = 4;
    auto s = asView<char16_t>(a);
    EXPECT_EQ(a, copyFrom<int>(s));
}

using testing::ElementsAre;
using testing::ElementsAreArray;

TEST(AsView, Array)
{
    std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asView<char16_t>(arr);
    EXPECT_THAT(s, ElementsAre(htole16(1), htole16(0), htole16(2), htole16(0)));
}

TEST(AsView, StringView)
{
    std::string_view sv = "ab";
    auto s = asView<uint8_t>(sv);
    EXPECT_THAT(s, ElementsAreArray(sv));
}

TEST(SpanExtract, TooSmall)
{
    const std::vector<char> v = {'c'};
    std::span<const char> s = v;
    EXPECT_THROW(extract<int>(s), std::runtime_error);
    EXPECT_THAT(s, ElementsAreArray(v));
}

TEST(SpanExtract, Basic)
{
    const std::vector<int> v = {4};
    std::span<const int> s = v;
    EXPECT_EQ(v[0], extract<int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(SpanExtract, Larger)
{
    const std::vector<int> v{3, 4, 5};
    std::span<const int> s = v;
    EXPECT_EQ(v[0], extract<int>(s));
    EXPECT_THAT(s, ElementsAre(v[1], v[2]));
}

TEST(SpanExtractRef, TooSmall)
{
    const std::vector<char> v = {'c'};
    std::span<const char> s = v;
    EXPECT_THROW(extractRef<Int>(s), std::runtime_error);
    EXPECT_THAT(s, ElementsAreArray(v));
}

TEST(SpanExtractRef, Basic)
{
    const std::vector<Int> v = {{4, 0, 0, 4}};
    std::span<const Int> s = v;
    EXPECT_EQ(v[0], extractRef<Int>(s));
    EXPECT_TRUE(s.empty());
}

TEST(SpanExtractRef, Larger)
{
    const std::vector<Int> v{{3}, {4}, {5}};
    std::span<const Int> s = v;
    EXPECT_EQ(v[0], extractRef<Int>(s));
    EXPECT_THAT(s, ElementsAre(v[1], v[2]));
}

TEST(AsSpan, ConstInt)
{
    const uint64_t data = htole64(0xffff0000);
    auto s = asSpan<uint32_t>(data);
    EXPECT_THAT(s, ElementsAre(htole32(0xffff0000), htole32(0x00000000)));
}

TEST(AsSpan, ConstArray)
{
    const std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asSpan<uint16_t>(arr);
    EXPECT_THAT(s, ElementsAre(htole16(1), htole16(0), htole16(2), htole16(0)));
}

TEST(AsSpan, Int)
{
    struct
    {
        uint64_t a;
        uint64_t* data()
        {
            return &a;
        }
    } data = {htole64(0xffff0000)};
    auto s = asSpan<uint16_t>(data);
    EXPECT_THAT(s, ElementsAre(htole16(0x0000), htole16(0xffff),
                               htole16(0x0000), htole16(0x0000)));
    s[2] = 0xfefe;
    EXPECT_THAT(s, ElementsAre(htole16(0x0000), htole16(0xffff),
                               htole16(0xfefe), htole16(0x0000)));
}

TEST(AsSpan, Array)
{
    std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asSpan<uint16_t>(arr);
    EXPECT_THAT(s, ElementsAre(htole16(1), htole16(0), htole16(2), htole16(0)));
}

TEST(AsSpan, Span)
{
    std::array<char, 2> arr = {'a', 'b'};
    auto sp1 = std::span<const char>(arr);
    auto s1 = asSpan<uint8_t>(sp1);
    EXPECT_THAT(s1, ElementsAreArray(arr));
    auto sp2 = std::span<char>(arr);
    auto s2 = asSpan<uint8_t>(sp2);
    EXPECT_THAT(s2, ElementsAreArray(arr));
}

} // namespace
} // namespace raw
} // namespace stdplus
