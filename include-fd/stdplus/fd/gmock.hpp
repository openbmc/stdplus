#pragma once
#include <stdplus/fd/intf.hpp>

#include <gmock/gmock.h>

namespace stdplus
{
namespace fd
{

class FdMock : public Fd
{
  public:
    MOCK_METHOD(std::span<std::byte>, read, (std::span<std::byte> buf),
                (override));
    MOCK_METHOD(std::span<std::byte>, recv,
                (std::span<std::byte> buf, RecvFlags flags), (override));
    MOCK_METHOD(std::span<const std::byte>, write,
                (std::span<const std::byte> data), (override));
    MOCK_METHOD(std::span<const std::byte>, send,
                (std::span<const std::byte> data, SendFlags flags), (override));
    MOCK_METHOD(size_t, lseek, (off_t offset, Whence whence), (override));
    MOCK_METHOD(void, truncate, (off_t size), (override));
    MOCK_METHOD(void, bind, (std::span<const std::byte> sockaddr), (override));
    MOCK_METHOD(void, connect, (std::span<const std::byte> sockaddr),
                (override));
    MOCK_METHOD(void, listen, (int backlog), (override));
    MOCK_METHOD((std::optional<std::tuple<int, std::span<std::byte>>>), accept,
                (std::span<std::byte> sockaddr), (override));
    MOCK_METHOD(void, setsockopt,
                (SockLevel level, SockOpt optname,
                 std::span<const std::byte> opt),
                (override));
    MOCK_METHOD(int, ioctl, (unsigned long id, void* data), (override));
    MOCK_METHOD(int, constIoctl, (unsigned long id, void* data),
                (const, override));
    MOCK_METHOD(void, fcntlSetfd, (FdFlags flags), (override));
    MOCK_METHOD(FdFlags, fcntlGetfd, (), (const, override));
    MOCK_METHOD(void, fcntlSetfl, (FileFlags flags), (override));
    MOCK_METHOD(FileFlags, fcntlGetfl, (), (const, override));
    MOCK_METHOD(std::span<std::byte>, mmap,
                (std::span<std::byte> window, ProtFlags prot, MMapFlags flags,
                 off_t offset),
                (override));
    MOCK_METHOD(void, munmap, (std::span<std::byte> window), (override));
};

} // namespace fd

using fd::FdMock;

} // namespace stdplus
