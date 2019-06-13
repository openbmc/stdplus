#include <fcntl.h>
#include <gtest/gtest.h>
#include <stdplus/fd/ops/open.hpp>
#include <stdplus/test/sys.hpp>

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

TEST(FdOpsOpen, Fail)
{
    testing::StrictMock<test::SysMock> mock;
    EXPECT_CALL(mock, open(StrEq("/test-path"), O_RDWR))
        .WillOnce(SetErrnoAndReturn(EINVAL, -1));
    EXPECT_THROW(open("/test-path", O_RDWR, &mock), std::system_error);
}

TEST(FdOpsOpen, Success)
{
    testing::StrictMock<test::SysMock> mock;
    EXPECT_CALL(mock, open(StrEq("/test-path"), O_RDWR)).WillOnce(Return(3));
    auto fd = open("/test-path", O_RDWR, &mock);
    EXPECT_CALL(mock, close(3)).WillOnce(Return(0));
}

} // namespace
} // namespace ops
} // namespace fd
} // namespace stdplus
