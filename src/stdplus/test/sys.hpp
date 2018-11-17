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
    MOCK_CONST_METHOD2(open, int(const char*, int));
    MOCK_CONST_METHOD1(dup, int(int));
    MOCK_CONST_METHOD1(close, int(int));
    MOCK_CONST_METHOD3(read, int(int, void*, size_t));
    MOCK_CONST_METHOD2(fcntl_setfl, int(int, int));
    MOCK_CONST_METHOD1(fcntl_getfl, int(int));
};

} // namespace test
} // namespace stdplus
