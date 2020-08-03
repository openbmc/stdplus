#pragma once
#include <cstddef>
#include <stdplus/types.hpp>

namespace stdplus
{

class Fd
{
  public:
    virtual ~Fd() = default;

    virtual span<std::byte> read(span<std::byte> buf) = 0;
    virtual span<std::byte> recv(span<std::byte> buf, int flags) = 0;
    virtual span<const std::byte> write(span<const std::byte> data) = 0;
    virtual span<const std::byte> send(span<const std::byte> data,
                                       int flags) = 0;
    virtual size_t lseek(off_t offset, int whence) = 0;
    virtual void truncate(off_t size) = 0;
    virtual void bind(span<const std::byte> sockaddr) = 0;
    virtual void setsockopt(int level, int optname,
                            span<const std::byte> opt) = 0;
    virtual int ioctl(unsigned long id, void* data) = 0;
    virtual int constIoctl(unsigned long id, void* data) const = 0;
    virtual void fcntlSetfd(int flags) = 0;
    virtual int fcntlGetfd() const = 0;
    virtual void fcntlSetfl(int flags) = 0;
    virtual int fcntlGetfl() const = 0;
};

} // namespace stdplus
