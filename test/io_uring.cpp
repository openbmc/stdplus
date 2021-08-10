#include <poll.h>

#include <stdplus/io_uring.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace stdplus
{

using testing::_;

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
            exit(77);
        }
        io_uring_queue_exit(&r);
    }
    std::array<testing::StrictMock<MockHandler>, 2> h;
    IoUring ring = {};
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

TEST_F(IoUringTest, HandleCalledOnDestroy)
{
    auto& sqe = ring.getSQE();
    io_uring_prep_nop(&sqe);
    ring.setHandler(sqe, &h[0]);
    EXPECT_CALL(h[0], handleCQE(_));
}

} // namespace stdplus
