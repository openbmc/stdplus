#include <stdplus/fd/gmock.hpp>

#include <gtest/gtest.h>

namespace stdplus
{

TEST(Mock, Instantiate)
{
    FdMock fd;
    const Fd& cfd = fd;
    EXPECT_CALL(fd, lseek(0, fd::Whence::Cur)).WillOnce(testing::Return(0));
    EXPECT_EQ(0, cfd.lseek(0, fd::Whence::Cur));
}

} // namespace stdplus
