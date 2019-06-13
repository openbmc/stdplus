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
    MOCK_CONST_METHOD2(open, int(const char*, int));
};

} // namespace test
} // namespace stdplus
