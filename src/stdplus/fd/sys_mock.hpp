#pragma once
#include <gmock/gmock.h>
#include <stdplus/fd/sys.hpp>

namespace stdplus
{
namespace fd
{

class SysMock : public virtual fd::Sys
{
  public:
    MOCK_CONST_METHOD1(close, int(int));
    MOCK_CONST_METHOD1(fcntl_dupfd_cloexec, int(int));
    MOCK_CONST_METHOD2(fcntl_setfd, int(int, int));
};

} // namespace fd
} // namespace stdplus
