#include <gtest/gtest.h>
#include <memory>
#include <stdplus/pinned.hpp>
#include <string>

namespace stdplus
{

static_assert(noexcept(Pinned<int>(4)));
static_assert(!noexcept(Pinned<std::string>("4")));

TEST(Pinned, Basic)
{
    EXPECT_EQ("hi", Pinned<std::string>("hi"));
    EXPECT_EQ("hi", Pinned<std::string>(std::string("hi")));
    auto s = std::string("hi");
    EXPECT_EQ("hi", Pinned<std::string>(s));
    EXPECT_EQ("hi", Pinned<std::string>(std::move(s)));
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
    Pinned<int> pi = 4;
    EXPECT_EQ(4, [](int& f) { return f; }(pi));
    EXPECT_EQ(4, [](int f) { return f; }(pi));
}

TEST(PinnedRef, Basic)
{
    auto uptr = std::make_unique<std::string>("hi");
    PinnedRef<std::string>(uptr).get()[0] = 'd';
    EXPECT_EQ("di", *uptr);
    PinnedRef<const std::string> cref(uptr);
    // cref.get()[0] = 'e';
    EXPECT_EQ("di", cref.get());

    auto sptr = std::make_shared<std::string>("hi");
    EXPECT_EQ("hi", PinnedRef<std::string>(sptr).get());

    Pinned<std::string> pstr("hi");
    EXPECT_EQ("hi", PinnedRef<std::string>(pstr).get());
    EXPECT_EQ("hi", PinnedRef<const std::string>(pstr).get());
    const Pinned<std::string> cpstr("hi");
    // EXPECT_EQ("hi", PinnedRef<std::string>(cpstr).get());
    EXPECT_EQ("hi", PinnedRef<const std::string>(cpstr).get());
}

TEST(PinnedRef, Fundamental)
{
    auto uptr = std::make_unique<int>(4);
    EXPECT_EQ(4, PinnedRef<int>(uptr));
    Pinned<int> pi = 4;
    EXPECT_EQ(4, PinnedRef<int>(pi));
    EXPECT_EQ(4, PinnedRef<const int>(pi));
}

} // namespace stdplus
