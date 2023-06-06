#include <stdplus/pinned.hpp>

#include <string>

#include <gtest/gtest.h>

namespace stdplus
{

static_assert(noexcept(Pinned<int>(4)));
static_assert(!noexcept(Pinned<std::string>("4")));

TEST(Pinned, Basic)
{
    EXPECT_EQ("hi", Pinned<std::string>("hi"));
    EXPECT_EQ("hi", Pinned(std::string("hi")));
    auto s = std::string("hi");
    EXPECT_EQ("hi", Pinned(s));
    EXPECT_EQ("hi", Pinned(std::move(s)));
    Pinned<std::string> ps = "hi";
    EXPECT_EQ("hi", Pinned<std::string>(ps));
    // EXPECT_EQ("hi", Pinned<std::string>(std::move(ps)));
    ps = "hi";
    EXPECT_EQ("hi", ps);
    s = std::string("hi");
    ps = s;
    EXPECT_EQ("hi", ps);
    ps = std::move(s);
    EXPECT_EQ("hi", ps);
    Pinned<std::string> ps2;
    ps2 = ps;
    EXPECT_EQ("hi", ps2);
    // ps2 = std::move(ps);
    // std::string s2 = std::move(ps2);
    EXPECT_EQ("hi", [](std::string& f) { return f; }(ps2));
    EXPECT_EQ("hi", [](std::string f) { return f; }(ps2));
}

TEST(Pinned, Fundamental)
{
    Pinned pi = 4;
    EXPECT_EQ(4, [](int& f) { return f; }(pi));
    EXPECT_EQ(4, [](int f) { return f; }(pi));
}

struct NoMove1
{
    NoMove1() = default;
    NoMove1(NoMove1&&) = delete;
    NoMove1& operator=(NoMove1&&) = default;
};

struct NoMove2
{
    NoMove2() = default;
    NoMove2(NoMove2&&) = default;
    NoMove2& operator=(NoMove2&&) = delete;
};

struct NoMove3
{
    NoMove3() = default;
    NoMove3(NoMove3&&) = delete;
    NoMove3& operator=(NoMove3&&) = delete;
};

TEST(PinnedRef, Basic)
{
    Pinned<std::string> pstr("hi");
    EXPECT_EQ("hi", PinnedRef<std::string>(pstr).get());
    EXPECT_EQ("hi", PinnedRef<const std::string>(pstr).get());
    EXPECT_EQ("hi", PinnedRef(pstr).get());

    const Pinned<std::string> cpstr("hi");
    // EXPECT_EQ("hi", PinnedRef<std::string>(cpstr).get());
    EXPECT_EQ("hi", PinnedRef<const std::string>(cpstr).get());
    EXPECT_EQ("hi", PinnedRef(cpstr).get());
}

TEST(PinnedRef, Fundamental)
{
    Pinned<int> pi = 4;
    EXPECT_EQ(4, PinnedRef<int>(pi));
    EXPECT_EQ(4, PinnedRef<const int>(pi));
    EXPECT_EQ(4, PinnedRef(pi));

    const Pinned<int> cpi = 4;
    // EXPECT_EQ(4, PinnedRef<int>(cpi));
    EXPECT_EQ(4, PinnedRef<const int>(cpi));
    EXPECT_EQ(4, PinnedRef(cpi));
}

TEST(PinnedRef, NoMove)
{
    // int i;
    // PinnedRef<int> ri(i);
    // NoMove1 nm1;
    // PinnedRef<NoMove1> rnm1(nm1);
    // NoMove2 nm2;
    // PinnedRef<NoMove2> rnm2(nm2);

    NoMove3 nm3;
    PinnedRef<NoMove3> rnm3(nm3);
    PinnedRef<const NoMove3> rnm3c(nm3);
    PinnedRef<const NoMove3> rnm3cr(rnm3);
    PinnedRef rnm3i(nm3);
    [[maybe_unused]] PinnedRef rnm3ir(rnm3);
    [[maybe_unused]] PinnedRef rnm3ic(rnm3c);

    const NoMove3 cnm3;
    // PinnedRef<NoMove3> crnm3(cnm3);
    PinnedRef<const NoMove3> crnm3c(cnm3);
    PinnedRef crnm3i(cnm3);
}

} // namespace stdplus
