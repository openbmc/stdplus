#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/sys_mock.hpp>

namespace stdplus
{
namespace fd
{
namespace
{

using testing::Return;
using testing::SetErrnoAndReturn;

class DupableFdTest : public testing::Test
{
  protected:
    testing::StrictMock<SysMock> mock, mock2;
    testing::InSequence seq;
};

TEST_F(DupableFdTest, BadCloexec)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC))
        .WillOnce(SetErrnoAndReturn(EBUSY, -1));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
    EXPECT_THROW(DupableFd(1, &mock), std::system_error);
}

TEST_F(DupableFdTest, BasicMoveConstruct)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    EXPECT_EQ(1, fd.getValue());
    EXPECT_EQ(&mock, fd.getSys());
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, BasicCopyConstruct)
{
    const int ifd = 1;
    EXPECT_CALL(mock, fcntl_dupfd_cloexec(1)).WillOnce(Return(2));
    DupableFd fd(ifd, &mock);
    EXPECT_EQ(2, fd.getValue());
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, MoveConstruct)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    DupableFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, MoveAssign)
{
    EXPECT_CALL(mock2, fcntl_setfd(2, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd2(2, &mock2);
    {
        EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
        DupableFd fd(1, &mock);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, CopyConstruct)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    EXPECT_CALL(mock, fcntl_dupfd_cloexec(1)).WillOnce(Return(2));
    DupableFd fd2(fd);
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, CopyAssign)
{
    EXPECT_CALL(mock2, fcntl_setfd(2, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd2(2, &mock2);
    {
        EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
        DupableFd fd(1, &mock);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        EXPECT_CALL(mock, fcntl_dupfd_cloexec(1)).WillOnce(Return(3));
        fd2 = fd;
        EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
    }
    EXPECT_CALL(mock, close(3)).WillOnce(Return(0));
}

TEST_F(DupableFdTest, Release)
{
    EXPECT_CALL(mock, fcntl_setfd(1, FD_CLOEXEC)).WillOnce(Return(0));
    DupableFd fd(1, &mock);
    EXPECT_EQ(1, fd.release());
}

} // namespace
} // namespace fd
} // namespace stdplus
