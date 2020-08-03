#pragma once
#include <stdplus/fd/intf.hpp>

namespace stdplus
{

class FdImpl : public Fd
{
  public:
    virtual int get() = 0;

    span<std::byte> read(span<std::byte> buf) override;
    span<std::byte> recv(span<std::byte> buf, int flags) override;
    span<const std::byte> write(span<const std::byte> data) override;
    span<const std::byte> send(span<const std::byte> data, int flags) override;
    size_t lseek(off_t offset, int whence) override;
    void truncate(off_t size) override;
    void bind(span<const std::byte> sockaddr) override;
    void setsockopt(int level, int optname, span<const std::byte> opt) override;
    int ioctl(unsigned long id, void* data) override;
    void fcntlSetfd(int flags) override;
    int fcntlGetfd() override;
    void fcntlSetfl(int flags) override;
    int fcntlGetfl() override;
};

} // namespace stdplus
