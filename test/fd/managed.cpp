#include <gtest/gtest.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/test/sys.hpp>

namespace stdplus
{
namespace fd
{
namespace
{

using testing::Return;

TEST(ManagedFd, BasicConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    ManagedFd fd(1, &mock);
    EXPECT_EQ(1, fd.value());
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(ManagedFd, MoveConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    ManagedFd fd(1, &mock);
    ManagedFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(ManagedFd, MoveAssign)
{
    testing::StrictMock<test::SysMock> mock1, mock2;
    ManagedFd fd2(2, &mock2);
    {
        ManagedFd fd(1, &mock1);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock1, close(1)).WillOnce(Return(0));
}

TEST(ManagedFd, MoveConstructFromDup)
{
    testing::StrictMock<test::SysMock> mock;
    DupableFd fd(1, &mock);
    ManagedFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(ManagedFd, MoveAssignFromDup)
{
    testing::StrictMock<test::SysMock> mock1, mock2;
    ManagedFd fd2(2, &mock2);
    {
        DupableFd fd(1, &mock1);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock1, close(1)).WillOnce(Return(0));
}

TEST(ManagedFd, CopyConstructFromDup)
{
    testing::StrictMock<test::SysMock> mock;
    DupableFd fd(1, &mock);
    EXPECT_CALL(mock, dup(1)).WillOnce(Return(2));
    ManagedFd fd2(fd);
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
}

TEST(ManagedFd, CopyAssignFromDup)
{
    testing::StrictMock<test::SysMock> mock1, mock2;
    ManagedFd fd2(2, &mock2);
    {
        DupableFd fd(1, &mock1);
        EXPECT_CALL(mock1, dup(1)).WillOnce(Return(3));
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = fd;
        EXPECT_CALL(mock1, close(1)).WillOnce(Return(0));
    }
    EXPECT_CALL(mock1, close(3)).WillOnce(Return(0));
}

} // namespace
} // namespace fd
} // namespace stdplus
