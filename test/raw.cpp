#include <catch2/catch.hpp>
#include <endian.h>
#include <stdexcept>
#include <stdplus/raw.hpp>
#include <string_view>
#include <vector>

namespace stdplus
{
namespace raw
{
namespace
{

TEST_CASE("Equal", "[Equal]")
{
	int a = 4;
	unsigned b = 4;
	CHECK(equal(a, b));
	b = 5;
	CHECK(!equal(a, b));
}

TEST_CASE("Copy From Empty", "[CopyFrom]")
{
    const std::string_view s;
    CHECK_THROWS_AS(copyFrom<int>(s), std::runtime_error);
    CHECK(s.empty());
}

TEST_CASE("Copy From Basic", "[CopyFrom]")
{
    int a = 4;
    const std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    CHECK(a == copyFrom<int>(s));
}

TEST_CASE("Copy From Partial", "[CopyFrom]")
{
    const std::vector<char> s = {'a', 'b', 'c'};
    CHECK('a' == copyFrom<char>(s));
    const char s2[] = "def";
    CHECK('d' == copyFrom<char>(s2));
}

TEST_CASE("Extract Too Small", "[Extract]")
{
    std::string_view s("a");
    CHECK_THROWS_AS(extract<int>(s), std::runtime_error);
    CHECK(1 == s.size());
}

TEST_CASE("Extract Basic", "[Extract]")
{
    int a = 4;
    std::string_view s(reinterpret_cast<char*>(&a), sizeof(a));
    CHECK(a == extract<int>(s));
    CHECK(s.empty());
}

TEST_CASE("Extract Partial", "[Extract]")
{
    std::string_view s("abc");
    CHECK('a' == extract<char>(s));
    CHECK(2 == s.size());
}

TEST_CASE("As View Byte", "[AsView]")
{
    int32_t a = 4;
    auto s = asView<uint8_t>(a);
    CHECK(a == copyFrom<int>(s));
}

TEST_CASE("As View Int", "[AsView]")
{
    int32_t a = 4;
    auto s = asView<char16_t>(a);
    CHECK(a == copyFrom<int>(s));
}

TEST_CASE("As View Arr", "[AsView]")
{
    std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asView<char16_t>(arr);
    REQUIRE(4 == s.size());
    CHECK(htole16(1) == s[0]);
    CHECK(htole16(0) == s[1]);
    CHECK(htole16(2) == s[2]);
    CHECK(htole16(0) == s[3]);
}

#ifdef STDPLUS_SPAN_TYPE
TEST_CASE("Span Extract TooSmall", "[Extract]")
{
    const std::vector<char> v = {'c'};
    span<const char> s = v;
    CHECK_THROWS_AS(extract<int>(s), std::runtime_error);
    CHECK(1 == s.size());
}

TEST_CASE("Span Extract Basic", "[Extract]")
{
    const std::vector<int> v = {4};
    span<const int> s = v;
    CHECK(v[0] == extract<int>(s));
    CHECK(s.empty());
}

TEST_CASE("Span Extract Larger", "[Extract]")
{
    const std::vector<int> v{3, 4, 5};
    span<const int> s = v;
    CHECK(v[0] == extract<int>(s));
    CHECK(v.size() - 1 == s.size());
}

TEST_CASE("As Span const", "[AsSpan]")
{
    const uint64_t data = htole64(0xffff0000);
    auto s = asSpan<uint32_t>(data);
    CHECK(s.size() == 2);
    CHECK(s[0] == htole32(0xffff0000));
    CHECK(s[1] == htole32(0x00000000));
}

TEST_CASE("As Span Arr const", "[AsSpan]")
{
    const std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asSpan<uint16_t>(arr);
    REQUIRE(4 == s.size());
    CHECK(htole16(1) == s[0]);
    CHECK(htole16(0) == s[1]);
    CHECK(htole16(2) == s[2]);
    CHECK(htole16(0) == s[3]);
}

TEST_CASE("As Span", "[AsSpan]")
{
    uint64_t data = htole64(0xffff0000);
    auto s = asSpan<uint16_t>(data);
    CHECK(s.size() == 4);
    s[2] = 0xfefe;
    CHECK(s[0] == htole16(0x0000));
    CHECK(s[1] == htole16(0xffff));
    CHECK(s[2] == htole16(0xfefe));
    CHECK(s[3] == htole16(0x0000));
}

TEST_CASE("As Span Arr", "[AsSpan]")
{
    std::vector<uint32_t> arr = {htole32(1), htole32(2)};
    auto s = asSpan<uint16_t>(arr);
    REQUIRE(4 == s.size());
    CHECK(htole16(1) == s[0]);
    CHECK(htole16(0) == s[1]);
    CHECK(htole16(2) == s[2]);
    CHECK(htole16(0) == s[3]);
}

#endif

} // namespace
} // namespace raw
} // namespace stdplus
