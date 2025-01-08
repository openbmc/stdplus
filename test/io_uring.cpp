#include <fmt/format.h>
#include <poll.h>
#include <sys/utsname.h>

#include <stdplus/io_uring.hpp>
#include <stdplus/util/cexec.hpp>

#include <array>
#include <charconv>
#include <chrono>
#include <optional>
#include <string_view>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace stdplus
{

using testing::_;

static std::string_view extractVerNum(std::string_view& str)
{
    auto ret = str.substr(0, str.find('.'));
    str.remove_prefix(ret.size() + (ret.size() == str.size() ? 0 : 1));
    return ret;
}

template <typename T>
static T intFromStr(std::string_view str)
{
    T ret;
    auto res = std::from_chars(str.data(), str.data() + str.size(), ret);
    if (res.ec != std::errc{} || res.ptr != str.data() + str.size())
    {
        throw std::invalid_argument("Bad kernel version");
    }
    return ret;
}

static bool isKernelSafe(std::string_view ver, uint8_t smajor, uint8_t sminor)
{
    auto major = intFromStr<uint8_t>(extractVerNum(ver));
    auto minor = intFromStr<uint8_t>(extractVerNum(ver));
    return major > smajor || (major == smajor && minor >= sminor);
}

TEST(KernelInfo, VersionSafe)
{
    EXPECT_THROW(isKernelSafe("a", 5, 10), std::invalid_argument);
    EXPECT_THROW(isKernelSafe("3a.3b.c", 5, 10), std::invalid_argument);
    EXPECT_FALSE(isKernelSafe("4.11.20-nfd", 5, 10));
    EXPECT_FALSE(isKernelSafe("4.11.20", 5, 10));
    EXPECT_FALSE(isKernelSafe("4.11", 5, 10));
    EXPECT_FALSE(isKernelSafe("5.9.0", 5, 10));
    EXPECT_TRUE(isKernelSafe("5.10.1", 5, 10));
    EXPECT_TRUE(isKernelSafe("6.0.0", 5, 10));
    EXPECT_TRUE(isKernelSafe("6.0.0-abc", 5, 10));
}

static bool checkKernelSafe(uint8_t smajor, uint8_t sminor)
{
    utsname uts;
    CHECK_ERRNO(uname(&uts), "uname");
    return isKernelSafe(uts.release, smajor, sminor);
}

TEST(KernelInfo, Print)
{
    utsname uts;
    ASSERT_NO_THROW(CHECK_ERRNO(uname(&uts), "uname"));
    fmt::print("{} {} {} {} {}", uts.sysname, uts.nodename, uts.release,
               uts.version, uts.machine);
}

TEST(Convert, ChronoToKTS)
{
    const auto ns = 700;
    const auto s = 5;
    const auto kts =
        chronoToKTS(std::chrono::nanoseconds(ns) + std::chrono::seconds(s));
    EXPECT_EQ(kts.tv_sec, s);
    EXPECT_EQ(kts.tv_nsec, ns);
}

class MockHandler : public IoUring::CQEHandler
{
  public:
    MOCK_METHOD(void, handleCQE, (io_uring_cqe&), (noexcept, override));
};

class IoUringTest : public testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        io_uring r;
        if (io_uring_queue_init(1, &r, 0) == -ENOSYS)
        {
            // Not supported, skip running this test
            GTEST_SKIP();
        }
        io_uring_queue_exit(&r);
    }
    std::array<testing::StrictMock<MockHandler>, 2> h;
    IoUring ring;

    void testFdWrite(int fd, int flags,
                     std::optional<int> expected_res = std::nullopt)
    {
        auto& sqe = ring.getSQE();
        std::string_view data = "Test\n";
        io_uring_prep_write(&sqe, fd, data.data(), data.size(), 0);
        sqe.flags |= flags;
        ring.setHandler(sqe, &h[0]);
        ring.submit();
        ring.wait();
        EXPECT_CALL(h[0], handleCQE(_)).WillOnce([&](io_uring_cqe& cqe) {
            EXPECT_EQ(cqe.res, expected_res.value_or(data.size()));
        });
        ring.process();
        testing::Mock::VerifyAndClearExpectations(&h[0]);
    }
};

TEST_F(IoUringTest, NullHandler)
{
    auto& sqe = ring.getSQE();
    io_uring_prep_nop(&sqe);
    ring.submit();
    ring.process();
}

TEST_F(IoUringTest, HandlerCalled)
{
    {
        auto& sqe = ring.getSQE();
        io_uring_prep_nop(&sqe);
        ring.setHandler(sqe, &h[0]);
    }

    // Nothing should happen without submission
    ring.process();

    {
        auto& sqe = ring.getSQE();
        io_uring_prep_nop(&sqe);
        ring.setHandler(sqe, &h[1]);
    }

    // Handle all of the outstanding requests
    ring.submit();
    EXPECT_CALL(h[0], handleCQE(_));
    EXPECT_CALL(h[1], handleCQE(_));
    ring.process();
    testing::Mock::VerifyAndClearExpectations(&h[0]);
    testing::Mock::VerifyAndClearExpectations(&h[1]);
}

TEST_F(IoUringTest, HandlerReplacement)
{
    auto& sqe = ring.getSQE();
    io_uring_prep_nop(&sqe);
    ring.setHandler(sqe, &h[0]);

    // Setting a new handler should cancel the old one
    EXPECT_CALL(h[0], handleCQE(_));
    ring.setHandler(sqe, &h[1]);
    testing::Mock::VerifyAndClearExpectations(&h[0]);

    // Setting a null handler should cancel the old one
    EXPECT_CALL(h[1], handleCQE(_));
    ring.setHandler(sqe, nullptr);
    testing::Mock::VerifyAndClearExpectations(&h[1]);

    // Set it back twice and make sure it isn't recognized idempotently
    ring.setHandler(sqe, &h[1]);
    ring.setHandler(sqe, &h[1]);

    // Make sure it still works
    ring.submit();
    EXPECT_CALL(h[1], handleCQE(_));
    ring.process();
    testing::Mock::VerifyAndClearExpectations(&h[1]);
}

TEST_F(IoUringTest, EventFd)
{
    auto& efd = ring.getEventFd();

    for (size_t i = 0; i < h.size(); ++i)
    {
        auto& sqe = ring.getSQE();
        io_uring_prep_nop(&sqe);
        ring.setHandler(sqe, &h[i]);
        ring.submit();
    }

    // Our event fd should become ready
    pollfd pfd;
    pfd.fd = efd.get();
    pfd.events = POLLIN;
    ASSERT_EQ(1, poll(&pfd, 1, 100));

    // Handle all of the outstanding requests
    EXPECT_CALL(h[0], handleCQE(_));
    EXPECT_CALL(h[1], handleCQE(_));
    ring.processEvents();
    testing::Mock::VerifyAndClearExpectations(&h[0]);
    testing::Mock::VerifyAndClearExpectations(&h[1]);

    // Our event fd should be empty
    ASSERT_EQ(0, poll(&pfd, 1, 100));
}

TEST_F(IoUringTest, Wait)
{
    auto& sqe = ring.getSQE();
    auto kts = chronoToKTS(std::chrono::milliseconds(1));
    io_uring_prep_timeout(&sqe, &kts, 0, 0);
    ring.setHandler(sqe, &h[0]);
    ring.submit();
    ring.wait(std::chrono::seconds(1));
    EXPECT_CALL(h[0], handleCQE(_));
}

TEST_F(IoUringTest, HandleCalledOnDestroy)
{
    auto& sqe = ring.getSQE();
    io_uring_prep_nop(&sqe);
    ring.setHandler(sqe, &h[0]);
    EXPECT_CALL(h[0], handleCQE(_));
}

TEST_F(IoUringTest, RegisterFiles)
{
    // Earlier kernels had buggy support for registered files
    if (!checkKernelSafe(5, 10))
    {
        GTEST_SKIP();
    }

    std::optional<IoUring::FileHandle> fh;

    // Slots are always allocated linearly and re-used if invalidated
    fh = ring.registerFile(STDERR_FILENO);
    EXPECT_GT(ring.getFiles().size(), 1);
    EXPECT_EQ(*fh, 0);
    fh = ring.registerFile(STDOUT_FILENO);
    EXPECT_EQ(*fh, 1);
    EXPECT_GT(ring.getFiles().size(), 2);
    EXPECT_EQ(ring.getFiles()[0], -1);
    EXPECT_EQ(ring.getFiles()[1], STDOUT_FILENO);

    // The first handle should have dropped and can be replaced
    fh = ring.registerFile(STDERR_FILENO);
    EXPECT_EQ(*fh, 0);
    EXPECT_GT(ring.getFiles().size(), 1);
    EXPECT_EQ(ring.getFiles()[0], STDERR_FILENO);
    EXPECT_EQ(ring.getFiles()[1], -1);

    // We should be able to write to stderr via the fixed file and regular fd
    testFdWrite(STDERR_FILENO, 0);
    testFdWrite(*fh, IOSQE_FIXED_FILE);

    // Without registration we should only be able to write to the regular fd
    fh.reset();
    testFdWrite(STDERR_FILENO, 0);
    testFdWrite(*fh, IOSQE_FIXED_FILE, -EBADF);

    std::vector<IoUring::FileHandle> fhs;
    EXPECT_LT(ring.getFiles().size(), 9);
    ring.reserveFiles(9);
    EXPECT_EQ(ring.getFiles().size(), 9);
    for (size_t i = 0; i < 9; ++i)
    {
        fhs.emplace_back(ring.registerFile(STDERR_FILENO));
        testFdWrite(fhs.back(), IOSQE_FIXED_FILE);
    }
    EXPECT_EQ(ring.getFiles().size(), 9);
    fhs.emplace_back(ring.registerFile(STDERR_FILENO));
    testFdWrite(fhs.back(), IOSQE_FIXED_FILE);
    EXPECT_GE(ring.getFiles().size(), 10);
}

} // namespace stdplus
