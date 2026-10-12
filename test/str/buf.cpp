#include <stdplus/str/buf.hpp>
#include <stdplus/str/cexpr.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

static constexpr auto makeIncStr(std::size_t len)
{
    stdplus::StrBuf ret;
    auto ptr = ret.append(len);
    for (std::size_t i = 0; i < len; i++)
    {
        ptr[i] = 'A' + (i % 26);
    }
    return ret;
}

constexpr auto data = cexprSv<[]() { return makeIncStr(sizeof(StrBuf) + 10); }>;

TEST(StrBuf, BasicInline)
{
    StrBuf buf;
    EXPECT_EQ(buf.size(), 0);
    auto old_ptr = buf.begin();

    // Ensure that we inline small amounts of data only
    std::copy(data.begin(), data.begin() + 4, std::back_inserter(buf));
    EXPECT_EQ(buf.begin(), old_ptr);
    EXPECT_EQ(buf.size(), 4);
    EXPECT_EQ(std::string_view(data).substr(0, 4), buf);

    // Ensure that the max data size inline doesn't dynamically allocate
    constexpr std::size_t max_size = sizeof(StrBuf) - sizeof(std::size_t);
    buf.append(max_size - 4);
    EXPECT_EQ(buf.begin(), old_ptr);
    EXPECT_EQ(buf.size(), max_size);
}

TEST(StrBuf, DynamicAppend)
{
    constexpr std::size_t seg = 15;

    StrBuf buf;
    auto old_ptr = buf.begin();
    std::copy(data.begin(), data.begin() + seg, buf.append(seg));

    // Ensure that inline -> dynamic is working
    std::copy(data.begin() + seg, data.end(), buf.append(data.size() - seg));
    EXPECT_NE(buf.begin(), old_ptr);
    EXPECT_EQ(data, buf);
    EXPECT_EQ(data.size(), buf.size());

    // Ensure dynamic allocation is large enough to allow additional
    // small appends with no reallocation
    old_ptr = buf.begin();
    std::copy(data.begin(), data.begin() + seg, buf.append(seg));
    EXPECT_EQ(buf.begin(), old_ptr);
    EXPECT_EQ(data.size() + seg, buf.size());
    EXPECT_EQ(data.substr(0, seg), std::string_view{buf}.substr(data.size()));

    // Ensure new dynamic allocations copy
    std::copy(data.begin() + seg, data.end(), buf.append(data.size() - seg));
    EXPECT_NE(buf.begin(), old_ptr);
    EXPECT_EQ(data.size() << 1, buf.size());
    EXPECT_EQ(data, std::string_view{buf}.substr(0, data.size()));
    EXPECT_EQ(data, std::string_view{buf}.substr(data.size()));
}

TEST(StrBuf, CopiesInline)
{
    StrBuf buf1;
    buf1.append(1)[0] = 'a';
    StrBuf buf2 = buf1;
    EXPECT_EQ(buf1, buf2);
    buf1.append(1)[0] = 'b';
    StrBuf buf3 = buf1;
    EXPECT_EQ("ab", buf3);
    EXPECT_EQ("ab", buf1);
    EXPECT_EQ("a", buf2);
    buf3 = buf2;
    EXPECT_EQ("a", buf3);
    EXPECT_EQ("ab", buf1);
    EXPECT_EQ("a", buf2);
}

TEST(StrBuf, MoveInline)
{
    StrBuf buf1;
    buf1.append(1)[0] = 'a';
    StrBuf buf2 = std::move(buf1);
    EXPECT_EQ("a", std::string_view{buf2});
    buf2.append(1)[0] = 'b';
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(0, buf1.size());
    StrBuf buf3 = buf1;
    EXPECT_EQ(0, buf3.size());
    buf3 = std::move(buf2);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(0, buf2.size());
    EXPECT_EQ("ab", std::string_view{buf3});
}

TEST(StrBuf, CopyInlToDyn)
{
    StrBuf buf1;
    buf1.append(1)[0] = 'a';
    StrBuf buf2;
    buf2.append(sizeof(StrBuf) + 10);
    buf2 = buf1;
    EXPECT_EQ("a", buf2);
}

TEST(StrBuf, MoveInlToDyn)
{
    StrBuf buf1;
    buf1.append(1)[0] = 'a';
    StrBuf buf2;
    buf2.append(sizeof(StrBuf) + 10);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    buf2 = std::move(buf1);
    EXPECT_EQ("", buf1);
    EXPECT_EQ("a", buf2);
}

TEST(StrBuf, MoveDynToInl)
{
    StrBuf buf1;
    std::copy(data.begin(), data.end(), buf1.append(data.size()));
    StrBuf buf2;
    buf2.append(1)[0] = 'a';
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    buf2 = std::move(buf1);
    EXPECT_EQ("", buf1);
    EXPECT_EQ(data, buf2);
}

TEST(StrBuf, MoveDynToDyn)
{
    StrBuf buf1;
    std::copy(data.begin(), data.end(), buf1.append(data.size()));
    StrBuf buf2;
    buf2.append(data.size());
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    buf2 = std::move(buf1);
    EXPECT_EQ("", buf1);
    EXPECT_EQ(data, buf2);
}

TEST(StrBuf, CopyDynToInl)
{
    StrBuf buf1;
    buf1.append(data.size());
    buf1.clear();
    buf1.append(1)[0] = 'a';
    StrBuf buf2 = buf1;
    StrBuf buf3;
    buf3.append(1)[0] = 'b';
    buf3 = buf1;
    EXPECT_EQ("a", buf1);
    EXPECT_EQ("a", buf2);
    EXPECT_EQ("a", buf3);
}

TEST(StrBuf, CopyDynToDyn)
{
    StrBuf buf1;
    std::copy(data.begin(), data.end(), buf1.append(data.size()));
    std::copy(data.begin(), data.end(), buf1.append(data.size()));
    StrBuf buf2 = buf1;
    StrBuf buf3;
    std::copy(data.begin(), data.end(), buf3.append(data.size()));
    buf3 = buf1;
    EXPECT_EQ(buf2, buf1);
    EXPECT_EQ(buf3, buf1);
    buf3.append(1)[0] = 'a';
    buf1 = buf3;
    EXPECT_EQ(buf3, buf1);
}

static_assert(!noexcept(std::declval<StrBuf&>().push_back('a')));

TEST(StrBuf, WideChar)
{
    BasicStrBuf<char16_t> u16buf;
    u16buf.push_back(u'x');
    EXPECT_EQ(std::u16string_view{u"x"}, u16buf);
    for (std::size_t i = 0; i < 200; ++i)
    {
        u16buf.push_back(u'y');
    }
    EXPECT_EQ(201u, u16buf.size());

    WStrBuf wbuf;
    wbuf.push_back(L'a');
    EXPECT_EQ(std::wstring_view{L"a"}, wbuf);
    for (std::size_t i = 0; i < 200; ++i)
    {
        wbuf.push_back(L'b');
    }
    EXPECT_EQ(201u, wbuf.size());
}

template <typename T, bool POCCA, bool POCMA, bool UniqueId = false>
struct StatefulAlloc
{
    using value_type = T;
    using propagate_on_container_copy_assignment = std::bool_constant<POCCA>;
    using propagate_on_container_move_assignment = std::bool_constant<POCMA>;
    using is_always_equal = std::false_type;

    static inline std::size_t next_id = 0;
    std::size_t id = UniqueId ? ++next_id : 0;

    constexpr StatefulAlloc() noexcept = default;
    constexpr explicit StatefulAlloc(std::size_t id) noexcept : id(id) {}

    constexpr T* allocate(std::size_t n)
    {
        return std::allocator<T>{}.allocate(n);
    }

    constexpr void deallocate(T* p, std::size_t n) noexcept
    {
        std::allocator<T>{}.deallocate(p, n);
    }

    constexpr bool operator==(const StatefulAlloc& other) const noexcept
    {
        return id == other.id;
    }
};

TEST(StrBuf, StatefulAllocator)
{
    using BufPOCCA =
        BasicStrBuf<char, 128, StatefulAlloc<char, true, true, true>>;
    BufPOCCA a1;
    std::copy(data.begin(), data.end(), a1.append(data.size()));
    BufPOCCA a2;
    std::copy(data.begin(), data.end(), a2.append(data.size()));
    a2 = a1;
    EXPECT_EQ(data, a2);
    BufPOCCA a3;
    a3.push_back('x');
    a2 = std::move(a3);
    EXPECT_EQ("x", a2);

    using BufNoPOCMAEqual =
        BasicStrBuf<char, 128, StatefulAlloc<char, false, false, false>>;
    static_assert(!noexcept(
        std::declval<BufNoPOCMAEqual&>() = std::declval<BufNoPOCMAEqual&&>()));
    BufNoPOCMAEqual b1;
    std::copy(data.begin(), data.end(), b1.append(data.size()));
    BufNoPOCMAEqual b2;
    b2 = std::move(b1);
    EXPECT_EQ(data, b2);

    using BufNoPOCMAUnequal =
        BasicStrBuf<char, 128, StatefulAlloc<char, false, false, true>>;
    BufNoPOCMAUnequal c1;
    std::copy(data.begin(), data.end(), c1.append(data.size()));
    BufNoPOCMAUnequal c2;
    c2 = std::move(c1);
    EXPECT_EQ(data, c2);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    EXPECT_EQ(0u, c1.size());
}

} // namespace stdplus
