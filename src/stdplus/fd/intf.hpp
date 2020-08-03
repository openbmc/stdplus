#pragma once
#include <cstddef>
#include <stdplus/types.hpp>

namespace stdplus
{

class Fd
{
  public:
    virtual ~Fd() = default;

    virtual stdplus::span<std::byte> read(stdplus::span<std::byte> buf) = 0;
    virtual stdplus::span<std::byte> recv(stdplus::span<std::byte> buf,
                                          int flags) = 0;
    virtual stdplus::span<const std::byte>
        write(stdplus::span<const std::byte> data) = 0;
    virtual stdplus::span<const std::byte>
        send(stdplus::span<const std::byte> data, int flags) = 0;
    virtual size_t lseek(off_t offset, int whence) = 0;
    virtual void truncate(off_t size) = 0;
    virtual void bind(stdplus::span<const std::byte> sockaddr) = 0;
    virtual void setsockopt(int level, int optname,
                            stdplus::span<const std::byte> opt) = 0;
    virtual int ioctl(unsigned long id, void* data) = 0;
    virtual void fcntl_setfl(int flags) = 0;
};

} // namespace stdplus
