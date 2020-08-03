#pragma once
#include <stdplus/fd/intf.hpp>
#include <gmock/gmock.h>

namespace stdplus
{

class FdMock : public Fd
{
  public:
    MOCK_METHOD(stdplus::span<std::byte>, read, (stdplus::span<std::byte> buf), (override));
    MOCK_METHOD(stdplus::span<std::byte>, recv, (stdplus::span<std::byte> buf, int flags), (override));
    MOCK_METHOD(stdplus::span<const std::byte>, write, (stdplus::span<const std::byte> data), (override));
    MOCK_METHOD(stdplus::span<const std::byte>, send, (stdplus::span<const std::byte> data, int flags), (override));
    MOCK_METHOD(size_t, lseek, (off_t offset, int whence), (override));
    MOCK_METHOD(void, truncate, (off_t size), (override));
    MOCK_METHOD(void, bind, (stdplus::span<const std::byte> sockaddr), (override));
    MOCK_METHOD(void, setsockopt, (int level, int optname,
                            stdplus::span<const std::byte> opt), (override));
    MOCK_METHOD(int, ioctl, (unsigned long id, void* data), (override));
    MOCK_METHOD(void, fcntl_setfl, (int flags), (override));
};

} // namespace stdplus
