#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/mock.hpp>
#include <stdplus/fd/ops/fcntl.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{
namespace
{

using testing::Return;
using testing::SetErrnoAndReturn;
using testing::StrEq;

class FdOpsFcntlTest : public testing::Test
{
  protected:
    MockFd fd;

    FdOpsFcntlTest() : fd(3)
    {
    }
};

TEST_F(FdOpsFcntlTest, SetNonblockFail)
{
    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value))
        .WillOnce(SetErrnoAndReturn(EINVAL, -1));
    EXPECT_THROW(setFileNonblock(fd, true), std::system_error);

    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value)).WillOnce(Return(0));
    EXPECT_CALL(fd.mock, fcntl_setfl(fd.value, 0))
        .WillOnce(SetErrnoAndReturn(EINVAL, -1));
    EXPECT_THROW(setFileNonblock(fd, false), std::system_error);
}

TEST_F(FdOpsFcntlTest, SetNonblockOn)
{
    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value)).WillOnce(Return(O_DIRECT));
    EXPECT_CALL(fd.mock, fcntl_setfl(fd.value, O_NONBLOCK | O_DIRECT))
        .WillOnce(Return(0));
    setFileNonblock(fd, true);
}

TEST_F(FdOpsFcntlTest, SetNonblockAlreadyOn)
{
    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value)).WillOnce(Return(O_NONBLOCK));
    EXPECT_CALL(fd.mock, fcntl_setfl(fd.value, O_NONBLOCK)).WillOnce(Return(0));
    setFileNonblock(fd, true);
}

TEST_F(FdOpsFcntlTest, SetNonblockOff)
{
    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value))
        .WillOnce(Return(O_NONBLOCK | O_DIRECT));
    EXPECT_CALL(fd.mock, fcntl_setfl(fd.value, O_DIRECT)).WillOnce(Return(0));
    setFileNonblock(fd, false);
}

TEST_F(FdOpsFcntlTest, SetNonblockAlreadyOff)
{
    EXPECT_CALL(fd.mock, fcntl_getfl(fd.value)).WillOnce(Return(O_DIRECT));
    EXPECT_CALL(fd.mock, fcntl_setfl(fd.value, O_DIRECT)).WillOnce(Return(0));
    setFileNonblock(fd, false);
}

} // namespace
} // namespace ops
} // namespace fd
} // namespace stdplus
