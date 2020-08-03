#pragma once
#include <gmock/gmock.h>
#include <stdplus/fd/intf.hpp>

namespace stdplus
{

class FdMock : public Fd
{
  public:
    MOCK_METHOD(span<std::byte>, read, (span<std::byte> buf), (override));
    MOCK_METHOD(span<std::byte>, recv, (span<std::byte> buf, int flags),
                (override));
    MOCK_METHOD(span<const std::byte>, write, (span<const std::byte> data),
                (override));
    MOCK_METHOD(span<const std::byte>, send,
                (span<const std::byte> data, int flags), (override));
    MOCK_METHOD(size_t, lseek, (off_t offset, int whence), (override));
    MOCK_METHOD(void, truncate, (off_t size), (override));
    MOCK_METHOD(void, bind, (span<const std::byte> sockaddr), (override));
    MOCK_METHOD(void, setsockopt,
                (int level, int optname, span<const std::byte> opt),
                (override));
    MOCK_METHOD(int, ioctl, (unsigned long id, void* data), (override));
    MOCK_METHOD(void, fcntlSetfl, (int flags), (override));
    MOCK_METHOD(int, fcntlGetfl, (), (override));
};

} // namespace stdplus
