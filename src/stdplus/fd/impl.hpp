#pragma once
#include <stdplus/fd/intf.hpp>

namespace stdplus
{
namespace fd
{

class FdImpl : public Fd
{
  public:
    virtual int get() const = 0;

    span<std::byte> read(span<std::byte> buf) override;
    span<std::byte> recv(span<std::byte> buf, RecvFlags flags) override;
    span<const std::byte> write(span<const std::byte> data) override;
    span<const std::byte> send(span<const std::byte> data,
                               SendFlags flags) override;
    size_t lseek(off_t offset, Whence whence) override;
    void truncate(off_t size) override;
    void bind(span<const std::byte> sockaddr) override;
    void listen(int backlog) override;
    std::tuple<std::optional<int>, span<std::byte>>
        accept(span<std::byte> sockaddr) override;
    void setsockopt(SockLevel level, SockOpt optname,
                    span<const std::byte> opt) override;
    int ioctl(unsigned long id, void* data) override;
    int constIoctl(unsigned long id, void* data) const override;
    void fcntlSetfd(FdFlags flags) override;
    FdFlags fcntlGetfd() const override;
    void fcntlSetfl(FileFlags flags) override;
    FileFlags fcntlGetfl() const override;
};

} // namespace fd

using fd::FdImpl;

} // namespace stdplus
