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
                (const, override));
    MOCK_METHOD(std::span<std::byte>, recv,
                (std::span<std::byte> buf, RecvFlags flags), (const, override));
    MOCK_METHOD((std::tuple<std::span<std::byte>, std::span<std::byte>>),
                recvfrom,
                (std::span<std::byte> buf, RecvFlags flags,
                 std::span<std::byte> sockaddr),
                (const, override));
    MOCK_METHOD(std::span<const std::byte>, write,
                (std::span<const std::byte> data), (const, override));
    MOCK_METHOD(std::span<const std::byte>, send,
                (std::span<const std::byte> data, SendFlags flags),
                (const, override));
    MOCK_METHOD(std::span<const std::byte>, sendto,
                (std::span<const std::byte> data, SendFlags flags,
                 std::span<const std::byte> sockaddr),
                (const, override));
    MOCK_METHOD(size_t, lseek, (off_t offset, Whence whence),
                (const, override));
    MOCK_METHOD(void, truncate, (off_t size), (const, override));
    MOCK_METHOD(void, bind, (std::span<const std::byte> sockaddr),
                (const, override));
    MOCK_METHOD(void, connect, (std::span<const std::byte> sockaddr),
                (const, override));
    MOCK_METHOD(void, listen, (int backlog), (const, override));
    MOCK_METHOD((std::optional<std::tuple<int, std::span<std::byte>>>), accept,
                (std::span<std::byte> sockaddr), (const, override));
    MOCK_METHOD(void, setsockopt,
                (SockLevel level, SockOpt optname,
                 std::span<const std::byte> opt),
                (const, override));
    MOCK_METHOD(int, ioctl, (unsigned long id, void* data), (const, override));
    MOCK_METHOD(int, constIoctl, (unsigned long id, void* data),
                (const, override));
    MOCK_METHOD(void, fcntlSetfd, (FdFlags flags), (const, override));
    MOCK_METHOD(FdFlags, fcntlGetfd, (), (const, override));
    MOCK_METHOD(void, fcntlSetfl, (FileFlags flags), (const, override));
    MOCK_METHOD(FileFlags, fcntlGetfl, (), (const, override));
    MOCK_METHOD(std::span<std::byte>, mmap,
                (std::byte * window, std::size_t size, ProtFlags prot,
                 MMapFlags flags, off_t offset),
                (const, override));
    MOCK_METHOD(void, munmap, (std::span<std::byte> window), (const, override));
};

} // namespace fd

using fd::FdMock;

} // namespace stdplus
