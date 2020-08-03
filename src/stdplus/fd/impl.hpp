#pragma once
#include <stdplus/fd/intf.hpp>

namespace stdplus
{

class FdImpl : public Fd
{
  public:
    virtual int get() = 0;

    stdplus::span<std::byte> read(stdplus::span<std::byte> buf) override;
    stdplus::span<std::byte> recv(stdplus::span<std::byte> buf,
                                  int flags) override;
    stdplus::span<const std::byte>
        write(stdplus::span<const std::byte> data) override;
    stdplus::span<const std::byte> send(stdplus::span<const std::byte> data,
                                        int flags) override;
    size_t lseek(off_t offset, int whence) override;
    void truncate(off_t size) override;
    void bind(stdplus::span<const std::byte> sockaddr) override;
    void setsockopt(int level, int optname,
                    stdplus::span<const std::byte> opt) override;
    int ioctl(unsigned long id, void* data) override;
    void fcntl_setfl(int flags) override;
};

} // namespace stdplus
