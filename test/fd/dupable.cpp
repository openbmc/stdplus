#include <gtest/gtest.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/test/sys.hpp>

namespace stdplus
{
namespace fd
{
namespace
{

using testing::Return;

TEST(DupableFd, BasicMoveConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    DupableFd fd(1, &mock);
    EXPECT_EQ(1, fd.value());
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(DupableFd, BasicCopyConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    const int ifd = 1;
    EXPECT_CALL(mock, dup(1)).WillOnce(Return(2));
    DupableFd fd(ifd, &mock);
    EXPECT_EQ(2, fd.value());
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
}

TEST(DupableFd, MoveConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    DupableFd fd(1, &mock);
    DupableFd fd2(std::move(fd));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(DupableFd, MoveAssign)
{
    testing::StrictMock<test::SysMock> mock1, mock2;
    DupableFd fd2(2, &mock2);
    {
        DupableFd fd(1, &mock1);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        fd2 = std::move(fd);
    }
    EXPECT_CALL(mock1, close(1)).WillOnce(Return(0));
}

TEST(DupableFd, CopyConstruct)
{
    testing::StrictMock<test::SysMock> mock;
    DupableFd fd(1, &mock);
    EXPECT_CALL(mock, dup(1)).WillOnce(Return(2));
    DupableFd fd2(fd);
    EXPECT_CALL(mock, close(2)).WillOnce(Return(0));
    EXPECT_CALL(mock, close(1)).WillOnce(Return(0));
}

TEST(DupableFd, CopyAssign)
{
    testing::StrictMock<test::SysMock> mock1, mock2;
    DupableFd fd2(2, &mock2);
    {
        DupableFd fd(1, &mock1);
        EXPECT_CALL(mock2, close(2)).WillOnce(Return(0));
        EXPECT_CALL(mock1, dup(1)).WillOnce(Return(3));
        fd2 = fd;
        EXPECT_CALL(mock1, close(1)).WillOnce(Return(0));
    }
    EXPECT_CALL(mock1, close(3)).WillOnce(Return(0));
}

} // namespace
} // namespace fd
} // namespace stdplus
