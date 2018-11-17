#pragma once
#include <gmock/gmock.h>
#include <stdplus/internal/sys.hpp>

namespace stdplus
{
namespace test
{

class SysMock : public internal::Sys
{
  public:
    MOCK_CONST_METHOD1(dup, int(int));
    MOCK_CONST_METHOD1(close, int(int));
};

} // namespace test
} // namespace stdplus
