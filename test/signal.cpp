#include <cstring>
#include <gtest/gtest.h>
#include <signal.h>
#include <stdplus/signal.hpp>

namespace stdplus
{
namespace signal
{
namespace
{

TEST(Signal, Block)
{
    constexpr int s = SIGINT;
    constexpr int otherS = SIGTERM;
    constexpr int notBlocked = SIGPROF;

    sigset_t expectedSet;
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &expectedSet));
    ASSERT_EQ(0, sigaddset(&expectedSet, otherS));
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &expectedSet, nullptr));
    ASSERT_EQ(0, sigismember(&expectedSet, notBlocked));
    ASSERT_EQ(0, sigismember(&expectedSet, s));
    ASSERT_EQ(0, sigaddset(&expectedSet, s));

    block(s);

    sigset_t newSet;
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &newSet));
    EXPECT_EQ(sigismember(&expectedSet, s), sigismember(&newSet, s));
    EXPECT_EQ(sigismember(&expectedSet, otherS), sigismember(&newSet, otherS));
    EXPECT_EQ(sigismember(&expectedSet, notBlocked),
              sigismember(&newSet, notBlocked));
}

TEST(Signal, StayBlocked)
{
    constexpr int s = SIGINT;
    constexpr int otherS = SIGTERM;
    constexpr int notBlocked = SIGPROF;

    sigset_t expectedSet;
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &expectedSet));
    ASSERT_EQ(0, sigaddset(&expectedSet, s));
    ASSERT_EQ(0, sigaddset(&expectedSet, otherS));
    ASSERT_EQ(0, sigismember(&expectedSet, notBlocked));
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &expectedSet, nullptr));

    block(s);

    sigset_t newSet;
    ASSERT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &newSet));
    EXPECT_EQ(sigismember(&expectedSet, s), sigismember(&newSet, s));
    EXPECT_EQ(sigismember(&expectedSet, otherS), sigismember(&newSet, otherS));
    EXPECT_EQ(sigismember(&expectedSet, notBlocked),
              sigismember(&newSet, notBlocked));
}

} // namespace
} // namespace signal
} // namespace stdplus
