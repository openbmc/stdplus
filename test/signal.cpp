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

TEST(SignalTest, BlockSignal)
{
    constexpr int s = SIGINT;
    constexpr int otherS = SIGTERM;
    constexpr int notBlocked = SIGPROF;

    sigset_t expectedSet;
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &expectedSet));
    EXPECT_EQ(0, sigaddset(&expectedSet, otherS));
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &expectedSet, nullptr));
    EXPECT_EQ(0, sigismember(&expectedSet, notBlocked));
    EXPECT_EQ(0, sigismember(&expectedSet, s));
    EXPECT_EQ(0, sigaddset(&expectedSet, s));

    block(s);

    sigset_t newSet;
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &newSet));
    EXPECT_EQ(sigismember(&expectedSet, s), sigismember(&newSet, s));
    EXPECT_EQ(sigismember(&expectedSet, otherS), sigismember(&newSet, otherS));
    EXPECT_EQ(sigismember(&expectedSet, notBlocked),
              sigismember(&newSet, notBlocked));
}

TEST(SignalTest, KeepBlockSignal)
{
    constexpr int s = SIGINT;
    constexpr int otherS = SIGTERM;
    constexpr int notBlocked = SIGPROF;

    sigset_t expectedSet;
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &expectedSet));
    EXPECT_EQ(0, sigaddset(&expectedSet, s));
    EXPECT_EQ(0, sigaddset(&expectedSet, otherS));
    EXPECT_EQ(0, sigismember(&expectedSet, notBlocked));
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, &expectedSet, nullptr));

    block(s);

    sigset_t newSet;
    EXPECT_EQ(0, sigprocmask(SIG_BLOCK, nullptr, &newSet));
    EXPECT_EQ(sigismember(&expectedSet, s), sigismember(&newSet, s));
    EXPECT_EQ(sigismember(&expectedSet, otherS), sigismember(&newSet, otherS));
    EXPECT_EQ(sigismember(&expectedSet, notBlocked),
              sigismember(&newSet, notBlocked));
}

} // namespace
} // namespace signal
} // namespace stdplus
