#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/ops/create.hpp>
#include <stdplus/fd/sys_mock.hpp>

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

TEST(FdOpsCreate, OpenFail)
{
    testing::StrictMock<SysMock> mock;
    EXPECT_CALL(mock, open(StrEq("/test-path"), O_RDWR))
        .WillOnce(SetErrnoAndReturn(EINVAL, -1));
    EXPECT_THROW(open("/test-path", O_RDWR, &mock), std::system_error);
}

TEST(FdOpsCreate, OpenSuccess)
{
    testing::StrictMock<SysMock> mock;
    testing::InSequence seq;
    EXPECT_CALL(mock, open(StrEq("/test-path"), O_RDWR)).WillOnce(Return(3));
    EXPECT_CALL(mock, fcntl_setfd(3, FD_CLOEXEC)).WillOnce(Return(0));
    auto fd = open("/test-path", O_RDWR, &mock);
    EXPECT_CALL(mock, close(3)).WillOnce(Return(0));
}

TEST(FdOpsCreate, SocketFail)
{
    testing::StrictMock<SysMock> mock;
    EXPECT_CALL(mock, socket(1, 2, 3)).WillOnce(SetErrnoAndReturn(EINVAL, -1));
    EXPECT_THROW(socket(1, 2, 3, &mock), std::system_error);
}

TEST(FdOpsCreate, SocketSuccess)
{
    testing::StrictMock<SysMock> mock;
    testing::InSequence seq;
    EXPECT_CALL(mock, socket(1, 2, 3)).WillOnce(Return(3));
    EXPECT_CALL(mock, fcntl_setfd(3, FD_CLOEXEC)).WillOnce(Return(0));
    auto fd = socket(1, 2, 3, &mock);
    EXPECT_CALL(mock, close(3)).WillOnce(Return(0));
}

} // namespace
} // namespace ops
} // namespace fd
} // namespace stdplus
