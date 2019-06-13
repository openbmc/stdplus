#include <gmock/gmock.h>
#include <stdplus/fd/base.hpp>
#include <stdplus/fd/sys_mock.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

class MockFd : public Fd
{
  public:
    int value;
    testing::StrictMock<SysMock> mock;

    MockFd(int value) : value(value)
    {
    }

    int getValue() const override
    {
        return value;
    }

    const Sys* getSys() const noexcept override
    {
        return &mock;
    }
};

} // namespace ops
} // namespace fd
} // namespace stdplus
