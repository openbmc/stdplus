#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/sys_mock.hpp>
#include <system_error>

namespace stdplus
{
namespace fd
{
namespace
{

using testing::Return;
using testing::SetErrnoAndReturn;

class ManagedFdTest : public testing::Test
{
  protected:
    testing::StrictMock<SysMock> mock, mock2;
    testing::InSequence seq;
};

TEST_F(ManagedFdTest, BadCloexec)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC))
        .WillOnce(SetErrnoAndReturn(EBUSY, -1));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
    EXPECT_THROW(ManagedFd(1, &mock), std::system_error);
}

TEST_F(ManagedFdTest, BasicConstruct)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd(1, &mock);
    EXPECT_EQ(1, fd.getValue());
    EXPECT_EQ(&mock, fd.getSys());
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, MoveConstruct)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd(1, &mock);
    ManagedFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, MoveAssign)
{
    EXPECT_CALL(mock2, fcntl_setfd(2, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd2(2, &mock2);
    {
        EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
        ManagedFd fd(1, &mock);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, MoveConstructFromDup)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    ManagedFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, MoveAssignFromDup)
{
    EXPECT_CALL(mock2, fcntl_setfd(2, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd2(2, &mock2);
    {
        EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
        DupableFd fd(1, &mock);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, CopyConstructFromDup)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    EXPECT_CALL(mock, fcntl_dupfd_cloexec(1)).WillOnce(Return(2));
    ManagedFd fd2(fd);
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, CopyAssignFromDup)
{
    EXPECT_CALL(mock2, fcntl_setfd(2, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd2(2, &mock2);
    {
        EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
        DupableFd fd(1, &mock);
        EXPECT_CALL(mock, fcntl_dupfd_cloexec(1)).WillOnce(Return(3));
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = fd;
        EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
    }
    EXPECT_CALL(mock, close(3)).WillOnce(Return(0));
}

TEST_F(ManagedFdTest, Release)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    ManagedFd fd(1, &mock);
    EXPECT_EQ(1, fd.release());
}

} // namespace
} // namespace fd
} // namespace stdplus
